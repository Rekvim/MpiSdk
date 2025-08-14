#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPointF>
#include <QThread>

#include "./ReportSaver.h"
#include "./Src/CustomChart/MyChart.h"
#include "Program.h"
#include "Registry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SetRegistry(Registry *registry);
    void setReport(const ReportSaver::Report &report);

signals:
    void SetDAC(qreal value);
    void runMainTest();
    void runStrokeTest();
    void runOptionalTest(quint8 test_num);

    void StopTest();
    void SetDO(quint8 DO_num, bool state);

private:
    Ui::MainWindow *ui;

    ReportSaver::Report m_report;
    ReportSaver *m_reportSaver;

    Program *m_program;
    QThread *m_programThread;

    QTimer *m_durationTimer = nullptr;
    QElapsedTimer m_elapsedTimer;
    qint64 m_totalTestMs = 0;

    Registry *m_registry;

    QHash<TextObjects, QLabel *> m_labels;
    QHash<TextObjects, QLineEdit *> m_lineEdits;
    QHash<Charts, MyChart *> m_charts;

    bool m_testing;
    MainTestSettings *m_mainTestSettings;
    StepTestSettings *m_stepTestSettings;
    OtherTestSettings *m_responseTestSettings;
    OtherTestSettings *m_resolutionTestSettings;

    QImage m_imageChartTask;
    QImage m_imageChartPressure;
    QImage m_imageChartFriction;
    QImage m_imageChartStep;

    void InitCharts();
    void SaveChart(Charts chart);
    void GetImage(QLabel *label, QImage *image);
    void InitReport();

private slots:
    void SetText(const TextObjects object, const QString &text);
    void SetTask(qreal task);
    void SetTextColor(const TextObjects object, const QColor color);
    void SetStepTestResults(QVector<StepTest::TestResult> results, quint32 T_value);
    void SetSensorsNumber(quint8 num);
    void SetButtonInitEnabled(bool enable);

    void onCountdownTimeout();
    void onTotalTestTimeMs(const quint64 totalMs);

    void AddPoints(Charts chart, QVector<Point> points);
    void ClearPoints(Charts chart);
    void SetChartVisible(Charts chart, quint16 series, bool visible);
    void ShowDots(bool visible);
    void DublSeries();
    void EnableSetTask(bool enable);
    void SetRegressionEnable(bool enable);

    void GetPoints(QVector<QVector<QPointF>> &points, Charts chart);

    void receivedParameters_mainTest(MainTestSettings::TestParameters &parameters);
    void receivedParameters_stepTest(StepTestSettings::TestParameters &parameters);
    void receivedParameters_resolutionTest(OtherTestSettings::TestParameters &parameters);
    void receivedParameters_responseTest(OtherTestSettings::TestParameters &parameters);

    void on_pushButton_report_generate_clicked();
    void on_pushButton_report_open_clicked();

    void on_pushButton_mainTest_start_clicked();
    void on_pushButton_mainTest_save_clicked();

    void on_pushButton_strokeTest_start_clicked();
    void on_pushButton_strokeTest_save_clicked();

    void on_pushButton_optionalTests_start_clicked();
    void on_pushButton_optionalTests_save_clicked();

    void on_pushButton_imageChartTask_clicked();
    void on_pushButton_imageChartPressure_clicked();
    void on_pushButton_imageChartFriction_clicked();

    void on_pushButton_signal_4mA_clicked();
    void on_pushButton_signal_8mA_clicked();
    void on_pushButton_signal_12mA_clicked();
    void on_pushButton_signal_16mA_clicked();
    void on_pushButton_signal_20mA_clicked();

    void Question(QString title, QString text, bool &result);
    void GetDirectory(QString currentPath, QString &result);
    void StartTest();
    void EndTest();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};
#endif // MAINWINDOW_H
