#include "../../include/ui/SynthUI.h"

#include <cmath>
#include <imgui_impl_sdl3.h>
#include <iostream>
#include "imgui_impl_sdlrenderer3.h"

const float SynthUI::noteFrequencies[13] = {
    261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f,
    392.00f, 415.30f, 440.00f, 466.16f, 493.88f, 523.25f
};

const char* SynthUI::noteNames[13] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"
};

const char* SynthUI::keyboardKeys = "sedrfgyhuijkl";

SynthUI::SynthUI(std::shared_ptr<SynthetizerConfig> synthParams)
    : window(nullptr), renderer(nullptr), params(synthParams), currentOctave(0) {
    std::fill(keyStates, keyStates + 13, false);
}

SynthUI::~SynthUI() {
    shutdown();
}

bool SynthUI::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Synth", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    isInitialized = true;
    return true;
}

void SynthUI::render() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Synthétiseur 4DEV4D");

    renderOscillatorControls();
    renderEnvelopeControls();
    renderFilterControls();
    renderVolumeControl();
    renderOctaveControl();
    renderVirtualKeyboard();

    ImGui::End();

    // Rendering avec SDL Renderer
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void SynthUI::shutdown() {
    if (isInitialized) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        isInitialized = false;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

bool SynthUI::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }

        handleKeyboard(event);
    }
    return true;
}

void SynthUI::handleKeyboard(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool isDown = (event.type == SDL_EVENT_KEY_DOWN);

        // Find which key was pressed
        for (int i = 0; i < 13; ++i) {
            if (event.key.key == keyboardKeys[i]) {
                bool wasDown = keyStates[i];
                keyStates[i] = isDown;

                if (isDown && !wasDown) {
                    if (noteOnCallback) noteOnCallback(i);
                } else if (!isDown && wasDown) {
                    if (noteOffCallback) noteOffCallback();
                }
                break;
            }
        }
    }
}

float SynthUI::noteToFrequency(int note, int octave) {
    return 220.0f * std::pow(2.0f, (octave + note) / 12.0f);
}

