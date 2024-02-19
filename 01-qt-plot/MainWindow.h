#pragma once

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <vector>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void appendAudioSignalChart(const std::vector<float>& audioData);

private:
    QWidget *centralWidget; // Central widget to hold the layout
    QVBoxLayout *layout;    // Layout to hold multiple chart views
};
