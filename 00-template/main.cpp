#include "Clover/IO/AudioFile/AudioFileService.h"
#include "Clover/IO/AudioFile/AudioFileWriteSettings.h"
#include "Clover/IO/Interface.h"

constexpr int SAMPLE_RATE = 48000;
constexpr int DURATION = 100 * SAMPLE_RATE;
constexpr int WT_SIZE = 1024;

constexpr bool SHOULD_RENDER = true;
constexpr bool SHOULD_PLAY = !SHOULD_RENDER;
constexpr int CHANNEL_COUNT_OUT = 2;


using namespace Clover;

int main(int argc, char *argv[])
{
    auto audioCallback =[&](IO::AudioCallbackArguments data) {
        float &L = *(data.output);
        float &R = *(data.output + 1);


        L = 0;
        R = 0;

        if (data.currentClockSample == DURATION) {
            return IO::AudioCallbackStatus::END;
        }
        return IO::AudioCallbackStatus::CONTINUE;
    };

    if (SHOULD_PLAY) {

        IO::Interface interface;
        IO::SystemAudioIoConfig systemAudioConfig = IO::GetSystemAudioIoConfig();

        interface.openDevice({
            -1,
            0,
            6,
            CHANNEL_COUNT_OUT,
            SAMPLE_RATE,
            0
        });

        interface.setAudioCallback(audioCallback);

        interface.start();
        interface.waitForStreamComplete();
        interface.stop();

    } else if (SHOULD_RENDER) {

        auto fileService = IO::AudioFile::AudioFileService::BuildInstance();
        fileService.Write(
            {
                .path="../render/00.wav",
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
