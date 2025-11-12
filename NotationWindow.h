#ifndef NOTATIONWINDOW_H
#define NOTATIONWINDOW_H

#pragma once
#include <QDialog>
#include <QSignalBlocker>
#include "ReportSaver.h"

namespace Ui {
class NotationWindow;
}

class NotationWindow : public QDialog
{
    Q_OBJECT
public:
    explicit NotationWindow(Registry& registry, QWidget* parent = nullptr);
    ~NotationWindow() = default;
private slots:
    void onComboBoxIndexChanged(int index);
    void onCheckBoxChanged();
private:
    Ui::NotationWindow *ui = nullptr;
    Registry& m_registry;
    Sensors *m_sensors = nullptr;

    void syncUIFromSensors();
    void loadFromRegistry();
    void persist();
};


#endif // NOTATIONWINDOW_H
