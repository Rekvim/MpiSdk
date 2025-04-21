#ifndef NOTATIONWINDOW_H
#define NOTATIONWINDOW_H

#include <QDialog>
#include "FileSaver.h"

namespace Ui {
class NotationWindow;
}

class NotationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NotationWindow(QWidget *parent = nullptr);
    ~NotationWindow();

    // Добавляем метод fillReport в public, чтобы он был доступен извне:
    void fillReport();
    const FileSaver::Report &getReport() const { return report_; }

    // Геттеры для доступа к данным, если нужны:
    QString getPressureSensor1() const { return m_pressureSensor1; }
    QString getPressureSensor2() const { return m_pressureSensor2; }
    QString getPressureSensor3() const { return m_pressureSensor3; }
    QString getPressureSensor4() const { return m_pressureSensor4; }
    QString getLinearMotionSensor() const { return m_linearMotionSensor; }

private slots:
    void onComboBoxIndexChanged(int index);
    void onCheckBoxChanged();

private:
    Ui::NotationWindow *ui;
    FileSaver::Report report_;

    QString m_pressureSensor1;
    QString m_pressureSensor2;
    QString m_pressureSensor3;
    QString m_pressureSensor4;
    QString m_linearMotionSensor;
};

#endif // NOTATIONWINDOW_H
