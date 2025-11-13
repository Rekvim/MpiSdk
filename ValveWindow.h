#ifndef VALVEWINDOW_H
#define VALVEWINDOW_H

#pragma once
#include <QDialog>
#include <QJsonObject>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QComboBox>
#include "Registry.h"
#include "./Src/Database/ValveDatabase.h"

namespace Ui {
class ValveWindow;
}

class ValveWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ValveWindow(Registry &registry, ValveDatabase db, QWidget *parent = nullptr);
    ~ValveWindow() = default;

private:
    void loadFromRegistry();
    void syncUIFromRegistry();

    void saveValveInfo();
    void saveMaterialsOfComponentParts();
    void saveListDetails();

    void populateModelsAndDn(int seriesId);
    void refreshCvForDn(std::optional<int> dnIdOpt);
    void setDnCvManualMode(bool manual);
    void setSaddleManualMode(bool manual);

    void ensureSaddleManualOption();
    inline bool isManual(const QComboBox* cb) const {
        return cb->currentText() == kManualInput;
    }

    std::optional<int> currentSeriesId() const;
    std::optional<int> resolveDnIdFromManual() const;
    std::optional<int> resolveCvIdFromManual(int dnSizeId) const;
    std::optional<int> currentModelId() const;
    std::optional<int> resolveSaddleMaterialIdFromManual() const;

    const QString kManualInput = tr("Ручной ввод");
    Ui::ValveWindow *ui;

    Registry &m_registry;
    ValveInfo *m_valveInfo;
    MaterialsOfComponentParts *m_materialsOfComponentParts;
    ListDetails *m_listDetails;

    ValveDatabase m_db;
    QMap<QString, QLineEdit*> m_partFields;
    QList<QString> m_diameter = {QStringLiteral("50.0"), QStringLiteral("86.0"), QStringLiteral("108.0"), QStringLiteral("125.0")};

private slots:
    void onSeriesEditingFinished();
    void onModelEditingFinished();

    void onDNChanged(int index);
    void onCVChanged(int index);

    void onManufacturerChanged(int index);
    void onPositionerTypeChanged(int index);

    void positionChanged(const QString &position);
    void updatePartNumbers();
    void strokeChanged(quint16 n);
    void toolChanged(quint16 n);
    void diameterChanged(const QString &text);

    void on_pushButton_clicked();
    void on_pushButton_clear_clicked();

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
            cb->addItem(kManualInput);
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
            cb->addItem(kManualInput);
        }
    }
};

#endif // VALVEWINDOW_H
