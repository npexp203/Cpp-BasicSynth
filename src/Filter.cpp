//
// Created by pc on 02-08-25.
//

#include "../include/Filter.h"

#include <algorithm>
#include <math.h>


// Recalculates filter coefficients when the cutoff frequency changes
// This is needed to adapt the filter behavior to the new frequency
void Filter::updateCoefficients(float cutoff, float resonance, float sampleRate) {
    float q = 0.5f / (1.0f - std::clamp(resonance, 0.0f, 0.99f));
    float omega = 2.0f * M_PI * cutoff / sampleRate;
    float alpha = std::sin(omega) / (2.0f * q);
    float cosw = std::cos(omega);
    float norm = 1.0f / (1.0f + alpha);

    a0 = (1.0f - cosw) * 0.5f * norm;
    a1 = (1.0f - cosw) * norm;
    a2 = (1.0f - cosw) * 0.5f * norm;
    b1 = -2.0f * cosw * norm;
    b2 = (1.0f - alpha) * norm;
}


// Applies the filter to a stereo audio buffer with optional LFO modulation
void Filter::processBuffer(float* buffer, int numFrames, float baseCutoff,
                           float autoAmount, float autoFreq, float resonance, float sampleRate) {
    float lfoIncrement = autoFreq / sampleRate;

    static float lastCutoff = -1.0f;

    for (int i = 0; i < numFrames * 2; i += 2) {
        // Calculate the LFO modulation
        float lfoValue = std::sin(2.0f * M_PI * lfoPhase);
        float modulation = lfoValue * autoAmount * 5000.0f;
        float currentCutoff = std::clamp(baseCutoff + modulation, 20.0f, 20000.0f);

        // Only update coefficients if the cutoff has changed by more than 100 Hz
        // This avoids unnecessary recalculations and keeps processing smooth
        if (std::abs(currentCutoff - lastCutoff) > 100.0f) {
            updateCoefficients(currentCutoff, resonance, sampleRate);
            lastCutoff = currentCutoff;
        }

        // Apply filter to left channel and update previous input/output history
        float inputL = buffer[i];
        float outputL = a0 * inputL + a1 * x1_L + a2 * x2_L - b1 * y1_L - b2 * y2_L;
        x2_L = x1_L;
        x1_L = inputL;
        y2_L = y1_L;
        y1_L = outputL;
        buffer[i] = outputL;

        // Apply filter to right channel and update previous input/output history
        float inputR = buffer[i + 1];
        float outputR = a0 * inputR + a1 * x1_R + a2 * x2_R - b1 * y1_R - b2 * y2_R;
        x2_R = x1_R;
        x1_R = inputR;
        y2_R = y1_R;
        y1_R = outputR;
        buffer[i + 1] = outputR;

        lfoPhase += lfoIncrement;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;
    }
}
