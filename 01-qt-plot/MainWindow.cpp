#include "MainWindow.h"
#include <QtWidgets>
#include <QtCharts>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this); // Create a central widget
    setCentralWidget(centralWidget);   // Set it as the central widget of the MainWindow

    layout = new QVBoxLayout(centralWidget); // Create a vertical layout for the central widget
}

void MainWindow::appendAudioSignalChart(const std::vector<float>& audioData)
{
    auto *series = new QLineSeries();
    for (size_t i = 0; i < audioData.size(); ++i) {
        series->append(static_cast<qreal>(i), static_cast<qreal>(audioData[i]));
    }

    auto *chart = new QChart();
    chart->addSeries(series);

    // Customize the X and Y axes after adding the series
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, audioData.size() - 1); // Set the range to match the data points
    axisX->setTickCount(audioData.size()); // Set tick count to match the number of data points for alignment
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-2, 2); // Set the range to match the data points
    axisY->applyNiceNumbers(); // Adjust the Y axis range to nice numbers for readability
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto *chartView = new QChartView(chart);
    chartView->setMinimumHeight(200);
    layout->addWidget(chartView);

    // Optional: Customize grid lines and labels appearance
    axisX->setGridLineVisible(true);
    axisY->setGridLineVisible(true);
}
