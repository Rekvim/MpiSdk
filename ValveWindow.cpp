#include "ui_ValveWindow.h"
#include "ValveWindow.h"

#include "./Src/ValidatorFactory/ValidatorFactory.h"

ValveWindow::ValveWindow(ValveDatabase& db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ValveWindow)
    , m_db(db)
{
    ui->setupUi(this);

    auto* validatorDigits = ValidatorFactory::create(ValidatorFactory::Type::Digits, this);

    auto* validatorDigitsDot = ValidatorFactory::create(ValidatorFactory::Type::DigitsDot, this);

    auto* validatorDigitsHyphens = ValidatorFactory::create(ValidatorFactory::Type::DigitsHyphens, this);

    auto* noSpecialChars = ValidatorFactory::create(ValidatorFactory::Type::NoSpecialChars, this);

    ui->lineEdit_valveSeries->setValidator(validatorDigits);
    ui->lineEdit_valveModel->setValidator(validatorDigitsHyphens);
    ui->lineEdit_range->setValidator(validatorDigitsDot);
    ui->lineEdit_serialNumber->setValidator(validatorDigitsHyphens);
    ui->lineEdit_PN->setValidator(validatorDigits);
    ui->lineEdit_valveStroke->setValidator(validatorDigitsDot);
    ui->lineEdit_positionerModel->setValidator(validatorDigitsHyphens);
    ui->lineEdit_driveModel->setValidator(nullptr);

    ui->lineEdit_materialCorpus->setValidator(noSpecialChars);
    ui->lineEdit_materialCap->setValidator(noSpecialChars);
    ui->lineEdit_materialBall->setValidator(noSpecialChars);
    ui->lineEdit_materialDisk->setValidator(noSpecialChars);
    ui->lineEdit_materialPlunger->setValidator(noSpecialChars);
    ui->lineEdit_materialShaft->setValidator(noSpecialChars);
    ui->lineEdit_materialStock->setValidator(noSpecialChars);
    ui->lineEdit_materialGuideSleeve->setValidator(noSpecialChars);
    ui->lineEdit_positionNumber->setValidator(noSpecialChars);

    ui->lineEdit_manufacturer->setValidator(noSpecialChars);
    ui->lineEdit_manufacturer->setEnabled(false);

    connect(ui->comboBox_manufacturer,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx){
                bool manual = ui->comboBox_manufacturer->itemText(idx) == m_manualInput;
                if (manual) {
                    ui->lineEdit_manufacturer->setEnabled(true);
                    ui->lineEdit_manufacturer->setFocus();
                }
                else {
                    ui->lineEdit_manufacturer->clear();
                    ui->lineEdit_manufacturer->setEnabled(false);
                }
            }
            );

    ui->doubleSpinBox_diameterPulley->setValue(m_diameter[0]);

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

    m_partFields.insert("plunger", ui->lineEdit_plunger);
    m_partFields.insert("saddle", ui->lineEdit_saddle);
    m_partFields.insert("UpperBushing", ui->lineEdit_upperBushing);
    m_partFields.insert("LowerBushing", ui->lineEdit_lowerBushing);
    m_partFields.insert("UpperORingSealingRing", ui->lineEdit_upperORing);
    m_partFields.insert("LowerORingSealingRing", ui->lineEdit_lowerORing);
    m_partFields.insert("stuffingBoxSeal", ui->lineEdit_stuffingBoxSeal);
    m_partFields.insert("driveDiaphragm", ui->lineEdit_driveDiaphragm);
    m_partFields.insert("setOfCovers", ui->lineEdit_covers);
    m_partFields.insert("shaft", ui->lineEdit_shaft);
    m_partFields.insert("saddleLock", ui->lineEdit_saddleLock);

    populateCombo(ui->comboBox_manufacturer, m_db.getManufacturers(), true);

    connect(ui->lineEdit_valveSeries, &QLineEdit::textEdited,
            this, &ValveWindow::onSeriesEditingFinished);

    int manufacturerId = ui->comboBox_manufacturer->currentData().toInt();
    QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    int seriesId = m_db.getSeriesIdByName(manufacturerId, seriesName);

    populateComboInts(ui->comboBox_DN, m_db.getValveDnSizes(seriesId));

    int currentDnSizeId = ui->comboBox_DN->currentData().toInt();

    populateComboInts(ui->comboBox_CV, m_db.getCvValues(currentDnSizeId));

    populateCombo(ui->comboBox_driveModel, m_db.getDriveModel(), true);

    populateCombo(ui->comboBox_materialBody, m_db.getBodyMaterials());

    connect(ui->lineEdit_valveSeries, &QLineEdit::editingFinished,
            this, &ValveWindow::onSeriesEditingFinished);

    connect(ui->lineEdit_valveModel,  &QLineEdit::textEdited,
            this, &ValveWindow::onModelEditingFinished);

    connect(ui->comboBox_DN, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::onDNChanged);

    connect(ui->comboBox_CV, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::updatePartNumbers);

    connect(ui->comboBox_materialSaddle, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::updatePartNumbers);

    connect(ui->comboBox_manufacturer,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &ValveWindow::onManufacturerChanged);

    if (ui->comboBox_DN->count() > 0)
        onDNChanged(0);

    connect(ui->comboBox_manufacturer, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int){
                ui->lineEdit_valveSeries->clear();
                ui->comboBox_DN->clear();
                ui->comboBox_CV->clear();
                ui->lineEdit_valveModel->clear();
                ui->comboBox_dinamicError->clear();
                ui->comboBox_materialSaddle->clear();
            });

    ui->lineEdit_driveModel->setEnabled(false);

    connect(ui->comboBox_driveModel,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int idx){
                bool manual = (ui->comboBox_driveModel->itemText(idx) == m_manualInput);
                if (manual) {
                    ui->lineEdit_driveModel->setEnabled(true);
                    ui->lineEdit_driveModel->setFocus();
                }
                else {
                    ui->lineEdit_driveModel->clear();
                    ui->lineEdit_driveModel->setEnabled(false);
                }
            });

    QString driveModel;
    if (ui->comboBox_driveModel->currentText() == m_manualInput) {
        driveModel = ui->lineEdit_driveModel->text().trimmed();
        ui->lineEdit_driveModel->setEnabled(true);

    } else {
        driveModel = ui->comboBox_driveModel->currentText();
    }

    connect(ui->comboBox_positionerType,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &ValveWindow::onPositionerTypeChanged);

    onPositionerTypeChanged(ui->comboBox_positionerType->currentIndex());
}

