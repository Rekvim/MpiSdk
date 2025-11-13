#include "ui_ValveWindow.h"
#include "ValveWindow.h"

// #include "./Src/ValidatorFactory/ValidatorFactory.h"

ValveWindow::ValveWindow(ValveDatabase& db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ValveWindow)
    , m_db(db)
{
    ui->setupUi(this);

    // QValidator *validatorDigits = ValidatorFactory::create(ValidatorFactory::Type::Digits, this);
    // QValidator *validatorDigitsDot = ValidatorFactory::create(ValidatorFactory::Type::DigitsDot, this);
    // QValidator *validatorDigitsHyphens = ValidatorFactory::create(ValidatorFactory::Type::DigitsHyphens, this);
    // QValidator *noSpecialChars = ValidatorFactory::create(ValidatorFactory::Type::NoSpecialChars, this);

    // ui->lineEdit_valveSeries->setValidator(validatorDigits);
    // ui->lineEdit_valveModel->setValidator(validatorDigitsHyphens);
    // ui->lineEdit_driveRange->setValidator(validatorDigitsDot);
    // ui->lineEdit_serialNumber->setValidator(validatorDigitsHyphens);
    // ui->lineEdit_PN->setValidator(validatorDigits);
    // ui->lineEdit_valveStroke->setValidator(validatorDigitsDot);
    // ui->lineEdit_positionerModel->setValidator(validatorDigitsHyphens);
    // ui->lineEdit_driveModel->setValidator(nullptr);

    // ui->lineEdit_materialCorpus->setValidator(noSpecialChars);
    // ui->lineEdit_materialCap->setValidator(noSpecialChars);
    // ui->lineEdit_materialBall->setValidator(noSpecialChars);
    // ui->lineEdit_materialDisk->setValidator(noSpecialChars);
    // ui->lineEdit_materialPlunger->setValidator(noSpecialChars);
    // ui->lineEdit_materialShaft->setValidator(noSpecialChars);
    // ui->lineEdit_materialStock->setValidator(noSpecialChars);
    // ui->lineEdit_materialGuideSleeve->setValidator(noSpecialChars);
    // ui->lineEdit_positionNumber->setValidator(noSpecialChars);

    // ui->lineEdit_manufacturer->setValidator(noSpecialChars);
    ui->lineEdit_manufacturer->setEnabled(false);

    connect(ui->comboBox_manufacturer, QOverload<int>::of(&QComboBox::currentIndexChanged),
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

    ui->lineEdit_diameterPulley->setText(m_diameter[0]);

    connect(ui->comboBox_strokeMovement, &QComboBox::currentIndexChanged,
            this, &ValveWindow::strokeChanged);

    connect(ui->comboBox_toolNumber, &QComboBox::currentIndexChanged,
            this, &ValveWindow::toolChanged);

    connect(ui->lineEdit_driveDiameter, &QLineEdit::textChanged,
            this, &ValveWindow::diameterChanged);

    diameterChanged(ui->lineEdit_driveDiameter->text());

    m_partFields.insert("plunger", ui->lineEdit_listDetails_plunger);
    m_partFields.insert("saddle", ui->lineEdit_listDetails_saddle);
    m_partFields.insert("UpperBushing", ui->lineEdit_listDetails_upperBushing);
    m_partFields.insert("LowerBushing", ui->lineEdit_listDetails_lowerBushing);
    m_partFields.insert("UpperORingSealingRing", ui->lineEdit_listDetails_upperORing);
    m_partFields.insert("LowerORingSealingRing", ui->lineEdit_listDetails_lowerORing);
    m_partFields.insert("stuffingBoxSeal", ui->lineEdit_listDetails_stuffingBoxSeal);
    m_partFields.insert("driveDiaphragm", ui->lineEdit_listDetails_driveDiaphragm);
    m_partFields.insert("setOfCovers", ui->lineEdit_listDetails_covers);
    m_partFields.insert("shaft", ui->lineEdit_listDetails_shaft);
    m_partFields.insert("saddleLock", ui->lineEdit_listDetails_saddleLock);

    populateCombo(ui->comboBox_manufacturer, m_db.getManufacturers(), true);

    connect(ui->lineEdit_valveSeries, &QLineEdit::textEdited,
            this, &ValveWindow::onSeriesEditingFinished);

    int manufacturerId = ui->comboBox_manufacturer->currentData().toInt();
    QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    int seriesId = m_db.getSeriesIdByName(manufacturerId, seriesName);

    populateComboInts(ui->comboBox_DN, m_db.getValveDnSizes(seriesId), true);

    int currentDnSizeId = ui->comboBox_DN->currentData().toInt();

    populateComboInts(ui->comboBox_CV, m_db.getCvValues(currentDnSizeId), true);

    connect(ui->comboBox_CV, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::onCVChanged);

    ui->lineEdit_DN->setEnabled(isManual(ui->comboBox_DN));
    if (!isManual(ui->comboBox_DN)) ui->lineEdit_DN->clear();

    ui->lineEdit_CV->setEnabled(isManual(ui->comboBox_CV));
    if (!isManual(ui->comboBox_CV)) ui->lineEdit_CV->clear();

    populateCombo(ui->comboBox_driveModel, m_db.getDriveModel(), true);

    populateCombo(ui->comboBox_materialBody, m_db.getBodyMaterials());

    connect(ui->lineEdit_valveModel, &QLineEdit::textEdited,
            this, &ValveWindow::onModelEditingFinished);

    connect(ui->comboBox_DN, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::onDNChanged);

    connect(ui->comboBox_CV, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::updatePartNumbers);

    connect(ui->lineEdit_DN, &QLineEdit::textEdited, this, [this](const QString&){
        auto dnIdOpt = resolveDnIdFromManual();
        refreshCvForDn(dnIdOpt);

        updatePartNumbers();
    });

    connect(ui->lineEdit_CV, &QLineEdit::textChanged,
            this, [this]{ updatePartNumbers(); });

    connect(ui->comboBox_materialSaddle,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int idx){
                const bool manual = (ui->comboBox_materialSaddle->itemText(idx) == m_manualInput);
                if (manual) {
                    ui->lineEdit_materialSaddle->setEnabled(true);
                    ui->lineEdit_materialSaddle->setFocus();

                    for (auto* le : m_partFields) le->clear();
                } else {
                    ui->lineEdit_materialSaddle->clear();
                    ui->lineEdit_materialSaddle->setEnabled(false);
                    updatePartNumbers();
                }
            });

    connect(ui->lineEdit_materialSaddle, &QLineEdit::textEdited, this, [this](const QString&){
        updatePartNumbers();
    });

    ui->lineEdit_materialSaddle->setEnabled(isManual(ui->comboBox_materialSaddle));
    if (!isManual(ui->comboBox_materialSaddle)) ui->lineEdit_materialSaddle->clear();

    connect(ui->comboBox_manufacturer, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::onManufacturerChanged);

    if (ui->comboBox_DN->count() > 0)
        onDNChanged(0);

    // connect(ui->comboBox_manufacturer, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //         this, [this](int){
    //             ui->lineEdit_valveSeries->clear();
    //             ui->comboBox_DN->clear();
    //             ui->comboBox_CV->clear();
    //             ui->lineEdit_valveModel->clear();
    //             //ui->comboBox_dinamicError->clear();
    //             ui->comboBox_materialSaddle->clear();
    //         });

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

    connect(ui->comboBox_positionerType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ValveWindow::onPositionerTypeChanged);

    onPositionerTypeChanged(ui->comboBox_positionerType->currentIndex());

    setDnCvManualMode(ui->comboBox_manufacturer->currentText() == m_manualInput);
    setSaddleManualMode(ui->comboBox_manufacturer->currentText() == m_manualInput);
}

