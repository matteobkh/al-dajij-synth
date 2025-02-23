#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <vector>
#include <memory>
#include <atomic>
#include <iostream>
#include "oscillator.h"
#include "lpf.h"

// Struct that contains all audio objects used in the program for easy access
struct AudioEngine {
    int sampleRate;
    std::vector<std::shared_ptr<Oscillator>> oscillators; // Oscillator bank 
    LowPassFilter filter;  // Single-pole low-pass filter

    // room for more objects

    std::atomic<float> masterVolume; // Master volume

    // Constructor, please specify sample rate
    AudioEngine(int sr, float vol = 1.0f)
        : sampleRate(sr), filter(sr), masterVolume(vol) {
            std::cout << "AudioEngine created: sampleRate = " 
                << sampleRate << std::endl;
        }

    //
    void addOscillator(float freq = 440.0f, float vol = 0.5f, float p = .0f, int wf = 0) {
        oscillators.push_back(std::make_shared<Oscillator>(sampleRate, freq, vol, p, wf));
    }
    void removeOscillator(int index) {
        oscillators.erase(oscillators.begin() + index);
    }
};

#endif // AUDIO_ENGINE_H
