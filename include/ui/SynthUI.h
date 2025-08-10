//
// Created by pc on 02-08-25.
//

#ifndef SYNTH_H
#define SYNTH_H
#pragma once
#include <functional>

#include "../audio/SynthetizerConfig.h"
#include <SDL3/SDL.h>
#include <memory>
#include <imgui_impl_sdlrenderer3.h>
class SynthUI {

public:
    SynthUI(std::shared_ptr<SynthetizerConfig> synthParams);
    ~SynthUI();

    bool initialize();
    void shutdown();
    bool handleEvents();
    void render();
    void setNoteOnCallback(std::function<void(float)> callback);
    void setNoteOffCallback(std::function<void()> callback);

private:
    std::function<void(float)> noteOnCallback;
    std::function<void()> noteOffCallback;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::shared_ptr<SynthetizerConfig> params;

    int currentOctave;
    bool keyStates[13];

    static const float noteFrequencies[13];
    static const char* noteNames[13];
    static const char* keyboardKeys;

    void renderOscillatorControls();
    void renderEnvelopeControls();
    void renderFilterControls();
    void renderVolumeControl();
    void renderOctaveControl();
    void renderVirtualKeyboard();

    float noteToFrequency(int note, int octave);
    void handleKeyboard(const SDL_Event& event);

};

#endif //SYNTH_H
