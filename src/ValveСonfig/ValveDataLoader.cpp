#include "ValveDataLoader.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <algorithm>

ValveDataLoader::ValveDataLoader(QObject *parent)
    : QObject(parent) {}

bool ValveDataLoader::loadFromFile(const QString &resourcePath) {
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << resourcePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Ошибка парсинга JSON";
        return false;
    }

    m_rootObj = doc.object();
    m_valveDataObj = m_rootObj.value("DN").toObject();
    return true;
}

QStringList ValveDataLoader::getManufacturers() const {
    return m_rootObj.value("manufacturer").toVariant().toStringList();
}

QStringList ValveDataLoader::getValveSeries() const {
    return m_rootObj.value("valveSeries").toVariant().toStringList();
}

QStringList ValveDataLoader::getValveModels() const {
    return m_rootObj.value("valveModel").toVariant().toStringList();
}

QStringList ValveDataLoader::getDriveModels() const {
    return m_rootObj.value("driveModel").toVariant().toStringList();
}

QStringList ValveDataLoader::getSaddleMaterials() const {
    return m_rootObj.value("saddleMaterials").toVariant().toStringList();
}

QStringList ValveDataLoader::getBodyMaterials() const {
    return m_rootObj.value("bodyMaterials").toVariant().toStringList();
}

QStringList ValveDataLoader::getDNList() const {
    QStringList dnKeys = m_valveDataObj.keys();
    std::sort(dnKeys.begin(), dnKeys.end(), [](const QString &a, const QString &b) {
        return a.toInt() < b.toInt();
    });
    return dnKeys;
}

QJsonObject ValveDataLoader::getValveData() const {
    return m_valveDataObj;
}
