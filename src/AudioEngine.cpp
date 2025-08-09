//
// Created by pc on 07-08-25.
//

#include "../include/AudioEngine.h"


AudioEngine::AudioEngine(SynthetizerConfig* p) : params(p) {}

void AudioEngine::processAudio(float* outputBuffer, int numFrames) {
    // Clear mix buffer
    std::fill(mixBuffer.begin(), mixBuffer.end(), 0.0f);

    float noteFreq = params->note_frequency.load();

    if (params->osc1_enabled.load()) {
        float freq = noteFreq + params->osc1_freq_offset.load();
        oscillators[0].generateBuffer(
            osc1Buffer.data(), numFrames,
            static_cast<WaveformType>(params->osc1_waveform.load()),
            freq, SAMPLE_RATE
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
            freq, SAMPLE_RATE
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
            freq, SAMPLE_RATE
        );
        for (int i = 0; i < numFrames * 2; ++i) {
            mixBuffer[i] += osc3Buffer[i];
        }
    }

    envelope.setAttackTime(params->attack_time.load(), SAMPLE_RATE);
    envelope.setReleaseTime(params->release_time.load(), SAMPLE_RATE);
    envelope.processBuffer(mixBuffer.data(), numFrames);

    filter.processBuffer(
        mixBuffer.data(), numFrames,
        params->filter_cutoff.load(),
        params->filter_auto_amount.load(),
        params->filter_auto_freq.load(),
        params->filter_resonance.load(),
        SAMPLE_RATE
    );

    float volume = params->volume.load();
    for (int i = 0; i < numFrames * 2; ++i) {
        outputBuffer[i] = mixBuffer[i] * volume;
    }
}

void AudioEngine::noteOn(int noteNumber) {
    float baseFreq = 220.0f; // A3
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
