#include "include/audio/AudioEngine.h"
#include "include/ui/SynthUI.h"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {

    // Create shared parameters
    std::cout << "salut demarrage" << std::endl;
    auto synthParams = std::make_shared<SynthetizerConfig>();
    std::cout << "salut parametre" << std::endl;

    // Initialize audio engine
    AudioEngine audioEngine(synthParams);
    std::cout << "salut audioengin" << std::endl;

    if (!audioEngine.initialize()) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
        return -1;
    }
    std::cout << "salut ui" << std::endl;

    // Initialize UI
    SynthUI synthUI(synthParams);
    if (!synthUI.initialize()) {
        std::cerr << "Failed to initialize UI" << std::endl;
        return -1;
    }
    std::cout << "salut callback" << std::endl;

    // Set up callbacks
    synthUI.setNoteOnCallback([&audioEngine](float frequency) {
        audioEngine.noteOn(frequency);
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