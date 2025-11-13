#include "./Src/Report/ReportBuilder.h"

void ReportBuilder::buildReport(
    ReportSaver::Report& report,
    const TelemetryStore& telemetryStore,
    const ObjectInfo& objectInfo,
    const Sensors& sensors,
    const ValveInfo& valveInfo,
    const OtherParameters& otherParams,
    const MaterialsOfComponentParts& materialsOfComponentParts,
    const ListDetails& listDetails,
    const QImage& imageChartTask,
    const QImage& imageChartPressure,
    const QImage& imageChartFriction
    ) {
    QString sheet_1 = "Отчет ОТ";

    // Лист: Отчет ОТ; Страница: 1; Блок: Данные по объекту
    report.data.push_back({sheet_1, 4, 4, objectInfo.object});
    report.data.push_back({sheet_1, 5, 4, objectInfo.manufactory});
    report.data.push_back({sheet_1, 6, 4, objectInfo.department});

    // Страница:Отчет; Блок: Краткая спецификация на клапан
    report.data.push_back({sheet_1, 4, 13, valveInfo.positionNumber});
    report.data.push_back({sheet_1, 5, 13, valveInfo.serialNumber});
    report.data.push_back({sheet_1, 6, 13, valveInfo.valveModel});
    report.data.push_back({sheet_1, 7, 13, valveInfo.manufacturer});
    report.data.push_back({sheet_1, 8, 13, QString("%1 / %2")
                                               .arg(valveInfo.DN)
                                               .arg(valveInfo.PN)});
    report.data.push_back({sheet_1, 9, 13, valveInfo.positionerModel});
    report.data.push_back({sheet_1, 10, 13, QString("%1")
                                                .arg(telemetryStore.supplyRecord.pressure_bar, 0, 'f', 2)});
    report.data.push_back({sheet_1, 11, 13, otherParams.safePosition});
    report.data.push_back({sheet_1, 12, 13, valveInfo.driveModel});
    report.data.push_back({sheet_1, 13, 13, otherParams.movement});
    report.data.push_back({sheet_1, 14, 13, materialsOfComponentParts.stuffingBoxSeal});

    // Материал деталей
    report.data.push_back({sheet_1, 9, 4, materialsOfComponentParts.corpus});
    report.data.push_back({sheet_1, 10, 4, materialsOfComponentParts.cap});
    report.data.push_back({sheet_1, 11, 4, materialsOfComponentParts.saddle});
    report.data.push_back({sheet_1, 11, 6, QString::number(valveInfo.CV)});
    report.data.push_back({sheet_1, 12, 4, materialsOfComponentParts.ball});
    report.data.push_back({sheet_1, 13, 4, materialsOfComponentParts.disk});
    report.data.push_back({sheet_1, 14, 4, materialsOfComponentParts.plunger});
    report.data.push_back({sheet_1, 15, 4, materialsOfComponentParts.shaft});
    report.data.push_back({sheet_1, 16, 4, materialsOfComponentParts.stock});
    report.data.push_back({sheet_1, 17, 4, materialsOfComponentParts.guideSleeve});

    // Страница: Отчет; Блок: Результат испытаний
    report.data.push_back({sheet_1, 25, 5,
                           QString("%1")
                               .arg(telemetryStore.mainTestRecord.dynamicErrorReal, 0, 'f', 2)});

    report.data.push_back({sheet_1, 25, 8,
                           QString("%1")
                               .arg(valveInfo.dinamicError, 0, 'f', 2)});

    // report.data.push_back({sheet_1, 27, 5, telemetryStore.dinamicRecord.dinamicIpReal});
    // report.data.push_back({sheet_1, 27, 8, telemetryStore.dinamicRecord.dinamicIpRecomend});

    report.data.push_back({sheet_1, 29, 5, QString("%1")
                                               .arg(telemetryStore.valveStrokeRecord.real, 0, 'f', 2)});
    report.data.push_back({sheet_1, 29, 8, valveInfo.valveStroke});

    report.data.push_back({sheet_1, 31, 5,
        QString("%1–%2")
            .arg(telemetryStore.mainTestRecord.springLow, 0, 'f', 2)
            .arg(telemetryStore.mainTestRecord.springHigh, 0, 'f', 2)
    });
    report.data.push_back({sheet_1, 31, 8, valveInfo.range});

    report.data.push_back({sheet_1, 33, 5,
        QString("%1–%2")
            .arg(telemetryStore.mainTestRecord.lowLimitPressure, 0, 'f', 2)
            .arg(telemetryStore.mainTestRecord.highLimitPressure, 0, 'f', 2)
    });

    report.data.push_back({
        sheet_1, 35, 5,
        QString("%1")
            .arg(telemetryStore.mainTestRecord.frictionPercent, 0, 'f', 2)
    });

    report.data.push_back({
        sheet_1, 37, 5,
        QString("%1")
            .arg(telemetryStore.mainTestRecord.frictionForce, 0, 'f', 3)
    });
    report.data.push_back({
        sheet_1, 51, 5, telemetryStore.strokeTestRecord.timeForwardMs
    });
    report.data.push_back({
        sheet_1, 51, 8, telemetryStore.strokeTestRecord.timeBackwardMs
    });

    report.data.push_back({sheet_1, 55, 10, listDetails.plunger});
    report.data.push_back({sheet_1, 56, 10, listDetails.saddle});
    report.data.push_back({sheet_1, 57, 10,
        QString("%1 / %2")
            .arg(listDetails.upperBushing)
            .arg(listDetails.lowerBushing)
    });
    report.data.push_back({sheet_1, 58, 10,
        QString("%1 / %2")
            .arg(listDetails.upperORing)
            .arg(listDetails.lowerORing)
    });
    report.data.push_back({sheet_1, 59, 10, listDetails.stuffingBoxSeal});
    report.data.push_back({sheet_1, 60, 10, listDetails.driveDiaphragm});
    report.data.push_back({sheet_1, 61, 10, listDetails.covers});
    report.data.push_back({sheet_1, 62, 10, listDetails.shaft});
    report.data.push_back({sheet_1, 63, 10, listDetails.saddleLock});

    // Пользователь и дата
    report.data.push_back({sheet_1, 65, 12, otherParams.date});

    report.data.push_back({sheet_1, 66, 5, sensors.Pressure1});
    report.data.push_back({sheet_1, 67, 5, sensors.Pressure2});
    report.data.push_back({sheet_1, 68, 5, sensors.Pressure3});
    report.data.push_back({sheet_1, 69, 5, sensors.Pressure4});
    report.data.push_back({sheet_1, 70, 5, sensors.Motion});

    // Пользователь
    report.data.push_back({sheet_1, 73, 4, objectInfo.FIO});

    // Страница: Отчет; Блок: Диагностические графики
    report.images.push_back({sheet_1, 84, 1, imageChartTask});
    report.images.push_back({sheet_1, 109, 1, imageChartPressure});
    report.images.push_back({sheet_1, 134, 1, imageChartFriction});

    // Страница: Отчет; Блок: Дата
    report.data.push_back({sheet_1, 158, 12, otherParams.date});

    report.validation.push_back({"=ЗИП!$A$1:$A$37", "J56:J65"});
    report.validation.push_back({"=Заключение!$B$1:$B$4", "E42"});
    report.validation.push_back({"=Заключение!$C$1:$C$3", "E44"});
    report.validation.push_back({"=Заключение!$E$1:$E$4", "E46"});
    report.validation.push_back({"=Заключение!$D$1:$D$5", "E48"});
    report.validation.push_back({"=Заключение!$F$3", "E50"});
}
