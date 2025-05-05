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
    QString materialSaddle;
    QString materialCorpus;
    QString materialCap;
    QString materialBall;
    QString materialDisk;
    QString materialPlunger;
    QString materialShaft;
    QString materialStock;
    QString materialGuideSleeve;
    QString materialStuffingBoxSeal;
};

struct ValveInfo
{
    QString positionNumber;
    QString serialNumber;
    QString manufacturer;
    QString valveModel;
    QString stroke;
    QString positioner;
    QString dinamicError;
    QString modelDrive;
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

    ObjectInfo *GetObjectInfo();
    void SaveObjectInfo();

    ValveInfo *GetValveInfo(const QString &positionNumber);
    ValveInfo *GetValveInfo();
    void SaveValveInfo();

    MaterialsOfComponentParts *GetMaterialsOfComponentParts();
    void SaveMaterialsOfComponentParts();

    AccessoriesSeries *GetAccessoriesSeries();
    void SaveAccessoriesSeries();

    OtherParameters *GetOtherParameters();
    bool CheckObject(const QString &object);
    bool CheckManufactory(const QString &manufactory);
    bool CheckDepartment(const QString &department);
    bool CheckPosition(const QString &position);
    QStringList GetPositions();
    QString GetLastPosition();

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
