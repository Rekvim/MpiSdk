#include "Registry.h"

Registry::Registry(QObject *parent)
    : QObject{parent}
    , m_settings("MPPI", "Data")
{
    m_objectInfo.object = m_settings.value("object", "").toString();
    m_objectInfo.manufactory = m_settings.value("manufactory", "").toString();
    m_objectInfo.department = m_settings.value("department", "").toString();
    m_objectInfo.FIO = m_settings.value("FIO", "").toString();
}

Sensors* Registry::getSensors() {
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    m_sensors.Pressure1 = m_settings.value("Pressure1", "").toString();
    m_sensors.Pressure2 = m_settings.value("Pressure2", "").toString();
    m_sensors.Pressure3 = m_settings.value("Pressure3", "").toString();
    m_sensors.Pressure4 = m_settings.value("Pressure4", "").toString();
    m_sensors.Motion = m_settings.value("Motion", "").toString();

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    return &m_sensors;
}
void Registry::saveSensors()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    m_settings.setValue("Pressure1", m_sensors.Pressure1);
    m_settings.setValue("Pressure2", m_sensors.Pressure2);
    m_settings.setValue("Pressure3", m_sensors.Pressure3);
    m_settings.setValue("Pressure4", m_sensors.Pressure4);
    m_settings.setValue("Motion", m_sensors.Motion);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
}

ObjectInfo *Registry::getObjectInfo()
{
    return &m_objectInfo;
}

void Registry::saveObjectInfo()
{
    m_settings.setValue("object", m_objectInfo.object);
    m_settings.setValue("manufactory", m_objectInfo.manufactory);
    m_settings.setValue("department", m_objectInfo.department);
    m_settings.setValue("FIO", m_objectInfo.FIO);
}

ValveInfo *Registry::getValveInfo(const QString &positionNumber)
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);
    m_settings.beginGroup(positionNumber);

    m_valveInfo.positionNumber = positionNumber;
    m_valveInfo.manufacturer = m_settings.value("manufacturer", "").toString();
    m_valveInfo.valveSeries = m_settings.value("valveSeries", "").toString();
    m_valveInfo.valveModel = m_settings.value("valveModel", "").toString();
    m_valveInfo.serialNumber = m_settings.value("serialNumber", "").toString();
    m_valveInfo.DN = m_settings.value("DN", "").toInt();
    m_valveInfo.CV = m_settings.value("CV", "").toInt();
    m_valveInfo.PN = m_settings.value("PN", "").toInt();
    m_valveInfo.valveStroke = m_settings.value("valveStroke", "").toString();
    m_valveInfo.positionerModel = m_settings.value("positionerModel", "").toString();
    m_valveInfo.dinamicError = m_settings.value("dinamicError", "").toDouble();
    m_valveInfo.driveModel = m_settings.value("modelDrive", "").toString();
    m_valveInfo.range = m_settings.value("range", "").toString();
    m_valveInfo.diameter = m_settings.value("diameter", "").toDouble();
    m_valveInfo.safePosition = m_settings.value("safePosition", "").toInt();
    m_valveInfo.driveType = m_settings.value("driveType", "").toInt();
    m_valveInfo.strokeMovement = m_settings.value("strokeMovement", "").toInt();
    m_valveInfo.toolNumber = m_settings.value("toolNumber", "").toInt();
    m_valveInfo.pulley = m_settings.value("pulley", "").toInt();


    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return &m_valveInfo;
}

ValveInfo *Registry::getValveInfo()
{
    return &m_valveInfo;
}

void Registry::saveValveInfo()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    m_settings.setValue("last_position", m_valveInfo.positionNumber);

    m_settings.beginGroup(m_valveInfo.positionNumber);

    m_settings.setValue("manufacturer", m_valveInfo.manufacturer);
    m_settings.setValue("valveSeries", m_valveInfo.valveSeries);
    m_settings.setValue("valveModel", m_valveInfo.valveModel);
    m_settings.setValue("serialNumber", m_valveInfo.serialNumber);
    m_settings.setValue("DN", m_valveInfo.DN);
    m_settings.setValue("PN", m_valveInfo.PN);
    m_settings.setValue("CV", m_valveInfo.CV);
    m_settings.setValue("valveStroke", m_valveInfo.valveStroke);
    m_settings.setValue("positionerModel", m_valveInfo.positionerModel);
    m_settings.setValue("dinamicError", m_valveInfo.dinamicError);
    m_settings.setValue("modelDrive", m_valveInfo.driveModel);
    m_settings.setValue("range", m_valveInfo.range);
    m_settings.setValue("diameter", m_valveInfo.diameter);
    m_settings.setValue("safePosition", m_valveInfo.safePosition);
    m_settings.setValue("driveType", m_valveInfo.driveType);
    m_settings.setValue("strokeMovement", m_valveInfo.strokeMovement);
    m_settings.setValue("toolNumber", m_valveInfo.toolNumber);
    m_settings.setValue("pulley", m_valveInfo.pulley);
    m_settings.setValue("materialStuffingBoxSeal", m_materialsOfComponentParts.stuffingBoxSeal);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.sync();
}