ValveWindow::~ValveWindow()
{
    delete ui;
}

void ValveWindow::onManufacturerChanged(int /*index*/)
{
    const bool manufacturerManual = (ui->comboBox_manufacturer->currentText() == m_manualInput);

    ui->lineEdit_valveSeries->clear();
    ui->comboBox_DN->clear();
    ui->comboBox_CV->clear();
    ui->lineEdit_valveModel->clear();
    ui->comboBox_materialSaddle->clear();

    if (manufacturerManual) {
        setDnCvManualMode(true);
        setSaddleManualMode(true);
    } else {
        setDnCvManualMode(false);
        setSaddleManualMode(false);

        const QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
        if (!seriesName.isEmpty()) {
            if (auto sidOpt = currentSeriesId(); sidOpt) {
                populateModelsAndDn(*sidOpt);
            }
        }
    }
}
void ValveWindow::setDnCvManualMode(bool manual)
{
    const QSignalBlocker b1(ui->comboBox_DN);
    const QSignalBlocker b2(ui->comboBox_CV);

    ui->comboBox_DN->clear();
    ui->comboBox_CV->clear();

    if (manual) {
        // Только "Ручной ввод" в обоих комбобоксах
        ui->comboBox_DN->addItem(m_manualInput);
        ui->comboBox_CV->addItem(m_manualInput);
        ui->comboBox_DN->setCurrentIndex(0);
        ui->comboBox_CV->setCurrentIndex(0);

        ui->lineEdit_DN->setEnabled(true);
        ui->lineEdit_CV->setEnabled(true);

        for (auto* le : m_partFields) le->clear();
    } else {
        ui->lineEdit_DN->clear();
        ui->lineEdit_CV->clear();
        ui->lineEdit_DN->setEnabled(false);
        ui->lineEdit_CV->setEnabled(false);

        ui->comboBox_DN->clear();
        ui->comboBox_CV->clear();
    }
}

