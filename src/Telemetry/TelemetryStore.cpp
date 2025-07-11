#ifndef TELEMETRYSTORE_H
#define TELEMETRYSTORE_H

#pragma once

#include <QVector>
#include <QString>
#include <QColor>

#include <QMetaType>

struct InitState {
    QString  deviceStatusText = "";
    QColor deviceStatusColor;
    QString initStatusText = "";
    QColor initStatusColor;
    QString connectedSensorsText = "";
    QColor connectedSensorsColor;
    QString startingPositionText = "";
    QColor startingPositionColor = "";
    QString finalPositionText = "";
    QColor finalPositionColor = "";
};

struct StepTestRecord {
    quint16 from;
    quint16 to;
    quint32 T_value;
    qreal overshoot;
};

struct StrokeTestRecord {
    quint64 timeForwardMs = 0;
    quint64 timeBackwardMs = 0;
};

struct MainTestRecord {
    double dynamicError_mean = 0.0;
    double dynamicError_meanPercent = 0.0;

    double dynamicError_max = 0.0;
    double dynamicError_maxPercent = 0.0;

    double dynamicErrorReal = 0.0;

    double lowLimitPressure = 0.0;
    double highLimitPressure = 0.0;

    double springLow = 0.0;
    double springHigh = 0.0;

    double pressureDifference = 0.0;
    double frictionForce = 0.0;
    double frictionPercent = 0.0;
};

struct ValveStrokeRecord {
    QString range = "";
    double real = 0.0;
};

struct SupplyRecord {
    double pressure_bar = 0.0;
};

class TelemetryStore {
public:
    InitState init;
    QVector<StepTestRecord> stepResults;
    StrokeTestRecord strokeTestRecord;
    ValveStrokeRecord valveStrokeRecord;
    SupplyRecord supplyRecord;
    MainTestRecord mainTestRecord;

    TelemetryStore() = default;

    void clearAll() {
        init = {};
        strokeTestRecord = {};
        mainTestRecord = {};
        // stepResults.clear();
        valveStrokeRecord = {};
        supplyRecord = {};
    }
};

#endif // TELEMETRYSTORE_H
