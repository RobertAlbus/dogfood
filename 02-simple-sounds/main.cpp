#include "Clover/Algorithm/Envelope/Basic.h"
#include "Clover/Algorithm/Wavetable/Generators.h"
#include "Clover/Algorithm/Wavetable/WavetableOscillatorMono.h"
#include "Clover/IO/InterfaceV2.h"
#include "Clover/Util/MusicTime.h"

#include <cstdio>

constexpr int SAMPLE_RATE = 48000;
constexpr int DURATION = 2 * SAMPLE_RATE;

using namespace Clover;

int main(int argc, char *argv[])
{

    float initialFreq = 1000;
    Wavetable::Wavetable<float> sineTable = Wavetable::Sine<float>(512);
    Wavetable::WavetableOscillatorMono<float> osc(SAMPLE_RATE, sineTable);
    osc.freq(initialFreq);
    Envelope::Basic<float> envelope(0.f, initialFreq - 30.f, (uint)DURATION);

    IO::InterfaceV2 interface;
    IO::SystemAudioIoConfig systemAudioConfig = IO::GetSystemAudioIoConfig();
    // systemAudioConfig.printDevices();

    try {
        interface.openDevice({
            -1,
            0,
            6,
            2,
            SAMPLE_RATE,
            0
        });
    } catch(IO::Exception e) {
        printf("%s\n", e.what());
        return 1;
    }

    Util::Time time(160, Clover::Base::sampleRate, &interface.clock);

    interface.setAudioCallback([&](IO::AudioCallbackArguments data) {
        float &L = *(data.output);
        float &R = *(data.output + 1);

        float signal = osc.process() * 0.5;
        float envelopeValue = envelope.process();
        osc.freq(initialFreq - envelopeValue);
        L = signal;
        R = signal;

        printf("%f - %f - %f\n", *(data.output), *(data.output + 1), signal);
        printf("%p - %p\n", (void*)data.output, (void*)(data.output + 1));

        printf("%i\n", data.currentClockSample);
        if (data.currentClockSample == DURATION) {
            return IO::AudioCallbackStatus::END;
        }
        return IO::AudioCallbackStatus::CONTINUE;
    });

    try {
        interface.start();
    } catch (IO::Exception e) {
        printf("%s\n", e.what());
        return 1;
    }

    interface.waitForStreamComplete();
    interface.stop();

    return 0;
}
