//
// Created by pc on 04-08-25.
//

#include "../../include/audio/Envelope.h"

#include <algorithm>
#include <iostream>

void Envelope::setAttackTime(float attackTime) {
    attackRate = attackTime > 0.0f ? 1.0f / (attackTime * SAMPLE_RATE) : 1000.0f;
}

void Envelope::setReleaseTime(float releaseTime) {
    releaseRate = releaseTime > 0.0f ? 1.0f / (releaseTime * SAMPLE_RATE) : 1000.0f;
}

void Envelope::noteOn() {
    state = State::ATTACK;
}

void Envelope::noteOff() {
    if (state != State::IDLE) {
        state = State::RELEASE;
    }
}

void Envelope::processBuffer(float* buffer, int numFrames) {
    for (int i = 0; i < numFrames * 2; i += 2) {
        switch (state) {
            case State::ATTACK:
                value += attackRate;
                if (value >= 1.0f) {
                    value = 1.0f;
                }
                break;


            case State::RELEASE:
                value -= releaseRate;
                if (value <= 0.0f) {
                    value = 0.0f;
                    state = State::IDLE;
                }
                break;

            case State::IDLE:
                value = 0.0f;
                break;
        }

        buffer[i] *= value;
        buffer[i + 1] *= value;
    }
}
