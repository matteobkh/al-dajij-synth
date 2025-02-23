
#include "portaudio.h"
#include "oscillator.h"
#include "audio.h"
#include "audioengine.h"

#define FRAMES_PER_BUFFER  (256)

#define SINEW  (0)
#define SQUAREW  (1)
#define SAWW  (2)

static PaStream* stream;

std::vector<float> audioBuffer(FRAMES_PER_BUFFER, 0.0f); // Store latest samples
std::mutex audioMutex;

static int paCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    /* std::vector<std::shared_ptr<Oscillator>>* data = 
        static_cast<std::vector<std::shared_ptr<Oscillator>>*>(userData);
    std::vector<std::shared_ptr<Oscillator>> d = *data; */
    AudioEngine* data = static_cast<AudioEngine*>(userData);
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    for(i=0; i<framesPerBuffer; i++)
    {
        float mix = 0;
        // Sum all oscillators in bank
        for(auto& osc : data->oscillators) {
            float vol = osc->volume.load();
            switch(osc->waveform){
                case SINEW:
                    mix += osc->sine() * vol;
                    break;
                case SQUAREW:
                    mix += osc->square() * vol;
                    break;
                case SAWW:
                    mix += osc->saw() * vol;
                    break;
            }
            osc->incrementPhase();
        }

        // Apply filter
        
        // mix = data->filter.process(mix);

        // Apply master volume
        mix *= data->masterVolume.load();

        *out++ = mix; /* left */
        *out++ = mix;  /* right */

        // Store samples for visualization (thread-safe)
        std::lock_guard<std::mutex> lock(audioMutex);
        audioBuffer[i % FRAMES_PER_BUFFER] = mix; // Circular buffer effect
    }

    return paContinue;
}

static void StreamFinished( void* userData )
{
    printf("Stream Completed\n");
}

bool openAudio(PaDeviceIndex index, AudioEngine& audioEngine)
{
    PaStreamParameters outputParameters;

    outputParameters.device = index;
    if (outputParameters.device == paNoDevice) {
        return false;
    }

    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
    if (pInfo != 0)
    {
        printf("Output device name: '%s'\r", pInfo->name);
    }

    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
        &stream,
        NULL, /* no input */
        &outputParameters,
        audioEngine.sampleRate,
        FRAMES_PER_BUFFER,
        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
        paCallback,
        &audioEngine    // pointer to audio engine to access audio objects
        );

    if (err != paNoError)
    {
        /* Failed to open stream to device !!! */
        return false;
    }

    err = Pa_SetStreamFinishedCallback(stream, &StreamFinished);

    if (err != paNoError)
    {
        Pa_CloseStream( stream );
        stream = 0;

        return false;
    }

    return true;
}

bool closeAudio()
{
    if (stream == 0)
        return false;

    PaError err = Pa_CloseStream( stream );
    stream = 0;

    return (err == paNoError);
}


bool startAudio()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StartStream( stream );

    return (err == paNoError);
}

bool stopAudio()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StopStream( stream );

    return (err == paNoError);
}
