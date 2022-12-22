// -----------------------------------------------------------------------------------
// telescope celestial object library
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT)

#include "../../../lib/convert/Convert.h"
#include "../../../libApp/commands/ProcessCmds.h"

#if AXIS1_PEC == ON
  #define NV_LIBRARY_DATA_BASE NV_PEC_BUFFER_BASE + PEC_BUFFER_SIZE_LIMIT
#else
  #define NV_LIBRARY_DATA_BASE NV_PEC_BUFFER_BASE + 0
#endif

#pragma pack(1)
const int rec_size = 16;
typedef struct {
  char name[11]; // 11
  byte code;     // 1 (low 4 bits are object class, high are catalog #)
  uint16_t RA;   // 2
  uint16_t Dec;  // 2
} libRecBase_t;

typedef union {
  libRecBase_t libRec;
  byte libRecBytes[rec_size];
} libRec_t;
#pragma pack()

class Library
{
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // select catalog by number (0 to 14)
    bool setCatalog(int num);

    // write current record from data
    // \param name: object name (to 12 chars)
    // \param code: object classification (0 to 15)
    // \param RA: in radians
    // \param Dec: in radians
    void writeVars(char* name, int code, double RA, double Dec);

    // read data for the current record
    // \param name: object name (to 12 chars)
    // \param code: object classification (0 to 15)
    // \param RA: in radians
    // \param Dec: in radians
    void readVars(char* name, int* code, double* RA, double* Dec);

    // move to catalogs first rec
    bool firstRec();

    // move to the catalog name rec
    bool nameRec();

    // move to the first unused record for this catalog
    bool firstFreeRec();

    // move to the previous record, if it exists
    bool prevRec();

    // move to the next record, if it exists
    bool nextRec();

    // move to the specified record (of this catalog), if it exists
    bool gotoRec(long num);

    // actual number of records for this catalog
    long recCount();

    // actual number of records for this library
    long recCountAll();

    // clears this record
    void clearCurrentRec();

    // clears this library
    void clearLib();

    // clears all libraries    
    void clearAll();

    // number records available for this library
    long recFreeAll();

  private:
    // currently selected record#   
    long recPos;            

    // last record#
    long recMax;            

    // 16 byte record
    libRec_t list;

    libRec_t readRec(long address);
    void writeRec(long address, libRec_t data);
    void clearRec(long address);
    inline double degRange(double d) { while (d >= 360.0) d -= 360.0; while (d < 0.0)  d += 360.0; return d; }

    int catalog;

    long byteMin;
    long byteMax;
};

extern Library library;

#endif
