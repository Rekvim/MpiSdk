#include "NotationWindow.h"
#include "ui_NotationWindow.h"
#include "ReportSaver.h"

NotationWindow::NotationWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotationWindow)
    , m_settings("MPPI", "Data")
{
    ui->setupUi(this);

    loadSettings();

    connect(ui->entry_testing, &QPushButton::clicked, this, &NotationWindow::accept);

    onCheckBoxChanged();

    connect(ui->comboBox_movings, &QComboBox::currentIndexChanged, this, &NotationWindow::onComboBoxIndexChanged);
    connect(ui->checkBox_pressure_1, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressure_2, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressure_3, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressure_4, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);

    onComboBoxIndexChanged(ui->comboBox_movings->currentIndex());
}

void NotationWindow::loadSettings()
{
    m_settings.beginGroup("NotationWindow");

    int idx = m_settings.value("comboIndex", 0).toInt();
    ui->comboBox_movings->setCurrentIndex(idx);

    ui->checkBox_pressure_1->setChecked(
        m_settings.value("pressure1", false).toBool());
    ui->checkBox_pressure_2->setChecked(
        m_settings.value("pressure2", false).toBool());
    ui->checkBox_pressure_3->setChecked(
        m_settings.value("pressure3", false).toBool());
    ui->checkBox_pressure_4->setChecked(
        m_settings.value("pressure4", false).toBool());

    m_settings.endGroup();

    // После установки checked-настроек обновляем поля:
    m_pressureSensor1 = ui->checkBox_pressure_1->isChecked()
                            ? ui->checkBox_pressure_1->text() : "";
    m_pressureSensor2 = ui->checkBox_pressure_2->isChecked()
                            ? ui->checkBox_pressure_2->text() : "";
    m_pressureSensor3 = ui->checkBox_pressure_3->isChecked()
                            ? ui->checkBox_pressure_3->text() : "";
    m_pressureSensor4 = ui->checkBox_pressure_4->isChecked()
                            ? ui->checkBox_pressure_4->text() : "";
    // То же для линейного сенсора, если нужно:
    m_linearMotionSensor = ui->comboBox_movings->currentText();

}

void NotationWindow::saveSettings()
{
    m_settings.beginGroup("NotationWindow");

    m_settings.setValue("comboIndex",
                        ui->comboBox_movings->currentIndex());

    m_settings.setValue("pressure1",
                        ui->checkBox_pressure_1->isChecked());
    m_settings.setValue("pressure2",
                        ui->checkBox_pressure_2->isChecked());
    m_settings.setValue("pressure3",
                        ui->checkBox_pressure_3->isChecked());
    m_settings.setValue("pressure4",
                        ui->checkBox_pressure_4->isChecked());

    m_settings.endGroup();
}

void NotationWindow::onComboBoxIndexChanged(int index)
{
    m_linearMotionSensor = ui->comboBox_movings->itemText(index);
        saveSettings();
}

void NotationWindow::onCheckBoxChanged()
{
    m_pressureSensor1 = ui->checkBox_pressure_1->isChecked() ? ui->checkBox_pressure_1->text() : "";
    m_pressureSensor2 = ui->checkBox_pressure_2->isChecked() ? ui->checkBox_pressure_2->text() : "";
    m_pressureSensor3 = ui->checkBox_pressure_3->isChecked() ? ui->checkBox_pressure_3->text() : "";
    m_pressureSensor4 = ui->checkBox_pressure_4->isChecked() ? ui->checkBox_pressure_4->text() : "";
    saveSettings();
}

void NotationWindow::SetRegistry(Registry *registry)
{
    m_registry = registry;
}

void NotationWindow::fillReport(ReportSaver::Report &report) {
    qDebug() << "PressureSensor1:" << getPressureSensor1();
    qDebug() << "PressureSensor2:" << getPressureSensor2();
    qDebug() << "PressureSensor3:" << getPressureSensor3();
    qDebug() << "PressureSensor4:" << getPressureSensor4();
    qDebug() << "LinearMotion:" << getLinearMotionSensor();


    if (!getPressureSensor1().isEmpty()) {
        report.data.push_back({67, 5, getPressureSensor1()});
    }

    if (!getPressureSensor2().isEmpty()) {
        report.data.push_back({68, 5, getPressureSensor2()});

    }
    if (!getPressureSensor3().isEmpty()) {
        report.data.push_back({69, 5, getPressureSensor3()});
    }
    if (!getPressureSensor4().isEmpty()) {
        report.data.push_back({70, 5, getPressureSensor4()});

    }
    if (!getLinearMotionSensor().isEmpty()) {
        report.data.push_back({71, 5, getLinearMotionSensor()});
    }
}


NotationWindow::~NotationWindow()
{
    delete ui;
}