ValveWindow::~ValveWindow()
{
    delete ui;
}

void ValveWindow::onManufacturerChanged(int /*index*/)
{
    ui->lineEdit_valveSeries->clear();
    ui->comboBox_DN->clear();
    ui->comboBox_CV->clear();
}

void ValveWindow::onPositionerTypeChanged(int index)
{
    const QString selected = ui->comboBox_positionerType->itemText(index);

    ui->comboBox_dinamicError->clear();

    if (selected == QStringLiteral("Интеллектуальный ЭПП")) {
        ui->comboBox_dinamicError->addItem(QStringLiteral("1.5"));
        ui->comboBox_dinamicError->setCurrentIndex(0);
    }
    else if (selected == QStringLiteral("ЭПП") || selected == QStringLiteral("ПП")) {
        ui->comboBox_dinamicError->addItem(QStringLiteral("2.5"));
        ui->comboBox_dinamicError->setCurrentIndex(0);
    }
}

void ValveWindow::populateModelsAndDn(int seriesId)
{
    populateComboInts(ui->comboBox_DN, m_db.getValveDnSizes(seriesId));
    onDNChanged(0);
}

void ValveWindow::onModelEditingFinished()
{
    ui->comboBox_dinamicError->clear();

    QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    int manId = ui->comboBox_manufacturer->currentData().toInt();
    int seriesId = m_db.getSeriesIdByName(manId, seriesName);

    QString model = ui->lineEdit_valveModel->text().trimmed();
    int modelId = m_db.getValveModelIdByName(seriesId, model);

    if (seriesId < 0 || modelId < 0) {
        ui->comboBox_dinamicError->clear();
        ui->comboBox_materialSaddle->clear();
        return;
    }

    populateComboInts(ui->comboBox_dinamicError, m_db.getDynamicErrors(modelId), true);

    populateCombo(ui->comboBox_materialSaddle, m_db.getSaddleMaterialsForModel(modelId));
}

