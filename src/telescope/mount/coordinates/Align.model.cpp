//--------------------------------------------------------------------------------------------------
// Shared alignment coefficient parsing and validation.

#include "Align.ref.h"
#include "Align.hs.h"

#if defined(MOUNT_PRESENT) && ALIGN_MAX_NUM_STARS > 1

#include <errno.h>
#include <stdlib.h>
#include "Align.bounds.h"

CommandError GeoAlign::setCoefficient(float &coefficient, const char *text, float limit, bool degrees) {
  char *end;
  errno = 0;
  const long value = strtol(text, &end, 10);
  if (end == text || *end != 0) return CE_PARAM_FORM;
  if (errno == ERANGE) return CE_PARAM_RANGE;
  const float radians = degrees ? degToRad(value) : arcsecToRad(value);
  if (!AlignBounds::valid(radians, limit)) return CE_PARAM_RANGE;
  coefficient = radians;
  return CE_NONE;
}

#endif
