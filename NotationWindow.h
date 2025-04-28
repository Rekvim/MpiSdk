#ifndef NOTATIONWINDOW_H
#define NOTATIONWINDOW_H

#pragma once
#include <QDialog>
#include <QSettings>
#include "ReportSaver.h"

namespace Ui {
class NotationWindow;
}

class NotationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NotationWindow(QWidget *parent = nullptr);
    ~NotationWindow();

    void SetRegistry(Registry *registry);

    void fillReport(ReportSaver::Report &report);

    QString getPressureSensor1() const { return m_pressureSensor1; }
    QString getPressureSensor2() const { return m_pressureSensor2; }
    QString getPressureSensor3() const { return m_pressureSensor3; }
    QString getPressureSensor4() const { return m_pressureSensor4; }
    QString getLinearMotionSensor() const { return m_linearMotionSensor; }

private slots:
    void onComboBoxIndexChanged(int index);
    void onCheckBoxChanged();
    // void onEntryTestingClicked();

private:
    Ui::NotationWindow *ui;
    ReportSaver::Report m_report;
    QSettings m_settings;
    Registry *m_registry;

    void loadSettings();
    void saveSettings();

    QString m_pressureSensor1;
    QString m_pressureSensor2;
    QString m_pressureSensor3;
    QString m_pressureSensor4;
    QString m_linearMotionSensor;
};

#endif // NOTATIONWINDOW_H
