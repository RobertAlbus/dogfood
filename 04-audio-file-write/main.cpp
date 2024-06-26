#include "Clover/Algorithm/Envelope/Basic.h"
#include "Clover/Algorithm/Math.h"
#include "Clover/Algorithm/Wavetable/Generators.h"
#include "Clover/Algorithm/Wavetable/OscillatorIndexCalculator.h"
#include "Clover/Algorithm/Wavetable/OscillatorInterface.h"
#include "Clover/Algorithm/Wavetable/WavetableOscillatorMono.h"
#include "Clover/IO/AudioFile/AudioFile.h"
#include "Clover/IO/AudioFile/AudioFileService.h"
#include "Clover/IO/AudioFile/AudioFileWriteSettings.h"
#include "Clover/IO/Interface.h"
#include "Clover/Midi/Notes.h"
#include "Clover/Util/MusicTime.h"

#include <cstdio>
#include <utility>

#include <algorithm>
#include <random>

constexpr int SAMPLE_RATE = 48000;
constexpr int DURATION = 100 * SAMPLE_RATE;
constexpr int WT_SIZE = 1024;

constexpr float FREQ_RANGE = 10000.f;
constexpr int NUM_OSCS = 18;
constexpr float OSC_FREQ_SPACING = FREQ_RANGE / NUM_OSCS;
constexpr float MOD_FREQ = 0.025;
constexpr float MOD_RANGE_MAX = 0.0125;

constexpr bool SHOULD_RENDER = true;
constexpr bool SHOULD_PLAY = !SHOULD_RENDER;
constexpr int CHANNEL_COUNT_OUT = 2;


using namespace Clover;

int main(int argc, char *argv[])
{
    Wavetable::Wavetable<float> modTable = Wavetable::Tri<float>(WT_SIZE);
    Wavetable::WavetableOscillatorMono<float> mod(SAMPLE_RATE, modTable);
    mod.freq(MOD_FREQ);

    Wavetable::Wavetable<float> wavetable = Wavetable::Sine<float>(WT_SIZE);
    std::vector<Wavetable::WavetableOscillatorMono<float>> oscs;
    oscs.reserve(NUM_OSCS);

    float KEY_SHIFT = 3.f;

    // F,Ab,C,Eb
    std::vector<float> freqs = {
        -12.f + Midi::Note::C4 + KEY_SHIFT,
        -12.f + Midi::Note::F4 + KEY_SHIFT,
        -12.f + Midi::Note::G4 + KEY_SHIFT,
        -12.f + Midi::Note::B4 + KEY_SHIFT,
        -12.f + Midi::Note::D5 + KEY_SHIFT,
        -12.f + Midi::Note::E5 + KEY_SHIFT,

        Midi::Note::C4 + KEY_SHIFT,
        Midi::Note::F4 + KEY_SHIFT,
        Midi::Note::G4 + KEY_SHIFT,
        Midi::Note::B4 + KEY_SHIFT,
        Midi::Note::D5 + KEY_SHIFT,
        Midi::Note::E5 + KEY_SHIFT,

        12.f + Midi::Note::C4 + KEY_SHIFT,
        12.f + Midi::Note::F4 + KEY_SHIFT,
        12.f + Midi::Note::G4 + KEY_SHIFT,
        12.f + Midi::Note::B4 + KEY_SHIFT,
        12.f + Midi::Note::D5 + KEY_SHIFT,
        12.f + Midi::Note::E5 + KEY_SHIFT,
    };

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(freqs), std::end(freqs), rng);

    for (int i = 0; i < NUM_OSCS; ++i) {
        Wavetable::WavetableOscillatorMono<float> osc {SAMPLE_RATE, wavetable};
        osc.freq(Algorithm::midi_to_frequency(freqs[i]));
        oscs.emplace_back(std::move(osc));
    }

    Envelope::Basic<float> envelope(0.f, 0.f, 1);

    auto audioCallback =[&](IO::AudioCallbackArguments data) {
        float &L = *(data.output);
        float &R = *(data.output + 1);


        float modValue = (mod.process() + 1) * 0.5;
        modValue = exp2(modValue) - 1;
        modValue *= NUM_OSCS;
        int modBottom = static_cast<int>(modValue);
        float modLerp = modValue - modBottom;
        
        int modTop = modBottom + 1;
        if (modTop > NUM_OSCS - 1) modTop = NUM_OSCS - 1;

        float signal = 0;
        for (int i = 0; i < NUM_OSCS; ++i) {
            if (i == modBottom) {
                signal += (oscs[i].process()) * modLerp;
                // signal += (oscs[i].process()) * (1.f - modLerp);
            } else if (i == modTop) {
                signal += (oscs[i].process()) * (1.f - modLerp);
                // signal += (oscs[i].process()) * modLerp;
            } else {
                oscs[i].process();
            }
        }

        float envelopeValue = envelope.process();
        mod.freq(MOD_FREQ + envelopeValue);

        L = signal;
        R = signal;

        float currentChunkProgress = fmod(data.currentClockSample, SAMPLE_RATE * 10);

        if (currentChunkProgress == 0.f) {
            envelope.set(MOD_RANGE_MAX, SAMPLE_RATE * 5);

        } else if (currentChunkProgress == SAMPLE_RATE * 5) {
            envelope.set(MOD_FREQ, SAMPLE_RATE * 5);
        }

        if (data.currentClockSample == DURATION) {
            return IO::AudioCallbackStatus::END;
        }
        return IO::AudioCallbackStatus::CONTINUE;
    };

    if (SHOULD_PLAY) {

        IO::Interface interface;
        IO::SystemAudioIoConfig systemAudioConfig = IO::GetSystemAudioIoConfig();
        // systemAudioConfig.printDevices();

        try {
            interface.openDevice({
            -1,
            0,
            6,
            CHANNEL_COUNT_OUT,
            SAMPLE_RATE,
            0
        });
        } catch(IoException e) {
            printf("%s\n", e.what());
            return 1;
        }

        interface.setAudioCallback(audioCallback);

        try {
            interface.start();
        } catch (IoException e) {
            printf("%s\n", e.what());
            return 1;
        }

        interface.waitForStreamComplete();
        interface.stop();

    } else if (SHOULD_RENDER) {

        auto fileService = IO::AudioFile::AudioFileService::BuildInstance();
        fileService.Write(
            {
                .path="../render/04.wav",
                .writeSettings=IO::AudioFile::WriteSettingsPcm::cd()
            },
            audioCallback,
            CHANNEL_COUNT_OUT,
            DURATION,
            SAMPLE_RATE
        );
    }

    return 0;
}
