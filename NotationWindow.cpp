#include "NotationWindow.h"
#include "ui_NotationWindow.h"

NotationWindow::NotationWindow(Registry &registry, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotationWindow)
    , m_registry(registry)
{
    ui->setupUi(this);

    loadFromRegistry();
    syncUIFromSensors();

    connect(ui->entry_testing, &QPushButton::clicked, this, [this]{
        persist();
        accept();
    });

    connect(ui->comboBox_movings, &QComboBox::currentIndexChanged,
            this, &NotationWindow::onComboBoxIndexChanged);

    connect(ui->checkBox_pressureSensor_1, &QCheckBox::toggled,
            this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressureSensor_2, &QCheckBox::toggled,
            this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressureSensor_3, &QCheckBox::toggled,
            this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressureSensor_4, &QCheckBox::toggled,
            this, &NotationWindow::onCheckBoxChanged);
}

void NotationWindow::loadFromRegistry()
{
    m_sensors = m_registry.getSensors();
    Q_ASSERT(m_sensors);
}

void NotationWindow::persist()
{
    m_registry.saveSensors();
}

void NotationWindow::onCheckBoxChanged()
{
    auto apply = [](QString &field, const QCheckBox *cb)
    {
        if (!cb->isChecked())
            field.clear();
        else if (field.isEmpty())
            field = cb->text();
    };

    apply(m_sensors->Pressure1, ui->checkBox_pressureSensor_1);
    apply(m_sensors->Pressure2, ui->checkBox_pressureSensor_2);
    apply(m_sensors->Pressure3, ui->checkBox_pressureSensor_3);
    apply(m_sensors->Pressure4, ui->checkBox_pressureSensor_4);
}

void NotationWindow::onComboBoxIndexChanged(int index)
{
    m_sensors->Motion = ui->comboBox_movings->itemText(index);
}

void NotationWindow::syncUIFromSensors()
{
    QSignalBlocker b1(ui->checkBox_pressureSensor_1);
    QSignalBlocker b2(ui->checkBox_pressureSensor_2);
    QSignalBlocker b3(ui->checkBox_pressureSensor_3);
    QSignalBlocker b4(ui->checkBox_pressureSensor_4);
    QSignalBlocker b5(ui->comboBox_movings);

    ui->checkBox_pressureSensor_1->setChecked(!m_sensors->Pressure1.isEmpty());
    ui->checkBox_pressureSensor_2->setChecked(!m_sensors->Pressure2.isEmpty());
    ui->checkBox_pressureSensor_3->setChecked(!m_sensors->Pressure3.isEmpty());
    ui->checkBox_pressureSensor_4->setChecked(!m_sensors->Pressure4.isEmpty());

    const int idx = ui->comboBox_movings->findText(
        m_sensors->Motion, Qt::MatchFixedString);
    ui->comboBox_movings->setCurrentIndex(idx >= 0 ? idx : 0);
}
