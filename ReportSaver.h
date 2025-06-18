#ifndef REPORTSAVER_H
#define REPORTSAVER_H

#pragma once
#include <QDir>
#include <QObject>

#include "./Src/CustomChart/MyChart.h"
#include "Registry.h"

class ReportSaver : public QObject
{
    Q_OBJECT
public:

    struct ExcelData
    {
        quint8 x;
        quint8 y;
        QString text;
    };

    struct ValidationData
    {
        QString formula;
        QString range;
    };

    struct Report
    {
        QVector<ExcelData> data;
        QVector<ValidationData> validation;
        QImage image1;
        QImage image2;
        QImage image3;
    };

    explicit ReportSaver(QObject *parent = nullptr);
    bool SaveReport(const Report &report);
    void SaveImage(MyChart *chart);

    void SetRegistry(Registry *registry);
    QDir Directory();

private:
    void CreateDir();
    QDir m_dir;
    bool m_created;
    Registry *m_registry;
signals:
    void Question(QString title, QString text, bool &result);
    void GetDirectory(QString current_path, QString &result);
};

#endif // FILESAVER_H
