#ifndef VALVEDATALOADER_H
#define VALVEDATALOADER_H

#pragma once
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct Variant {
    double cv;
    // по материалу седла: saddleMaterial → (partKey → partNumber)
    QMap<QString,QMap<QString,QString>> materials;
};

class ValveDataLoader : public QObject
{
    Q_OBJECT

public:
    explicit ValveDataLoader(QObject *parent = nullptr);
    bool loadFromFile(const QString &resourcePath);

    QStringList cvListForDN(const QString &dn) const;
    QMap<QString,QString> materialsFor(
        const QString &dn,
        double cv,
        const QString &saddleMat) const;
    const QMap<QString, QMap<double, QMap<QString, QMap<QString,QString>>>>& data() const;

    QStringList getManufacturers() const;
    QStringList getValveSeries() const;
    QStringList getValveModels() const;
    QStringList getDriveModels() const;
    QStringList getSaddleMaterials() const;
    QStringList getBodyMaterials() const;
    QStringList getDNList() const;



    QJsonObject getValveData() const;
private:
    QMap<QString, QMap<
                      double,
                      QMap<
                          QString,                  // saddleMaterial
                          QMap<QString, QString>    // partKey → partNumber
                          >
                      >> m_data;
    QJsonObject m_rootObj;
    QJsonObject m_valveDataObj;
};

#endif // VALVEDATALOADER_H
