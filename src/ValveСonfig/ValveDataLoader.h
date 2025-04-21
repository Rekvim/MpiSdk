#ifndef VALVEDATALOADER_H
#define VALVEDATALOADER_H

#include <QObject>
#include <QString>

#include <QJsonObject>
#include <QJsonArray>

class ValveDataLoader : public QObject
{
    Q_OBJECT

public:
    explicit ValveDataLoader(QObject *parent = nullptr);
    bool loadFromFile(const QString &resourcePath);

    QStringList getManufacturers() const;
    QStringList getValveSeries() const;
    QStringList getValveModels() const;
    QStringList getDriveModels() const;
    QStringList getSaddleMaterials() const;
    QStringList getBodyMaterials() const;
    QStringList getDNList() const;

    QJsonObject getValveData() const;
private:
    QJsonObject m_rootObj;
    QJsonObject m_valveDataObj;
};

#endif // VALVEDATALOADER_H
