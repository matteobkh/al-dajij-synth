#ifndef LPF_H
#define LPF_H

#include <cmath>
#include <atomic>
#include <iostream>
#include <mutex>

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

// IIR single-pole low-pass filter
class LowPassFilter {
private:
    float prev;  // Previous output sample
    float alpha; // Filter coefficient
    std::mutex filterMutex; // Protects prev

public:
    int sampleRate;
    std::atomic<float> frequency;
    bool passthrough;

    // Constructor
    LowPassFilter(int sr, float freq = 2000.0f, bool pt = false) 
        : prev(.0f), sampleRate(sr), frequency(freq), passthrough(pt) {
        updateAlpha();
        std::cout << "Filter Created: sampleRate = " << sampleRate 
              << ", frequency = " << frequency.load() << ", alpha = " << alpha << std::endl;
    }

    // Update filter coefficient when frequency changes
    void updateAlpha() {
        float f = frequency.load();
        alpha = (2.0f * M_PI * f) / (2.0f * M_PI * f + sampleRate);
    }

    // Process input sample
    float process(float input) {
        if (passthrough)
            return input;

        std::lock_guard<std::mutex> lock(filterMutex); // Prevent race conditions
        prev = prev + alpha * (input - prev);
        return prev;
    }
};

#endif // LPF_H