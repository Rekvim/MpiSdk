#include "ui_ValveWindow.h"
#include "ValveWindow.h"
#include "ValidatorFactory.h"

ValveWindow::ValveWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ValveWindow)
{
    ui->setupUi(this);

    auto* validatorDigits = ValidatorFactory::create(ValidatorFactory::Type::Digits, this);

    auto* validatorLettersHyphens = ValidatorFactory::create(ValidatorFactory::Type::LettersHyphens, this);

    auto* noSpecialChars = ValidatorFactory::create(ValidatorFactory::Type::NoSpecialChars, this);

    ui->lineEdit_positionNumber->setValidator(noSpecialChars);
    ui->lineEdit_manufacturer->setValidator(noSpecialChars);
    ui->lineEdit_serial->setValidator(noSpecialChars);
    ui->lineEdit_PN->setValidator(validatorDigits);
    ui->lineEdit_valveStroke->setValidator(noSpecialChars);
    ui->lineEdit_positioner->setValidator(noSpecialChars);
    ui->lineEdit_dinamicError->setValidator(noSpecialChars);
    ui->lineEdit_modelDrive->setValidator(noSpecialChars);
    ui->lineEdit_range->setValidator(noSpecialChars);

    ui->lineEdit_materialStuffingBoxSeal->setValidator(noSpecialChars);
    ui->lineEdit_materialCorpus->setValidator(noSpecialChars);
    ui->lineEdit_materialCap->setValidator(noSpecialChars);
    ui->lineEdit_materialBall->setValidator(noSpecialChars);
    ui->lineEdit_materialDisk->setValidator(noSpecialChars);
    ui->lineEdit_materialPlunger->setValidator(noSpecialChars);
    ui->lineEdit_materialShaft->setValidator(noSpecialChars);
    ui->lineEdit_materialStock->setValidator(noSpecialChars);
    ui->lineEdit_materialGuideSleeve->setValidator(noSpecialChars);

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

    ui->comboBox_manufacturer->clear();
    ui->comboBox_manufacturer->addItems(m_loader.getManufacturers());

    ui->comboBox_valveModel->clear();
    ui->comboBox_valveModel->addItems(m_loader.getValveModels());

    ui->comboBox_driveModel->clear();
    ui->comboBox_driveModel->addItems(m_loader.getDriveModels());

    ui->comboBox_materialSaddle->clear();
    ui->comboBox_materialSaddle->addItems(m_loader.getSaddleMaterials());

    ui->comboBox_materialBody->clear();
    ui->comboBox_materialBody->addItems(m_loader.getBodyMaterials());

    m_valveDataObj = m_loader.getValveData();
    ui->comboBox_DN->clear();
    ui->comboBox_DN->addItems(m_loader.getDNList());

    m_partFields.insert("plunger", ui->lineEdit_plunger);
    m_partFields.insert("saddle", ui->lineEdit_saddle);
    m_partFields.insert("bushing", ui->lineEdit_bushing);
    m_partFields.insert("oRingSealingRing", ui->lineEdit_oRing);
    m_partFields.insert("stuffingBoxSeal", ui->lineEdit_stuffingSeal);
    m_partFields.insert("driveDiaphragm", ui->lineEdit_diaphragm);
    m_partFields.insert("setOfCovers", ui->lineEdit_covers);
    m_partFields.insert("shaft", ui->lineEdit_shaft);
    m_partFields.insert("saddleLock", ui->lineEdit_saddleLock);

    connect(ui->comboBox_DN, &QComboBox::currentTextChanged,
            this, &ValveWindow::onDNChanged);
    connect(ui->comboBox_CV, &QComboBox::currentTextChanged,
            this, &ValveWindow::updatePartNumbers);
    connect(ui->comboBox_materialSaddle, &QComboBox::currentTextChanged,
            this, &ValveWindow::updatePartNumbers);

    if (ui->comboBox_DN->count() > 0)
        onDNChanged(ui->comboBox_DN->currentText());
}

ValveWindow::~ValveWindow()
{
    delete ui;
}

