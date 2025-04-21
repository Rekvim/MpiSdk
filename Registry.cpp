#include "Registry.h"

Registry::Registry(QObject *parent)
    : QObject{parent}
    , settings_("MPPI", "Data")
{
    object_info_.object = settings_.value("object", "").toString();
    object_info_.manufactory = settings_.value("manufactory", "").toString();
    object_info_.department = settings_.value("department", "").toString();
    object_info_.FIO = settings_.value("FIO", "").toString();
}

ObjectInfo *Registry::GetObjectInfo()
{
    return &object_info_;
}

void Registry::SaveObjectInfo()
{
    settings_.setValue("object", object_info_.object);
    settings_.setValue("manufactory", object_info_.manufactory);
    settings_.setValue("department", object_info_.department);
    settings_.setValue("FIO", object_info_.FIO);
}

ValveInfo *Registry::GetValveInfo(const QString &positionNumber)
{

    settings_.beginGroup(object_info_.object);
    settings_.beginGroup(object_info_.manufactory);
    settings_.beginGroup(object_info_.department);
    settings_.beginGroup(positionNumber);

    valve_info_.positionNumber = positionNumber;
    valve_info_.manufacturer = settings_.value("manufacturer", "").toString();
    valve_info_.valveModel = settings_.value("valveModel", "").toString();
    valve_info_.serialNumber = settings_.value("serialNumber", "").toString();
    valve_info_.DN = settings_.value("DN", "").toInt();;
    valve_info_.PN = settings_.value("PN", "").toString();
    valve_info_.stroke = settings_.value("stroke", "").toString();
    valve_info_.positioner = settings_.value("positioner", "").toString();
    valve_info_.dinamicError = settings_.value("dinamicError", "").toString();
    valve_info_.modelDrive = settings_.value("model_drive", "").toString();
    valve_info_.range = settings_.value("range", "").toString();
    valve_info_.materialStuffingBoxSeal = settings_.value("material", "").toString();
    valve_info_.diameter = settings_.value("diameter", "").toDouble();
    valve_info_.safePosition = settings_.value("safePosition", "").toInt();
    valve_info_.driveType = settings_.value("driveType", "").toInt();
    valve_info_.strokeMovement = settings_.value("strokeMovement", "").toInt();
    valve_info_.toolNumber = settings_.value("toolNumber", "").toInt();
    valve_info_.pulley = settings_.value("pulley", "").toInt();
    valve_info_.CV = settings_.value("CV", "").toInt();

    valve_info_.material1 = settings_.value("material1", "").toString();
    valve_info_.material2 = settings_.value("material2", "").toString();
    valve_info_.material4 = settings_.value("material4", "").toString();
    valve_info_.material5 = settings_.value("material5", "").toString();
    valve_info_.material6 = settings_.value("material6", "").toString();
    valve_info_.material7 = settings_.value("material7", "").toString();
    valve_info_.material8 = settings_.value("material8", "").toString();
    valve_info_.material9 = settings_.value("material9", "").toString();

    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();

    return &valve_info_;
}

ValveInfo *Registry::GetValveInfo()
{
    return &valve_info_;
}

void Registry::SaveValveInfo()
{
    settings_.beginGroup(object_info_.object);
    settings_.beginGroup(object_info_.manufactory);
    settings_.beginGroup(object_info_.department);

    settings_.setValue("last_position", valve_info_.positionNumber);

    settings_.beginGroup(valve_info_.positionNumber);

    settings_.setValue("manufacturer", valve_info_.manufacturer);
    settings_.setValue("model", valve_info_.valveModel);
    settings_.setValue("serial", valve_info_.serialNumber);
    settings_.setValue("DN", valve_info_.DN);
    settings_.setValue("PN", valve_info_.PN);
    settings_.setValue("CV", valve_info_.CV);
    settings_.setValue("stroke", valve_info_.stroke);
    settings_.setValue("positioner", valve_info_.positioner);
    settings_.setValue("dinamic_error", valve_info_.dinamicError);
    settings_.setValue("modelDrive", valve_info_.modelDrive);
    settings_.setValue("range", valve_info_.range);
    settings_.setValue("materialStuffingBoxSeal", valve_info_.materialStuffingBoxSeal);
    settings_.setValue("diameter", valve_info_.diameter);
    settings_.setValue("safePosition", valve_info_.safePosition);
    settings_.setValue("driveType", valve_info_.driveType);
    settings_.setValue("strokeMovement", valve_info_.strokeMovement);
    settings_.setValue("toolNumber", valve_info_.toolNumber);
    settings_.setValue("pulley", valve_info_.pulley);

    settings_.setValue("material1", valve_info_.material1);
    settings_.setValue("material2", valve_info_.material2);
    settings_.setValue("material4", valve_info_.material4);
    settings_.setValue("material5", valve_info_.material5);
    settings_.setValue("material6", valve_info_.material6);
    settings_.setValue("material7", valve_info_.material7);
    settings_.setValue("material8", valve_info_.material8);
    settings_.setValue("material9", valve_info_.material9);

    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();
}

OtherParameters *Registry::GetOtherParameters()
{
    return &other_parameters_;
}

bool Registry::CheckObject(const QString &object)
{
    return settings_.childGroups().contains(object);
}

bool Registry::CheckManufactory(const QString &manufactory)
{
    settings_.beginGroup(object_info_.object);

    bool result = settings_.childGroups().contains(manufactory);

    settings_.endGroup();

    return result;
}

bool Registry::CheckDepartment(const QString &department)
{
    settings_.beginGroup(object_info_.object);
    settings_.beginGroup(object_info_.manufactory);

    bool result = settings_.childGroups().contains(department);

    settings_.endGroup();
    settings_.endGroup();

    return result;
}

bool Registry::CheckPosition(const QString &position)
{
    settings_.beginGroup(object_info_.object);
    settings_.beginGroup(object_info_.manufactory);
    settings_.beginGroup(object_info_.department);

    bool result = settings_.childGroups().contains(position);

    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();

    return result;
}

QStringList Registry::GetPositions()
{
    settings_.beginGroup(object_info_.object);
    settings_.beginGroup(object_info_.manufactory);
    settings_.beginGroup(object_info_.department);

    QStringList result = settings_.childGroups();

    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();

    return result;
}

QString Registry::GetLastPosition()
{
    settings_.beginGroup(object_info_.object);
    settings_.beginGroup(object_info_.manufactory);
    settings_.beginGroup(object_info_.department);

    QString result = settings_.value("last_position", "").toString();

    settings_.endGroup();
    settings_.endGroup();
    settings_.endGroup();

    return result;
}