void ValveWindow::onSeriesEditingFinished()
{
    QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    if (seriesName.isEmpty())
        return;

    int manId = ui->comboBox_manufacturer->currentData().toInt();
    int seriesId = m_db.getSeriesIdByName(manId, seriesName);
    if (seriesId < 0) {
        ui->comboBox_DN->clear();
        ui->comboBox_CV->clear();
        ui->lineEdit_valveModel->clear();
        ui->comboBox_dinamicError->clear();
        ui->comboBox_materialSaddle->clear();
        return;
    }

    if (seriesName.length() >= 2) {
        QString prefix = seriesName.left(2);
        ui->lineEdit_valveModel->setText(prefix + "-");
    } else {
        ui->lineEdit_valveModel->clear();
    }

    populateModelsAndDn(seriesId);
}

void ValveWindow::onDNChanged(int index)
{
    int dnSizeId = ui->comboBox_DN->itemData(index).toInt();

    ui->comboBox_CV->clear();

    populateComboInts(ui->comboBox_CV, m_db.getCvValues(dnSizeId));

    if (ui->comboBox_CV->count() > 0)
        ui->comboBox_CV->setCurrentIndex(0);

    updatePartNumbers();
}

void ValveWindow::updatePartNumbers()
{
    int dnSizeId = ui->comboBox_DN->currentData().toInt();
    int cvValueId = ui->comboBox_CV->currentData().toInt();
    int saddleMaterialId = ui->comboBox_materialSaddle->currentData().toInt();

    auto parts = m_db.getValveComponents(dnSizeId, cvValueId, saddleMaterialId);

    static const QMap<QString, QString> displayNames = {
        { "plunger", "Плунжер" },
        { "saddle", "Седло" },
        { "UpperBushing", "Втулка (верхняя)" },
        { "LowerBushing", "Втулка (нижняя)" },
        { "UpperORingSealingRing", "О-кольцо (верхнее)" },
        { "LowerORingSealingRing", "О-кольцо (нижнее)" },
        { "stuffingBoxSeal", "Манжета" },
        { "driveDiaphragm", "Диафрагма привода" },
        { "setOfCovers", "Крышки" },
        { "shaft", "Вал" },
        { "saddleLock", "Фиксатор седла" }
    };

    auto keys = m_partFields.keys();
    for (int i = 0, n = keys.size(); i < n; ++i) {
        const QString& key = keys.at(i);
        QLineEdit* le = m_partFields.value(key);
        QString code = parts.value(key);
        if (code.isEmpty()) {
            le->clear();
        } else {
            QString label = displayNames.value(key, key);
            le->setText(QString("%1: %2").arg(code, label));
        }
    }
}

