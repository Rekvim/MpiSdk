#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPointF>
#include <QThread>

#include "ReportSaver.h"
#include "MyChart.h"
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

private slots:
    void SetText(const TextObjects object, const QString &text);
    void SetTask(qreal task);
    void SetTextColor(const TextObjects object, const QColor color);
    void SetStepTestResults(QVector<StepTest::TestResult> results, quint32 T_value);
    void SetSensorsNumber(quint8 num);
    void SetButtonInitEnabled(bool enable);

    void onCountdownTimeout();

    void AddPoints(Charts chart, QVector<Point> points);
    void ClearPoints(Charts chart);
    void SetChartVisible(Charts chart, quint16 series, bool visible);
    void ShowDots(bool visible);
    void DublSeries();
    void EnableSetTask(bool enable);
    void GetPoints(QVector<QVector<QPointF>> &points, Charts chart);
    void SetRegressionEnable(bool enable);
    void GetMainTestParameters(MainTestSettings::TestParameters &parameters);
    void GetStepTestParameters(StepTestSettings::TestParameters &parameters);
    void GetResolutionTestParameters(OtherTestSettings::TestParameters &parameters);
    void GetResponseTestParameters(OtherTestSettings::TestParameters &parameters);
    void Question(QString title, QString text, bool &result);
    void GetDirectory(QString currentPath, QString &result);
    void StartTest();
    void EndTest();
    void ButtonStartMain();
    void ButtonStartStroke();
    void ButtonStartOptional();

private:
    Ui::MainWindow *ui;

    ReportSaver::Report m_notationReport;
    ReportSaver::Report m_valveReport;
    ReportSaver::Report m_currentReport;
    ReportSaver::Report m_report;
    ReportSaver *m_reportSaver;

    Program *m_program;
    QThread *m_programThread;

    QTimer* m_durationTimer;
    QElapsedTimer m_elapsedTimer;
    qint64 m_totalTestMs;

    Registry *m_registry;

    QHash<TextObjects, QLabel *> m_labels;
    QHash<TextObjects, QLineEdit *> m_lineEdits;
    QHash<Charts, MyChart *> m_charts;

    bool m_testing;
    MainTestSettings *m_mainTestSettings;
    StepTestSettings *m_stepTestSettings;
    OtherTestSettings *m_responseTestSettings;
    OtherTestSettings *m_resolutionTestSettings;

    void InitCharts();
    void SaveChart(Charts chart);
    void GetImage(QLabel *label, QImage *image);
    void InitReport();


protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
signals:
    void SetDAC(qreal value);
    void StartMainTest();
    void StartStrokeTest();
    void StartOptionalTest(quint8 test_num);
    void StopTest();
    void SetDO(quint8 DO_num, bool state);
};
#endif // MAINWINDOW_H
