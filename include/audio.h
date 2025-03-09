#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include "audioengine.h"
#include "portaudio.h"

enum Waveforms {
    SINEW,
    SQUAREW,
    SAWW
};

extern std::vector<float> audioBuffer;  // Shared buffer for waveform visualization
extern std::mutex audioMutex;           // Protects buffer access

bool openAudio(PaDeviceIndex index, AudioEngine& audioEngine);
bool closeAudio();
bool startAudio();
bool stopAudio();

// Handles PortAudio initialization
class ScopedPaHandler
{
public:
    ScopedPaHandler()
        : _result(Pa_Initialize())
    {
    }
    ~ScopedPaHandler()
    {
        if (_result == paNoError)
        {
            Pa_Terminate();
        }
    }

    PaError result() const { return _result; }

private:
    PaError _result;
};

#endif // AUDIO_H