void ValveWindow::setSaddleManualMode(bool manual)
{
    const QSignalBlocker b(ui->comboBox_materialSaddle);

    ui->comboBox_materialSaddle->clear();

    if (manual) {
        ui->comboBox_materialSaddle->addItem(m_manualInput);
        ui->comboBox_materialSaddle->setCurrentIndex(0);
        ui->lineEdit_materialSaddle->setEnabled(true);
        ui->lineEdit_materialSaddle->setFocus();

        for (auto* le : m_partFields) le->clear();
    } else {
        // обычный режим — просто гарантируем наличие пункта
        ensureSaddleManualOption();
        ui->lineEdit_materialSaddle->clear();
        ui->lineEdit_materialSaddle->setEnabled(false);
    }
}

std::optional<int> ValveWindow::currentSeriesId() const
{
    const int manId = ui->comboBox_manufacturer->currentData().toInt();
    const QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    const int seriesId = m_db.getSeriesIdByName(manId, seriesName);
    if (seriesId < 0) return std::nullopt;
    return seriesId;
}

std::optional<int> ValveWindow::resolveDnIdFromManual() const
{
    bool ok = false;
    const int dn = ui->lineEdit_DN->text().trimmed().toInt(&ok);
    if (!ok) return std::nullopt;

    const auto sidOpt = currentSeriesId();
    if (!sidOpt) return std::nullopt;

    const auto dnList = m_db.getValveDnSizes(*sidOpt);
    for (const auto& [id, val] : dnList) {
        if (val == dn) return id;
    }
    return std::nullopt;
}

std::optional<int> ValveWindow::resolveCvIdFromManual(int dnSizeId) const
{
    bool ok = false;
    const int cv = ui->lineEdit_CV->text().trimmed().toInt(&ok);
    if (!ok) return std::nullopt;

    const auto cvList = m_db.getCvValues(dnSizeId);
    for (const auto& [id, val] : cvList) {
        if (val == cv) return id;
    }
    return std::nullopt;
}

void ValveWindow::onPositionerTypeChanged(int index)
{
    const QString selected = ui->comboBox_positionerType->itemText(index);

    ui->comboBox_dinamicError->clear(); // всегда очищаем перед установкой

    if (selected == QStringLiteral("Интеллектуальный ЭПП")) {
        ui->comboBox_dinamicError->addItem(QStringLiteral("1.5"));
    }
    else {
        ui->comboBox_dinamicError->addItem(QStringLiteral("2.5"));
    }

    ui->comboBox_dinamicError->setCurrentIndex(0);
}

