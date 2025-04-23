#include <QMessageBox>
#include "ui_ValveWindow.h"
#include "ValveWindow.h"
#include "./src/ValveСonfig/ValveDataLoader.h"

ValveWindow::ValveWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ValveWindow)
{
    ui->setupUi(this);

    QRegularExpression regular;
    regular.setPattern("[^/?*:;{}\\\\]+");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regular, this);

    ui->lineEdit_positionNumber->setValidator(validator);
    ui->lineEdit_manufacturer->setValidator(validator);
    ui->lineEdit_serial->setValidator(validator);
    ui->lineEdit_PN->setValidator(validator);
    ui->lineEdit_stroke->setValidator(validator);
    ui->lineEdit_positioner->setValidator(validator);
    ui->lineEdit_dinamicError->setValidator(validator);
    ui->lineEdit_modelDrive->setValidator(validator);
    ui->lineEdit_range->setValidator(validator);
    ui->lineEdit_materialStuffingBoxSeal->setValidator(validator);
    ui->lineEdit_material_1->setValidator(validator);
    ui->lineEdit_material_2->setValidator(validator);
    ui->lineEdit_material_4->setValidator(validator);
    ui->lineEdit_material_5->setValidator(validator);
    ui->lineEdit_material_6->setValidator(validator);
    ui->lineEdit_material_7->setValidator(validator);
    ui->lineEdit_material_8->setValidator(validator);
    ui->lineEdit_material_9->setValidator(validator);

    ui->doubleSpinBox_diameter_pulley->setValue(m_diameter[0]);

    connect(ui->comboBox_stroke_movement,
            &QComboBox::currentIndexChanged,
            this,
            &ValveWindow::StrokeChanged);
    connect(ui->comboBox_toolNumber,
            &QComboBox::currentIndexChanged,
            this,
            &ValveWindow::ToolChanged);
    connect(ui->doubleSpinBox_diameter,
            &QDoubleSpinBox::valueChanged,
            this,
            &ValveWindow::DiameterChanged);

    connect(ui->pushButton, &QPushButton::clicked, this, &ValveWindow::ButtonClick);
    connect(ui->pushButton_clear, &QPushButton::clicked, this, &ValveWindow::Clear);

    DiameterChanged(ui->doubleSpinBox_diameter->value());

    ValveDataLoader loader;
    if (!loader.loadFromFile(":/db_valveData/ValveConfig.json")) {
        return;
    }

    ui->comboBox_manufacturer->clear();
    ui->comboBox_manufacturer->addItems(loader.getManufacturers());

    ui->comboBox_valveModel->clear();
    ui->comboBox_valveModel->addItems(loader.getValveModels());

    ui->comboBox_driveModel->clear();
    ui->comboBox_driveModel->addItems(loader.getDriveModels());

    ui->comboBox_saddleMaterials->clear();
    ui->comboBox_saddleMaterials->addItems(loader.getSaddleMaterials());

    ui->comboBox_materialBody->clear();
    ui->comboBox_materialBody->addItems(loader.getBodyMaterials());

    ui->comboBox_DN->clear();
    ui->comboBox_DN->addItems(loader.getDNList());

    // Сохраняем для будущей логики
    m_valveDataObj = loader.getValveData();

    // Инициализация первого значения
    if (ui->comboBox_DN->count() > 0) {
        ui->comboBox_DN->setCurrentIndex(0);
        on_comboBox_DN_currentIndexChanged(ui->comboBox_DN->currentText());
    }

    QJsonArray variantsArray = m_valveDataObj.value(ui->comboBox_DN->currentText()).toArray();

    ui->comboBox_CV->clear();
    // Вывод CV из DN
    for (const QJsonValue &val : variantsArray) {
        QJsonObject variant = val.toObject();
        double cvValue = variant.value("CV").toDouble();
        ui->comboBox_CV->addItem(QString::number(cvValue));
    }
}

ValveWindow::~ValveWindow()
{
    delete ui;
}

void ValveWindow::on_comboBox_DN_currentIndexChanged(const QString &dn)
{
    ui->comboBox_CV->clear();
    if (!m_valveDataObj.contains(dn))
        return;

    // 1) получаем массив вариантов для этого DN
    QJsonArray variants = m_valveDataObj.value(dn).toArray();

    // 2) пробегаем по каждому объекту и достаём CV
    for (const QJsonValue &vv : variants) {
        QJsonObject obj = vv.toObject();
        // Если JSON у тебя ключит "Cv" с маленькой v — используй именно "Cv"
        double cv = obj.value("CV").toDouble();
        ui->comboBox_CV->addItem(QString::number(cv));
    }
}


