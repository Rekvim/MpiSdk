#ifndef VALVEWINDOW_H
#define VALVEWINDOW_H

#pragma once
#include <QDialog>
#include <QJsonObject>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>

#include "Registry.h"
#include "ReportSaver.h"
#include "./src/ValveСonfig/ValveDataLoader.h"
#include "./Src/database/ValveDatabase.h"

namespace Ui {
class ValveWindow;
}

class ValveWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ValveWindow(QWidget *parent = nullptr);
    ~ValveWindow();
    void SetRegistry(Registry *registry);
    void fillReport(ReportSaver::Report &report);

private:
    void SaveValveInfo();
    Ui::ValveWindow *ui;

    void populateModelsAndDn(int seriesId);

    Registry *m_registry;
    ValveInfo *m_valveInfo;
    MaterialsOfComponentParts *m_materialsOfComponentParts;
    QJsonObject m_valveDataObj;
    ValveDataLoader m_loader;
    ValveDatabase m_db;
    QMap<QString, QLineEdit*> m_partFields;
    const QString m_manualInput = "Ручной ввод";
    static constexpr qreal m_diameter[] = {50.0, 86.0, 108.0, 125.0};

private slots:
    void onSeriesEditingFinished();
    void onDNChanged(int index);
    void PositionChanged(const QString &position);
    void ButtonClick();
    void updatePartNumbers();
    void StrokeChanged(quint16 n);
    void ToolChanged(quint16 n);
    void DiameterChanged(qreal value);
    void Clear();
};

#endif // VALVEWINDOW_H
