#include "include/audio/AudioEngine.h"
#include "include/ui/SynthUI.h"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {

    // Create shared parameters
    auto synthParams = std::make_shared<SynthetizerConfig>();

    // Initialize audio engine
    AudioEngine audioEngine(synthParams);
    // a voir si mettre dans le synthui
    if (!audioEngine.initialize()) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
        return -1;
    }

    // Initialize UI
    SynthUI synthUI(synthParams);
    if (!synthUI.initialize()) {
        std::cerr << "Failed to initialize UI" << std::endl;
        return -1;
    }

    // A VOIR AUSSI ICI
    // Set up callbacks
    synthUI.setNoteOnCallback([&audioEngine](int noteIndex) {
        audioEngine.noteOn(noteIndex);
    });

    synthUI.setNoteOffCallback([&audioEngine]() {
        audioEngine.noteOff();
    });

    if (!audioEngine.start()) {
        std::cerr << "Failed to start audio engine" << std::endl;
        return -1;
    }

    // Main loop
    bool running = true;
    while (running) {
        running = synthUI.handleEvents();
        synthUI.render();
    }

    // Cleanup
    audioEngine.stop();
    synthUI.shutdown();
    audioEngine.shutdown();

    return 0;
}