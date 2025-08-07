//
// Created by pc on 02-08-25.
//

#ifndef FILTER_H
#define FILTER_H
#pragma once

class Filter {


public:
    void processBuffer(float* buffer, int numFrames, float baseCutoff,
                       float autoAmount, float autoFreq, float resonance, float sampleRate);
private:
    float a0, a1, a2, b1, b2;
    float x1_L = 0.0f, x2_L = 0.0f, y1_L = 0.0f, y2_L = 0.0f;
    float x1_R = 0.0f, x2_R = 0.0f, y1_R = 0.0f, y2_R = 0.0f;
    float lfoPhase = 0.0f;

    void updateCoefficients(float cutoff, float resonance, float sampleRate);
};

#endif //FILTER_H
