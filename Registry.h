#ifndef REGISTRY_H
#define REGISTRY_H

#pragma once
#include <QDate>
#include <QObject>
#include <QSettings>

struct ObjectInfo
{
    QString object;
    QString manufactory;
    QString department;
    QString FIO;
};

struct AccessoriesSeries {
    QString plunger;
    QString saddle;
    QString bushing;
    QString oRing;
    QString stuffingSeal;
    QString diaphragm;
    QString covers;
    QString shaft;
    QString saddleLock;
};
struct MaterialsOfComponentParts {
    QString saddle;
    QString corpus;
    QString cap;
    QString ball;
    QString disk;
    QString plunger;
    QString shaft;
    QString stock;
    QString guideSleeve;
    QString stuffingBoxSeal;
};
struct ValveInfo
{
    QString positionNumber;
    QString serialNumber;
    QString manufacturer;
    QString valveModel;
    QString valveSeries;
    QString valveStroke;
    QString positionerModel;
    QString dinamicError;
    QString driveModel;
    QString range;

    quint32 DN;
    quint32 CV;
    quint32 PN;

    quint32 safePosition;
    quint32 driveType;
    quint32 strokeMovement;
    quint32 toolNumber;

    qreal diameter;
    qreal pulley;
};

struct OtherParameters
{
    QString date;
    QString safePosition;
    QString movement;
};

class Registry : public QObject
{
    Q_OBJECT
public:
    explicit Registry(QObject *parent = nullptr);

    ObjectInfo *getObjectInfo();
    void saveObjectInfo();

    ValveInfo *getValveInfo(const QString &positionNumber);
    ValveInfo *getValveInfo();
    void saveValveInfo();

    MaterialsOfComponentParts *getMaterialsOfComponentParts();
    void saveMaterialsOfComponentParts();

    AccessoriesSeries *getAccessoriesSeries();
    void saveAccessoriesSeries();

    OtherParameters *getOtherParameters();
    bool checkObject(const QString &object);
    bool checkManufactory(const QString &manufactory);
    bool checkDepartment(const QString &department);
    bool checkPosition(const QString &position);
    QStringList getPositions();
    QString getLastPosition();

private:
    ObjectInfo m_objectInfo;
    ValveInfo m_valveInfo;
    AccessoriesSeries m_accessoriesSeries;
    MaterialsOfComponentParts m_materialsOfComponentParts;

    QSettings m_settings;
    OtherParameters m_otherParameters;
signals:
};

#endif // REGISTRY_H
