#ifndef LPF_H
#define LPF_H

#include <cmath>
#include <atomic>
#include <iostream>
#include <mutex>

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define NUM_POLES (4)

// IIR multi-pole low-pass filter
class LowPassFilter {
private:
    float polesL[NUM_POLES]; // Filter poles
    float polesR[NUM_POLES];
    float alpha; // Filter coefficient
    std::mutex filterMutex; // Protects poles

public:
    int sampleRate;
    std::atomic<float> frequency;
    bool passthrough;
    int nPoles;

    LowPassFilter(int sr, float freq = 2000.0f, bool pt = false, int np = NUM_POLES) 
        : polesL{0.,0.,0.,0.}, polesR{0.,0.,0.,0.}, sampleRate(sr), frequency(freq), passthrough(pt), nPoles(np) {
        updateAlpha();
        std::cout << "Filter created: sampleRate = " << sampleRate 
            << ", frequency = " << frequency.load() 
            << ", alpha = " << alpha << std::endl;
    }

    // Update filter coefficient when frequency changes
    void updateAlpha() {
        float f = frequency.load();
        alpha = (2.0f * M_PI * f) / (2.0f * M_PI * f + sampleRate);
    }

    float process(float input) {
        if (passthrough)
            return input;

        std::lock_guard<std::mutex> lock(filterMutex); //Prevent race conditions
        for(auto& p : polesL) { //always computes all poles for now, in the interest of optimization this should be changed
            p = p + alpha * (input - p);
            input = p;
        }
        return polesL[nPoles-1];
    }

    void processStereo(float& left, float& right) {
        if (passthrough)
            return;

        float prev = left;
        std::lock_guard<std::mutex> lock(filterMutex); //Prevent race conditions
        for(auto& p : polesL) {
            p = p + alpha * (prev - p);
            prev = p;
        }
        left = polesL[nPoles-1];
        prev = right;
        for(auto& p : polesR) {
            p = p + alpha * (prev - p);
            prev = p;
        }
        right = polesR[nPoles-1];
        return;
    }
};

#endif // LPF_H