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

ObjectInfo *Registry::GetObjectInfo()
{
    return &m_objectInfo;
}

void Registry::SaveObjectInfo()
{
    m_settings.setValue("object", m_objectInfo.object);
    m_settings.setValue("manufactory", m_objectInfo.manufactory);
    m_settings.setValue("department", m_objectInfo.department);
    m_settings.setValue("FIO", m_objectInfo.FIO);
}

ValveInfo *Registry::GetValveInfo(const QString &positionNumber)
{

    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);
    m_settings.beginGroup(positionNumber);

    m_valveInfo.positionNumber = positionNumber;
    m_valveInfo.manufacturer = m_settings.value("manufacturer", "").toString();
    m_valveInfo.valveModel = m_settings.value("valveModel", "").toString();
    m_valveInfo.serialNumber = m_settings.value("serialNumber", "").toString();
    m_valveInfo.DN = m_settings.value("DN", "").toInt();;
    m_valveInfo.PN = m_settings.value("PN", "").toInt();
    m_valveInfo.stroke = m_settings.value("stroke", "").toString();
    m_valveInfo.positioner = m_settings.value("positioner", "").toString();
    m_valveInfo.dinamicError = m_settings.value("dinamicError", "").toString();
    m_valveInfo.modelDrive = m_settings.value("modelDrive", "").toString();
    m_valveInfo.range = m_settings.value("range", "").toString();
    m_valveInfo.diameter = m_settings.value("diameter", "").toDouble();
    m_valveInfo.safePosition = m_settings.value("safePosition", "").toInt();
    m_valveInfo.driveType = m_settings.value("driveType", "").toInt();
    m_valveInfo.strokeMovement = m_settings.value("strokeMovement", "").toInt();
    m_valveInfo.toolNumber = m_settings.value("toolNumber", "").toInt();
    m_valveInfo.pulley = m_settings.value("pulley", "").toInt();
    m_valveInfo.CV = m_settings.value("CV", "").toInt();


    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return &m_valveInfo;
}

ValveInfo *Registry::GetValveInfo()
{
    return &m_valveInfo;
}

void Registry::SaveValveInfo()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    m_settings.setValue("last_position", m_valveInfo.positionNumber);

    m_settings.beginGroup(m_valveInfo.positionNumber);

    m_settings.setValue("manufacturer", m_valveInfo.manufacturer);
    m_settings.setValue("valveModel", m_valveInfo.valveModel);
    m_settings.setValue("serialNumber", m_valveInfo.serialNumber);
    m_settings.setValue("DN", m_valveInfo.DN);
    m_settings.setValue("PN", m_valveInfo.PN);
    m_settings.setValue("CV", m_valveInfo.CV);
    m_settings.setValue("stroke", m_valveInfo.stroke);
    m_settings.setValue("positioner", m_valveInfo.positioner);
    m_settings.setValue("dinamicError", m_valveInfo.dinamicError);
    m_settings.setValue("modelDrive", m_valveInfo.modelDrive);
    m_settings.setValue("range", m_valveInfo.range);
    m_settings.setValue("diameter", m_valveInfo.diameter);
    m_settings.setValue("safePosition", m_valveInfo.safePosition);
    m_settings.setValue("driveType", m_valveInfo.driveType);
    m_settings.setValue("strokeMovement", m_valveInfo.strokeMovement);
    m_settings.setValue("toolNumber", m_valveInfo.toolNumber);
    m_settings.setValue("pulley", m_valveInfo.pulley);
    m_settings.setValue("materialStuffingBoxSeal", m_materialsOfComponentParts.materialStuffingBoxSeal);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.sync();
}

MaterialsOfComponentParts *Registry::GetMaterialsOfComponentParts() {
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);
    m_settings.beginGroup(m_valveInfo.positionNumber);

    m_materialsOfComponentParts.materialStuffingBoxSeal = m_settings.value("materialStuffingBoxSeal", "").toString();
    m_materialsOfComponentParts.materialCorpus = m_settings.value("materialCorpus", "").toString();
    m_materialsOfComponentParts.materialCap = m_settings.value("materialCap", "").toString();
    m_materialsOfComponentParts.materialBall = m_settings.value("materialBall", "").toString();
    m_materialsOfComponentParts.materialDisk = m_settings.value("materialDisk", "").toString();
    m_materialsOfComponentParts.materialPlunger = m_settings.value("materialPlunger", "").toString();
    m_materialsOfComponentParts.materialShaft = m_settings.value("materialShaft", "").toString();
    m_materialsOfComponentParts.materialStock = m_settings.value("materialStock", "").toString();
    m_materialsOfComponentParts.materialGuideSleeve = m_settings.value("materialGuideSleeve", "").toString();

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();

    return &m_materialsOfComponentParts;
}

void Registry::SaveMaterialsOfComponentParts()
{
    const auto &mat = m_materialsOfComponentParts;

    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);

    QString pos = m_valveInfo.positionNumber;
    m_settings.beginGroup(pos);

    m_settings.setValue("materialStuffingBoxSeal", mat.materialStuffingBoxSeal);
    m_settings.setValue("materialCorpus", mat.materialCorpus);
    m_settings.setValue("materialCap", mat.materialCap);
    m_settings.setValue("materialBall", mat.materialBall);
    m_settings.setValue("materialDisk", mat.materialDisk);
    m_settings.setValue("materialPlunger", mat.materialPlunger);
    m_settings.setValue("materialShaft", mat.materialShaft);
    m_settings.setValue("materialStock", mat.materialStock);
    m_settings.setValue("materialGuideSleeve", mat.materialGuideSleeve);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.sync();
}

void Registry::SaveAccessoriesSeries()
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);
    m_settings.beginGroup(m_objectInfo.department);
    m_settings.beginGroup(m_valveInfo.positionNumber);

    m_settings.setValue("accessoryPlunger", m_accessoriesSeries.plunger);
    m_settings.setValue("accessorySaddle", m_accessoriesSeries.saddle);
    m_settings.setValue("accessoryBushing", m_accessoriesSeries.bushing);
    m_settings.setValue("accessoryORing", m_accessoriesSeries.oRing);
    m_settings.setValue("accessoryStuffingSeal", m_accessoriesSeries.stuffingSeal);
    m_settings.setValue("accessoryDiaphragm", m_accessoriesSeries.diaphragm);
    m_settings.setValue("accessoryCovers", m_accessoriesSeries.covers);
    m_settings.setValue("accessoryShaft", m_accessoriesSeries.shaft);
    m_settings.setValue("accessorySaddleLock",m_accessoriesSeries.saddleLock);

    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
    m_settings.endGroup();
}

AccessoriesSeries *Registry::GetAccessoriesSeries()
{
    return &m_accessoriesSeries;
}

OtherParameters *Registry::GetOtherParameters()
{
    return &m_otherParameters;
}

bool Registry::CheckObject(const QString &object)
{
    return m_settings.childGroups().contains(object);
}

bool Registry::CheckManufactory(const QString &manufactory)
{
    m_settings.beginGroup(m_objectInfo.object);

    bool result = m_settings.childGroups().contains(manufactory);

    m_settings.endGroup();

    return result;
}

bool Registry::CheckDepartment(const QString &department)
{
    m_settings.beginGroup(m_objectInfo.object);
    m_settings.beginGroup(m_objectInfo.manufactory);

    bool result = m_settings.childGroups().contains(department);

    m_settings.endGroup();
    m_settings.endGroup();

    return result;
}

bool Registry::CheckPosition(const QString &position)
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

QStringList Registry::GetPositions()
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

QString Registry::GetLastPosition()
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
