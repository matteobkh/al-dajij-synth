#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <atomic>
#include <vector>
#include <memory>
#include <cmath>

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

// Represents an oscillator with frequency, volume and methods for waveforms and
// phase increment
struct Oscillator {
    int sampleRate;
    std::atomic<float> frequency;
    std::atomic<float> volume;
    float phase;
    int waveform;

    // Constructor for oscillator struct
    Oscillator(int sr, float freq = 440.0f, float vol = 0.5f, float p = .0f, int wf = 0) 
        : sampleRate(sr), frequency(freq), volume(vol), phase(p), waveform(wf) {}
    
    // Sinusoidal waveform
    float sine() {
        return sinf(2.0f * M_PI * phase);
    }
    // Square waveform
    float square() {
        return phase < 0.5 ? 1.0f : -1.0f;
    }
    // Sawtooth waveform
    float saw() {
        return 1 - fmod(2.0f * phase, 2.0f);
    }
    // Increments the phase variable (always between 0f and 1f)
    void incrementPhase() {
        phase = fmod(phase + frequency / sampleRate, 1);
    }
};

#endif // OSCILLATOR_H