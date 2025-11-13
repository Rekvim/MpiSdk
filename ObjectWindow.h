#ifndef OBJECTWINDOW_H
#define OBJECTWINDOW_H

#pragma once
#include <QDialog>
#include <QMessageBox>
#include <QScreen>

#include "Registry.h"

namespace Ui {
class ObjectWindow;
}

class ObjectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectWindow(Registry &registry, QWidget *parent = nullptr);
    ~ObjectWindow() = default;

private:
    Ui::ObjectWindow *ui;
    Registry& m_registry;
    ObjectInfo *m_objectInfo;

    void loadFromRegistry();
    void syncUIFromObjectInfo();
    void SaveObjectInfo();

private slots:
    void on_pushButton_clicked();
};

#endif // OBJECTWINDOW_H
