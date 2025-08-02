//
// Created by pc on 02-08-25.
//

#ifndef OSCILLATOR_H
#define OSCILLATOR_H
class Oscillator {
public:
    enum class WaveForm{SINE,SQUARE,TRIANGLE,SAWTOOTH};
    float getSample(float time);
    Oscillator(WaveForm waveform,float freq);
    void setFrequencyOffset(float offset);


private:
    WaveForm waveform;
    float frequency;
    float frequencyOffset = 0.0f;

};

#endif //OSCILLATOR_H
