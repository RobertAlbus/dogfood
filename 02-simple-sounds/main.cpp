#include "Clover/Algorithm/Wavetable/Generators.h"
#include "Clover/Algorithm/Wavetable/WavetableOscillatorMono.h"
#include "Clover/IO/Interface.h"
#include "Clover/Util/MusicTime.h"

constexpr int SAMPLE_RATE = 48000;

using namespace Clover;
int main(int argc, char *argv[])
{
    Wavetable::Wavetable<float> sineTable = Wavetable::Sine<float>(512);
    Wavetable::WavetableOscillatorMono<float> osc(SAMPLE_RATE, sineTable);

    IO::Interface interface;
    Util::Time time(160, Clover::Base::sampleRate, &interface.clock);








    return 0;
}
