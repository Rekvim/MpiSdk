#ifndef VALVEDATABASE_H
#define VALVEDATABASE_H

#pragma once
#include <QSqlDatabase>
#include <QMap>
#include <QPair>
#include <QString>
#include <QVector>

class ValveDatabase {
public:
    explicit ValveDatabase(const QSqlDatabase &database) :
        m_db(database) {}

    ~ValveDatabase() = default;

    int getSeriesIdByName(int manufacturerId, const QString &seriesName) const;

    QVector<QPair<int, QString>> getManufacturers() const;
    QVector<QPair<int, QString>> getValveSeries(int manufacturerId) const;
    QVector<QPair<int, QString>> getValveModels(int seriesId) const;
    int getValveModelIdByName(int seriesId, const QString &modelName) const;
    QVector<QPair<int, qreal>> getDynamicErrors(int valveModelId) const;
    QVector<QPair<int, int>> getValveDnSizes(int seriesId) const;
    QVector<QPair<int, double>> getCvValues(int valveDnSizeId) const;
    QVector<QPair<int, QString>> getSaddleMaterials() const;
    QVector<QPair<int, QString>> getSaddleMaterialsForModel(int valveModelId) const;
    QVector<QPair<int, QString>> getDriveModel() const;
    QVector<QPair<int, QString>> getBodyMaterials() const;
    QVector<QPair<int, QString>> getAllSeries() const;

    QMap<QString, QString> getValveComponents(int valveDnSizeId,
                                              int cvValueId,
                                              int saddleMaterialId) const;

private:
    QSqlDatabase m_db;
};

#endif // VALVEDATABASE_H
