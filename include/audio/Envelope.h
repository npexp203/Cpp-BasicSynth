//
// Created by pc on 04-08-25.
//

#ifndef ENVELOPE_H
#define ENVELOPE_H
#pragma once
#include "SynthetizerConfig.h"
enum class State {
    IDLE,
    ATTACK,
    RELEASE,
};

class Envelope {
public:
    void setAttackTime(float attackTime);
    void setDecayTime(float decayTime);
    void setReleaseTime(float releaseTime);

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
#endif //ENVELOPE_H
