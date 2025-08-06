#ifndef PROGRAM_H
#define PROGRAM_H

#pragma once
#include <QColor>
#include <QObject>
#include <QPointF>
#include <QEventLoop>
#include <QMessageBox>
#include <QTimer>

#include "OtherTestSettings.h"
#include "Registry.h"
#include "StepTestSettings.h"

#include "./Src/Mpi/Mpi.h"
#include "./Src/Tests/StepTest.h"
#include "./Src/Tests/MainTest.h"

enum class TextObjects {
    LineEdit_linearSensor,
    LineEdit_linearSensorPercent,
    LineEdit_pressureSensor_1,
    LineEdit_pressureSensor_2,
    LineEdit_pressureSensor_3,

    Label_deviceStatusValue,
    Label_deviceInitValue,
    Label_connectedSensorsNumber,
    Label_startingPositionValue,
    Label_finalPositionValue,

    Label_pressureDifferenceValue,
    Label_frictionForceValue,
    Label_frictionPercentValue,

    Label_dynamicErrorMean,
    Label_dynamicErrorMeanPercent,

    Label_dynamicErrorMax,
    Label_dynamicErrorMaxPercent,

    Label_valveStroke_range,
    Label_lowLimitValue,
    Label_highLimitValue,

    Label_forward,
    Label_backward,

    LineEdit_dinamic_error,
    LineEdit_stroke,
    LineEdit_range,
    LineEdit_friction,
    LineEdit_friction_percent,
    LineEdit_forward,
    LineEdit_backward,
    LineEdit_range_pressure

};

enum class Charts {
    Task,
    Pressure,
    Friction,
    Response,
    Resolution,
    Stroke,
    Step,
    Trend,
    Cyclic,
    Solenoid
};

struct Point
{
    quint8 seriesNum;
    qreal X;
    qreal Y;
};

class Program : public QObject
{
    Q_OBJECT
public:
    explicit Program(QObject *parent = nullptr);
    void SetRegistry(Registry *registry);

signals:
    void SetText(const TextObjects object, const QString &text);
    void SetTextColor(const TextObjects object, const QColor color);
    void SetTask(qreal task);
    void SetSensorNumber(quint8 num);
    void SetButtonInitEnabled(bool enable);
    void SetGroupDOVisible(bool visible);
    void StopTest();

    void AddPoints(Charts chart, QVector<Point> points);
    void SetVisible(Charts chart, quint16 series, bool visible);
    void ClearPoints(Charts chart);
    void ShowDots(bool visible);
    void EnableSetTask(bool enable);
    void DublSeries();
    void GetPoints(QVector<QVector<QPointF>> &points, Charts chart);
    void ReleaseBlock();
    void SetRegressionEnable(bool enable);

    void GetMainTestParameters(MainTestSettings::TestParameters &parameters);
    void GetStepTestParameters(StepTestSettings::TestParameters &parameters);
    void GetResolutionTestParameters(OtherTestSettings::TestParameters &parameters);
    void GetResponseTestParameters(OtherTestSettings::TestParameters &parameters);
    void Question(QString title, QString text, bool &result);

    void SetStepResults(QVector<StepTest::TestResult> results, quint32 T_value);
    void SetButtonsDOChecked(quint8 status);
    void SetCheckboxDIChecked(quint8 status);

public slots:
    void AddRegression(const QVector<QPointF> &points);
    void AddFriction(const QVector<QPointF> &points);
    void GetPoints_maintest(QVector<QVector<QPointF>> &points);
    void GetPoints_steptest(QVector<QVector<QPointF>> &points);
    void EndTest();

    void SetDAC_real(qreal value);
    void SetDAC_int(quint16 value);

    void button_init();
    void MainTestStart();
    void StrokeTestStart();
    void OptionalTestStart(quint8 test_num);
    void TerminateTest();

    void button_open();
    void button_report();
    void button_pixmap1();
    void button_pixmap2();
    void button_pixmap3();
    void button_set_position();
    void button_DO(quint8 DO_num, bool state);
    void checkbox_autoinit(int state);

private:
    Registry *m_registry;
    MPI m_mpi;
    QTimer *m_timerSensors;
    QTimer *m_timerDI;
    quint64 m_startTime;
    quint64 m_initTime;
    bool m_testing;
    QEventLoop *m_dacEventloop;
    bool m_stopSetDac;
    bool m_waitForButton = false;

private slots:
    void UpdateSensors();
    void UpdateCharts_maintest();
    void UpdateCharts_stroketest();
    void UpdateCharts_optiontest(Charts chart);

    void MainTestResults(MainTest::TestResults results);
    void StepTestResults(QVector<StepTest::TestResult> results, quint32 T_value);
    void StrokeTestResults(quint64 forwardTime, quint64 backwardTime);

    void SetTimeStart();

    void SetDac(quint16 dac,
                quint32 sleepMs = 0,
                bool waitForStop = false,
                bool waitForStart = false);
};

#endif // PROGRAM_H