void ValveWindow::SetRegistry(Registry *registry)
{
    m_registry = registry;

    ui->comboBox_positionNumber->clear();
    ui->comboBox_positionNumber->addItems(m_registry->GetPositions());
    ui->comboBox_positionNumber->addItem(m_manualInput);

    QString last_position = m_registry->GetLastPosition();
    if (last_position == "") {
        ui->comboBox_positionNumber->setCurrentIndex(ui->comboBox_positionNumber->count() - 1);
    } else {
        ui->comboBox_positionNumber->setCurrentIndex(ui->comboBox_positionNumber->findText(last_position));
        PositionChanged(last_position);
    }

    connect(ui->comboBox_positionNumber,
            &QComboBox::currentTextChanged,
            this,
            &ValveWindow::PositionChanged);
}

void ValveWindow::SaveValveInfo()
{
    if (ui->comboBox_positionNumber->currentText() == m_manualInput)
        m_valveInfo = m_registry->GetValveInfo(ui->lineEdit_positionNumber->text());

    m_valveInfo->manufacturer = ui->lineEdit_manufacturer->text();
    m_valveInfo->serialNumber = ui->lineEdit_serial->text();
    m_valveInfo->PN = ui->lineEdit_PN->text();
    m_valveInfo->stroke = ui->lineEdit_stroke->text();
    m_valveInfo->positioner = ui->lineEdit_positioner->text();
    m_valveInfo->dinamicError = ui->lineEdit_dinamicError->text();
    m_valveInfo->modelDrive = ui->lineEdit_modelDrive->text();
    m_valveInfo->range = ui->lineEdit_range->text();
    m_valveInfo->materialStuffingBoxSeal = ui->lineEdit_materialStuffingBoxSeal->text();
    m_valveInfo->manufacturer = ui->lineEdit_manufacturer->text();

    m_valveInfo->diameter = ui->doubleSpinBox_diameter->value();
    m_valveInfo->pulley = ui->doubleSpinBox_diameter_pulley->value();

    m_valveInfo->DN = ui->comboBox_DN->currentIndex();
    m_valveInfo->valveModel = ui->comboBox_valveModel->currentText();
    m_valveInfo->safePosition = ui->comboBox_safePosition->currentIndex();
    m_valveInfo->driveType = ui->comboBox_driveType->currentIndex();
    m_valveInfo->strokeMovement = ui->comboBox_stroke_movement->currentIndex();
    m_valveInfo->toolNumber = ui->comboBox_toolNumber->currentIndex();

    m_valveInfo->material1 = ui->lineEdit_material_1->text();
    m_valveInfo->material2 = ui->lineEdit_material_2->text();
    // m_valveInfo->material3 = ui->lineEdit_material_3->text();
    m_valveInfo->material4 = ui->lineEdit_material_4->text();
    m_valveInfo->material5 = ui->lineEdit_material_5->text();
    m_valveInfo->material6 = ui->lineEdit_material_6->text();
    m_valveInfo->material7 = ui->lineEdit_material_7->text();
    m_valveInfo->material8 = ui->lineEdit_material_8->text();
    m_valveInfo->material9 = ui->lineEdit_material_9->text();

    m_registry->SaveValveInfo();
}

void ValveWindow::PositionChanged(const QString &position)
{
    if (position == m_manualInput) {
        ui->lineEdit_positionNumber->setEnabled(true);
        return;
    }

    m_valveInfo = m_registry->GetValveInfo(position);

    ui->lineEdit_positionNumber->setText(position);
    ui->lineEdit_positionNumber->setEnabled(false);

    ui->lineEdit_manufacturer->setText(m_valveInfo->manufacturer);
    ui->lineEdit_serial->setText(m_valveInfo->serialNumber);
    ui->lineEdit_PN->setText(m_valveInfo->PN);
    ui->lineEdit_stroke->setText(m_valveInfo->stroke);
    ui->lineEdit_positioner->setText(m_valveInfo->positioner);
    ui->lineEdit_dinamicError->setText(m_valveInfo->dinamicError);
    ui->lineEdit_modelDrive->setText(m_valveInfo->modelDrive);
    ui->lineEdit_range->setText(m_valveInfo->range);
    ui->lineEdit_materialStuffingBoxSeal->setText(m_valveInfo->materialStuffingBoxSeal);

    ui->doubleSpinBox_diameter->setValue(m_valveInfo->diameter);

    ui->comboBox_DN->setCurrentIndex(m_valveInfo->DN);
    // ui->comboBox_valveModel->currentText(m_valveInfo->valveModel);

    ui->comboBox_safePosition->setCurrentIndex(m_valveInfo->safePosition);
    ui->comboBox_driveType->setCurrentIndex(m_valveInfo->driveType);
    ui->comboBox_stroke_movement->setCurrentIndex(m_valveInfo->strokeMovement);
    ui->comboBox_toolNumber->setCurrentIndex(m_valveInfo->toolNumber);

    ui->doubleSpinBox_diameter_pulley->setValue(m_valveInfo->pulley);

    ui->lineEdit_material_1->setText(m_valveInfo->material1);
    ui->lineEdit_material_2->setText(m_valveInfo->material2);
    // ui->lineEdit_material_3->setText(m_valveInfo->material3);
    ui->lineEdit_material_4->setText(m_valveInfo->material4);
    ui->lineEdit_material_5->setText(m_valveInfo->material5);
    ui->lineEdit_material_6->setText(m_valveInfo->material6);
    ui->lineEdit_material_7->setText(m_valveInfo->material7);
    ui->lineEdit_material_8->setText(m_valveInfo->material8);
    ui->lineEdit_material_9->setText(m_valveInfo->material9);
}