void ValveWindow::refreshCvForDn(std::optional<int> dnIdOpt)
{
    ui->comboBox_CV->clear();

    if (!dnIdOpt || *dnIdOpt <= 0) {
        // DN не распознан — оставляем только "Ручной ввод"
        ui->comboBox_CV->addItem(m_manualInput);
        ui->comboBox_CV->setCurrentIndex(0);
        ui->lineEdit_CV->setEnabled(true);
        return;
    }

    // Заполняем CV для распознанного DN и добавляем "Ручной ввод"
    populateComboInts(ui->comboBox_CV, m_db.getCvValues(*dnIdOpt), /*includeManual=*/true);

    // По умолчанию ставим первый CV (если он есть), иначе "Ручной ввод"
    if (ui->comboBox_CV->count() > 1) {
        ui->comboBox_CV->setCurrentIndex(0);   // первый CV
        ui->lineEdit_CV->clear();
        ui->lineEdit_CV->setEnabled(false);
    } else {
        ui->comboBox_CV->setCurrentIndex(0);   // "Ручной ввод"
        ui->lineEdit_CV->setEnabled(true);
    }
}

void ValveWindow::ensureSaddleManualOption()
{
    if (ui->comboBox_materialSaddle->findText(m_manualInput) < 0) {
        ui->comboBox_materialSaddle->addItem(m_manualInput);
    }
}

void ValveWindow::populateModelsAndDn(int seriesId)
{
    populateComboInts(ui->comboBox_DN, m_db.getValveDnSizes(seriesId), true);
    onDNChanged(0);
}

void ValveWindow::onModelEditingFinished()
{
    if (isManual(ui->comboBox_manufacturer)) {
        return;
    }

    // ui->comboBox_dinamicError->clear();

    QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    int manId = ui->comboBox_manufacturer->currentData().toInt();
    int seriesId = m_db.getSeriesIdByName(manId, seriesName);

    QString model = ui->lineEdit_valveModel->text().trimmed();
    int modelId = m_db.getValveModelIdByName(seriesId, model);

    if (seriesId < 0 || modelId < 0) {
        // ui->comboBox_dinamicError->clear();
        ui->comboBox_materialSaddle->clear();
        return;
    }

    // populateComboInts(ui->comboBox_dinamicError, m_db.getDynamicErrors(modelId));

    populateCombo(ui->comboBox_materialSaddle, m_db.getSaddleMaterialsForModel(modelId), true);

    if (ui->comboBox_manufacturer->currentText() == m_manualInput) {
        // остаёмся в ручном режиме для материала седла
        int manualIdx = ui->comboBox_materialSaddle->findText(m_manualInput);
        if (manualIdx >= 0) {
            ui->comboBox_materialSaddle->setCurrentIndex(manualIdx);
            ui->lineEdit_materialSaddle->setEnabled(true);
        }
    }
}

std::optional<int> ValveWindow::currentModelId() const
{
    const auto sidOpt = currentSeriesId();
    if (!sidOpt) return std::nullopt;

    const QString model = ui->lineEdit_valveModel->text().trimmed();
    const int modelId = m_db.getValveModelIdByName(*sidOpt, model);
    if (modelId < 0) return std::nullopt;
    return modelId;
}

std::optional<int> ValveWindow::resolveSaddleMaterialIdFromManual() const
{
    const auto midOpt = currentModelId();
    if (!midOpt) return std::nullopt;

    const QString typed = ui->lineEdit_materialSaddle->text().trimmed();
    if (typed.isEmpty()) return std::nullopt;

    const auto list = m_db.getSaddleMaterialsForModel(*midOpt);
    for (const auto& [id, text] : list) {
        if (QString::compare(text.trimmed(), typed, Qt::CaseInsensitive) == 0)
            return id;
    }
    return std::nullopt;
}

