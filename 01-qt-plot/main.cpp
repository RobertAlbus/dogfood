#include "Algorithm/Wavetable/WavetableOscillatorMono.h"

#include <QApplication>
#include "MainWindow.h"

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <vector>


std::vector<float> reduceToSizeWithPeaks(const std::vector<float>& originalSignal, size_t desiredSize) {
    std::vector<float> reducedSignal;
    reducedSignal.reserve(desiredSize);
    double spanSize = static_cast<double>(originalSignal.size()) / static_cast<double>(desiredSize);

    for (size_t i = 0; i < desiredSize; ++i) {
        size_t startIdx = static_cast<size_t>(std::ceil(spanSize * i));
        size_t endIdx = std::min(static_cast<size_t>(std::ceil(spanSize * (i + 1))), originalSignal.size());
        auto peakIter = std::max_element(originalSignal.begin() + startIdx, originalSignal.begin() + endIdx,
                                         [](float a, float b) { return std::abs(a) < std::abs(b); });
        float peakValue = (peakIter != originalSignal.begin() + endIdx) ? *peakIter : 0.0f;
        reducedSignal.push_back(peakValue);
    }

    return reducedSignal;
}

int main(int argc, char *argv[])
{
    int sampleRate = 48000;
    int signalSize = sampleRate * 0.25;
    Clover::Wavetable::WavetableOscillatorMono<float> osc(sampleRate);
    osc.sine(16);
    osc.freq(120.f);
    std::vector<float> signalSine {};
    signalSine.reserve(signalSize);
    std::vector<float> signalRectified {};
    signalRectified.reserve(signalSize);
    std::vector<float> signalLpf {};
    signalLpf.reserve(signalSize);

    for (int i = 0; i < signalSize/4; i++) {
        signalSine.push_back(0.f);
        signalRectified.push_back(std::abs(0.f));
    }

    for (int i = 0; i < signalSize/2; i++) {
        float signal = osc.process();
        signalSine.push_back(signal);
        signalRectified.push_back(std::abs(signal));

    }

    for (int i = 0; i < signalSize /4; i++) {
        signalSine.push_back(0.f);
        signalRectified.push_back(std::abs(0.f));
    }

    for (int i = 0, end = static_cast<int>(signalRectified.size()); i < end; ++i) {
        float lpfAlpha = 0.002f;
        if (i==0) {
            float lpf = (signalRectified[i] * lpfAlpha);
            signalLpf.push_back(lpf);
        } else {
            float lpf = (signalRectified[i] * lpfAlpha) + (signalLpf[i-1] * (1.f - lpfAlpha));
            signalLpf.push_back(lpf);
        }
    }

    QApplication app(argc, argv); // Initialize the QApplication

    MainWindow mainWindow;
    mainWindow.appendAudioSignalChart(reduceToSizeWithPeaks(signalSine, 512));
    mainWindow.appendAudioSignalChart(reduceToSizeWithPeaks(signalRectified, 512));
    mainWindow.appendAudioSignalChart(reduceToSizeWithPeaks(signalLpf, 512));
    mainWindow.show();

    return app.exec(); // Enter the Qt application event loop
}
