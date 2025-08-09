//
// Created by pc on 07-08-25.
//

#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#pragma once

#include <array>
#include <algorithm>
#include <cmath>

#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"
#include "SynthetizerConfig.h" // ex-SynthParams

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100
#endif

class AudioEngine {
private:
    std::array<Oscillator, 3> oscillators;
    Envelope envelope;
    Filter filter;
    SynthetizerConfig* params;

    std::array<float, BUFFER_SIZE * 2> osc1Buffer;
    std::array<float, BUFFER_SIZE * 2> osc2Buffer;
    std::array<float, BUFFER_SIZE * 2> osc3Buffer;
    std::array<float, BUFFER_SIZE * 2> mixBuffer;

public:
    explicit AudioEngine(SynthetizerConfig* p);

    void processAudio(float* outputBuffer, int numFrames);
    void noteOn(int noteNumber);
    void noteOff();
};

#endif //AUDIOENGINE_H