void ValveWindow::onDNChanged(const QString &dn)
{
    ui->comboBox_CV->clear();
    auto variants = m_valveDataObj.value(dn).toArray();
    for (auto vv : variants) {
        double cv = vv.toObject().value("CV").toDouble();
        ui->comboBox_CV->addItem(QString::number(cv));
    }
    if (ui->comboBox_CV->count()>0)
        ui->comboBox_CV->setCurrentIndex(0);

    updatePartNumbers();
}

void ValveWindow::updatePartNumbers()
{
    QString dn  = ui->comboBox_DN->currentText();
    double cv   = ui->comboBox_CV->currentText().toDouble();
    QString mat = ui->comboBox_materialSaddle->currentText();

    auto parts = m_loader.materialsFor(dn, cv, mat);

    static const QMap<QString, QString> displayNames = {
        {"plunger", "Плунжер"},
        {"saddle", "Седло"},
        {"bushing", "Втулка"},
        {"oRingSealingRing", "Уплотнительное кольцо"},
        {"stuffingBoxSeal", "Манжета"},
        {"driveDiaphragm", "Диафрагма привода"},
        {"setOfCovers", "Крышки"},
        {"shaft", "Вал"},
        {"saddleLock", "Фиксатор седла"}
    };

    for (const auto &key : m_partFields.keys()) {
        QString code = parts.value(key, QStringLiteral(""));
        QString name = displayNames.value(key, key);
        m_partFields[key]->setText(code + ": " + name);
    }
}

void ValveWindow::SetRegistry(Registry *registry)
{
    m_registry = registry;

    m_valveInfo = m_registry->GetValveInfo();

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
    m_valveInfo->stroke = ui->lineEdit_valveStroke->text();
    m_valveInfo->positioner = ui->lineEdit_positioner->text();
    m_valveInfo->dinamicError = ui->lineEdit_dinamicError->text();
    m_valveInfo->modelDrive = ui->lineEdit_modelDrive->text();
    m_valveInfo->range = ui->lineEdit_range->text();
    m_valveInfo->manufacturer = ui->lineEdit_manufacturer->text();

    m_valveInfo->diameter = ui->doubleSpinBox_diameter->value();
    m_valveInfo->pulley = ui->doubleSpinBox_diameter_pulley->value();

    m_valveInfo->DN = ui->comboBox_DN->currentIndex();
    m_valveInfo->valveModel = ui->comboBox_valveModel->currentText();
    m_valveInfo->safePosition = ui->comboBox_safePosition->currentIndex();
    m_valveInfo->driveType = ui->comboBox_driveType->currentIndex();
    m_valveInfo->strokeMovement = ui->comboBox_stroke_movement->currentIndex();
    m_valveInfo->toolNumber = ui->comboBox_toolNumber->currentIndex();

    m_materialsOfComponentParts->materialStuffingBoxSeal = ui->lineEdit_materialStuffingBoxSeal->text();
    m_materialsOfComponentParts->materialCorpus = ui->lineEdit_materialCorpus->text();
    m_materialsOfComponentParts->materialCap = ui->lineEdit_materialCap->text();
    m_materialsOfComponentParts->materialBall = ui->lineEdit_materialBall->text();
    m_materialsOfComponentParts->materialDisk = ui->lineEdit_materialDisk->text();
    m_materialsOfComponentParts->materialPlunger = ui->lineEdit_materialPlunger->text();
    m_materialsOfComponentParts->materialShaft = ui->lineEdit_materialShaft->text();
    m_materialsOfComponentParts->materialStock = ui->lineEdit_materialStock->text();
    m_materialsOfComponentParts->materialGuideSleeve = ui->lineEdit_materialGuideSleeve->text();

    m_registry->SaveMaterialsOfComponentParts();
    m_registry->SaveValveInfo();
}

