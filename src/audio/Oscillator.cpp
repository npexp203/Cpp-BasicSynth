//
// Created by pc on 02-08-25.
//

#include "../../include/audio/Oscillator.h"


Oscillator::Oscillator():
                            frequency(440.0f),
                            waveform(WaveformType::TRIANGLE),
                            phase(0.0f),
                            noise_gen(std::random_device{}()),
                            noise_dist(-0.5f,0.5f) {}

float Oscillator::getFrequency() const {
    return frequency;
}



WaveformType Oscillator::getWaveform() const {
    return waveform;
}
void Oscillator::setWaveForm(WaveformType waveform) {
    this->waveform = waveform;
}
void Oscillator::setFrequency(float freq) {
    frequency = freq;
}

void Oscillator::reset() {
    phase = 0.0f;
}


void Oscillator::generateBuffer(float* buffer, int numFrames, WaveformType waveform,
                       float frequency, float sampleRate) {
    float phaseIncrement = frequency / sampleRate;

    for (int i = 0; i < numFrames * 2; i += 2) {
        float sample = 0.0f;

        switch (waveform) {

            case WaveformType::TRIANGLE:
                sample = phase < 0.5f
                    ? 4.0f * phase - 1.0f
                    : 3.0f - 4.0f * phase;
                sample *= 0.5f;
                break;

            case WaveformType::SAW:
                sample = (2.0f * phase - 1.0f) * 0.5f;
                break;

            case WaveformType::NOISE:
                sample = noise_dist(noise_gen);
                break;
        }

        buffer[i] = sample;       // Left always even
        buffer[i + 1] = sample;   // Right always odd

        phase += phaseIncrement;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
    }
}


