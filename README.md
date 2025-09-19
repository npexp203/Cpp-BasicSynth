# Modular Synthesizer in cpp 

A simple modular synthesizer written in **C++**, using multithreading to ensure workflow:
- **Audio thread**: sound synthesis with PortAudio 
- **UI thread**: graphical interface with SDL3 + Dear ImGui   

---

## Requirements
- **C++20** compatible compiler  
- **CMake ≥ 3.16**  

## How to run ? 
mkdir build
cd build
cmake ..
cmake --build .
./synth.exe

## Features

- **Three oscillators** with selectable waveforms:
  - Triangle
  - Saw
  - Noise
- **Envelope controls**:
  - Attack
  - Release
- **Filter controls**:
  - Cutoff
  - Resonance
- **LFO (Low-Frequency Oscillator)**:
  - LFO Amount
  - LFO Frequency
- **Global controls**:
  - Volume
  - Octave selection
- **Playable keyboard**:
  - Clickable buttons in the UI
  - Keyboard shortcuts corresponding to the keys shown
  - <img width="900" height="720" alt="image" src="https://github.com/user-attachments/assets/c7bd3492-fd4f-4001-a7b8-da5ffbfbadd9" />