void ValveWindow::PositionChanged(const QString &position)
{
    if (position == m_manualInput) {
        ui->lineEdit_positionNumber->setEnabled(true);
        return;
    }

    m_valveInfo = m_registry->GetValveInfo(position);
    m_materialsOfComponentParts = m_registry->GetMaterialsOfComponentParts();

    ui->lineEdit_positionNumber->setText(position);
    ui->lineEdit_positionNumber->setEnabled(false);

    ui->lineEdit_manufacturer->setText(m_valveInfo->manufacturer);
    ui->lineEdit_serial->setText(m_valveInfo->serialNumber);
    ui->lineEdit_PN->setText(m_valveInfo->PN);
    ui->lineEdit_valveStroke->setText(m_valveInfo->stroke);
    ui->lineEdit_positioner->setText(m_valveInfo->positioner);
    ui->lineEdit_dinamicError->setText(m_valveInfo->dinamicError);
    ui->lineEdit_modelDrive->setText(m_valveInfo->modelDrive);
    ui->lineEdit_range->setText(m_valveInfo->range);


    ui->doubleSpinBox_diameter->setValue(m_valveInfo->diameter);

    ui->comboBox_DN->setCurrentIndex(m_valveInfo->DN);
    // ui->comboBox_valveModel->currentText(m_valveInfo->valveModel);

    ui->comboBox_safePosition->setCurrentIndex(m_valveInfo->safePosition);
    ui->comboBox_driveType->setCurrentIndex(m_valveInfo->driveType);
    ui->comboBox_stroke_movement->setCurrentIndex(m_valveInfo->strokeMovement);
    ui->comboBox_toolNumber->setCurrentIndex(m_valveInfo->toolNumber);

    ui->doubleSpinBox_diameter_pulley->setValue(m_valveInfo->pulley);


    ui->lineEdit_materialStuffingBoxSeal->setText(m_materialsOfComponentParts->materialStuffingBoxSeal);
    ui->lineEdit_materialCorpus->setText(m_materialsOfComponentParts->materialCorpus);
    ui->lineEdit_materialCap->setText(m_materialsOfComponentParts->materialCap);
    ui->lineEdit_materialBall->setText(m_materialsOfComponentParts->materialBall);
    ui->lineEdit_materialDisk->setText(m_materialsOfComponentParts->materialDisk);
    ui->lineEdit_materialPlunger->setText(m_materialsOfComponentParts->materialPlunger);
    ui->lineEdit_materialShaft->setText(m_materialsOfComponentParts->materialShaft);
    ui->lineEdit_materialStock->setText(m_materialsOfComponentParts->materialStock);
    ui->lineEdit_materialGuideSleeve->setText(m_materialsOfComponentParts->materialGuideSleeve);
}

void ValveWindow::ButtonClick()
{
    if (ui->lineEdit_positionNumber->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер позиции");
        return;
    }

    if ((ui->lineEdit_manufacturer->text().isEmpty()) or (ui->lineEdit_serial->text().isEmpty())
        or (ui->lineEdit_PN->text().isEmpty()) or (ui->lineEdit_valveStroke->text().isEmpty())
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

void ValveWindow::fillReport(ReportSaver::Report &report) {

    report.data.push_back({56, 10, ui->lineEdit_plunger->text()});

    report.data.push_back({57, 10, ui->lineEdit_saddle->text()});

    report.data.push_back({58, 10, ui->lineEdit_bushing->text()});

    report.data.push_back({59, 10, ui->lineEdit_oRing->text()});

    report.data.push_back({60, 10, ui->lineEdit_stuffingSeal->text()});

    report.data.push_back({61, 10, ui->lineEdit_diaphragm->text()});

    report.data.push_back({62, 10, ui->lineEdit_covers->text()});

    report.data.push_back({63, 10, ui->lineEdit_shaft->text()});

    report.data.push_back({64, 10, ui->lineEdit_saddleLock->text()});
}

void ValveWindow::Clear()
{
    ui->lineEdit_manufacturer->setText("");
    ui->lineEdit_serial->setText("");
    ui->lineEdit_PN->setText("");
    ui->lineEdit_valveStroke->setText("");
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

    ui->lineEdit_materialCorpus->setText("");
    ui->lineEdit_materialCap->setText("");
    ui->lineEdit_materialBall->setText("");
    ui->lineEdit_materialDisk->setText("");
    ui->lineEdit_materialPlunger->setText("");
    ui->lineEdit_materialShaft->setText("");
    ui->lineEdit_materialStock->setText("");
    ui->lineEdit_materialGuideSleeve->setText("");
}
