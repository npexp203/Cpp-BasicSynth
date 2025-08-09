//
// Created by pc on 02-08-25.
//

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <random>
enum class WaveformType{TRIANGLE,SAW,NOISE};

class Oscillator {
public:
    Oscillator();

    WaveformType getWaveform() const;
    float getFrequency() const;
    float getSampleRate() const ;

    void setSampleRate(float sr);
    void setWaveForm(WaveformType waveform);
    void setFrequency(float freq);

    void generateBuffer(float* buffer, int numFrames, WaveformType waveform,
                       float frequency, float sampleRate);




private:
    //no offset frequency move because its on audioEngine class
    WaveformType waveform;
    float frequency;
    float phase;
    float sampleRate;
    //Random number generator for noise sound using mersenne twister
    std::mt19937 noise_gen;
    // Float distribution b
    std::uniform_real_distribution<float> noise_dist;


};

#endif //OSCILLATOR_H
