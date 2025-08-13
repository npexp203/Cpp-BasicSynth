#include "include/audio/AudioEngine.h"
#include "include/ui/SynthUI.h"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {

    // Create shared parameters
    auto synthParams = std::make_shared<SynthetizerConfig>();

    // Initialize audio engine
    AudioEngine audioEngine(synthParams);
    audioEngine.initialize();

    // Initialize UI
    SynthUI synthUI(synthParams);
    synthUI.initialize();
    synthUI.run();

    return 0;
}