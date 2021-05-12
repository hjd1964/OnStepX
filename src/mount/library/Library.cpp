// -----------------------------------------------------------------------------------
// telescope celestial object library

#include "../../OnStepX.h"
#include "Library.h"

Library library;
char const * objectStr[] = {"UNK", "OC", "GC", "PN", "DN", "SG", "EG", "IG", "KNT", "SNR", "GAL", "CN", "STR", "PLA", "CMT", "AST"};

Library::Library()
{
  catalog = 0;

  byteMin = NV_PEC_BUFFER_BASE + PEC_BUFFER_SIZE_LIMIT;
  byteMax = nv.size - 1;

  long byteCount = (byteMax - byteMin) + 1;
  if (byteCount < 0) byteCount = 0;
  if (byteCount > 262143) byteCount = 262143; // maximum 256KB

  recMax = byteCount/rec_size; // maximum number of records
}

Library::~Library()
{
}

void Library::init() {
  // This is now in the Init() function, because on boards
  // with an I2C EEPROM nv.init() has to be called before
  // anything else
  firstRec();
}

bool Library::setCatalog(int16_t num)
{
  if (num < 0 || num > 14) return false;

  catalog = num;
  return firstRec();
}

void Library::writeVars(char* name, int16_t code, double RA, double Dec)
{
  libRec_t work;
  for (int16_t l = 0; l < 11; l++) work.libRec.name[l] = name[l];
  work.libRec.code = (code | (catalog << 4));

  // convert into ulong, RA=0..360
  RA = degRange(RA)/360.0;
  // convert into ulong, Dec=0..180
  if (Dec > 90.0) Dec = 90.0; 
  if (Dec < -90.0) Dec = -90.0;
  Dec = Dec + 90.0; Dec = Dec/180.0;
  uint16_t r = round(RA*65536.0);
  uint16_t d = round(Dec*65536.0);
  
  work.libRec.RA  = r;
  work.libRec.Dec = d;

  writeRec(recPos, work);
}

void Library::readVars(char* name, int16_t* code, double* RA, double* Dec)
{
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
  
  // convert from ulong
  *RA = (double)r;
  *RA = (*RA/65536.0)*360.0;
  *Dec = (double)d;
  *Dec = ((*Dec/65536.0)*180.0)-90.0;
}

libRec_t Library::readRec(long address)
{
  libRec_t work;
  long l = address*rec_size + byteMin;
  nv.readBytes(l, (uint8_t*)&work.libRecBytes, 16);
  return work;
}

void Library::writeRec(long address, libRec_t data)
{
  if (address >= 0 && address < recMax) {
    long l = address*rec_size + byteMin;
    for (int m = 0; m < 16; m++) nv.write(l+m, data.libRecBytes[m]);
  }
}

void Library::clearRec(long address)
{
  if (address >= 0 && address < recMax) {
    long l = address*rec_size+byteMin;
    int code = 15 << 4;
    nv.write(l + 11, (byte)code); // catalog code 15 = deleted
  }
}

bool Library::firstRec()
{
  libRec_t work;

  // see if first record is for the currentLib
  recPos = 0;
  work = readRec(recPos);
  int16_t cat = (int16_t)work.libRec.code >> 4;
  if (work.libRec.name[0] != '$' && cat == catalog) return true;

  // otherwise find the first one, if it exists
  return nextRec();
}


bool Library::nameRec()
{
  libRec_t work;

  int16_t cat;
  recPos = -1;
  
  do
  {
    recPos++; if (recPos >= recMax) break;
    work = readRec(recPos);

    cat = (int16_t)work.libRec.code >> 4;

    if (work.libRec.name[0] == '$' && cat == catalog) break;
  } while (recPos < recMax);
  if (recPos >= recMax) { recPos = recMax - 1; return false; }

  return true;
}

bool Library::firstFreeRec()
{
  libRec_t work;

  int16_t cat;
  recPos = -1;
  
  do
  {
    recPos++; if (recPos >= recMax) break;
    work = readRec(recPos);

    cat = (int16_t)work.libRec.code>>4;
  
    if (cat == 15) break; // unused?
  } while (recPos < recMax);
  if (recPos >= recMax) { recPos = recMax - 1; return false; }

  return true;
}

bool Library::prevRec()
{
  libRec_t work;

  int16_t cat;
  
  do
  {
    recPos--; if (recPos < 0) break;
    work = readRec(recPos);

    cat = (int16_t)work.libRec.code >> 4;
    if (work.libRec.name[0] != '$' && cat == catalog) break;
  } while (recPos >= 0);
  if (recPos < 0) { recPos = 0; return false; }

  return true;
}

bool Library::nextRec()
{
  libRec_t work;

  int16_t cat;
 
  do
  {
    recPos++; if (recPos >= recMax) break;
    work=readRec(recPos);

    cat = (int16_t)work.libRec.code >> 4;
    if (work.libRec.name[0] != '$' && cat == catalog) break;
  } while (recPos < recMax);
  if (recPos >= recMax) { recPos = recMax-1; return false; }

  return true;
}

bool Library::gotoRec(long num)
{
  libRec_t work;

  int16_t cat;
  long r = 0;
  long c = 0;
  
  for (long l = 0; l < recMax; l++) {
    work=readRec(l); r = l;

    cat = (int16_t)work.libRec.code >> 4;
    if (work.libRec.name[0] != '$' && cat == catalog) c++;
    if (c == num) break;
  }
  if (c == num) { recPos = r; return true; } else return false;
}

long Library::recCount()
{
  libRec_t work;

  int16_t cat;
  long c = 0;
  
  for (long l = 0; l < recMax; l++) {
    work = readRec(l);

    cat = (int16_t)work.libRec.code >> 4;
    if (work.libRec.name[0] != '$' && cat == catalog) c++;
  }
  
  return c;
}

long Library::recCountAll()
{
  libRec_t work;

  int16_t cat;
  long c = 0;
  
  for (long l = 0; l < recMax; l++) {
    work = readRec(l);

    cat = (int16_t)work.libRec.code >> 4;
    if (cat >= 0 && cat <= 14) c++;
  }
  
  return c;
}


long Library::recFreeAll()
{
  return recMax - recCountAll();
}

void Library::clearCurrentRec()
{
  libRec_t work;

  int16_t cat;

  work = readRec(recPos);

  cat = (int16_t)work.libRec.code >> 4;
  if (cat == catalog) clearRec(recPos);
}

void Library::clearLib()
{
  libRec_t work;

  int16_t cat;

  for (long l = 0; l < recMax; l++) {
    work = readRec(l);

    cat = (int16_t)work.libRec.code >> 4;
    if (cat == catalog) clearRec(l);
  }
}

void Library::clearAll()
{
  for (long l = 0; l < recMax; l++) clearRec(l);
}
