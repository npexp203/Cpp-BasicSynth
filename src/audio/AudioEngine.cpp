//
// Created by pc on 07-08-25.
//

#include "../../include/audio/AudioEngine.h"

#include <iostream>


AudioEngine::AudioEngine(std::shared_ptr<SynthetizerConfig> p ) : stream(nullptr),params(p) {}
AudioEngine::~AudioEngine() {
    shutdown();
}

void AudioEngine::initialize() {
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error in Pa_Initialize(): " << Pa_GetErrorText(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    Pa_OpenDefaultStream(&stream,
                     0,
                     2,
                     paFloat32,
                     SAMPLE_RATE,
                     FRAMES_PER_BUFFER,
                     audioCallback,
                     this);

    if (err != paNoError) {
        std::cerr << "PortAudio open failed: " << Pa_GetErrorText(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Start stream immediately
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio start failed: " << Pa_GetErrorText(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Audio engine initialized successfully!" << std::endl;
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

    // Clear the stereo output buffer (L+R channels, so numFrames * 2 samples)
    std::fill_n(outputBuffer, numFrames * 2, 0.0f);
    // Clear the internal mixing buffer
    std::fill(mixBuffer.begin(), mixBuffer.end(), 0.0f);

    static int lastNoteNumber = -1;
    int currentNote = params->noteNumber.load();

    // If the note number changed since the last callback
    if (currentNote != lastNoteNumber) {
        if (currentNote >= 0) {
            noteOn(currentNote);
        } else {
            noteOff();
        }
        // Remember this note for next time
        lastNoteNumber = currentNote;
    }
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
