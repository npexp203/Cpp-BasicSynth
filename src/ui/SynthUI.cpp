#include "../../include/ui/SynthUI.h"

#include <cmath>
#include <imgui_impl_sdl3.h>
#include <iostream>
#include "imgui_impl_sdlrenderer3.h"

// Using shared_ptr to share ownership of the same SynthetizerConfig
// between UI and audio engine. The object is automatically destroyed
// when no shared_ptr instances reference it anymore.
SynthUI::SynthUI(std::shared_ptr<SynthetizerConfig> synthParams)
    : window(nullptr), renderer(nullptr), params(synthParams), currentOctave(0) {
    // Initialize all 13 key states (physical/virtual keyboard keys) to "not pressed" (false)
    std::fill(keyStates, keyStates + 13, false);
}

SynthUI::~SynthUI() {
    shutdown();
}

void SynthUI::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Resizable window
    window = SDL_CreateWindow("Synth", 900, 700, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Imgui return a ref to a structure imguiIo that contains every in and out parameter
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Nav with keyboard
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    isInitialized = true;
    std::cout << "UI initialized successfully!" << std::endl;
}

void SynthUI::render() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // good window without border
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("synth", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);

    // every component of the app
    renderOscillatorControls();
    ImGui::Separator();
    renderEnvelopeControls();
    ImGui::Separator();
    renderFilterControls();
    ImGui::Separator();

    renderVolumeControl();
    renderOctaveControl();
    renderVirtualKeyboard();

    ImGui::End();

    // Rendering with sdl render
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 45, 45, 48, 255);
    SDL_RenderClear(renderer);
    // draw ui
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    // back buffer to front
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
    // gets every event
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // give sdl event to imgui
        ImGui_ImplSDL3_ProcessEvent(&event);

        //quit "x"
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }

        handleKeyboard(event);
    }
    return true;
}


void SynthUI::handleKeyboard(const SDL_Event& event) {
    // Check if the event is either a key press or key release
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        // isDown = true if key pressed, false if released
        bool isDown = (event.type == SDL_EVENT_KEY_DOWN);

        SDL_Keycode keysSDL3[13] = {
            SDLK_S, SDLK_E, SDLK_D, SDLK_R, SDLK_F, SDLK_G, SDLK_Y,
            SDLK_H, SDLK_U, SDLK_J, SDLK_I, SDLK_K, SDLK_L
        };

        // Iterate through all synth keys to see if one matches the event key
        for (int i = 0; i < 13; ++i) {
            if (event.key.key == keysSDL3[i]) {
                // Store previous state of this key
                bool wasDown = keyStates[i];
                // Update current state
                keyStates[i] = isDown;

                if (isDown && !wasDown) {
                    // Key was just pressed → set note number in parameters
                    params->noteNumber = i;
                } else if (!isDown && wasDown) {
                    // Key was just released → reset note number
                    params->noteNumber = -1;
                }
                break;
            }
        }
    }
}


float SynthUI::noteToFrequency(int noteIndex, int octaveOffset) {
    int semitoneOffset = ((octaveOffset + 1) * 12) + noteIndex - 9;
    return 440.0f * std::pow(2.0f, semitoneOffset / 12.0f);
}

void SynthUI::renderOscillatorControls() {
        // Oscillator 1,Use .load() to safely read the atomic value from another thread (audio thread) without data races

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

void SynthUI::renderEnvelopeControls() {
        float attack = params->attack_time.load();
        if (ImGui::SliderFloat("Attack", &attack, 0.0f, 1.0f)) {
            params->attack_time = attack;
        }

        float release = params->release_time.load();
        if (ImGui::SliderFloat("Release", &release, 0.0f, 2.0f)) {
            params->release_time = release;
        }
}

void SynthUI::renderFilterControls() {
        float cutoff = params->filter_cutoff.load();
        if (ImGui::SliderFloat("Filter Cutoff", &cutoff, 20.0f, 20000.0f, "%.0f Hz")) {
            params->filter_cutoff = cutoff;
        }

        float resonance = params->filter_resonance.load();
        if (ImGui::SliderFloat("Filter Resonance", &resonance, 0.0f, 1.0f)) {
            params->filter_resonance = resonance;
        }

        float autoAmount = params->filter_auto_amount.load();
        if (ImGui::SliderFloat("Filter LFO amount", &autoAmount, 0.0f, 1.0f)) {
            params->filter_auto_amount = autoAmount;
        }

        float autoFreq = params->filter_auto_freq.load();
        if (ImGui::SliderFloat("Filter LFO frequency", &autoFreq, 1.0f, 20.0f)) {
            params->filter_auto_freq = autoFreq;
        }
    }

void SynthUI::renderVolumeControl() {
        float volume = params->volume.load();
        if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
            params->volume = volume;
        }
}

void SynthUI::renderOctaveControl() {
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

// Renders a 13-key virtual keyboard using ImGui.
// - Supports mouse-driven note on/off (monophonic).
// - Visually reflects both physical (real keyboard) and virtual (mouse) presses.
void SynthUI::renderVirtualKeyboard() {
    static bool virtualKeyStates[13] = {false};
    static int lastVirtualKeyPressed = -1;

    for (int i = 0; i < 13; ++i) {
        if (i > 0) ImGui::SameLine();

        // Visual state = pressed by physical keyboard OR by mouse
        bool isVisuallyPressed = keyStates[i] || virtualKeyStates[i];

        if (isVisuallyPressed) {
            // Highlight pressed keys
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.6f, 0.2f, 1.0f));
        }

        std::string buttonLabel = std::to_string(i);


        // creating the button
        if (ImGui::Button(buttonLabel.c_str())) {
        }

        if (ImGui::IsItemActive() && ImGui::IsMouseDown(0)) {
            if (!virtualKeyStates[i]) {

                // if another virtual key was active, turn it off first
                if (lastVirtualKeyPressed != -1 && lastVirtualKeyPressed != i) {
                    virtualKeyStates[lastVirtualKeyPressed] = false;
                    params->noteNumber = -1;
                    params->note_on = false;
                }
                // Turn this key on
                params->noteNumber = i;
                params->note_on = true;
                virtualKeyStates[i] = true;
                lastVirtualKeyPressed = i;
            }

            // NOTE OFF (mouse released / item deactive)
        } else if (virtualKeyStates[i] && !ImGui::IsItemActive()) {

            // If this key was virtually pressed and is no longer active, turn it off
            virtualKeyStates[i] = false;
            params->noteNumber = -1;
            params->note_on = false;

            // Reset mono tracking if we just released the last pressed key
            if (lastVirtualKeyPressed == i) {
                lastVirtualKeyPressed = -1;
            }
        }


        // Restore previous button color if we pushed one
        if (isVisuallyPressed) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::Text("Physical keys: S E D R F G Y H U J I K L");
}

void SynthUI::run() {
    bool running = true;
    while (running) {
        running = handleEvents();
        render();
    }
}
