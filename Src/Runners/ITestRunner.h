#ifndef ITESTRUNNER_H
#define ITESTRUNNER_H

#pragma once
#include <QObject>
#include <QPoint>

class TelemetryStore;

class ITestRunner : public QObject {
    Q_OBJECT
public:
    explicit ITestRunner(QObject* parent=nullptr) : QObject(parent) {}
    ~ITestRunner() override = default;

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void releaseBlock() = 0;

signals:
    void requestClearChart(int chartIndex);
    void totalTestTimeMs(quint64);
    void endTest();

    void addPoints(int chart, const QVector<struct QPoint>&);
    void clearPoints(int chart);

    void telemetryUpdated(const TelemetryStore&);
    void requestSetDac(quint16 dac, quint32 sleepMs, bool waitForStop, bool waitForStart);

};


#endif // ITESTRUNNER_H
