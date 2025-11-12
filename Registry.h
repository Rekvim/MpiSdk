#ifndef REGISTRY_H
#define REGISTRY_H

#pragma once
#include <QObject>
#include <QSettings>

struct ObjectInfo
{
    QString object;
    QString manufactory;
    QString department;
    QString FIO;
};

struct Sensors {
    QString Pressure1;
    QString Pressure2;
    QString Pressure3;
    QString Pressure4;
    QString Motion;
};

struct ListDetails {
    QString plunger;
    QString saddle;
    QString upperBushing;
    QString lowerBushing;
    QString upperORing;
    QString lowerORing;
    QString stuffingBoxSeal;
    QString driveDiaphragm;
    QString covers;
    QString shaft;
    QString saddleLock;
};

struct MaterialsOfComponentParts {
    QString plunger;
    QString saddle;
    QString cap;
    QString corpus;
    QString ball;
    QString disk;
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
    QString driveModel;
    QString range;

    quint32 DN;
    quint32 CV;
    quint32 PN;

    quint32 safePosition;
    quint32 driveType;
    quint32 strokeMovement;
    quint32 toolNumber;

    qreal dinamicError;
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

    Sensors *getSensors();
    void saveSensors();
    // void loadSensors();

    ValveInfo *getValveInfo(const QString &positionNumber);
    ValveInfo *getValveInfo();
    void saveValveInfo();

    MaterialsOfComponentParts *getMaterialsOfComponentParts();
    void saveMaterialsOfComponentParts();

    ListDetails *getListDetails();
    void saveListDetails();

    OtherParameters *getOtherParameters();
    bool checkObject(const QString &object);
    bool checkManufactory(const QString &manufactory);
    bool checkDepartment(const QString &department);
    bool checkPosition(const QString &position);
    QStringList getPositions();
    QString getLastPosition();

private:
    ObjectInfo m_objectInfo;
    Sensors m_sensors;
    ValveInfo m_valveInfo;
    ListDetails m_listDetails;
    MaterialsOfComponentParts m_materialsOfComponentParts;

    QSettings m_settings;
    OtherParameters m_otherParameters;
signals:
};

#endif // REGISTRY_H
