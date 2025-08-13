#ifndef SYNTH_H
#define SYNTH_H
#pragma once
#include <functional>

#include "../audio/SynthetizerConfig.h"
#include <SDL3/SDL.h>
#include <memory>

class SynthUI {
public:
    SynthUI(std::shared_ptr<SynthetizerConfig> synthParams);
    ~SynthUI();

    void initialize();
    void shutdown();
    bool handleEvents();
    void render();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::shared_ptr<SynthetizerConfig> params;
    int currentOctave;
    bool keyStates[13];
    bool isInitialized = false;


    static const char* noteNames[13];

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