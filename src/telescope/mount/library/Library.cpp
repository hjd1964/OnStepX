// -----------------------------------------------------------------------------------
// telescope celestial object library

#include "Library.h"

#if defined(MOUNT_PRESENT)

#include "../../../lib/nv/Nv.h"

#include "../../Telescope.h"

char const * objectStr[] = {"UNK", "OC", "GC", "PN", "DN", "SG", "EG", "IG", "KNT", "SNR", "GAL", "CN", "STR", "PLA", "CMT", "AST"};

bool Library::init() {
  if (ready) return true;

  catalog = 0;
  NvVolume& nvVolume = nv().volume();
  if (!nvVolume.isMounted()) {
    DLF("WRN: Library::init(); NV volume not mounted");
    return false;
  }

  // Bind IvPartition to the LIBRARY partition by name.
  if (nvIv.init(nvVolume, "LIBRARY")) {
    VLF("MSG: Nv, partition 'LIBRARY' mounted");
  } else {
    DLF("WRN: Library::init(); can't find library partition!");
    return false;
  }

  recMax = nvIv.sizeBytes()/rec_size; // maximum number of records

  if (recMax == 0) {
    DLF("WRN: Library::init(); recMax == 0, no library space available");
    return false;
  }

  VF("MSG: Mount, library allocated "); V(recMax); VLF(" catalog records");

  ready = true;

  firstRec();

  return true;
}

// select catalog by number (0 to 14)
bool Library::setCatalog(int num) {
  if (!ready) return false;
  if (num < 0 || num > 14) return false;

  catalog = num;
  return firstRec();
}

// write current record from data
// \param name: object name (to 12 chars)
// \param code: object classification (0 to 15)
// \param RA: in radians
// \param Dec: in radians
void Library::writeVars(char* name, int code, double RA, double Dec) {
  if (!ready) return;

  libRec_t work;

  for (int16_t l = 0; l < 11; l++) work.libRec.name[l] = name[l];
  work.libRec.code = (code | (catalog << 4));

  // convert from radians to degrees
  RA = radToDeg(RA);
  Dec = radToDeg(Dec);

  // convert into an unsigned int, RA=0..360
  RA = degRange(RA)/360.0;
  // convert into an unsigned int, Dec=0..180
  if (Dec > 90.0) Dec = 90.0; 
  if (Dec < -90.0) Dec = -90.0;
  Dec = Dec + 90.0; Dec = Dec/180.0;
  uint16_t r = round(RA*65536.0);
  uint16_t d = round(Dec*65536.0);
  
  work.libRec.RA  = r;
  work.libRec.Dec = d;

  writeRec(recPos, work);
}

// read data for the current record
// \param name: object name (to 12 chars)
// \param code: object classification (0 to 15)
// \param RA: in radians
// \param Dec: in radians
void Library::readVars(char* name, int* code, double* RA, double* Dec) {
  if (!ready) { name[0] = 0; *code = 0; *RA = 0.0; *Dec = 0.0; return; }

  libRec_t work;
  work = readRec(recPos);

  int16_t cat = work.libRec.code >> 4;

  // empty? or not found
  if (cat == 15 || cat != catalog) { name[0] = 0; *code = 0; *RA = 0.0; *Dec = 0.0; return; }

  for (int16_t l = 0; l < 11; l++) name[l] = work.libRec.name[l];
  name[11] = 0;
  
  *code = work.libRec.code & 15;
  uint16_t r = work.libRec.RA;
  uint16_t d = work.libRec.Dec;
  
  // convert from unsigned int
  *RA = (double)r;
  *RA = (*RA/65536.0)*360.0;
  *Dec = (double)d;
  *Dec = ((*Dec/65536.0)*180.0)-90.0;

  // convert from degrees to radians
  *RA = degToRad(*RA);
  *Dec = degToRad(*Dec);
}

// move to catalogs first rec
bool Library::firstRec() {
  if (!ready) return false;

  libRec_t work;

  // see if first record is for the currentLib
  recPos = 0;
  work = readRec(recPos);
  int16_t cat = (int16_t)work.libRec.code >> 4;
  if (work.libRec.name[0] != '$' && cat == catalog) return true;

  // otherwise find the first one, if it exists
  return nextRec();
}

