#ifndef OBJECTWINDOW_H
#define OBJECTWINDOW_H

#include <QDialog>
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
    Registry *registry_;
    ObjectInfo *object_info_;
    Ui::ObjectWindow *ui;
    void SaveObjectInfo();
private slots:
    void ButtonClick();
};

#endif // OBJECTWINDOW_H