void ValveWindow::onSeriesEditingFinished()
{
    if (isManual(ui->comboBox_manufacturer)) {
        return;
    }

    QString seriesName = ui->lineEdit_valveSeries->text().trimmed();
    if (seriesName.isEmpty())
        return;

    int manId = ui->comboBox_manufacturer->currentData().toInt();
    int seriesId = m_db.getSeriesIdByName(manId, seriesName);
    if (seriesId < 0) {
        ui->comboBox_DN->clear();
        ui->comboBox_CV->clear();
        ui->lineEdit_valveModel->clear();
        // ui->comboBox_dinamicError->clear();
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
    // Режим "Ручной ввод" для DN через комбобокс
    if (ui->comboBox_DN->itemText(index) == m_manualInput) {
        ui->lineEdit_DN->setEnabled(true);

        // Пытаемся распознать dnId из уже введённого текста (если пользователь уже что-то набрал)
        auto dnIdOpt = resolveDnIdFromManual();
        refreshCvForDn(dnIdOpt);

        // Очистка деталей до выбора CV
        for (auto* le : m_partFields) le->clear();
        return;
    }

    // Обычный режим (DN из списка)
    ui->lineEdit_DN->clear();
    ui->lineEdit_DN->setEnabled(false);

    const int dnSizeId = ui->comboBox_DN->itemData(index).toInt();

    // Заполняем CV по выбранному DN
    refreshCvForDn(dnSizeId);

    // После заполнения CV — пересчитать детали (если не "Ручной ввод" для CV)
    updatePartNumbers();
}

void ValveWindow::onCVChanged(int index)
{
    if (ui->comboBox_CV->itemText(index) == m_manualInput) {
        ui->lineEdit_CV->setEnabled(true);
        // В режиме ручного CV не обращаемся к БД за комплектующими — они завязаны на id
        for (auto* le : m_partFields)
            le->clear();
        return;
    }

    ui->lineEdit_CV->clear();
    ui->lineEdit_CV->setEnabled(false);

    // Пересчитать комплектующие в штатном режиме
    updatePartNumbers();
}

void ValveWindow::updatePartNumbers()
{
    // 1) Определяем dnSizeId
    std::optional<int> dnIdOpt;
    if (isManual(ui->comboBox_DN)) {
        dnIdOpt = resolveDnIdFromManual();
    } else {
        dnIdOpt = ui->comboBox_DN->currentData().toInt();
    }

    // 2) Если DN не определён (не найден по ручному вводу/не выбран) — чистим и выходим
    if (!dnIdOpt || *dnIdOpt <= 0) {
        for (auto* le : m_partFields) le->clear();
        return;
    }

    // 3) Определяем cvValueId
    std::optional<int> cvIdOpt;
    if (isManual(ui->comboBox_CV)) {
        cvIdOpt = resolveCvIdFromManual(*dnIdOpt);
    } else {
        cvIdOpt = ui->comboBox_CV->currentData().toInt();
    }

    // 4) Если CV не определён — тоже чистим и выходим
    if (!cvIdOpt || *cvIdOpt <= 0) {
        for (auto* le : m_partFields) le->clear();
        return;
    }

    // 5) Обычная загрузка комплектующих
    int saddleMaterialId = -1;
    if (isManual(ui->comboBox_materialSaddle)) {
        if (auto idOpt = resolveSaddleMaterialIdFromManual(); idOpt && *idOpt > 0) {
            saddleMaterialId = *idOpt;
        } else {
            // материал не распознан — чистим детали и выходим
            for (auto* le : m_partFields) le->clear();
            return;
        }
    } else {
        saddleMaterialId = ui->comboBox_materialSaddle->currentData().toInt();
    }
    const auto parts = m_db.getValveComponents(*dnIdOpt, *cvIdOpt, saddleMaterialId);

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

    const auto keys = m_partFields.keys();
    for (int i = 0, n = keys.size(); i < n; ++i) {
        const QString& key = keys.at(i);
        QLineEdit* le = m_partFields.value(key);
        const QString code = parts.value(key);
        if (code.isEmpty()) {
            le->clear();
        } else {
            const QString label = displayNames.value(key, key);
            le->setText(QString("%1: %2").arg(code, label));
        }
    }
}


void ValveWindow::setRegistry(Registry *registry)
{
    Q_ASSERT(registry != nullptr);
    m_registry = registry;

    m_valveInfo = m_registry->getValveInfo();
    m_materialsOfComponentParts = m_registry->getMaterialsOfComponentParts();
    m_listDetails = m_registry->getListDetails();

    ui->comboBox_positionNumber->clear();
    ui->comboBox_positionNumber->addItems(m_registry->getPositions());
    ui->comboBox_positionNumber->addItem(m_manualInput);

    QString lastPosition = m_registry->getLastPosition();
    if (lastPosition.isEmpty()) {
        ui->comboBox_positionNumber->setCurrentIndex(ui->comboBox_positionNumber->count() - 1);
    } else {
        int idx = ui->comboBox_positionNumber->findText(lastPosition);
        if (idx >= 0) {
            ui->comboBox_positionNumber->setCurrentIndex(idx);
            positionChanged(lastPosition);
        }
    }

    connect(ui->comboBox_positionNumber,
            &QComboBox::currentTextChanged,
            this,
            &ValveWindow::positionChanged);
}
void ValveWindow::saveValveInfo()
{
    const QString pos = (ui->comboBox_positionNumber->currentText() == m_manualInput)
                            ? ui->lineEdit_positionNumber->text().trimmed()
                            : ui->comboBox_positionNumber->currentText().trimmed();

    if (pos.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер позиции.");
        return;
    }

    m_valveInfo = m_registry->getValveInfo(pos);

    if (ui->comboBox_positionNumber->currentText() == m_manualInput)
        m_valveInfo = m_registry->getValveInfo(ui->lineEdit_positionNumber->text());

    if (pos.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер позиции.");
        return;
    }

    if (ui->comboBox_manufacturer->currentText() == m_manualInput)
        m_valveInfo->manufacturer = ui->lineEdit_manufacturer->text();
    else
        m_valveInfo->manufacturer = ui->comboBox_manufacturer->currentText();

    if (ui->comboBox_driveModel->currentText() == m_manualInput)
        m_valveInfo->driveModel = ui->lineEdit_driveModel->text().trimmed();
    else
        m_valveInfo->driveModel = ui->comboBox_driveModel->currentText();

    if (ui->comboBox_materialSaddle->currentText() == m_manualInput)
        m_materialsOfComponentParts->saddle = ui->lineEdit_materialSaddle->text().trimmed();
    else
        m_materialsOfComponentParts->saddle = ui->comboBox_materialSaddle->currentText();

    m_valveInfo->serialNumber = ui->lineEdit_serialNumber->text();
    m_valveInfo->valveStroke = ui->lineEdit_strokValve->text();
    m_valveInfo->positionerModel = ui->lineEdit_positionerModel->text();

    m_valveInfo->range = ui->lineEdit_driveRange->text();

    m_valveInfo->valveSeries = ui->lineEdit_valveSeries->text();

    m_valveInfo->valveModel = ui->lineEdit_valveModel->text();

    m_valveInfo->diameter = ui->lineEdit_driveDiameter->text().toDouble();
    m_valveInfo->pulley = ui->lineEdit_diameterPulley->text().toDouble();

    m_valveInfo->DN = ui->comboBox_DN->currentText().toInt();
    m_valveInfo->CV = ui->comboBox_CV->currentText().toInt();
    m_valveInfo->PN = ui->lineEdit_PN->text().toInt();

    m_valveInfo->safePosition = ui->comboBox_safePosition->currentIndex();
    m_valveInfo->driveType = ui->comboBox_driveType->currentIndex();
    m_valveInfo->strokeMovement = ui->comboBox_strokeMovement->currentIndex();
    // m_valveInfo->dinamicError = ui->comboBox_dinamicError->currentIndex();
    m_valveInfo->dinamicError = ui->comboBox_dinamicError->currentText().toDouble();

    m_valveInfo->toolNumber = ui->comboBox_toolNumber->currentIndex();

    // m_materialsOfComponentParts->saddle = ui->comboBox_materialSaddle->currentText();
    m_materialsOfComponentParts->corpus = ui->lineEdit_materialCorpus->text();
    m_materialsOfComponentParts->cap = ui->lineEdit_materialCap->text();
    m_materialsOfComponentParts->ball = ui->lineEdit_materialBall->text();
    m_materialsOfComponentParts->disk = ui->lineEdit_materialDisk->text();
    m_materialsOfComponentParts->plunger = ui->lineEdit_materialPlunger->text();
    m_materialsOfComponentParts->shaft = ui->lineEdit_materialShaft->text();
    m_materialsOfComponentParts->stock = ui->lineEdit_materialStock->text();
    m_materialsOfComponentParts->guideSleeve = ui->lineEdit_materialGuideSleeve->text();
    m_materialsOfComponentParts->stuffingBoxSeal = ui->comboBox_materialStuffingBoxSeal->currentText();

    // Перечень деталей
    m_listDetails->plunger = ui->lineEdit_listDetails_plunger->text();
    m_listDetails->saddle = ui->lineEdit_listDetails_saddle->text();
    m_listDetails->upperBushing = ui->lineEdit_listDetails_upperBushing->text();
    m_listDetails->lowerBushing = ui->lineEdit_listDetails_lowerBushing->text();
    m_listDetails->upperORing = ui->lineEdit_listDetails_upperORing->text();
    m_listDetails->lowerORing = ui->lineEdit_listDetails_lowerORing->text();
    m_listDetails->stuffingBoxSeal = ui->lineEdit_listDetails_stuffingBoxSeal->text();
    m_listDetails->driveDiaphragm = ui->lineEdit_listDetails_driveDiaphragm->text();
    m_listDetails->covers = ui->lineEdit_listDetails_covers->text();
    m_listDetails->shaft = ui->lineEdit_listDetails_shaft->text();
    m_listDetails->saddleLock = ui->lineEdit_listDetails_saddleLock->text();

    m_registry->saveListDetails();
    m_registry->saveMaterialsOfComponentParts();
    m_registry->saveValveInfo();
}

void ValveWindow::positionChanged(const QString &position)
{
    m_valveInfo = m_registry->getValveInfo(position);
    m_materialsOfComponentParts = m_registry->getMaterialsOfComponentParts();
    m_listDetails = m_registry->getListDetails();

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
    ui->lineEdit_strokValve->setText(m_valveInfo->valveStroke);
    ui->lineEdit_positionerModel->setText(m_valveInfo->positionerModel);

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

    {
        const QString savedMat = m_materialsOfComponentParts->saddle.trimmed();
        int matIdx = ui->comboBox_materialSaddle->findText(savedMat, Qt::MatchFixedString);
        if (matIdx >= 0) {
            ui->comboBox_materialSaddle->setCurrentIndex(matIdx);
            ui->lineEdit_materialSaddle->clear();
            ui->lineEdit_materialSaddle->setEnabled(false);
        } else {
            int manualIdx = ui->comboBox_materialSaddle->findText(m_manualInput);
            if (manualIdx >= 0) {
                ui->comboBox_materialSaddle->setCurrentIndex(manualIdx);
                ui->lineEdit_materialSaddle->setText(savedMat);
                ui->lineEdit_materialSaddle->setEnabled(true);
            }
        }
    }

    ui->lineEdit_driveRange->setText(m_valveInfo->range);

    ui->comboBox_safePosition->setCurrentIndex(m_valveInfo->safePosition);
    ui->comboBox_driveType->setCurrentIndex(m_valveInfo->driveType);
    ui->comboBox_strokeMovement->setCurrentIndex(m_valveInfo->strokeMovement);
    ui->comboBox_toolNumber->setCurrentIndex(m_valveInfo->toolNumber);
    // ui->comboBox_dinamicError->setCurrentIndex(QString::number(m_valveInfo->dinamicError));

    ui->lineEdit_diameterPulley->setText(QString::number(m_valveInfo->pulley));
    ui->lineEdit_driveDiameter->setText(QString::number(m_valveInfo->diameter));

    // ui->lineEdit_materialStuffingBoxSeal->setText(m_materialsOfComponentParts->stuffingBoxSeal);
    ui->lineEdit_materialCorpus->setText(m_materialsOfComponentParts->corpus);
    ui->lineEdit_materialCap->setText(m_materialsOfComponentParts->cap);
    ui->lineEdit_materialBall->setText(m_materialsOfComponentParts->ball);
    ui->lineEdit_materialDisk->setText(m_materialsOfComponentParts->disk);
    ui->lineEdit_materialPlunger->setText(m_materialsOfComponentParts->plunger);
    ui->lineEdit_materialShaft->setText(m_materialsOfComponentParts->shaft);
    ui->lineEdit_materialStock->setText(m_materialsOfComponentParts->stock);
    ui->lineEdit_materialGuideSleeve->setText(m_materialsOfComponentParts->guideSleeve);

    ui->lineEdit_listDetails_plunger->setText(m_listDetails->plunger);
    ui->lineEdit_listDetails_saddle->setText(m_listDetails->saddle);
    ui->lineEdit_listDetails_upperBushing->setText(m_listDetails->upperBushing);
    ui->lineEdit_listDetails_lowerBushing->setText(m_listDetails->lowerBushing);
    ui->lineEdit_listDetails_upperORing->setText(m_listDetails->upperORing);
    ui->lineEdit_listDetails_lowerORing->setText(m_listDetails->lowerORing);
    ui->lineEdit_listDetails_stuffingBoxSeal->setText(m_listDetails->stuffingBoxSeal);
    ui->lineEdit_listDetails_driveDiaphragm->setText(m_listDetails->driveDiaphragm);
    ui->lineEdit_listDetails_covers->setText(m_listDetails->covers);
    ui->lineEdit_listDetails_shaft->setText(m_listDetails->shaft);
    ui->lineEdit_listDetails_saddleLock->setText(m_listDetails->saddleLock);
}

void ValveWindow::strokeChanged(quint16 n)
{
    ui->comboBox_toolNumber->setEnabled(n == 1);
    ui->lineEdit_diameterPulley->setEnabled(
        (n == 1)
        && (ui->comboBox_toolNumber->currentIndex() == ui->comboBox_toolNumber->count() - 1));
}

void ValveWindow::toolChanged(quint16 n)
{
    if (n == ui->comboBox_toolNumber->count() - 1) {
        ui->lineEdit_diameterPulley->setEnabled(true);
    } else {
        ui->lineEdit_diameterPulley->setEnabled(false);
        ui->lineEdit_diameterPulley->setText(m_diameter[n]);
    }
}

void ValveWindow::diameterChanged(const QString &text)
{
    qreal value = text.toDouble();
    ui->label_square->setText(QString::number(M_PI * value * value / 4.0, 'f', 2));
}

void ValveWindow::on_pushButton_clicked()
{
    if (ui->lineEdit_positionNumber->text().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), QStringLiteral("Введите номер позиции"));
        return;
    }

    if ((ui->lineEdit_serialNumber->text().isEmpty()) or (ui->lineEdit_PN->text().isEmpty())
        or (ui->lineEdit_strokValve->text().isEmpty()) or (ui->lineEdit_positionerModel->text().isEmpty())
        or (ui->lineEdit_driveRange->text().isEmpty())) {
        QMessageBox::StandardButton button
            = QMessageBox::question(this,
                                    QStringLiteral("Предупреждение"),
                                    QStringLiteral("Введены не все данные, вы действительно хотите продолжить?"));

        if (button == QMessageBox::StandardButton::No) {
            return;
        }
    }

    OtherParameters *otherParameters = m_registry->getOtherParameters();
    otherParameters->safePosition = ui->comboBox_safePosition->currentText();
    otherParameters->movement = ui->comboBox_strokeMovement->currentText();

    saveValveInfo();
    accept();
}