// move to the catalog name rec
bool Library::nameRec() {
  if (!ready) return false;

  for (recPos = 0; recPos < recMax; ++recPos) {
    const libRec_t work = readRec(recPos);
    const int16_t cat = (int16_t)(work.libRec.code >> 4);

    if (work.libRec.name[0] == '$' && cat == catalog) return true;
  }

  // not found, point at the last record
  recPos = (uint16_t)(recMax - 1);
  return false;
}

// move to the first unused record for this catalog
bool Library::firstFreeRec() {
  if (!ready) return false;

  for (recPos = 0; recPos < recMax; ++recPos) {
    const libRec_t work = readRec(recPos);
    const int16_t cat = (int16_t)(work.libRec.code >> 4);

    if (cat == 15) return true;  // unused slot
  }

  // not found, point at the last record
  recPos = (uint16_t)(recMax - 1);
  return false;
}

bool Library::prevRec() {
  if (!ready) return false;

  while (true) {
    if (recPos == 0) return false;   // no earlier record

    recPos--;
    libRec_t work = readRec(recPos);
    int16_t cat = (int16_t)(work.libRec.code >> 4);

    if (work.libRec.name[0] != '$' && cat == catalog) return true;
  }
}

// move to the next record, if it exists
bool Library::nextRec() {
  if (!ready) return false;

  while (true) {
    if (recPos >= (uint16_t)(recMax - 1)) return false;

    ++recPos;

    libRec_t work = readRec(recPos);
    int16_t cat = (int16_t)(work.libRec.code >> 4);

    if (work.libRec.name[0] != '$' && cat == catalog) return true;
  }
}

// move to the specified record (of this catalog), if it exists
bool Library::gotoRec(uint16_t num) {
  if (!ready) return false;

  libRec_t work;
  int16_t cat;
  uint16_t lastAddress = 0;
  uint16_t count = 0;
  
  for (uint16_t address = 0; address < recMax; address++) {
    work = readRec(address);
    lastAddress = address;

    cat = (int16_t)work.libRec.code >> 4;
    if (work.libRec.name[0] != '$' && cat == catalog) count++;
    if (count == num) break;
  }
  if (count == num) { recPos = lastAddress; return true; } else return false;
}

// actual number of records for this catalog
uint16_t Library::recCount() {
  if (!ready) return 0;

  libRec_t work;
  int16_t cat;
  uint16_t count = 0;
  
  for (uint16_t address = 0; address < recMax; address++) {
    work = readRec(address);

    cat = (int16_t)work.libRec.code >> 4;
    if (work.libRec.name[0] != '$' && cat == catalog) count++;
  }
  
  return count;
}

// actual number of records for this library
uint16_t Library::recCountAll() {
  if (!ready) return 0;

  libRec_t work;
  int16_t cat;
  uint16_t c = 0;
  
  for (uint16_t address = 0; address < recMax; address++) {
    work = readRec(address);

    cat = (int16_t)work.libRec.code >> 4;
    if (cat >= 0 && cat <= 14) c++;
  }
  
  return c;
}

// clears this record
void Library::clearCurrentRec() {
  if (!ready) return;

  libRec_t work;
  int16_t cat;

  work = readRec(recPos);

  cat = (int16_t)work.libRec.code >> 4;
  if (cat == catalog) clearRec(recPos);
}

// clears this library
void Library::clearLib() {
  if (!ready) return;

  libRec_t work;
  int16_t cat;

  for (uint16_t address = 0; address < recMax; address++) {
    work = readRec(address);

    cat = (int16_t)work.libRec.code >> 4;
    if (cat == catalog) clearRec(address);
  }
}

// clear library (clear all catalogs)
void Library::clearAll() {
  if (!ready) return;
  for (uint16_t l = 0; l < recMax; l++) clearRec(l);
}

// number records available for this library
uint16_t Library::recFreeAll() {
  if (!ready) return 0;
  return recMax - recCountAll();
}

libRec_t Library::readRec(uint16_t address) {
  libRec_t work{};
  if (!ready) return work;
  if (address >= recMax) return work;

  nvIv.readBlock(address, work.libRecBytes);
  return work;
}

void Library::writeRec(uint16_t address, libRec_t data) {
  if (!ready) return;
  if (address >= recMax) return;

  nvIv.writeBlock(address, data.libRecBytes);
}

void Library::clearRec(uint16_t address) {
  if (!ready) return;
  if (address >= recMax) return;

  uint16_t index = address*rec_size;
  uint8_t code = 0xF0;
  nvIv.writeBytes(index + 11, &code, 1); // catalog code 15 = unused
}

Library library;

#endif