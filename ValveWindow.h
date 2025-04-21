#ifndef VALVEWINDOW_H
#define VALVEWINDOW_H

#include <QDialog>
#include <QJsonObject>
#include "Registry.h"

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

private:
    void SaveValveInfo();

    Ui::ValveWindow *ui;
    Registry *m_registry;
    ValveInfo *m_valveInfo;
    QJsonObject m_valveDataObj;
    const QString m_manualInput = "Ручной ввод";
    static constexpr qreal m_diameter[] = {50.0, 86.0, 108.0, 125.0};
private slots:
    void on_comboBox_DN_currentIndexChanged(const QString &dn);
    void PositionChanged(const QString &position);
    void ButtonClick();
    void StrokeChanged(quint16 n);
    void ToolChanged(quint16 n);
    void DiameterChanged(qreal value);
    void Clear();
};

#endif // VALVEWINDOW_H
