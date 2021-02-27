// -----------------------------------------------------------------------------------
// non-volatile storage base class

#pragma once

#include "Arduino.h"

class NonVolatileStorage {
  public:
    // prepare EEPROM/FLASH/etc. for operation
    virtual bool init(uint16_t size);

    // call frequently to perform any operations that need to happen in the background
    virtual void poll();

    // returns true if all data in any cache has been written
    virtual bool committed();

    // returns true if all data in nv has passed ongoing validation checks
    bool valid();

    // causes write and update to actually write to EEPROM
    // normally both write and update read first and only write if the
    // value has changed (and possibly later after being cached)
    void writeThrough(bool state);

    // reads data from memory
    uint8_t readFromCache(uint16_t i);

    void writeToCache(uint16_t i, uint8_t j);

    // read byte at position i
    uint8_t read(uint16_t i);

    // read position i into value j
    void read(uint16_t i, uint8_t* j);
    void read(uint16_t i, int8_t* j);
    void read(uint16_t i, uint16_t* j);
    void read(uint16_t i, int16_t* j);
    void read(uint16_t i, uint32_t* j);
    void read(uint16_t i, int32_t* j);
    void read(uint16_t i, float* j);
    void read(uint16_t i, double* j);
    void read(uint16_t i, char* j, int16_t maxLen);

    // write position i into value j
    inline void write(uint16_t i, uint8_t j) { update (i,j); }
    inline void write(uint16_t i, int8_t j) { update (i,j); }
    inline void write(uint16_t i, uint16_t j) { update (i,j); }
    inline void write(uint16_t i, int16_t j) { update (i,j); }
    inline void write(uint16_t i, uint32_t j) { update (i,j); }
    inline void write(uint16_t i, int32_t j) { update (i,j); }
    inline void write(uint16_t i, float j) { update (i,j); }
    inline void write(uint16_t i, double j) { update (i,j); }
    inline void write(uint16_t i, char* j) { update (i,j); }

    // update position i into value j
    void update(uint16_t i, uint8_t j);
    void update(uint16_t i, int8_t j);
    void update(uint16_t i, uint16_t j);
    void update(uint16_t i, int16_t j);
    void update(uint16_t i, uint32_t j);
    void update(uint16_t i, int32_t j);
    void update(uint16_t i, float j);
    void update(uint16_t i, double j);
    void update(uint16_t i, char* j);

    // read count bytes (up to 255) starting at position i into value j
    // for char arrays a negative count represents the maximum length read (if a terminating null is not found)
    void readBytes(uint16_t i, uint8_t *j, int16_t count);
    // update count bytes (up to 255) starting at position i from value j
    void updateBytes(uint16_t i, uint8_t *j, int16_t count);

    // NV size in bytes
    uint16_t size = 0;

  protected:
    // returns false if ready to read or write immediately
    virtual bool busy();

    // read byte at position i from storage
    virtual uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    virtual void writeToStorage(uint16_t i, uint8_t j);

    bool writeOnUpdate = false;

    uint16_t cacheIndex = -1;
    uint16_t cacheSize = 0;
    uint8_t* cache;
    uint16_t cacheStateSize = 0;
    uint8_t* cacheStateRead;
    uint8_t* cacheStateWrite;
};
