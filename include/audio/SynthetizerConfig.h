//
// Created by pc on 07-08-25.
//

#ifndef SYNTHETIZERCONFIG_H
#define SYNTHETIZERCONFIG_H
#pragma once

#include <atomic>

enum class WaveformType{TRIANGLE,SAW,NOISE};
constexpr int SAMPLE_RATE = 44100;
constexpr int FRAMES_PER_BUFFER = 256;



// All defaults parameters are atomic for real-time thread-safe access from audio thread
struct SynthetizerConfig {
    // Oscillator enabled
    std::atomic<bool> osc1_enabled{false};
    std::atomic<bool> osc2_enabled{false};
    std::atomic<bool> osc3_enabled{false};

    //(0= Triangle 1= Saw 2= Noise)
    std::atomic<int> osc1_waveform{0};
    std::atomic<int> osc2_waveform{1};
    std::atomic<int> osc3_waveform{2};

    // Oscillators offsets (demi-tons)
    std::atomic<float> osc1_freq_offset{0.0f};
    std::atomic<float> osc2_freq_offset{-2.0f};
    std::atomic<float> osc3_freq_offset{3.0f};

    // in seconds
    std::atomic<float> attack_time{0.1f};
    std::atomic<float> release_time{0.5f};

    // LFO filter parameters
    std::atomic<float> filter_cutoff{10000.0f};
    std::atomic<float> filter_resonance{0.0f};
    std::atomic<float> filter_auto_amount{0.0f};
    std::atomic<float> filter_auto_freq{5.0f};

    std::atomic<float> volume{0.5f};
    std::atomic<int> octave{0};

    std::atomic<int> noteNumber { -1};
    std::atomic<bool> note_on{false};
    std::atomic<float> note_frequency{440.0f};
};


#endif