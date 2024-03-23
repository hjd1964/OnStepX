// -----------------------------------------------------------------------------------
// non-volatile storage base class

#include "NV.h"
#include "../debug/Debug.h"

#ifndef NV_WIPE
  #define NV_WIPE OFF
#endif

bool NonVolatileStorage::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
  // set nv size
  this->size = size;
  // set cache size, defaults to 0 otherwise
  if (cacheEnable) cacheSize = size; else cacheSize = 0;
  waitMs = wait;
  if (waitMs == 0) delayedCommitEnabled = false; else delayedCommitEnabled = true;

  cacheStateSize = cacheSize/8 + 1;
  if (cacheSize == 0) return true;

  cache = new uint8_t[cacheSize];
  cacheStateRead  = new uint8_t[cacheStateSize];
  cacheStateWrite = new uint8_t[cacheStateSize];

  // mark entire read cache as dirty
  for (uint16_t i = 0; i < cacheStateSize; i++) cacheStateRead[i] = 255;
  // mark entire write cache as clean
  for (uint16_t i = 0; i < cacheStateSize; i++) cacheStateWrite[i] = 0;

  // stop compiler warnings
  (void)(checkEnable);
  (void)(wire);
  (void)(address);

  return true;
}

void NonVolatileStorage::setReadOnly(bool state) {
  readOnlyMode = state;
}

// wait for all commits to finish, blocking
void NonVolatileStorage::wait() {
  committed();
  uint32_t startTime = millis();

  VF("MSG: NV, waiting for commit");
  long passes = 0;
  while (!committed()) {
    poll(false);
    delay(1);

    passes++;
    if (passes % 4000 == 0) { V("."); }

    if ((long)(millis() - startTime) > 180000) {
      VF(" timed out with "); V(cacheSizeDirtyCount); VLF(" remaining");
      initError = true;
      return;
    }
  }
  VL(".");
}

// returns true if NV holds the correct key value in addresses 0..4
// except returns false if #define NV_WIPE ON exists
bool NonVolatileStorage::isKeyValid(uint32_t uniqueKey) {
  if (NV_WIPE == OFF) {
    bool state = readAndWriteThrough;
    readAndWriteThrough = true;
    keyMatches = readUL(0) == uniqueKey;
    readAndWriteThrough = state;
  } else keyMatches = false;
  return keyMatches;
};

// write the key value into addresses 0..3
void NonVolatileStorage::writeKey(uint32_t uniqueKey) {
  VLF("MSG: NV, writing key");
  bool readOnlyState = readOnlyMode;
  readOnlyMode = false;
  bool readAndWriteState = readAndWriteThrough;
  readAndWriteThrough = true;
  write(0, uniqueKey);
  readOnlyMode = readOnlyState;
  readAndWriteThrough = readAndWriteState;
}

// write pattern to all nv memory
void NonVolatileStorage::wipe(uint8_t j) {
  VF("MSG: NV, wipe with value 0x");
  char s[8];
  sprintf(s, "%02X", j);
  V(s);
  for (uint16_t i = 0; i < size; i++) write(i, (char)j);
  VL("");
}

// verify and wipe nv
bool NonVolatileStorage::verify() {
  initError = false;

  long errors = 0;
  VLF("MSG: NV, verify phase 1");
  wipe(0xff);
  wait();
  ignoreCache(true);
  for (uint16_t i = 0; i < size - 1; i++) { if (read(i) != 0xff) errors++; }
  ignoreCache(false);

  VLF("MSG: NV, verify phase 2");
  wipe(0x00);
  wait();
  ignoreCache(true);
  for (uint16_t i = 0; i < size - 1; i++) { if (read(i) != 0x00) errors++; }
  ignoreCache(false);

  if (errors == 0) {
    VLF("MSG: NV, verify success");
  } else {
    DF("ERR: NV, verify found "); D(errors); DLF(" errors");
    initError = true;
  }

  return initError;
}

void NonVolatileStorage::poll(bool disableInterrupts) {
  if (cacheSize == 0 || cacheClean) return;

  int8_t dirtyW = 0, dirtyR = 0;

  if (busy()) return;

  // check 20 byte chunks of cache for data that needs processing
  for (uint8_t j = 0; j < 20; j++) {
    cacheIndex++;
    if (cacheIndex >= cacheSize) {
      if (cacheCleanThisPass) cacheClean = true;
      cacheIndex = 0;
      cacheCleanThisPass = true;
    }

    if (!delayedCommitEnabled || (long)(millis() - commitReadyTimeMs) >= 0)
      dirtyW = bitRead(cacheStateWrite[cacheIndex/8], cacheIndex%8); else cacheCleanThisPass = false;
    dirtyR = bitRead(cacheStateRead[cacheIndex/8], cacheIndex%8);
    if (dirtyW || dirtyR) { cacheCleanThisPass = false; break; }
  }

  if (dirtyW) {
    uint16_t p = pageWriteSize;

    // if not a page boundary use a page size of 1
    if ((cacheIndex % p) != 0) p = 1; else

    for (uint16_t k = 0; k < p; k++) {
      // if a page write would exceed the NV size use a page size of 1
      if (cacheIndex + k >= cacheSize) { p = 1; break; }
      // check that the read cache for these locations is clean otherwise use a page size of 1
      if (bitRead(cacheStateRead[(cacheIndex + k)/8], (cacheIndex + k)%8)) { p = 1; break; }
    }

    // write the page and update the cache write state
    writePageToStorage(cacheIndex, &cache[cacheIndex], p);
    for (uint16_t k = 0; k < p; k++) {
      bitWrite(cacheStateWrite[(cacheIndex + k)/8], (cacheIndex + k)%8, 0);
    }
  } else {
    if (dirtyR) {
      cache[cacheIndex] = readFromStorage(cacheIndex);
      bitWrite(cacheStateRead[cacheIndex/8], cacheIndex%8, 0);
    }
  }

  /*
  int32_t dirtyWriteCount = 0;
  static int32_t lastDirtyWriteCount = 0;
  for (uint16_t i = 0; i < cacheSize; i++) { if (bitRead(cacheStateWrite[i/8], i%8)) dirtyWriteCount++; }
  if (lastDirtyWriteCount != dirtyWriteCount) { V("MSG: NV, cache "); V(dirtyWriteCount); VL(" bytes to be written"); }

  int32_t dirtyReadCount = 0;
  static int32_t lastDirtyReadCount = 0;
  for (uint16_t i = 0; i < cacheSize; i++) { if (bitRead(cacheStateRead[i/8], i%8)) dirtyReadCount++; }
  if (lastDirtyReadCount != dirtyReadCount) { V("MSG: NV, cache "); V(dirtyReadCount); VL(" bytes to be read"); }
  */

  // stop compiler warnings
  (void)(disableInterrupts);
}