void ValveWindow::on_pushButton_clear_clicked()
{
    ui->lineEdit_manufacturer->clear();
    ui->lineEdit_serialNumber->clear();
    ui->lineEdit_PN->clear();
    ui->lineEdit_strokValve->clear();
    ui->lineEdit_positionerModel->clear();
    ui->lineEdit_driveModel->clear();
    ui->lineEdit_driveRange->clear();
    ui->lineEdit_valveModel->clear();

    ui->lineEdit_materialCorpus->clear();
    ui->lineEdit_materialCap->clear();
    ui->lineEdit_materialBall->clear();
    ui->lineEdit_materialDisk->clear();
    ui->lineEdit_materialPlunger->clear();
    ui->lineEdit_materialShaft->clear();
    ui->lineEdit_materialStock->clear();
    ui->lineEdit_materialGuideSleeve->clear();
    ui->comboBox_materialSaddle->setCurrentIndex(0);
    ui->comboBox_materialStuffingBoxSeal->setCurrentIndex(0);
    ui->comboBox_DN->setCurrentIndex(0);
    ui->comboBox_safePosition->setCurrentIndex(0);
    ui->comboBox_driveType->setCurrentIndex(0);
    ui->comboBox_strokeMovement->setCurrentIndex(0);
    ui->comboBox_toolNumber->setCurrentIndex(0);

    ui->lineEdit_driveDiameter->setText("1.0");
    ui->lineEdit_diameterPulley->setText(m_diameter[0]);
}

