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
    explicit ObjectWindow(QWidget *parent = nullptr);
    void LoadFromReg(Registry *registry);
    ~ObjectWindow();

private:

    Ui::ObjectWindow *ui;
    Registry *m_registry;
    ObjectInfo *m_objectInfo;

    void SaveObjectInfo();
private slots:
    void ButtonClick();
};

#endif // OBJECTWINDOW_H
