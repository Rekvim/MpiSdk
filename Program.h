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
    Trend
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
    void setRegistry(Registry *registry);

signals:
    void setText(const TextObjects object, const QString &text);
    void setTextColor(const TextObjects object, const QColor color);
    void setTask(qreal task);
    void setSensorNumber(quint8 num);
    void setButtonInitEnabled(bool enable);
    void setGroupDOVisible(bool visible);
    void stopTest();

    void addPoints(Charts chart, QVector<Point> points);
    void setVisible(Charts chart, quint16 series, bool visible);
    void clearPoints(Charts chart);
    void showDots(bool visible);
    void enableSetTask(bool enable);
    void dublSeries();
    void getPoints(QVector<QVector<QPointF>> &points, Charts chart);
    void releaseBlock();
    void setRegressionEnable(bool enable);

    void mainTestFinished();
    void getParameters_mainTest(MainTestSettings::TestParameters &parameters);
    void getParameters_stepTest(StepTestSettings::TestParameters &parameters);
    void getParameters_resolutionTest(OtherTestSettings::TestParameters &parameters);
    void getParameters_responseTest(OtherTestSettings::TestParameters &parameters);

    void question(QString &title, QString &text, bool &result);

    void setStepResults(QVector<StepTest::TestResult> results, quint32 T_value);
    void setButtonsDOChecked(quint8 status);
    void setCheckboxDIChecked(quint8 status);

    void totalTestTimeMs(quint64 totalMs);

public slots:
    void addRegression(const QVector<QPointF> &points);
    void addFriction(const QVector<QPointF> &points);
    void getPoints_maintest(QVector<QVector<QPointF>> &points);
    void getPoints_steptest(QVector<QVector<QPointF>> &points);
    void endTest();

    void setDac_real(qreal value);
    void setDac_int(quint16 value);

    void initialize();

    void runningMainTest();
    void runningStrokeTest();
    void runningOptionalTest(const quint8 test_num);

    void terminateTest();

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

    inline qreal calcPercent(qreal value, bool invert = false) {
        qreal percent = ((value - 4.0) / 16.0) * 100.0;
        percent = qBound<qreal>(0.0, percent, 100.0);
        return invert ? (100.0 - percent) : percent;
    }

private slots:
    void updateSensors();
    void updateCharts_mainTest();
    void updateCharts_strokeTest();
    void updateCharts_optionTest(Charts chart);

    void results_mainTest(const MainTest::TestResults &results);
    void results_strokeTest(const quint64 forwardTime, const quint64 backwardTime);
    void results_stepTest(const QVector<StepTest::TestResult> &results, const quint32 T_value);

    void setTimeStart();

    void setDac(quint16 dac,
                quint32 sleepMs = 0,
                bool waitForStop = false,
                bool waitForStart = false);
};

#endif // PROGRAM_H
