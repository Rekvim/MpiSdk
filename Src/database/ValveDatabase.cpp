#include "ValveDatabase.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QIODevice>
#include <QCoreApplication>

ValveDatabase::ValveDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    auto path = QCoreApplication::applicationDirPath() + "/database.db";
    m_db.setDatabaseName(path);
    qDebug() << path;

    if (!m_db.open()) {
        qCritical() << "Cannot open database:" << m_db.lastError().text();
    }
}

ValveDatabase::~ValveDatabase()
{
    if (m_db.isOpen())
        m_db.close();
}

QVector<QPair<int, QString>> ValveDatabase::getManufacturers()
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q("SELECT id, name FROM manufacturers");
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

QVector<QPair<int, QString>> ValveDatabase::getValveSeries(int manufacturerId)
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q;
    q.prepare("SELECT id, name FROM valve_series WHERE manufacturer_id = ?");
    q.addBindValue(manufacturerId);
    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return out;
    }
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

int ValveDatabase::getSeriesIdByName(int manufacturerId, const QString &seriesName)
{
    QSqlQuery q(m_db);
    q.prepare(R"sql(
        SELECT id
          FROM valve_series
         WHERE name = ?
           AND manufacturer_id = ?
    )sql");
    q.addBindValue(seriesName);
    q.addBindValue(manufacturerId);
    if (!q.exec()) {
        qWarning() << "getSeriesIdByName exec error:" << q.lastError().text();
        return -1;
    }
    if (q.next())
        return q.value(0).toInt();
    return -1;
}

QVector<QPair<int, QString>> ValveDatabase::getAllSeries()
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q("SELECT id, name FROM valve_series");
    if (!q.exec()) {
        qWarning() << "getAllSeries failed:" << q.lastError().text();
        return out;
    }
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

QVector<QPair<int, QString>> ValveDatabase::getValveModels(int seriesId)
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q;
    q.prepare("SELECT id, name FROM valve_models WHERE series_id = ?");
    q.addBindValue(seriesId);
    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return out;
    }
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

QVector<QPair<int, QString>> ValveDatabase::getSaddleMaterialsForModel(int valveModelId)
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q(m_db);
    q.prepare(R"sql(
        SELECT sm.id, sm.name
          FROM valve_models AS vm
          JOIN saddle_materials AS sm
            ON sm.id = vm.saddle_material_1
            OR sm.id = vm.saddle_material_2
         WHERE vm.id = ?
    )sql");
    q.addBindValue(valveModelId);
    if (!q.exec()) {
        qWarning() << "getSaddleMaterialsForModel failed:" << q.lastError().text();
        return out;
    }
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

int ValveDatabase::getValveModelIdByName(int seriesId, const QString &modelName)
{
    QSqlQuery q(m_db);
    q.prepare(R"sql(
        SELECT id
          FROM valve_models
         WHERE series_id = ?
           AND name      = ?
    )sql");
    q.addBindValue(seriesId);
    q.addBindValue(modelName);
    if (!q.exec()) {
        qWarning() << "getValveModelIdByName(exec) failed:" << q.lastError().text();
        return -1;
    }
    if (q.next())
        return q.value(0).toInt();
    return -1;
}

QVector<QPair<int, qreal>> ValveDatabase::getDynamicErrors(int valveModelId)
{
    QVector<QPair<int, qreal>> out;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, value FROM dinamic_error WHERE valve_model_id = ?");
    q.addBindValue(valveModelId);
    if (!q.exec()) {
        qWarning() << "getDynamicErrors failed:" << q.lastError().text();
        return out;
    }
    while (q.next()) {
        int id     = q.value(0).toInt();
        qreal val  = q.value(1).toReal();
        out.append(qMakePair(id, val));
    }
    return out;
}
QVector<QPair<int, int>> ValveDatabase::getValveDnSizes(int seriesId)
{
    QVector<QPair<int, int>> out;
    QSqlQuery q;
    q.prepare("SELECT id, dn_size FROM valve_dn_sizes WHERE series_id = ?");
    q.addBindValue(seriesId);
    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return out;
    }
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toInt());
    }
    return out;
}

QVector<QPair<int, double>> ValveDatabase::getCvValues(int valveDnSizeId)
{
    QVector<QPair<int, double>> out;
    QSqlQuery q;
    q.prepare("SELECT id, cv FROM cv_values WHERE valve_dn_size_id = ?");
    q.addBindValue(valveDnSizeId);
    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return out;
    }
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toDouble());
    }
    return out;
}

QVector<QPair<int, QString>> ValveDatabase::getDriveModel()
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q("SELECT id, name FROM drive_model");
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

QVector<QPair<int, QString>> ValveDatabase::getSaddleMaterials()
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q("SELECT id, name FROM saddle_materials");
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

QVector<QPair<int, QString>> ValveDatabase::getBodyMaterials()
{
    QVector<QPair<int, QString>> out;
    QSqlQuery q("SELECT id, name FROM body_materials");
    while (q.next()) {
        out.emplaceBack(q.value(0).toInt(), q.value(1).toString());
    }
    return out;
}

QMap<QString, QString> ValveDatabase::getValveComponents(int valveDnSizeId,
                                                         int cvValueId,
                                                         int saddleMaterialId)
{
    QMap<QString, QString> m;
    QSqlQuery q;
    q.prepare(QStringLiteral(
        "SELECT plunger, saddle, UpperBushing, LowerBushing, UpperORingSealingRing, LowerORingSealingRing, stuffingBoxSeal, "
        "driveDiaphragm, setOfCovers, shaft, saddleLock "
        "FROM valve_components "
        "WHERE valve_dn_size_id = ? "
        "AND cv_value_id = ? "
        "AND saddle_material_id = ?"
    ));

    q.addBindValue(valveDnSizeId);
    q.addBindValue(cvValueId);
    q.addBindValue(saddleMaterialId);
    if (!q.exec()) {
        qWarning() << q.lastError().text();
        return m;
    }
    if (q.next()) {
        m["plunger"] = q.value(0).toString();
        m["saddle"] = q.value(1).toString();
        m["UpperBushing"] = q.value(2).toString();
        m["LowerBushing"] = q.value(3).toString();
        m["UpperORingSealingRing"] = q.value(4).toString();
        m["LowerORingSealingRing"] = q.value(5).toString();
        m["stuffingBoxSeal"] = q.value(6).toString();
        m["driveDiaphragm"] = q.value(7).toString();
        m["setOfCovers"] = q.value(8).toString();
        m["shaft"] = q.value(9).toString();
        m["saddleLock"] = q.value(10).toString();
    }
    return m;
}