void SynthUI::renderOscillatorControls() {
    if (ImGui::CollapsingHeader("Oscillators", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Oscillator 1
        bool osc1_enabled = params->osc1_enabled.load();
        if (ImGui::Checkbox("Oscillator 1", &osc1_enabled)) {
            params->osc1_enabled = osc1_enabled;
        }

        int osc1_wave = params->osc1_waveform.load();
        const char* waveforms[] = {"TRIANGLE", "SAW", "NOISE"};
        if (ImGui::Combo("OSC1 Waveform", &osc1_wave, waveforms, 3)) {
            params->osc1_waveform = osc1_wave;
        }

        float osc1_offset = params->osc1_freq_offset.load();
        if (ImGui::SliderFloat("OSC1 Frequency Offset", &osc1_offset, -5.0f, 5.0f)) {
            params->osc1_freq_offset = osc1_offset;
        }

        // Oscillator 2
        bool osc2_enabled = params->osc2_enabled.load();
        if (ImGui::Checkbox("Oscillator 2", &osc2_enabled)) {
            params->osc2_enabled = osc2_enabled;
        }

        int osc2_wave = params->osc2_waveform.load();
        if (ImGui::Combo("OSC2 Waveform", &osc2_wave, waveforms, 3)) {
            params->osc2_waveform = osc2_wave;
        }

        float osc2_offset = params->osc2_freq_offset.load();
        if (ImGui::SliderFloat("OSC2 Frequency Offset", &osc2_offset, -5.0f, 5.0f)) {
            params->osc2_freq_offset = osc2_offset;
        }

        // Oscillator 3
        bool osc3_enabled = params->osc3_enabled.load();
        if (ImGui::Checkbox("Oscillator 3", &osc3_enabled)) {
            params->osc3_enabled = osc3_enabled;
        }

        int osc3_wave = params->osc3_waveform.load();
        if (ImGui::Combo("OSC3 Waveform", &osc3_wave, waveforms, 3)) {
            params->osc3_waveform = osc3_wave;
        }

        float osc3_offset = params->osc3_freq_offset.load();
        if (ImGui::SliderFloat("OSC3 Frequency Offset", &osc3_offset, -5.0f, 5.0f)) {
            params->osc3_freq_offset = osc3_offset;
        }
    }
}

void SynthUI::renderEnvelopeControls() {
    if (ImGui::CollapsingHeader("Envelope", ImGuiTreeNodeFlags_DefaultOpen)) {
        float attack = params->attack_time.load();
        if (ImGui::SliderFloat("Attack", &attack, 0.0f, 1.0f)) {
            params->attack_time = attack;
        }

        float release = params->release_time.load();
        if (ImGui::SliderFloat("Release", &release, 0.0f, 2.0f)) {
            params->release_time = release;
        }
    }
}

void SynthUI::renderFilterControls() {
    if (ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen)) {
        float cutoff = params->filter_cutoff.load();
        if (ImGui::SliderFloat("Filter Cutoff", &cutoff, 20.0f, 20000.0f, "%.0f Hz")) {
            params->filter_cutoff = cutoff;
        }

        float resonance = params->filter_resonance.load();
        if (ImGui::SliderFloat("Filter Resonance", &resonance, 0.0f, 1.0f)) {
            params->filter_resonance = resonance;
        }

        float autoAmount = params->filter_auto_amount.load();
        if (ImGui::SliderFloat("Filter auto-variation amount", &autoAmount, 0.0f, 1.0f)) {
            params->filter_auto_amount = autoAmount;
        }

        float autoFreq = params->filter_auto_freq.load();
        if (ImGui::SliderFloat("Filter auto-variation frequency", &autoFreq, 1.0f, 20.0f)) {
            params->filter_auto_freq = autoFreq;
        }
    }
}

void SynthUI::renderVolumeControl() {
    if (ImGui::CollapsingHeader("Volume", ImGuiTreeNodeFlags_DefaultOpen)) {
        float volume = params->volume.load();
        if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
            params->volume = volume;
        }
    }
}

void SynthUI::renderOctaveControl() {
    if (ImGui::CollapsingHeader("Octave Control", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("-")) {
            if (currentOctave > -2) {
                currentOctave--;
                params->octave = currentOctave;
            }
        }
        ImGui::SameLine();
        ImGui::Text("Octave: %d", currentOctave);
        ImGui::SameLine();
        if (ImGui::Button("+")) {
            if (currentOctave < 1) {
                currentOctave++;
                params->octave = currentOctave;
            }
        }
    }
}

void SynthUI::renderVirtualKeyboard() {
    if (ImGui::CollapsingHeader("Virtual Keyboard", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (int i = 0; i < 13; ++i) {
            if (i > 0) ImGui::SameLine();

            bool isPressed = keyStates[i];
            if (isPressed) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.6f, 0.2f, 1.0f));
            }

            std::string buttonLabel = std::to_string(i);

            if (ImGui::Button(buttonLabel.c_str())) {
                if (!isPressed) {
                    if (noteOnCallback) noteOnCallback(i);
                    keyStates[i] = true;
                }
            }

            if (isPressed) {
                ImGui::PopStyleColor();
            }
        }

        ImGui::Text("Keyboard mapping: %s", keyboardKeys);
        ImGui::Text("Notes: 0=C, 1=C#, 2=D, 3=D#, 4=E, 5=F, 6=F#, 7=G, 8=G#, 9=A, 10=A#, 11=B, 12=C");
    }
}

void SynthUI::setNoteOnCallback(std::function<void(int)> callback) {
    noteOnCallback = callback;
}

void SynthUI::setNoteOffCallback(std::function<void()> callback) {
    noteOffCallback = callback;
}