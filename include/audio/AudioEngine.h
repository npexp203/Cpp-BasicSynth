// include/audio/AudioEngine.h
#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <portaudio.h>
#include <memory>

#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"
#include "SynthetizerConfig.h"

class AudioEngine {

public:
    // a verif mettre réf ou quoi
    explicit AudioEngine(std::shared_ptr<SynthetizerConfig> p);
    ~AudioEngine();

    bool initialize();
    bool start();
    void stop();
    void shutdown();

    void processAudio(float* outputBuffer, int numFrames);
    void noteOn(int noteNumber);
    void noteOff();
private:
    // PortAudio
    PaStream* stream;

    std::array<Oscillator, 3> oscillators;
    Envelope envelope;
    Filter filter;
    std::shared_ptr<SynthetizerConfig> params;

    std::array<float, BUFFER_SIZE * 2> osc1Buffer;
    std::array<float, BUFFER_SIZE * 2> osc2Buffer;
    std::array<float, BUFFER_SIZE * 2> osc3Buffer;
    std::array<float, BUFFER_SIZE * 2> mixBuffer;

    static int audioCallback(const void* inputBuffer, void* outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData);

};
#endif //AUDIOENGINE_H