//
// Created by pc on 02-08-25.
//

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <random>
#include "SynthetizerConfig.h"
class Oscillator {
public:
    Oscillator();

    WaveformType getWaveform() const;
    float getFrequency() const;

    void setWaveForm(WaveformType waveform);
    void setFrequency(float freq);

    void generateBuffer(float* buffer, int numFrames, WaveformType waveform,
                       float frequency, float sampleRate);
    void reset();

private:
    //no offset frequency move because its on audioEngine class
    WaveformType waveform;
    float frequency;
    float phase;
    //Random number generator for noise sound using mersenne twister
    std::mt19937 noise_gen;
    // Float distribution b
    std::uniform_real_distribution<float> noise_dist;


};

#endif //OSCILLATOR_H
