#include "ValveDataLoader.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>

ValveDataLoader::ValveDataLoader(QObject *parent)
    : QObject(parent) {

    if (!loadFromFile(":/db_valveData/ValveConfig.json")) {
        return;
    }
}

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
    qDebug() << "Ключи DN в JSON:" << m_valveDataObj.keys();

    for (auto it = m_valveDataObj.constBegin(); it != m_valveDataObj.constEnd(); ++it) {
        const QString &dn = it.key();
        const QJsonArray arr = it.value().toArray();
        for (const QJsonValue &v : arr) {
            const QJsonObject o = v.toObject();
            double cv = o.value("CV").toDouble();
            const QJsonObject matsAll = o.value("materials").toObject();

            for (auto matIt = matsAll.constBegin(); matIt != matsAll.constEnd(); ++matIt) {
                const QString &saddleMat = matIt.key();
                const QJsonObject partObj = matIt.value().toObject();

                for (auto partIt = partObj.constBegin(); partIt != partObj.constEnd(); ++partIt) {
                    m_data[dn][cv][saddleMat][partIt.key()] = partIt.value().toString();
                }
            }
        }
    }

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

QMap<QString,QString> ValveDataLoader::materialsFor(
    const QString &dn,
    double cv,
    const QString &saddleMat) const
{
    QMap<QString,QString> out;
    const QJsonArray arr = m_valveDataObj.value(dn).toArray();
    for (auto v : arr) {
        const QJsonObject o = v.toObject();
        if (qFuzzyCompare(o.value("CV").toDouble() + 1.0, cv + 1.0)) {
            QJsonObject matsAll = o.value("materials").toObject();

            QJsonObject partObj;
            if (matsAll.contains(saddleMat) && matsAll.value(saddleMat).isObject()) {
                partObj = matsAll.value(saddleMat).toObject();
            } else {
                partObj = matsAll;
            }

            for (auto key : partObj.keys()) {
                out[key] = partObj.value(key).toString();
            }
            break;
        }
    }
    return out;
}


QStringList ValveDataLoader::cvListForDN(const QString &dn) const {
    auto mapCV = m_data.value(dn);
    QStringList out;
    for (auto cv : mapCV.keys())
        out << QString::number(cv);
    std::sort(out.begin(), out.end(),
              [](auto &a, auto &b){ return a.toDouble() < b.toDouble(); });
    return out;
}


const QMap<QString, QMap<double, QMap<QString, QMap<QString,QString>>>>& ValveDataLoader::data() const
{
    return m_data;
}
