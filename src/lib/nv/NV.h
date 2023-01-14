// -----------------------------------------------------------------------------------
// non-volatile storage base class

#pragma once

#include <Arduino.h>
#include <Wire.h>

class NonVolatileStorage {
  public:
    // prepare      EEPROM, FLASH based emulation, etc. for operation
    // size:        NV size in bytes
    // cacheEnable: enable or disable the cache (note NV size must be divisible by 8 if enabled)
    // wait:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // checkEnable: enable or disable checksum error detection
    // wire:        I2C interface pointer (set to NULL if not used)
    // address:     I2C address
    // result:      true if the device was found, or false if not
    virtual bool init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire = NULL, uint8_t address = 0);

    // protects already written data if true, defaults to false
    void setReadOnly(bool state);

    // check to see if read only operation is set
    inline bool isReadOnly() { return readOnlyMode; }

    // wait for all commits to finish, blocking
    void wait();

    // returns true if NV holds the correct key value in addresses 0..4
    // except returns false if #define NV_WIPE ON exists
    bool isKeyValid(uint32_t uniqueKey);

    // returns true if the NV key was checked and correct
    inline bool hasValidKey() { return keyMatches; }

    // write the key value into addresses 0..3
    void writeKey(uint32_t uniqueKey);

    // write pattern to all nv memory
    void wipe(uint8_t j = 0);

    // verify and wipe nv
    bool verify();

    // call frequently to perform any operations that need to happen in the background
    virtual void poll(bool disableInterrupts = true);

    // returns true if all data in any cache has been written or the commit has been done
    virtual bool committed();

    // returns true if all data in nv has passed ongoing validation checks
    bool valid();

    // causes read/write to ignore cache, write by update is disabled also
    void ignoreCache(bool state);

    // reads data from memory
    uint8_t readFromCache(uint16_t i);

    void writeToCache(uint16_t i, uint8_t j);

    // read unsigned char at position i
    uint8_t read(uint16_t i);

    // read unsigned char at position i
    uint8_t readUC(uint16_t i);
    // read signed char at position i
    int8_t readC(uint16_t i);
    // read unsigned int (16bit) starting at position i
    uint16_t readUI(uint16_t i);
    // read signed int (16bit) starting at position i
    int16_t readI(uint16_t i);
    // read unsigned long (32bit) starting at position i
    uint32_t readUL(uint16_t i);
    // read signed long (32bit) starting at position i
    int32_t readL(uint16_t i);
    // read float (32bit) starting at position i
    float readF(uint16_t i);
    // read double (32 or 64bit) starting at position i
    double readD(uint16_t i);
    // read char array (maxLen up to 64 bytes) starting at position i
    void readStr(uint16_t i, char* j, int16_t maxLen);

    // write value j starting at position i
    inline void write(uint16_t i,  uint8_t j) { update (i,j); }
    inline void write(uint16_t i,     char j) { update (i,j); }
    inline void write(uint16_t i,   int8_t j) { update (i,j); }
    inline void write(uint16_t i, uint16_t j) { update (i,j); }
    inline void write(uint16_t i,  int16_t j) { update (i,j); }
    inline void write(uint16_t i, uint32_t j) { update (i,j); }
    inline void write(uint16_t i,  int32_t j) { update (i,j); }
    inline void write(uint16_t i,    float j) { update (i,j); }
    inline void write(uint16_t i,   double j) { update (i,j); }
    inline void write(uint16_t i,    char* j) { update (i,j); }

    // update value j starting at position i
    inline void update(uint16_t i,  uint8_t j) { writeToCache(i, j); }
    inline void update(uint16_t i,     char j) { update(i,(uint8_t)j); }
    inline void update(uint16_t i,   int8_t j) { update(i,(uint8_t)j); }
    inline void update(uint16_t i, uint16_t j) { updateBytes(i, (uint8_t*)&j, sizeof(uint16_t)); }
    inline void update(uint16_t i,  int16_t j) { updateBytes(i, (uint8_t*)&j, sizeof(int16_t)); }
    inline void update(uint16_t i, uint32_t j) { updateBytes(i, (uint8_t*)&j, sizeof(uint32_t)); }
    inline void update(uint16_t i,  int32_t j) { updateBytes(i, (uint8_t*)&j, sizeof(int32_t)); }
    inline void update(uint16_t i,    float j) { updateBytes(i, (uint8_t*)&j, sizeof(float)); }
    inline void update(uint16_t i,   double j) { updateBytes(i, (uint8_t*)&j, sizeof(double)); }
    inline void update(uint16_t i,    char* j) { updateBytes(i, j, strlen(j) + 1); }

    // checks bytes for null (up to 32767) starting at position i into value j
    // for char arrays a negative count represents the maximum length read (if a terminating null is not found)
    bool isNull(uint16_t i, int16_t count);
    // read count bytes (up to 32767) starting at position i into value j
    // for char arrays a negative count represents the maximum length read (if a terminating null is not found)
    void readBytes(uint16_t i, void *j, int16_t count);
    // update count bytes (up to 32767) starting at position i from value j
    // for char arrays a negative count represents the maximum length to write (if a terminating null is not found)
    void updateBytes(uint16_t i, void *j, int16_t count);
    // write count bytes (up to 32767) starting at position i from value j
    // for char arrays a negative count represents the maximum length to write (if a terminating null is not found)
    inline void writeBytes(uint16_t i, void *j, int16_t count) { updateBytes(i, j, count); }

    // NV size in bytes
    uint16_t size = 0;

    bool initError = false;

  protected:
    // returns false if ready to read or write immediately
    virtual bool busy();

    // read byte at position i from storage
    virtual uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    virtual void writeToStorage(uint16_t i, uint8_t j);

    // write value j of count bytes to position starting at i in storage
    // these writes must be aligned with the page size!
    virtual void writePageToStorage(uint16_t i, uint8_t *j, uint8_t count) { writeToStorage(i, *j); (void)(count); }

    // default page write size is 1
    int pageWriteSize = 1;

    bool readAndWriteThrough = false;
    bool readOnlyMode = false;

    uint16_t cacheIndex = -1;
    uint16_t cacheSize = 0;
    uint8_t* cache;
    uint16_t cacheStateSize = 0;
    uint8_t* cacheStateRead;
    uint8_t* cacheStateWrite;
    uint16_t cacheSizeDirtyCount = 0;

    uint32_t waitMs = 0;

    bool keyMatches = false;

    // a quick to access flag to tell us if delayed commit is enabled
    bool delayedCommitEnabled = false;

    // true if the entire cache was clean for the pass 
    bool cacheCleanThisPass = false;
    // if the entire cache is up to date, skip the scanning
    bool cacheClean = false;

    uint32_t commitReadyTimeMs = 0;
};
