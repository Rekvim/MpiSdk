#include "NotationWindow.h"
#include "ui_NotationWindow.h"
#include "FileSaver.h"

NotationWindow::NotationWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotationWindow)
{
    ui->setupUi(this);
    connect(ui->entry_testing, &QPushButton::clicked, this, &NotationWindow::accept);


    connect(ui->comboBox_movings, &QComboBox::currentIndexChanged, this, &NotationWindow::onComboBoxIndexChanged);
    connect(ui->checkBox_pressure_1, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressure_2, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressure_3, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);
    connect(ui->checkBox_pressure_4, &QCheckBox::toggled, this, &NotationWindow::onCheckBoxChanged);

    onComboBoxIndexChanged(ui->comboBox_movings->currentIndex());
}

void NotationWindow::onComboBoxIndexChanged(int index)
{
    m_linearMotionSensor = ui->comboBox_movings->itemText(index);
}

void NotationWindow::onCheckBoxChanged()
{
    m_pressureSensor1 = ui->checkBox_pressure_1->isChecked() ? ui->checkBox_pressure_1->text() : "";
    m_pressureSensor2 = ui->checkBox_pressure_2->isChecked() ? ui->checkBox_pressure_2->text() : "";
    m_pressureSensor3 = ui->checkBox_pressure_3->isChecked() ? ui->checkBox_pressure_3->text() : "";
    m_pressureSensor4 = ui->checkBox_pressure_4->isChecked() ? ui->checkBox_pressure_4->text() : "";
}

// Добавление данных в генерацию отчета
void NotationWindow::fillReport() {
    qDebug() << "PressureSensor1:" << getPressureSensor1();
    qDebug() << "PressureSensor2:" << getPressureSensor2();
    qDebug() << "PressureSensor3:" << getPressureSensor3();
    qDebug() << "PressureSensor4:" << getPressureSensor4();
    qDebug() << "LinearMotion:" << getLinearMotionSensor();


    if (!getPressureSensor1().isEmpty()) {
        QLineEdit *temp = new QLineEdit;
        temp->setText(getPressureSensor1());
        report_.data.push_back({67, 5, temp});
    }

    if (!getPressureSensor2().isEmpty()) {
        QLineEdit *temp = new QLineEdit;
        temp->setText(getPressureSensor2());
        report_.data.push_back({68, 5, temp});
    }
    if (!getPressureSensor3().isEmpty()) {
        QLineEdit *temp = new QLineEdit;
        temp->setText(getPressureSensor3());
        report_.data.push_back({69, 5, temp});
    }
    if (!getPressureSensor4().isEmpty()) {
        QLineEdit *temp = new QLineEdit;
        temp->setText(getPressureSensor4());
        report_.data.push_back({70, 5, temp});
    }
    if (!getLinearMotionSensor().isEmpty()) {
        QLineEdit *temp = new QLineEdit;
        temp->setText(getLinearMotionSensor());
        report_.data.push_back({71, 5, temp});
    }
}


NotationWindow::~NotationWindow()
{
    delete ui;
}