void ValveWindow::ButtonClick()
{
    if (ui->lineEdit_positionNumber->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер позиции");
        return;
    }

    if ((ui->lineEdit_manufacturer->text().isEmpty()) or (ui->lineEdit_serial->text().isEmpty())
        or (ui->lineEdit_PN->text().isEmpty()) or (ui->lineEdit_stroke->text().isEmpty())
        or (ui->lineEdit_positioner->text().isEmpty()) or (ui->lineEdit_dinamicError->text().isEmpty())
        or (ui->lineEdit_modelDrive->text().isEmpty()) or (ui->lineEdit_range->text().isEmpty())
        or (ui->lineEdit_materialStuffingBoxSeal->text().isEmpty())) {
        QMessageBox::StandardButton button
            = QMessageBox::question(this,
                                    "Предупреждение",
                                    "Введены не все данные, вы действительно хотети продолжить?");

        if (button == QMessageBox::StandardButton::No) {
            return;
        }
    }

    OtherParameters *otherParameters = m_registry->GetOtherParameters();
    otherParameters->safePosition = ui->comboBox_safePosition->currentText();
    otherParameters->movement = ui->comboBox_stroke_movement->currentText();
    SaveValveInfo();

    accept();
}

void ValveWindow::StrokeChanged(quint16 n)
{
    ui->comboBox_toolNumber->setEnabled(n == 1);
    ui->doubleSpinBox_diameter_pulley->setEnabled(
        (n == 1)
        && (ui->comboBox_toolNumber->currentIndex() == ui->comboBox_toolNumber->count() - 1));
}

void ValveWindow::ToolChanged(quint16 n)
{
    if (n == ui->comboBox_toolNumber->count() - 1) {
        ui->doubleSpinBox_diameter_pulley->setEnabled(true);
    } else {
        ui->doubleSpinBox_diameter_pulley->setEnabled(false);
        ui->doubleSpinBox_diameter_pulley->setValue(m_diameter[n]);
    }
}

void ValveWindow::DiameterChanged(qreal value)
{
    ui->label_square->setText(QString().asprintf("%.2f", M_PI * value * value / 4));
}

void ValveWindow::Clear()
{
    ui->lineEdit_manufacturer->setText("");
    ui->lineEdit_serial->setText("");
    ui->lineEdit_PN->setText("");
    ui->lineEdit_stroke->setText("");
    ui->lineEdit_positioner->setText("");
    ui->lineEdit_dinamicError->setText("");
    ui->lineEdit_modelDrive->setText("");
    ui->lineEdit_range->setText("");
    ui->lineEdit_materialStuffingBoxSeal->setText("");

    ui->comboBox_valveModel->setCurrentIndex(0);
    ui->comboBox_DN->setCurrentIndex(0);
    ui->doubleSpinBox_diameter->setValue(1.0);
    ui->comboBox_safePosition->setCurrentIndex(0);
    ui->comboBox_driveType->setCurrentIndex(0);
    ui->comboBox_stroke_movement->setCurrentIndex(0);
    ui->comboBox_toolNumber->setCurrentIndex(0);
    ui->doubleSpinBox_diameter_pulley->setValue(m_diameter[0]);

    ui->lineEdit_material_1->setText("");
    ui->lineEdit_material_2->setText("");
    // ui->lineEdit_material_3->setText("");
    ui->lineEdit_material_4->setText("");
    ui->lineEdit_material_5->setText("");
    ui->lineEdit_material_6->setText("");
    ui->lineEdit_material_7->setText("");
    ui->lineEdit_material_8->setText("");
    ui->lineEdit_material_9->setText("");
}
