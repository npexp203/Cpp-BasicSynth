//
// Created by pc on 04-08-25.
//

#ifndef ENVELOPE_H
#define ENVELOPE_H

#endif //ENVELOPE_H
enum class State {
    IDLE,
    ATTACK,
    SUSTAIN,
    RELEASE,
};

class Envelope {
public:
    void setAttackTime(float attackTime, float sampleRate);
    void setDecayTime(float decayTime, float sampleRate);
    void setSustainLevel(float level);
    void setReleaseTime(float releaseTime, float sampleRate);

    void noteOn();
    void noteOff();

    void processBuffer(float* buffer, int numFrames);
private:
    State state = State::IDLE;
    float value = 0.0f;

    float attackRate = 0.0f;
    float sustainLevel = 1.0f;
    float releaseRate = 0.0f;


};
