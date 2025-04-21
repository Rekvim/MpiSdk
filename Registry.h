#ifndef REGISTRY_H
#define REGISTRY_H

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

struct ValveInfo
{
    QString positionNumber; // Номер позиции
    QString manufacturer; // Производитель ЗРА (торговая марка)
    QString valveModel; // Модель клапана
    QString serialNumber; // Серийный номер

    QString saddleMaterials; // Материал седла
    quint32 DN;
    quint32 CV;
    QString PN;
    QString stroke; //
    QString positioner; //
    QString dinamicError; //
    QString modelDrive; //
    QString range; //
    QString materialStuffingBoxSeal; // Материал сальникового уплотнения:
    qreal diameter; //
    quint32 safePosition; //
    quint32 driveType; //
    quint32 strokeMovement; //
    quint32 toolNumber; //
    qreal pulley; //
    QString material1;
    QString material2;
    QString material4;
    QString material5;
    QString material6;
    QString material7;
    QString material8;
    QString material9;
};

struct OtherParameters
{
    QString data;
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
    ValveInfo *GetValveInfo(const QString &position);
    ValveInfo *GetValveInfo();
    void SaveValveInfo();
    OtherParameters *GetOtherParameters();
    bool CheckObject(const QString &object);
    bool CheckManufactory(const QString &manufactory);
    bool CheckDepartment(const QString &department);
    bool CheckPosition(const QString &position);
    QStringList GetPositions();
    QString GetLastPosition();

private:
    QSettings settings_;
    ObjectInfo object_info_;
    ValveInfo valve_info_;
    OtherParameters other_parameters_;
signals:
};

#endif // REGISTRY_H
