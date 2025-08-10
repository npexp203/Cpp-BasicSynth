//
// Created by pc on 07-08-25.
//

#include "../../include/audio/AudioEngine.h"

#include <iostream>


AudioEngine::AudioEngine(std::shared_ptr<SynthetizerConfig> p ) : stream(nullptr),params(p) {}
AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        std::cerr << "No default output device" << std::endl;
        return false;
    }

    outputParameters.channelCount = CHANNELS;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
        &stream,
        nullptr,
        &outputParameters,
        SAMPLE_RATE,
        BUFFER_SIZE,
        paClipOff,
        audioCallback,
        this);

    if (err != paNoError) {
        std::cerr << "PortAudio open failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    return true;
}

bool AudioEngine::start() {
    if (!stream) return false;

    PaError err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio start failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    return true;
}

void AudioEngine::stop() {
    if (stream) {
        Pa_StopStream(stream);
    }
}

void AudioEngine::shutdown() {
    if (stream) {
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    Pa_Terminate();
}

int AudioEngine::audioCallback(const void* inputBuffer, void* outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void* userData) {
    AudioEngine* engine = static_cast<AudioEngine*>(userData);
    float* output = static_cast<float*>(outputBuffer);

    engine->processAudio(output, framesPerBuffer);
    return paContinue;
}

void AudioEngine::processAudio(float* outputBuffer, int numFrames) {

    float noteFreq = params->note_frequency.load();


    if (params->osc1_enabled.load()) {
        float freq = noteFreq + params->osc1_freq_offset.load();
        oscillators[0].generateBuffer(
            osc1Buffer.data(), numFrames,
            static_cast<WaveformType>(params->osc1_waveform.load()),
            freq
        );
        for (int i = 0; i < numFrames * 2; ++i) {
            mixBuffer[i] += osc1Buffer[i];
        }
    }

    if (params->osc2_enabled.load()) {
        float freq = noteFreq + params->osc2_freq_offset.load();
        oscillators[1].generateBuffer(
            osc2Buffer.data(), numFrames,
            static_cast<WaveformType>(params->osc2_waveform.load()),
            freq
        );
        for (int i = 0; i < numFrames * 2; ++i) {
            mixBuffer[i] += osc2Buffer[i];
        }
    }

    if (params->osc3_enabled.load()) {
        float freq = noteFreq + params->osc3_freq_offset.load();
        oscillators[2].generateBuffer(
            osc3Buffer.data(), numFrames,
            static_cast<WaveformType>(params->osc3_waveform.load()),
            freq
        );
        for (int i = 0; i < numFrames * 2; ++i) {
            mixBuffer[i] += osc3Buffer[i];
        }
    }

    envelope.setAttackTime(params->attack_time.load());
    envelope.setReleaseTime(params->release_time.load());
    envelope.processBuffer(mixBuffer.data(), numFrames);

    filter.processBuffer(
        mixBuffer.data(), numFrames,
        params->filter_cutoff.load(),
        params->filter_auto_amount.load(),
        params->filter_auto_freq.load(),
        params->filter_resonance.load()
    );

    float volume = params->volume.load();
    for (int i = 0; i < numFrames * 2; ++i) {
        outputBuffer[i] = mixBuffer[i] * volume;
    }
}

void AudioEngine::noteOn(int noteNumber) {
    float baseFreq = 220.0f;
    int octave = params->octave.load();
    float frequency = baseFreq * std::pow(2.0f, (octave + noteNumber / 12.0f));

    params->note_frequency.store(frequency);
    params->note_on.store(true);

    envelope.noteOn();
}

void AudioEngine::noteOff() {
    params->note_on.store(false);
    envelope.noteOff();
}
