// -----------------------------------------------------------------------------
// Overflow-safe Sigma–Delta PWM Dithering
// Enable with:   #define SERVO_SIGMA_DELTA_DITHERING ON
//
// Purpose:
//   When PWM resolution is low (e.g., sidereal ≈ a few counts), this modulator
//   time-averages between adjacent integer counts so the long-term average
//   equals a floating target (e.g., 4.4 → mix of 4 and 5). This code never
//   overflows because it only carries the fractional residual in [0,1).
// -----------------------------------------------------------------------------

#pragma once

#ifdef SERVO_SIGMA_DELTA_DITHERING

// Lightweight state holder for first-order sigma–delta dithering
// We store only the fractional residual between ticks
struct SigmaDeltaDither {
    // Fractional residue in [0,1). Carries the part we couldn't emit this tick
    float resid = 0.0f;

    // Reset internal state (call when you reinitialize control or modes change)
    inline void reset() { resid = 0.0f; }

    // convert a floating desired count into an integer count for this tick,
    // such that the time-average of outputs equals the floating target
    inline int32_t dither_counts(float desiredCounts,
                                    int32_t minCount,
                                    int32_t maxCount)
    {
        // Safety clamp the desired average to valid bounds.
        if (desiredCounts < (float)minCount) desiredCounts = (float)minCount;
        if (desiredCounts > (float)maxCount) desiredCounts = (float)maxCount;

        // Add residual: carry fractional error forward from previous tick
        // e.g. 4.4 + 0.7 = 5.1 → emit 5 now, resid becomes 0.1
        float sum = desiredCounts + resid;

        // Emit the integer part of this tick
        int32_t out = (int32_t)floorf(sum);

        // Keep the fractional residue for next tick (always in [0,1))
        resid = sum - (float)out;

        // safety clamp (probably not needed, but just in case)
        if (out < minCount) out = minCount;
        if (out > maxCount) out = maxCount;

        return out; // Integer PWM counts to write this tick
    }

};

#endif
