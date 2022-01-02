// -----------------------------------------------------------------------------------
// non-volatile storage base class

#include "NV.h"

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

bool NonVolatileStorage::isKeyValid(uint32_t uniqueKey) {
  bool state = readAndWriteThrough;
  readAndWriteThrough = true;
  keyMatches = readUL(0) == uniqueKey;
  readAndWriteThrough = state;
  return keyMatches;
};

// write the key value into addresses 0..3, blocking waits for all commits
void NonVolatileStorage::writeKey(uint32_t uniqueKey) {
  write(0, uniqueKey);
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
    if (!readOnlyMode) writeToStorage(cacheIndex, cache[cacheIndex]);
    bitWrite(cacheStateWrite[cacheIndex/8], cacheIndex%8, 0);
  } else {
    if (dirtyR) {
      cache[cacheIndex] = readFromStorage(cacheIndex);
      bitWrite(cacheStateRead[cacheIndex/8], cacheIndex%8, 0);
    }
  }

  // stop compiler warnings
  (void)(disableInterrupts);
}

bool NonVolatileStorage::committed() {
  uint8_t dirty = 0;

  for (uint16_t j = 0; j < cacheSize; j++) {
    dirty = bitRead(cacheStateWrite[j/8], j%8);
    if (dirty) break;
  }

  return !dirty;
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

void NonVolatileStorage::readBytes(uint16_t i, void *j, int16_t count) {
  if (count < 0) {
    count = -count;
    for (uint16_t k = 0; k < count; k++) { *(uint8_t*)j = read(i++); if (*(uint8_t*)j == 0) return; else j = (uint8_t*)j + 1; }
  } else {
    for (uint16_t k = 0; k < count; k++) { *(uint8_t*)j = read(i++); j = (uint8_t*)j + 1; }
  }
}

void NonVolatileStorage::updateBytes(uint16_t i, void *j, int16_t count) {
  if (count < 0) {
    count = -count;
    for (uint16_t k = 0; k < count; k++) { update(i++, *(uint8_t*)j); if (*(uint8_t*)j == 0) return; else j = (uint8_t*)j + 1; }
  } else {
    for (uint16_t k = 0; k < count; k++) { update(i++, *(uint8_t*)j); j = (uint8_t*)j + 1; }
  }
}

bool NonVolatileStorage::busy() {
  return false;
}

int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}
