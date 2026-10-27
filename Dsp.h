#pragma once
// Pure C++ DSP core (no JUCE dependency, so it can be unit-tested anywhere).
//   y = tanhf(drive * x) * output
// Both gains are set in dB and smoothed (one-pole, ~10 ms) to avoid zipper noise.

#include <math.h>   // tanhf, powf, expf

namespace tanhdrive
{
    inline float dbToGain (float db) noexcept { return powf (10.0f, db * 0.05f); }

    class Processor
    {
    public:
        void prepare (double sampleRate) noexcept
        {
            const float sr = (float) (sampleRate > 0.0 ? sampleRate : 44100.0);
            coef = 1.0f - expf (-1.0f / (0.010f * sr));       // ~10 ms smoothing
        }

        // jump straight to the given values (used when playback starts)
        void reset (float driveDb, float outputDb) noexcept
        {
            driveTarget  = driveGain  = dbToGain (driveDb);
            outputTarget = outputGain = dbToGain (outputDb);
        }

        void setDriveDb  (float db) noexcept { driveTarget  = dbToGain (db); }
        void setOutputDb (float db) noexcept { outputTarget = dbToGain (db); }

        // in-place, any number of channels
        void process (float* const* channels, int numChannels, int numSamples) noexcept
        {
            for (int i = 0; i < numSamples; ++i)
            {
                driveGain  += (driveTarget  - driveGain)  * coef;
                outputGain += (outputTarget - outputGain) * coef;

                for (int c = 0; c < numChannels; ++c)
                {
                    float x = channels[c][i];
                    if (! (x == x) || x > 1.0e6f || x < -1.0e6f)   // NaN / absurd values -> silence
                        x = 0.0f;

                    channels[c][i] = tanhf (driveGain * x) * outputGain;
                }
            }
        }

    private:
        float coef = 0.002f;
        float driveGain = 1.0f,  driveTarget = 1.0f;
        float outputGain = 1.0f, outputTarget = 1.0f;
    };
}
