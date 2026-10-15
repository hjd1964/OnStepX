//--------------------------------------------------------------------------------------------------
// Shared alignment coefficient bounds for uploads and saved model validation.
#pragma once

#include <math.h>
#include "../../../lib/Constants.h"
#include "../../../lib/Macros.h"

namespace AlignBounds {
// Power-of-two bounds covering the full cumulative search ranges.
constexpr float polar = (float)arcsecToRad(32768.0);
constexpr float cone = (float)arcsecToRad(16384.0);
constexpr float ortho = (float)arcsecToRad(512.0);
constexpr float axisFlex = ortho;
constexpr float tubeFlex = (float)arcsecToRad(256.0);

// Index offsets, periodic phases, and unfitted harmonic amplitudes retain their bounds.
constexpr float index = (float)Deg360;
constexpr float phase = index;
constexpr float harmonic = (float)arcsecToRad(16384.0);

inline bool valid(float value, float limit) {
  return isfinite(value) && fabsf(value) <= limit;
}
}
