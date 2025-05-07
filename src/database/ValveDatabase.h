#ifndef VALVEDATABASE_H
#define VALVEDATABASE_H

#include <QSqlDatabase>
#include <QMap>
#include <QPair>
#include <QString>
#include <QVector>

class ValveDatabase {
public:
    explicit ValveDatabase();
    ~ValveDatabase();

    int getSeriesIdByName(const QString &seriesName);

    QVector<QPair<int, QString>> getManufacturers();

    QVector<QPair<int, QString>> getValveSeries(int manufacturerId);

    QVector<QPair<int, QString>> getValveModels(int seriesId);

    QVector<QPair<int, int>> getValveDnSizes(int seriesId);

    QVector<QPair<int, double>> getCvValues(int valveDnSizeId);

    QVector<QPair<int, QString>> getSaddleMaterials();

    QVector<QPair<int, QString>> getDriveModel();

    QVector<QPair<int, QString>> getBodyMaterials();

    QVector<QPair<int, QString>> getAllSeries();

    QMap<QString, QString> getValveComponents(int valveDnSizeId,
                                              int cvValueId,
                                              int saddleMaterialId);

private:
    QSqlDatabase m_db;
};

#endif // VALVEDATABASE_H