MaterialsOfComponentParts *Registry::getMaterialsOfComponentParts() {
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);
    m_settings.beginGroup(m_valveInfo.positionNumber);

    m_materialsOfComponentParts.stuffingBoxSeal = m_settings.value("StuffingBoxSeal", "").toString();
    m_materialsOfComponentParts.corpus = m_settings.value("Corpus", "").toString();
    m_materialsOfComponentParts.cap = m_settings.value("Cap", "").toString();
    m_materialsOfComponentParts.ball = m_settings.value("Ball", "").toString();
    m_materialsOfComponentParts.disk = m_settings.value("Disk", "").toString();
    m_materialsOfComponentParts.plunger = m_settings.value("Plunger", "").toString();
    m_materialsOfComponentParts.shaft = m_settings.value("Shaft", "").toString();
    m_materialsOfComponentParts.stock = m_settings.value("Stock", "").toString();
    m_materialsOfComponentParts.guideSleeve = m_settings.value("GuideSleeve", "").toString();

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return &m_materialsOfComponentParts;
}

void Registry::saveMaterialsOfComponentParts()
{
    const MaterialsOfComponentParts &material = m_materialsOfComponentParts;

    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    QString positionNumber = m_valveInfo.positionNumber;
    m_settings.beginGroup(positionNumber);

    m_settings.setValue("StuffingBoxSeal", material.stuffingBoxSeal);
    m_settings.setValue("Corpus", material.corpus);
    m_settings.setValue("Cap", material.cap);
    m_settings.setValue("Ball", material.ball);
    m_settings.setValue("Disk", material.disk);
    m_settings.setValue("Plunger", material.plunger);
    m_settings.setValue("Shaft", material.shaft);
    m_settings.setValue("Stock", material.stock);
    m_settings.setValue("GuideSleeve", material.guideSleeve);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.sync();
}

void Registry::saveListDetails()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);
    m_settings.beginGroup(m_valveInfo.positionNumber);

    m_settings.setValue("ListDetails_Plunger", m_listDetails.plunger);
    m_settings.setValue("ListDetails_Saddle", m_listDetails.saddle);
    m_settings.setValue("ListDetails_UpperBushing", m_listDetails.upperBushing);
    m_settings.setValue("ListDetails_LowerBushing", m_listDetails.lowerBushing);
    m_settings.setValue("ListDetails_UpperORing", m_listDetails.upperORing);
    m_settings.setValue("ListDetails_LowerORing", m_listDetails.lowerORing);
    m_settings.setValue("ListDetails_StuffingBoxSeal", m_listDetails.stuffingBoxSeal);
    m_settings.setValue("ListDetails_DriveDiaphragm", m_listDetails.driveDiaphragm);
    m_settings.setValue("ListDetails_Covers", m_listDetails.covers);
    m_settings.setValue("ListDetails_Shaft", m_listDetails.shaft);
    m_settings.setValue("ListDetails_SaddleLock",m_listDetails.saddleLock);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    m_settings.sync();
}

ListDetails *Registry::getListDetails()
{
    return &m_listDetails;
}

OtherParameters *Registry::getOtherParameters()
{
    return &m_otherParameters;
}

bool Registry::checkObject(const QString &object)
{
    return m_settings.childGroups().contains(object);
}

bool Registry::checkManufactory(const QString &manufactory)
{
    m_settings.beginGroup(m_objectInfo.object);

    bool result = m_settings.childGroups().contains(manufactory);

    m_settings.endGroup();

    return result;
}

bool Registry::checkDepartment(const QString &department)
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);

    bool result = m_settings.childGroups().contains(department);

    m_settings.endGroup();
    m_settings.endGroup();

    return result;
}

bool Registry::checkPosition(const QString &position)
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    bool result = m_settings.childGroups().contains(position);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return result;
}

QStringList Registry::getPositions()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    QStringList result = m_settings.childGroups();

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return result;
}

QString Registry::getLastPosition()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    QString result = m_settings.value("last_position", "").toString();

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return result;
}
