#include "OtherTestSettings.h"
#include "ui_OtherTestSettings.h"

OtherTestSettings::OtherTestSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OtherTestSettings)
{
    ui->setupUi(this);

    ui->pushButton_change_value->setEnabled(false);
    ui->pushButton_delete_value->setEnabled(false);
    ui->pushButton_change_step->setEnabled(false);
    ui->pushButton_delete_step->setEnabled(false);

    connect(ui->listWidget_value, &QListWidget::currentRowChanged, this, [=](int v) {
        ui->pushButton_change_value->setEnabled(v >= 0);
        ui->pushButton_delete_value->setEnabled((v >= 0) && (ui->listWidget_value->count() > 1));
    });

    connect(ui->pushButton_add_value, &QPushButton::clicked, this, [=]() {
        ui->listWidget_value->addItem("50.0");
        ui->listWidget_value->setCurrentRow(ui->listWidget_value->count() - 1);
    });

    connect(ui->pushButton_delete_value, &QPushButton::clicked, this, [=]() {
        delete ui->listWidget_value->currentItem();

        if (ui->listWidget_value->count() == 1) {
            ui->pushButton_delete_value->setEnabled(false);
        }

        ui->listWidget_value->setCurrentRow(-1);
    });

    connect(ui->pushButton_change_value, &QPushButton::clicked, this, [=]() {
        bool ok;
        double d = QInputDialog::getDouble(this,
                                           "Ввод числа",
                                           "Значение:",
                                           ui->listWidget_value->currentItem()->text().toDouble(),
                                           0.0,
                                           100.0,
                                           1,
                                           &ok);

        if (ok) {
            ui->listWidget_value->currentItem()->setText(QString::asprintf("%.1f", d));
        }
    });

    connect(ui->listWidget_step, &QListWidget::currentRowChanged, this, [=](int v) {
        ui->pushButton_change_step->setEnabled(v >= 0);
        ui->pushButton_delete_step->setEnabled((v >= 0) && (ui->listWidget_step->count() > 1));
    });

    connect(ui->pushButton_add_step, &QPushButton::clicked, this, [=]() {
        ui->listWidget_step->addItem("3.0");
        ui->listWidget_step->setCurrentRow(ui->listWidget_step->count() - 1);
    });

    connect(ui->pushButton_delete_step, &QPushButton::clicked, this, [=]() {
        delete ui->listWidget_step->currentItem();

        if (ui->listWidget_step->count() == 1) {
            ui->pushButton_delete_step->setEnabled(false);
        }

        ui->listWidget_step->setCurrentRow(-1);
    });

    connect(ui->pushButton_change_step, &QPushButton::clicked, this, [=]() {
        bool ok;
        double d = QInputDialog::getDouble(this,
                                           "Ввод числа",
                                           "Значение:",
                                           ui->listWidget_step->currentItem()->text().toDouble(),
                                           0.0,
                                           100.0,
                                           1,
                                           &ok);

        if (ok) {
            ui->listWidget_step->currentItem()->setText(QString::asprintf("%.1f", d));
        }
    });

    connect(ui->timeEdit, &QTimeEdit::timeChanged, this, [&](QTime time) {
        if (time > m_maxTime) {
            ui->timeEdit->setTime(m_maxTime);
        }
        if (time < m_minTime) {
            ui->timeEdit->setTime(m_minTime);
        }
    });
}

OtherTestSettings::~OtherTestSettings()
{
    delete ui;
}

void OtherTestSettings::reverse()
{
    ui->listWidget_value->clear();
    ui->listWidget_value->addItem("75.0");
    ui->listWidget_value->addItem("50.0");
    ui->listWidget_value->addItem("25.0");
}

OtherTestSettings::TestParameters OtherTestSettings::getParameters() const
{
    TestParameters testParameters;

    testParameters.delay = ui->timeEdit->time().msecsSinceStartOfDay();

    for (int i = 0; i < ui->listWidget_value->count(); i++) {
        testParameters.points.append(ui->listWidget_value->item(i)->text().toDouble());
    }

    for (int i = 0; i < ui->listWidget_step->count(); i++) {
        testParameters.steps.append(ui->listWidget_step->item(i)->text().toDouble());
    }

    return testParameters;
}

qint64 OtherTestSettings::totalTestTimeMillis() const
{
    TestParameters params = getParameters();

    int P = params.points.size(); // количество точек
    int S = params.steps.size();  // количество шагов
    qint64 delay = params.delay;  // задержка в миллисекундах

    // N_values = 1 старт + 2 прохода по всем точкам (каждый: 1 базовая + S шагов)
    qint64 N_values = 1 + 2 * P * (1 + S);

    // total = 10 сек на первый уровень + delay на каждый элемент массива
    return 10000 + N_values * delay;
}