bool NonVolatileStorage::committed() {
  cacheSizeDirtyCount = 0;

  for (uint16_t i = 0; i < cacheSize; i++) {
    if (bitRead(cacheStateWrite[i/8], i%8)) {
      cacheSizeDirtyCount++;
    }
  }

  return !cacheSizeDirtyCount;
}

bool valid() {
  return true;
}

void NonVolatileStorage::ignoreCache(bool state) {
  readAndWriteThrough = state;
}

uint8_t NonVolatileStorage::readFromCache(uint16_t i) {
  if (cacheSize == 0 || readAndWriteThrough) return readFromStorage(i);

  uint8_t dirty = bitRead(cacheStateRead[i/8], i%8);
  if (dirty) {
    uint8_t j = readFromStorage(i);
    
    // store and mark as clean
    cache[i] = j;
    bitWrite(cacheStateRead[i/8], i%8, 0);

    return j;
  } else return cache[i];
}

void NonVolatileStorage::writeToCache(uint16_t i, uint8_t j) {
  // no longer clean
  cacheClean = false;

  if (readAndWriteThrough) if (!readOnlyMode) writeToStorage(i, j);

  if (cacheSize == 0) {
    if (!readOnlyMode) {
      if (!readAndWriteThrough) {
        if (j != readFromStorage(i)) writeToStorage(i, j);
      } else writeToStorage(i, j);
    }

    commitReadyTimeMs = millis() + waitMs;
    return;
  }

  uint8_t k = readFromCache(i);
  if (j != k) {
    cache[i] = j;

    // mark write as dirty (needs to be written)
    bitWrite(cacheStateWrite[i/8], i%8, 1);

    // mark read as clean (so we don't overwrite the cache)
    bitWrite(cacheStateRead[i/8], i%8, 0);
  }

  commitReadyTimeMs = millis() + waitMs;
}

uint8_t  NonVolatileStorage::read(uint16_t i)   { return readFromCache(i); }
uint8_t  NonVolatileStorage::readUC(uint16_t i) { return read(i); }
int8_t   NonVolatileStorage::readC (uint16_t i) { return read(i); }
uint16_t NonVolatileStorage::readUI(uint16_t i) { uint16_t j; readBytes(i, (uint8_t*)&j, sizeof(uint16_t)); return j; }
int16_t  NonVolatileStorage::readI (uint16_t i) { int16_t j;  readBytes(i, (uint8_t*)&j, sizeof(int16_t));  return j; }
uint32_t NonVolatileStorage::readUL(uint16_t i) { uint32_t j; readBytes(i, (uint8_t*)&j, sizeof(uint32_t)); return j; }
int32_t  NonVolatileStorage::readL (uint16_t i) { int32_t j;  readBytes(i, (uint8_t*)&j, sizeof(int32_t));  return j; }
float    NonVolatileStorage::readF (uint16_t i) { float j;    readBytes(i, (uint8_t*)&j, sizeof(float));    return j; }
double   NonVolatileStorage::readD (uint16_t i) { double j;   readBytes(i, (uint8_t*)&j, sizeof(double));   return j; }
void     NonVolatileStorage::readStr(uint16_t i, char* j, int16_t maxLen) { readBytes(i, j, -maxLen); }

bool NonVolatileStorage::isNull(uint16_t i, int16_t count) {
  if (count < 0) count = -count;
  for (int16_t k = 0; k < count; k++) { if (read(i++) != 0) return false; }
  return true;
}

void NonVolatileStorage::readBytes(uint16_t i, void *j, int16_t count) {
  if (count < 0) {
    count = -count;
    for (int16_t k = 0; k < count; k++) { *(uint8_t*)j = read(i++); if (*(uint8_t*)j == 0) return; else j = (uint8_t*)j + 1; }
  } else {
    for (int16_t k = 0; k < count; k++) { *(uint8_t*)j = read(i++); j = (uint8_t*)j + 1; }
  }
}

void NonVolatileStorage::updateBytes(uint16_t i, void *j, int16_t count) {
  if (count < 0) {
    count = -count;
    for (int16_t k = 0; k < count; k++) { update(i++, *(uint8_t*)j); if (*(uint8_t*)j == 0) return; else j = (uint8_t*)j + 1; }
  } else {
    for (int16_t k = 0; k < count; k++) { update(i++, *(uint8_t*)j); j = (uint8_t*)j + 1; }
  }
}

bool NonVolatileStorage::busy() {
  return false;
}

int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}