void ValveWindow::SetRegistry(Registry *registry)
{
    Q_ASSERT(registry != nullptr);
    m_registry = registry;

    m_valveInfo = m_registry->GetValveInfo();
    m_materialsOfComponentParts = m_registry->GetMaterialsOfComponentParts();

    ui->comboBox_positionNumber->clear();
    ui->comboBox_positionNumber->addItems(m_registry->GetPositions());
    ui->comboBox_positionNumber->addItem(m_manualInput);

    QString lastPosition = m_registry->GetLastPosition();
    if (lastPosition.isEmpty()) {
        ui->comboBox_positionNumber->setCurrentIndex(ui->comboBox_positionNumber->count() - 1);
    } else {
        int idx = ui->comboBox_positionNumber->findText(lastPosition);
        if (idx >= 0) {
            ui->comboBox_positionNumber->setCurrentIndex(idx);
            PositionChanged(lastPosition);
        }
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

    if (ui->comboBox_manufacturer->currentText() == m_manualInput)
        m_valveInfo->manufacturer = ui->lineEdit_manufacturer->text();
    else
        m_valveInfo->manufacturer = ui->comboBox_manufacturer->currentText();

    if (ui->comboBox_driveModel->currentText() == m_manualInput)
        m_valveInfo->driveModel = ui->lineEdit_driveModel->text().trimmed();
    else
        m_valveInfo->driveModel = ui->comboBox_driveModel->currentText();



    m_valveInfo->serialNumber = ui->lineEdit_serialNumber->text();
    m_valveInfo->valveStroke = ui->lineEdit_valveStroke->text();
    m_valveInfo->positionerModel = ui->lineEdit_positionerModel->text();

    m_valveInfo->range = ui->lineEdit_range->text();

    m_valveInfo->valveSeries = ui->lineEdit_valveSeries->text();

    m_valveInfo->valveModel = ui->lineEdit_valveModel->text();

    m_valveInfo->diameter = ui->doubleSpinBox_diameter->value();
    m_valveInfo->pulley = ui->doubleSpinBox_diameterPulley->value();

    m_valveInfo->DN = ui->comboBox_DN->currentText().toInt();
    m_valveInfo->CV = ui->comboBox_CV->currentText().toInt();
    m_valveInfo->PN = ui->lineEdit_PN->text().toInt();

    m_valveInfo->safePosition = ui->comboBox_safePosition->currentIndex();
    m_valveInfo->driveType = ui->comboBox_driveType->currentIndex();
    m_valveInfo->strokeMovement = ui->comboBox_stroke_movement->currentIndex();
    m_valveInfo->toolNumber = ui->comboBox_toolNumber->currentIndex();

    m_materialsOfComponentParts->saddle = ui->comboBox_materialSaddle->currentText();
    m_materialsOfComponentParts->corpus = ui->lineEdit_materialCorpus->text();
    m_materialsOfComponentParts->cap = ui->lineEdit_materialCap->text();
    m_materialsOfComponentParts->ball = ui->lineEdit_materialBall->text();
    m_materialsOfComponentParts->disk = ui->lineEdit_materialDisk->text();
    m_materialsOfComponentParts->plunger = ui->lineEdit_materialPlunger->text();
    m_materialsOfComponentParts->shaft = ui->lineEdit_materialShaft->text();
    m_materialsOfComponentParts->stock = ui->lineEdit_materialStock->text();
    m_materialsOfComponentParts->guideSleeve = ui->lineEdit_materialGuideSleeve->text();
    m_materialsOfComponentParts->stuffingBoxSeal = ui->comboBox_materialStuffingBoxSeal->currentText();

    m_registry->SaveMaterialsOfComponentParts();
    m_registry->SaveValveInfo();
}

void ValveWindow::PositionChanged(const QString &position)
{
    m_valveInfo = m_registry->GetValveInfo(position);
    m_materialsOfComponentParts = m_registry->GetMaterialsOfComponentParts();

    if (position == m_manualInput) {
        ui->lineEdit_positionNumber->setEnabled(true);
        return;
    }

    ui->lineEdit_positionNumber->setText(position);
    ui->lineEdit_positionNumber->setEnabled(false);

    ui->lineEdit_manufacturer->setText(m_valveInfo->manufacturer);

    ui->lineEdit_valveSeries->setText(m_valveInfo->valveSeries);
    onSeriesEditingFinished();

    ui->lineEdit_valveModel->setText(m_valveInfo->valveModel);
    onModelEditingFinished();

    ui->lineEdit_serialNumber->setText(m_valveInfo->serialNumber);
    ui->lineEdit_PN->setText(QString::number(m_valveInfo->PN));
    ui->lineEdit_valveStroke->setText(m_valveInfo->valveStroke);
    ui->lineEdit_positionerModel->setText(m_valveInfo->positionerModel);
    // ui->lineEdit_dinamicError->setText(m_valveInfo->dinamicError);

    const QString loaded = m_valveInfo->driveModel;
    int idx = ui->comboBox_driveModel->findText(loaded);
    if (idx >= 0) {
        ui->comboBox_driveModel->setCurrentIndex(idx);
        ui->lineEdit_driveModel->clear();
        ui->lineEdit_driveModel->setEnabled(false);
    } else {
        int manualIdx = ui->comboBox_driveModel->findText(m_manualInput);
        ui->comboBox_driveModel->setCurrentIndex(manualIdx);
        ui->lineEdit_driveModel->setText(loaded);
        ui->lineEdit_driveModel->setEnabled(true);
    }

    ui->lineEdit_range->setText(m_valveInfo->range);


    ui->doubleSpinBox_diameter->setValue(m_valveInfo->diameter);

    ui->comboBox_safePosition->setCurrentIndex(m_valveInfo->safePosition);
    ui->comboBox_driveType->setCurrentIndex(m_valveInfo->driveType);
    ui->comboBox_stroke_movement->setCurrentIndex(m_valveInfo->strokeMovement);
    ui->comboBox_toolNumber->setCurrentIndex(m_valveInfo->toolNumber);

    ui->doubleSpinBox_diameterPulley->setValue(m_valveInfo->pulley);

    // ui->lineEdit_materialStuffingBoxSeal->setText(m_materialsOfComponentParts->stuffingBoxSeal);
    ui->lineEdit_materialCorpus->setText(m_materialsOfComponentParts->corpus);
    ui->lineEdit_materialCap->setText(m_materialsOfComponentParts->cap);
    ui->lineEdit_materialBall->setText(m_materialsOfComponentParts->ball);
    ui->lineEdit_materialDisk->setText(m_materialsOfComponentParts->disk);
    ui->lineEdit_materialPlunger->setText(m_materialsOfComponentParts->plunger);
    ui->lineEdit_materialShaft->setText(m_materialsOfComponentParts->shaft);
    ui->lineEdit_materialStock->setText(m_materialsOfComponentParts->stock);
    ui->lineEdit_materialGuideSleeve->setText(m_materialsOfComponentParts->guideSleeve);
}

void ValveWindow::StrokeChanged(quint16 n)
{
    ui->comboBox_toolNumber->setEnabled(n == 1);
    ui->doubleSpinBox_diameterPulley->setEnabled(
        (n == 1)
        && (ui->comboBox_toolNumber->currentIndex() == ui->comboBox_toolNumber->count() - 1));
}

void ValveWindow::ToolChanged(quint16 n)
{
    if (n == ui->comboBox_toolNumber->count() - 1) {
        ui->doubleSpinBox_diameterPulley->setEnabled(true);
    } else {
        ui->doubleSpinBox_diameterPulley->setEnabled(false);
        ui->doubleSpinBox_diameterPulley->setValue(m_diameter[n]);
    }
}

void ValveWindow::DiameterChanged(qreal value)
{
    ui->label_square->setText(QString().asprintf("%.2f", M_PI * value * value / 4));
}

void ValveWindow::fillReport(ReportSaver::Report &report) {

    report.data.push_back({56, 10, ui->lineEdit_plunger->text()});

    report.data.push_back({57, 10, ui->lineEdit_saddle->text()});

    report.data.push_back({58, 10, ui->lineEdit_upperBushing->text()});

    report.data.push_back({59, 10, ui->lineEdit_lowerBushing->text()});

    report.data.push_back({60, 10, ui->lineEdit_upperORing->text()});

    report.data.push_back({61, 10, ui->lineEdit_lowerORing->text()});

    report.data.push_back({62, 10, ui->lineEdit_stuffingBoxSeal->text()});

    report.data.push_back({63, 10, ui->lineEdit_driveDiaphragm->text()});

    report.data.push_back({64, 10, ui->lineEdit_covers->text()});

    report.data.push_back({65, 10, ui->lineEdit_shaft->text()});

    report.data.push_back({65, 10, ui->lineEdit_saddleLock->text()});
}

void ValveWindow::Clear()
{
    ui->lineEdit_manufacturer->setText("");
    ui->lineEdit_serialNumber->setText("");
    ui->lineEdit_PN->setText("");
    ui->lineEdit_valveStroke->setText("");
    ui->lineEdit_positionerModel->setText("");
    // ui->lineEdit_dinamicError->setText("");
    ui->lineEdit_driveModel->setText("");
    ui->lineEdit_range->setText("");
    ui->lineEdit_valveModel->setText("");

    ui->lineEdit_materialCorpus->setText("");
    ui->lineEdit_materialCap->setText("");
    ui->lineEdit_materialBall->setText("");
    ui->lineEdit_materialDisk->setText("");
    ui->lineEdit_materialPlunger->setText("");
    ui->lineEdit_materialShaft->setText("");
    ui->lineEdit_materialStock->setText("");
    ui->lineEdit_materialGuideSleeve->setText("");

    ui->comboBox_materialStuffingBoxSeal->setCurrentIndex(0);
    ui->comboBox_DN->setCurrentIndex(0);
    ui->comboBox_safePosition->setCurrentIndex(0);
    ui->comboBox_driveType->setCurrentIndex(0);
    ui->comboBox_stroke_movement->setCurrentIndex(0);
    ui->comboBox_toolNumber->setCurrentIndex(0);

    ui->doubleSpinBox_diameter->setValue(1.0);
    ui->doubleSpinBox_diameterPulley->setValue(m_diameter[0]);
}

void ValveWindow::ButtonClick()
{

    if (ui->lineEdit_positionNumber->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер позиции");
        return;
    }

    if ((ui->lineEdit_serialNumber->text().isEmpty()) or (ui->lineEdit_PN->text().isEmpty())
        or (ui->lineEdit_valveStroke->text().isEmpty()) or (ui->lineEdit_positionerModel->text().isEmpty())
        or (ui->lineEdit_range->text().isEmpty())) {
        QMessageBox::StandardButton button
            = QMessageBox::question(this,
                                    "Предупреждение",
                                    "Введены не все данные, вы действительно хотите продолжить?");

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
