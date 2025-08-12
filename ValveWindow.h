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
#include "./Src/ValveConfig/ValveDataLoader.h"
#include "./Src/database/ValveDatabase.h"

namespace Ui {
class ValveWindow;
}

class ValveWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ValveWindow(ValveDatabase& db, QWidget *parent = nullptr);
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
    ValveDatabase& m_db;
    QMap<QString, QLineEdit*> m_partFields;
    const QString m_manualInput = "Ручной ввод";
    static constexpr qreal m_diameter[] = {50.0, 86.0, 108.0, 125.0};

private slots:
    void onSeriesEditingFinished();
    void onModelEditingFinished();

    void onDNChanged(int index);
    void onManufacturerChanged(int index);
    void onPositionerTypeChanged(int index);

    void PositionChanged(const QString &position);
    void ButtonClick();
    void updatePartNumbers();
    void StrokeChanged(quint16 n);
    void ToolChanged(quint16 n);
    void DiameterChanged(const QString &text);
    void Clear();

protected:
    // Заполнение QComboBox из контейнера пар <int, QString>
    template<typename Container>
    void populateCombo(QComboBox* cb, const Container& items, bool includeManual = false)
    {
        cb->clear();
        for (const auto& [id, text] : items) {
            cb->addItem(text, id);
        }
        if (includeManual) {
            cb->addItem(m_manualInput);
        }
    }

    // Заполнение QComboBox из контейнера пар <int, int>
    template<typename Container>
    void populateComboInts(QComboBox* cb, const Container& items, bool includeManual = false)
    {
        cb->clear();
        for (const auto& [id, val] : items) {
            cb->addItem(QString::number(val), id);
        }
        if (includeManual) {
            cb->addItem(m_manualInput);
        }
    }
};

#endif // VALVEWINDOW_H
