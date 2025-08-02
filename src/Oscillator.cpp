//
// Created by pc on 02-08-25.
//

#include "../include/Oscillator.h"
#include <cmath>


Oscillator::Oscillator(WaveForm form, float freq): frequency(freq), waveform(form) {
}
void Oscillator::setFrequencyOffset(float offset) {
    if (offset < -5.0f) offset = -5.0f;
    if (offset > 5.0f)  offset = 5.0f;

    frequencyOffset = offset;
}



float Oscillator::getSample(float time) {
float phase = 2.0f * M_PI * (frequency + frequencyOffset) * time;
    float sample = 0.0f;

    switch (waveform) {
        case WaveForm::SINE:
            sample = sin(phase);
            break;

        case WaveForm::SQUARE:
            sample = sin(phase) > 0 ? 1.0f : -1.0f;
            break;

        case WaveForm::TRIANGLE: {
            float t_norm = fmod(phase / (2.0f * M_PI), 1.0f);
            sample = 4.0f * fabs(t_norm - 0.5f) - 1.0f;
            break;
        }

        case WaveForm::SAWTOOTH: {
            float t_norm = fmod(phase / (2.0f * M_PI), 1.0f);
            sample = 2.0f * t_norm - 1.0f;
            break;
        }
        default:
            sample = 0.0f;
    }

}


