#ifndef REPORTBUILDER_H
#define REPORTBUILDER_H

#pragma once
#include "./Src/Report/ReportSaver.h"
#include "./Src/Telemetry/TelemetryStore.h"

class ReportBuilder {
public:
    ReportBuilder() = default;
    ~ReportBuilder() = default;

    void buildReport(
        ReportSaver::Report& report,
        const TelemetryStore& telemetryStore,
        const ObjectInfo& objectInfo,
        const Sensors& sensors,
        const ValveInfo& valveInfo,
        const OtherParameters& otherParams,
        const MaterialsOfComponentParts& materialsOfComponentParts,
        const ListDetails& listDetails,
        const QImage& imageChartTask = QImage(),
        const QImage& imageChartPressure = QImage(),
        const QImage& imageChartFriction = QImage()
        );

    QString templatePath() const { return QStringLiteral(":/excel/Report.xlsx"); }
};

#endif // REPORTBUILDER_H
