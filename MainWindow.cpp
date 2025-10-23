#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_testing = false;

    ui->tabWidget->setCurrentIndex(0);

    m_mainTestSettings = new MainTestSettings(this);
    m_stepTestSettings = new StepTestSettings(this);
    m_responseTestSettings = new OtherTestSettings(this);
    m_resolutionTestSettings = new OtherTestSettings(this);

    m_reportSaver = new ReportSaver(this);

    m_labels[TextObjects::Label_deviceStatusValue] = ui->label_deviceStatusValue;
    m_labels[TextObjects::Label_deviceInitValue] = ui->label_deviceInitValue;
    m_labels[TextObjects::Label_connectedSensorsNumber] = ui->label_connectedSensorsNumber;
    m_labels[TextObjects::Label_startingPositionValue] = ui->label_startingPositionValue;
    m_labels[TextObjects::Label_finalPositionValue] = ui->label_finalPositionValue;
    m_labels[TextObjects::Label_pressureDifferenceValue] = ui->label_pressureDifferenceValue;
    m_labels[TextObjects::Label_frictionForceValue] = ui->label_frictionForceValue;
    m_labels[TextObjects::Label_frictionPercentValue] = ui->label_frictionPercentValue;
    m_labels[TextObjects::Label_dynamicErrorMean] = ui->label_dynamicErrorMean;
    m_labels[TextObjects::Label_dynamicErrorMeanPercent] = ui->label_dynamicErrorMeanPercent;
    m_labels[TextObjects::Label_dynamicErrorMax] = ui->label_dynamicErrorMax;
    m_labels[TextObjects::Label_dynamicErrorMaxPercent] = ui->label_dynamicErrorMaxPercent;
    m_labels[TextObjects::Label_lowLimitValue] = ui->label_lowLimitValue;
    m_labels[TextObjects::Label_highLimitValue] = ui->label_highLimitValue;
    m_labels[TextObjects::Label_forward] = ui->label_forward;
    m_labels[TextObjects::Label_backward] = ui->label_backward;

    m_lineEdits[TextObjects::LineEdit_linearSensor] = ui->lineEdit_linearSensor;
    m_lineEdits[TextObjects::LineEdit_linearSensorPercent] = ui->lineEdit_linearSensorPercent;
    m_lineEdits[TextObjects::LineEdit_pressureSensor_1] = ui->lineEdit_pressureSensor_1;
    m_lineEdits[TextObjects::LineEdit_pressureSensor_2] = ui->lineEdit_pressureSensor_2;
    m_lineEdits[TextObjects::LineEdit_pressureSensor_3] = ui->lineEdit_pressureSensor_3;
    m_lineEdits[TextObjects::LineEdit_dinamic_error] = ui->lineEdit_dinamic_real;

    m_lineEdits[TextObjects::LineEdit_stroke] = ui->lineEdit_stroke_real;
    m_labels[TextObjects::Label_valveStroke_range] = ui->label_valveStroke_range;

    m_lineEdits[TextObjects::LineEdit_range] = ui->lineEdit_range_real;
    m_lineEdits[TextObjects::LineEdit_friction] = ui->lineEdit_friction;
    m_lineEdits[TextObjects::LineEdit_friction_percent] = ui->lineEdit_friction_percent;
    m_lineEdits[TextObjects::LineEdit_forward] = ui->lineEdit_time_forward;
    m_lineEdits[TextObjects::LineEdit_backward] = ui->lineEdit_time_backward;
    m_lineEdits[TextObjects::LineEdit_range_pressure] = ui->lineEdit_range_pressure;

    m_program = new Program;
    m_programThread = new QThread(this);
    m_program->moveToThread(m_programThread);

    m_durationTimer = new QTimer(this);
    m_durationTimer->setInterval(100);
    connect(m_durationTimer, &QTimer::timeout,
            this, &MainWindow::onCountdownTimeout);

    connect(m_programThread, &QThread::finished,
            m_program, &QObject::deleteLater);

    connect(m_program, &Program::totalTestTimeMs,
            this, &MainWindow::onTotalTestTimeMs);

    connect(ui->pushButton_init, &QPushButton::clicked,
            m_program, &Program::initialize);

    connect(ui->pushButton_set, &QPushButton::clicked,
            m_program, &Program::button_set_position);

    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged,
            m_program, &Program::checkbox_autoinit);

    connect(this, &MainWindow::setDO,
            m_program, &Program::button_DO);

    connect(this, &MainWindow::runMainTest,
            m_program, &Program::runningMainTest);

    connect(m_program, &Program::mainTestFinished,
            this, &MainWindow::promptSaveCharts);

    connect(this, &MainWindow::runStrokeTest,
            m_program, &Program::runningStrokeTest);

    connect(this, &MainWindow::runOptionalTest,
            m_program, &Program::runningOptionalTest);

    connect(this, &MainWindow::stopTest,
            m_program, &Program::terminateTest);

    connect(m_program, &Program::stopTest,
            this, &MainWindow::endTest);

    connect(m_program, &Program::setText,
            this, &MainWindow::setText);

    connect(m_program, &Program::setTextColor,
            this, &MainWindow::setTextColor);

    connect(this, &MainWindow::setDac,
            m_program, &Program::setDac_real);

    connect(ui->doubleSpinBox_task,
            qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,
            [&](double value) {
                if (qRound(value * 1000) != ui->verticalSlider_task->value()) {
                    if (ui->verticalSlider_task->isEnabled())
                        emit setDac(value);
                    ui->verticalSlider_task->setValue(qRound(value * 1000));
                }
            });

    connect(ui->verticalSlider_task, &QSlider::valueChanged,
            this, [&](int value) {
        if (qRound(ui->doubleSpinBox_task->value() * 1000) != value) {
            if (ui->doubleSpinBox_task->isEnabled())
                emit setDac(value / 1000.0);
            ui->doubleSpinBox_task->setValue(value / 1000.0);
        }
    });

    ui->label_arrowUp->setCursor(Qt::PointingHandCursor);
    ui->label_arrowDown->setCursor(Qt::PointingHandCursor);

    ui->label_arrowUp->installEventFilter(this);
    ui->label_arrowDown->installEventFilter(this);

    connect(m_program, &Program::setTask,
            this, &MainWindow::setTask);

    connect(m_program, &Program::setSensorNumber,
            this, &MainWindow::setSensorsNumber);

    connect(m_program, &Program::setButtonInitEnabled,
            this, &MainWindow::setButtonInitEnabled);

    connect(m_program, &Program::enableSetTask,
            this, &MainWindow::enableSetTask);

    connect(m_program, &Program::setStepResults,
            this, &MainWindow::setStepTestResults);

    connect(m_program, &Program::getParameters_mainTest,
            this, &MainWindow::receivedParameters_mainTest,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::getParameters_stepTest,
            this, &MainWindow::receivedParameters_stepTest,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::getParameters_resolutionTest,
            this, &MainWindow::receivedParameters_resolutionTest,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::getParameters_responseTest,
            this, &MainWindow::receivedParameters_responseTest,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::question,
            this, &MainWindow::question,
            Qt::BlockingQueuedConnection);

    connect(m_reportSaver, &ReportSaver::Question,
            this, &MainWindow::question,
            Qt::DirectConnection);

    connect(m_reportSaver, &ReportSaver::GetDirectory,
            this, &MainWindow::getDirectory,
            Qt::DirectConnection);

    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged, this, [&](int state) {
        ui->pushButton_set->setEnabled(state == Qt::Unchecked);
    });

    setSensorsNumber(0);

    ui->tableWidget_stepResults->setColumnCount(2);
    ui->tableWidget_stepResults->setHorizontalHeaderLabels({"T86", "Перерегулирование"});
    ui->tableWidget_stepResults->resizeColumnsToContents();

    ui->label_arrowUp->setCursor(Qt::PointingHandCursor);
    ui->label_arrowDown->setCursor(Qt::PointingHandCursor);

    ui->label_arrowUp->installEventFilter(this);
    ui->label_arrowDown->installEventFilter(this);

    ui->label_arrowUp->setCursor(Qt::PointingHandCursor);
    ui->label_arrowDown->setCursor(Qt::PointingHandCursor);

    ui->label_arrowUp->installEventFilter(this);
    ui->label_arrowDown->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    m_programThread->quit();
    m_programThread->wait();
}

void MainWindow::on_pushButton_signal_4mA_clicked()
{
    ui->doubleSpinBox_task->setValue(4.0);
}
void MainWindow::on_pushButton_signal_8mA_clicked()
{
    ui->doubleSpinBox_task->setValue(8.0);
}
void MainWindow::on_pushButton_signal_12mA_clicked()
{
    ui->doubleSpinBox_task->setValue(12.0);
}
void MainWindow::on_pushButton_signal_16mA_clicked()
{
    ui->doubleSpinBox_task->setValue(16.0);
}
void MainWindow::on_pushButton_signal_20mA_clicked()
{
    ui->doubleSpinBox_task->setValue(20.0);
}

void MainWindow::onCountdownTimeout()
{
    quint64 elapsedMs = m_elapsedTimer.elapsed();
    qint64 remainingMs = static_cast<qint64>(m_totalTestMs) - static_cast<qint64>(elapsedMs);
    if (remainingMs < 0) remainingMs = 0;

    auto formatHMS = [](quint64 ms) -> QString {
        const quint64 hours = ms / 3600000ULL;  ms %= 3600000ULL;
        const quint64 mins = ms / 60000ULL;  ms %=60000ULL;
        const quint64 secs = ms / 1000ULL;
        const quint64 msec = ms % 1000ULL;
        return QString("%1:%2:%3.%4")
            .arg(hours, 2, 10, QChar('0'))
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'))
            .arg(msec, 3, 10, QChar('0'));
    };

    ui->statusbar->showMessage(
        QStringLiteral("Тест в процессе. До завершения теста осталось: %1 (прошло %2 из %3)")
            .arg(formatHMS(static_cast<quint64>(remainingMs)))
            .arg(formatHMS(static_cast<quint64>(elapsedMs)))
            .arg(formatHMS(m_totalTestMs))
        );

    if (remainingMs == 0) {
        m_durationTimer->stop();
    }
}

void MainWindow::onTotalTestTimeMs(quint64 totalMs)
{
    m_totalTestMs = totalMs;
    m_elapsedTimer.restart();

    if (!m_durationTimer) {
        m_durationTimer = new QTimer(this);
        m_durationTimer->setInterval(250);
        connect(m_durationTimer, &QTimer::timeout,
                this, &MainWindow::onCountdownTimeout);
    }

    auto formatHMS = [](quint64 ms) -> QString {
        const quint64 hours = ms / 3600000ULL;
        ms %= 3600000ULL;
        const quint64 minutes = ms / 60000ULL;
        ms %= 60000ULL;
        const quint64 seconds = ms / 1000ULL;
        const quint64 msec = ms % 1000ULL;

        return QString("%1:%2:%3.%4")
            .arg(hours,   2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
            .arg(msec,    3, 10, QChar('0'));
    };

    ui->statusbar->showMessage(
        QStringLiteral("Плановая длительность теста: %1").arg(formatHMS(m_totalTestMs))
        );

    m_durationTimer->start();
    onCountdownTimeout();
}

void MainWindow::endTest()
{
    m_testing = false;
    ui->statusbar->showMessage("Тест завершен");

    if (m_durationTimer) {
        m_durationTimer->stop();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (auto w = qobject_cast<QWidget*>(watched)) {
        if (!w->isEnabled()) {
            return false;
        }
    }
    if (watched == ui->label_arrowUp && event->type() == QEvent::MouseButtonRelease) {
        double cur = ui->doubleSpinBox_task->value();
        double nxt = cur + 0.05;
        if (nxt > ui->doubleSpinBox_task->maximum())
            nxt = ui->doubleSpinBox_task->maximum();
        ui->doubleSpinBox_task->setValue(nxt);
        return true;
    }
    if (watched == ui->label_arrowDown && event->type() == QEvent::MouseButtonRelease) {
        double cur = ui->doubleSpinBox_task->value();
        double nxt = cur - 0.05;
        if (nxt < ui->doubleSpinBox_task->minimum())
            nxt = ui->doubleSpinBox_task->minimum();
        ui->doubleSpinBox_task->setValue(nxt);
        return true;
    }
    if (watched == ui->label_arrowUp) {
        if (event->type() == QEvent::Enter) {
            ui->label_arrowUp->setPixmap(QPixmap(":/Src/img/arrowUpHover.png"));
            return true;
        }
        if (event->type() == QEvent::Leave) {
            ui->label_arrowUp->setPixmap(QPixmap(":/Src/img/arrowUp.png"));
            return true;
        }
    }
    if (watched == ui->label_arrowDown) {
        if (event->type() == QEvent::Enter) {
            ui->label_arrowDown->setPixmap(QPixmap(":/Src/img/arrowDownHover.png"));
            return true;
        }
        if (event->type() == QEvent::Leave) {
            ui->label_arrowDown->setPixmap(QPixmap(":/Src/img/arrowDown.png"));
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::setRegistry(Registry *registry)
{
    m_registry = registry;

    m_reportSaver->SetRegistry(registry);

    ValveInfo *valveInfo = m_registry->GetValveInfo();

    ObjectInfo *objectInfo = m_registry->GetObjectInfo();

    ui->lineEdit_object->setText(objectInfo->object);
    ui->lineEdit_manufacture->setText(objectInfo->manufactory);
    ui->lineEdit_department->setText(objectInfo->department);
    ui->lineEdit_FIO->setText(objectInfo->FIO);


    OtherParameters *otherParameters = m_registry->GetOtherParameters();

    ui->lineEdit_date->setText(otherParameters->date);
    ui->lineEdit_movement->setText(otherParameters->movement);
    ui->lineEdit_safePosition->setText(otherParameters->safePosition);

    ui->lineEdit_positionNumber->setText(valveInfo->positionNumber);
    ui->lineEdit_manufacturer->setText(valveInfo->manufacturer);
    ui->lineEdit_valveSeries->setText(valveInfo->valveSeries);
    ui->lineEdit_valveModel->setText(valveInfo->valveModel);
    ui->lineEdit_serialNumber->setText(valveInfo->serialNumber);
    ui->lineEdit_DN->setText(QString::number(valveInfo->DN));
    ui->lineEdit_PN->setText(QString::number(valveInfo->PN));
    ui->lineEdit_CV->setText(QString::number(valveInfo->CV));
    ui->lineEdit_positionerModel->setText(valveInfo->positionerModel);
    ui->lineEdit_modelDrive->setText(valveInfo->driveModel);

    ui->lineEdit_dinamicRecomend->setText(valveInfo->dinamicError);
    ui->lineEdit_stroke_recomend->setText(valveInfo->valveStroke);
    ui->lineEdit_range_recomend->setText(valveInfo->range);


    MaterialsOfComponentParts *materialInfo = m_registry->GetMaterialsOfComponentParts();

    ui->lineEdit_materialStuffingBoxSeal->setText(materialInfo->stuffingBoxSeal);
    ui->lineEdit_materialCap->setText(materialInfo->cap);
    ui->lineEdit_materialCorpus->setText(materialInfo->corpus);
    ui->lineEdit_materialSaddle->setText(materialInfo->saddle);
    ui->lineEdit_materialBall->setText(materialInfo->ball);
    ui->lineEdit_materialDisk->setText(materialInfo->disk);
    ui->lineEdit_materialPlunger->setText(materialInfo->plunger);
    ui->lineEdit_materialShaft->setText(materialInfo->shaft);
    ui->lineEdit_materialStock->setText(materialInfo->stock);
    ui->lineEdit_materialGuideSleeve->setText(materialInfo->guideSleeve);


    if (valveInfo->safePosition != 0) {
        m_stepTestSettings->reverse();
        m_responseTestSettings->reverse();
        m_resolutionTestSettings->reverse();
    }

    initCharts();

    m_program->setRegistry(registry);
    m_programThread->start();

    m_reportSaver->SetRegistry(registry);
    // InitReport();
}


void MainWindow::setText(TextObjects object, const QString &text)
{
    if (m_lineEdits.contains(object)) {
        m_lineEdits[object]->setText(text);
    }

    if (m_labels.contains(object)) {
        m_labels[object]->setText(text);
    }
}

void MainWindow::setTask(qreal task)
{
    quint16 i_task = qRound(task * 1000);

    if (ui->doubleSpinBox_task->value() != i_task / 1000.0) {
        ui->doubleSpinBox_task->setValue(i_task / 1000.0);
    }

    if (ui->verticalSlider_task->value() != i_task) {
        ui->verticalSlider_task->setSliderPosition(i_task);
    }
}

void MainWindow::setTextColor(TextObjects object, const QColor color)
{
    if (m_labels.contains(object)) {
        m_labels[object]->setStyleSheet("color:" + color.name(QColor::HexRgb));
    }
}

void MainWindow::setStepTestResults(const QVector<StepTest::TestResult> &results, quint32 T_value)
{
    ui->tableWidget_stepResults->setHorizontalHeaderLabels(
        {QString("T%1").arg(T_value), "Перерегулирование"});

    ui->tableWidget_stepResults->setRowCount(results.size());
    QStringList rowNames;
    for (int i = 0; i < results.size(); ++i) {
        QString time = results.at(i).T_value == 0
            ? "Ошибка"
            : QTime(0, 0).addMSecs(results.at(i).T_value).toString("m:ss.zzz");
        ui->tableWidget_stepResults->setItem(i, 0, new QTableWidgetItem(time));
        QString overshoot = QString("%1%").arg(results.at(i).overshoot, 4, 'f', 2);
        ui->tableWidget_stepResults->setItem(i, 1, new QTableWidgetItem(overshoot));
        QString rowName = QString("%1-%2").arg(results.at(i).from).arg(results.at(i).to);
        rowNames << rowName;
    }
    ui->tableWidget_stepResults->setVerticalHeaderLabels(rowNames);
    ui->tableWidget_stepResults->resizeColumnsToContents();
}

void MainWindow::setSensorsNumber(quint8 num)
{
    bool noSensors = num == 0;

    ui->verticalSlider_task->setEnabled(!noSensors);
    ui->doubleSpinBox_task->setEnabled(!noSensors);

    ui->pushButton_strokeTest_start->setEnabled(!noSensors);
    ui->pushButton_optionalTests_start->setEnabled(!noSensors);
    ui->pushButton_mainTest_start->setEnabled(num > 1);

    ui->tabWidget->setTabEnabled(1, num > 1);
    ui->tabWidget->setTabEnabled(2, !noSensors);
    ui->tabWidget->setTabEnabled(3, !noSensors);
    ui->groupBox_SettingCurrentSignal->setEnabled(!noSensors);

    if (num > 0) {
        ui->checkBox_showCurve_task->setVisible(num > 1);
        ui->checkBox_showCurve_moving->setVisible(num > 1);
        ui->checkBox_showCurve_pressure_1->setVisible(num > 1);
        ui->checkBox_showCurve_pressure_2->setVisible(num > 2);
        ui->checkBox_showCurve_pressure_3->setVisible(num > 3);


        ui->checkBox_showCurve_task->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_moving->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_pressure_1->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_pressure_2->setCheckState(num > 2 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_pressure_3->setCheckState(num > 3 ? Qt::Checked : Qt::Unchecked);
    }
}

void MainWindow::setButtonInitEnabled(bool enable)
{
    ui->pushButton_init->setEnabled(enable);
}

void MainWindow::addPoints(Charts chart, QVector<Point> points)
{
    for (const auto point : points)
        m_charts[chart]->addPoint(point.seriesNum, point.X, point.Y);
}

void MainWindow::clearPoints(Charts chart)
{
    m_charts[chart]->clear();
}

void MainWindow::setChartVisible(Charts chart, quint16 series, bool visible)
{
    m_charts[chart]->visible(series, visible);
}

void MainWindow::showDots(bool visible)
{
    m_charts[Charts::Task]->showdots(visible);
    m_charts[Charts::Pressure]->showdots(visible);
}

void MainWindow::dublSeries()
{
    m_charts[Charts::Task]->dublSeries(1);
    m_charts[Charts::Task]->dublSeries(2);
    m_charts[Charts::Task]->dublSeries(3);
    m_charts[Charts::Task]->dublSeries(4);
    m_charts[Charts::Pressure]->dublSeries(0);
}

void MainWindow::enableSetTask(bool enable)
{
    ui->verticalSlider_task->setEnabled(enable);
    ui->doubleSpinBox_task->setEnabled(enable);
}

void MainWindow::getPoints(QVector<QVector<QPointF>> &points, Charts chart)
{
    points.clear();
    if (chart == Charts::Task) {
        QPair<QList<QPointF>, QList<QPointF>> pointsLinear = m_charts[Charts::Task]->getPoints(1);

        QPair<QList<QPointF>, QList<QPointF>> pointsPressure = m_charts[Charts::Pressure]->getPoints(0);

        points.push_back({pointsLinear.first.begin(), pointsLinear.first.end()});
        points.push_back({pointsLinear.second.begin(), pointsLinear.second.end()});
        points.push_back({pointsPressure.first.begin(), pointsPressure.first.end()});
        points.push_back({pointsPressure.second.begin(), pointsPressure.second.end()});
    }
    if (chart == Charts::Step) {
        QPair<QList<QPointF>, QList<QPointF>> pointsLinear = m_charts[Charts::Step]->getPoints(1);
        QPair<QList<QPointF>, QList<QPointF>> pointsTask = m_charts[Charts::Step]->getPoints(0);

        points.clear();
        points.push_back({pointsLinear.first.begin(), pointsLinear.first.end()});
        points.push_back({pointsTask.first.begin(), pointsTask.first.end()});
    }
}

void MainWindow::setRegressionEnable(bool enable)
{
    ui->checkBox_regression->setEnabled(enable);
    ui->checkBox_regression->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
}

void MainWindow::receivedParameters_mainTest(MainTestSettings::TestParameters &parameters)
{
    if (m_mainTestSettings->exec() == QDialog::Accepted) {
        parameters = m_mainTestSettings->getParameters();
        return;
    }
    return;
}

void MainWindow::receivedParameters_stepTest(StepTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (m_stepTestSettings->exec() == QDialog::Accepted) {
        parameters = m_stepTestSettings->getParameters();
    }
}

void MainWindow::receivedParameters_resolutionTest(OtherTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (m_resolutionTestSettings->exec() == QDialog::Accepted) {
        parameters = m_resolutionTestSettings->getParameters();
    }
}

void MainWindow::receivedParameters_responseTest(OtherTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (m_responseTestSettings->exec() == QDialog::Accepted) {
        parameters = m_responseTestSettings->getParameters();
    }
}

void MainWindow::question(const QString &title, const QString &text, bool &result)
{
    result = (QMessageBox::question(NULL, title, text) == QMessageBox::Yes);
}

void MainWindow::getDirectory(const QString &currentPath, QString &result)
{
    result = QFileDialog::getExistingDirectory(this,
        "Выберите папку для сохранения изображений",
        currentPath);
}

void MainWindow::startTest()
{
    m_testing = true;
    ui->statusbar->showMessage("Тест в процессе");
}

void MainWindow::on_pushButton_mainTest_start_clicked()
{
    if (m_testing) {
        if (QMessageBox::question(this, "Внимание!", "Вы действительно хотите завершить тест")
            == QMessageBox::Yes) {
            emit stopTest();
        }
    } else {
        emit runMainTest();
        startTest();
    }
}
void MainWindow::on_pushButton_mainTest_save_clicked()
{
    if (ui->tabWidget_mainTests->currentWidget() == ui->tab_mainTests_task) {
        saveChart(Charts::Task);
    } else if (ui->tabWidget_mainTests->currentWidget() == ui->tab_mainTests_pressure) {
        saveChart(Charts::Pressure);
    } else if (ui->tabWidget_mainTests->currentWidget() == ui->tab_mainTests_friction) {
        saveChart(Charts::Friction);
    }
}

void MainWindow::promptSaveCharts()
{
    if (m_userCanceled)
        return;

    auto answer = QMessageBox::question(
        this,
        tr("Сохранение результатов"),
        tr("Тест MainTest завершён.\nСохранитаь графики Task, Pressure и Friction?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
        );

    if (answer == QMessageBox::Yes) {
        saveChart(Charts::Task);
        saveChart(Charts::Pressure);
        saveChart(Charts::Friction);
        QMessageBox::information(
            this,
            tr("Готово"),
            tr("Графики сохранены в текущую папку отчётов.")
            );
    }
}

void MainWindow::on_pushButton_strokeTest_start_clicked()
{
    if (m_testing) {
        if (QMessageBox::question(this, "Внимание!", "Вы действительно хотите завершить тест")
            == QMessageBox::Yes) {
            emit stopTest();
        }
    } else {
        emit runStrokeTest();
        startTest();
    }
}
void MainWindow::on_pushButton_strokeTest_save_clicked()
{
    saveChart(Charts::Stroke);
}

void MainWindow::on_pushButton_optionalTests_start_clicked()
{
    if (m_testing) {
        if (QMessageBox::question(this, "Внимание!", "Вы действительно хотите завершить тест")
            == QMessageBox::Yes) {
            emit stopTest();
        }
    } else {
        emit runOptionalTest(ui->tabWidget_optionalTests->currentIndex());
        startTest();
    }
}
void MainWindow::on_pushButton_optionalTests_save_clicked()
{
    if (ui->tabWidget_optionalTests->currentWidget() == ui->tab_optionalTests_response) {
        saveChart(Charts::Response);
    } else if (ui->tabWidget_optionalTests->currentWidget() == ui->tab_optionalTests_resolution) {
        saveChart(Charts::Resolution);
    } else if (ui->tabWidget_optionalTests->currentWidget() == ui->tab_optionalTests_step) {
        saveChart(Charts::Step);
    }
}

void MainWindow::initCharts()
{
    ValveInfo *valveInfo = m_registry->GetValveInfo();
    bool rotate = (valveInfo->strokeMovement != 0);

    m_charts[Charts::Task] = ui->Chart_task;
    m_charts[Charts::Task]->setName("Task");
    m_charts[Charts::Task]->useTimeaxis(false);
    m_charts[Charts::Task]->addAxis("%.2f bar");
    if (!rotate)
        m_charts[Charts::Task]->addAxis("%.2f mm");
    else
        m_charts[Charts::Task]->addAxis("%.2f deg");

    m_charts[Charts::Task]->addSeries(1, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Task]->addSeries(1, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));
    m_charts[Charts::Task]->addSeries(0, "Датчик давления 1", QColor::fromRgb(0, 0, 255));
    m_charts[Charts::Task]->addSeries(0, "Датчик давления 2", QColor::fromRgb(0, 200, 0));
    m_charts[Charts::Task]->addSeries(0, "Датчик давления 3", QColor::fromRgb(150, 0, 200));


    m_charts[Charts::Friction] = ui->Chart_friction;
    m_charts[Charts::Friction]->setName("Friction");
    m_charts[Charts::Friction]->addAxis("%.2f H");
    m_charts[Charts::Friction]->addSeries(0, "Трение от перемещения", QColor::fromRgb(255, 0, 0));
    if (!rotate) {
        m_charts[Charts::Friction]->setLabelXformat("%.2f mm");
    } else {
        m_charts[Charts::Friction]->setLabelXformat("%.2f deg");
    }


    m_charts[Charts::Pressure] = ui->Chart_pressure;
    m_charts[Charts::Pressure]->setName("Pressure");
    m_charts[Charts::Pressure]->useTimeaxis(false);
    m_charts[Charts::Pressure]->setLabelXformat("%.2f bar");
    if (!rotate) {
        m_charts[Charts::Pressure]->addAxis("%.2f mm");
    } else {
        m_charts[Charts::Pressure]->addAxis("%.2f deg");
    }
    m_charts[Charts::Pressure]->addSeries(0, "Перемещение от давления", QColor::fromRgb(255, 0, 0));
    m_charts[Charts::Pressure]->addSeries(0, "Линейная регрессия", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Pressure]->visible(1, false);


    m_charts[Charts::Resolution] = ui->Chart_resolution;
    m_charts[Charts::Resolution]->setName("Resolution");
    m_charts[Charts::Resolution]->useTimeaxis(true);
    m_charts[Charts::Resolution]->addAxis("%.2f%%");
    m_charts[Charts::Resolution]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Resolution]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));


    m_charts[Charts::Response] = ui->Chart_response;
    m_charts[Charts::Response]->setName("Response");
    m_charts[Charts::Response]->useTimeaxis(true);
    m_charts[Charts::Response]->addAxis("%.2f%%");
    m_charts[Charts::Response]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Response]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));


    m_charts[Charts::Stroke] = ui->Chart_stroke;
    m_charts[Charts::Stroke]->setName("Stroke");
    m_charts[Charts::Stroke]->useTimeaxis(true);
    m_charts[Charts::Stroke]->addAxis("%.2f%%");
    m_charts[Charts::Stroke]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Stroke]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));


    m_charts[Charts::Step] = ui->Chart_step;
    m_charts[Charts::Step]->setName("Step");
    m_charts[Charts::Step]->useTimeaxis(true);
    m_charts[Charts::Step]->addAxis("%.2f%%");
    m_charts[Charts::Step]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Step]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));


    m_charts[Charts::Trend] = ui->Chart_trend;
    m_charts[Charts::Trend]->useTimeaxis(true);
    m_charts[Charts::Trend]->addAxis("%.2f%%");
    m_charts[Charts::Trend]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Trend]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));
    m_charts[Charts::Trend]->setMaxRange(60000);

    connect(m_program, &Program::addPoints,
            this, &MainWindow::addPoints);

    connect(m_program, &Program::clearPoints,
            this, &MainWindow::clearPoints);

    connect(m_program, &Program::dublSeries,
            this, &MainWindow::dublSeries);

    connect(m_program, &Program::setVisible,
            this, &MainWindow::setChartVisible);

    connect(m_program, &Program::setRegressionEnable,
            this, &MainWindow::setRegressionEnable);

    connect(m_program, &Program::showDots,
            this, &MainWindow::showDots);

    connect(ui->checkBox_showCurve_task, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(0, k != 0);
    });

    connect(ui->checkBox_showCurve_moving, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(1, k != 0);
    });

    connect(ui->checkBox_showCurve_pressure_1, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(2, k != 0);
    });

    connect(ui->checkBox_showCurve_pressure_2, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(3, k != 0);
    });

    connect(ui->checkBox_showCurve_pressure_3, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(4, k != 0);
    });

    connect(ui->checkBox_regression, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Pressure]->visible(1, k != 0);
    });

    connect( m_program, &Program::getPoints,
        this, &MainWindow::getPoints,
        Qt::BlockingQueuedConnection
    );

    ui->checkBox_showCurve_task->setCheckState(Qt::Unchecked);
    ui->checkBox_showCurve_moving->setCheckState(Qt::Unchecked);
    ui->checkBox_showCurve_pressure_1->setCheckState(Qt::Unchecked);
    ui->checkBox_showCurve_pressure_2->setCheckState(Qt::Unchecked);
    ui->checkBox_showCurve_pressure_3->setCheckState(Qt::Unchecked);

    ui->checkBox_showCurve_task->setVisible(false);
    ui->checkBox_showCurve_moving->setVisible(false);
    ui->checkBox_showCurve_pressure_1->setVisible(false);
    ui->checkBox_showCurve_pressure_2->setVisible(false);
    ui->checkBox_showCurve_pressure_3->setVisible(false);
}

void MainWindow::saveChart(Charts chart)
{
    m_reportSaver->SaveImage(m_charts[chart]);

    QPixmap pix = m_charts[chart]->grab();
    QImage img = pix.toImage();

    switch (chart) {
    case Charts::Task:
        ui->label_imageChartTask->setPixmap(pix);
        m_imageChartTask = img;
        break;
    case Charts::Stroke:
        break;
    case Charts::Response:
        break;
    case Charts::Resolution:
        break;
    case Charts::Step:
        m_imageChartStep = img;
        break;
    case Charts::Pressure:
        ui->label_imageChartPressure->setPixmap(pix);
        m_imageChartPressure = img;
        break;
    case Charts::Friction:
        ui->label_imageChartFriction->setPixmap(pix);
        m_imageChartFriction = img;
        break;
    case Charts::Trend:
        break;
    default:
        break;
    }

    ui->label_imageChartTask->setScaledContents(true);
    ui->label_imageChartPressure->setScaledContents(true);
    ui->label_imageChartFriction->setScaledContents(true);
}

void MainWindow::setReport(const ReportSaver::Report &report)
{
    m_report = report;
}

void MainWindow::getImage(QLabel *label, QImage *image)
{
    QString imgPath = QFileDialog::getOpenFileName(this,
        "Выберите файл",
        m_reportSaver->Directory().absolutePath(),
        "Изображения (*.jpg *.png *.bmp)");

    if (!imgPath.isEmpty()) {
        QImage img(imgPath);
        *image = img.scaled(1000, 430, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::on_pushButton_imageChartTask_clicked()
{
    getImage(ui->label_imageChartTask, &m_imageChartTask);
}
void MainWindow::on_pushButton_imageChartPressure_clicked()
{
    getImage(ui->label_imageChartPressure, &m_imageChartPressure);
}
void MainWindow::on_pushButton_imageChartFriction_clicked()
{
    getImage(ui->label_imageChartFriction, &m_imageChartFriction);
}

void MainWindow::initReport()
{
    auto dataFromOtherFiles = m_report.data;

    // Общие поля
    m_report.data.push_back({5, 4, ui->lineEdit_object->text()});
    m_report.data.push_back({6, 4, ui->lineEdit_manufacture->text()});
    m_report.data.push_back({7, 4, ui->lineEdit_department->text()});

    // Параметры клапана
    m_report.data.push_back({5, 13, ui->lineEdit_positionNumber->text()});
    m_report.data.push_back({6, 13, ui->lineEdit_serialNumber->text()});
    m_report.data.push_back({7, 13, ui->lineEdit_valveModel->text()});
    m_report.data.push_back({8, 13, ui->lineEdit_manufacturer->text()});

    m_report.data.push_back({9, 13, ui->lineEdit_DN->text() + "/" + ui->lineEdit_PN->text()});
    m_report.data.push_back({10, 13, ui->lineEdit_positionerModel->text()});
    m_report.data.push_back({11, 13, ui->lineEdit_pressure->text()});
    m_report.data.push_back({12, 13, ui->lineEdit_safePosition->text()});
    m_report.data.push_back({13, 6, ui->lineEdit_CV->text()});
    m_report.data.push_back({13, 13, ui->lineEdit_modelDrive->text()});
    m_report.data.push_back({14, 13, ui->lineEdit_movement->text()});
    m_report.data.push_back({15, 13, ui->lineEdit_materialStuffingBoxSeal->text()});

    // Динамические характеристики
    m_report.data.push_back({26, 5, ui->lineEdit_dinamic_real->text()});
    m_report.data.push_back({26, 8, ui->lineEdit_dinamicRecomend->text()});
    m_report.data.push_back({28, 5, ui->lineEdit_dinamic_ip_real->text()});
    m_report.data.push_back({28, 8, ui->lineEdit_dinamic_ip_recomend->text()});
    m_report.data.push_back({30, 5, ui->lineEdit_stroke_real->text()});
    m_report.data.push_back({30, 8, ui->lineEdit_stroke_recomend->text()});
    m_report.data.push_back({32, 5, ui->lineEdit_range_real->text()});
    m_report.data.push_back({32, 8, ui->lineEdit_range_recomend->text()});
    m_report.data.push_back({34, 5, ui->lineEdit_range_pressure->text()});
    m_report.data.push_back({36, 5, ui->lineEdit_friction_percent->text()});
    m_report.data.push_back({38, 5, ui->lineEdit_friction->text()});

    // Временные показатели
    m_report.data.push_back({52, 5,  ui->lineEdit_time_forward->text()});
    m_report.data.push_back({52, 8,  ui->lineEdit_time_backward->text()});

    // Пользователь и дата
    m_report.data.push_back({74, 4, ui->lineEdit_FIO->text()});

    m_report.data.push_back({66, 12, ui->lineEdit_date->text()});
    m_report.data.push_back({159, 12, ui->lineEdit_date->text()});

    // Материалы
    m_report.data.push_back({11, 4, ui->lineEdit_materialCorpus->text()});
    m_report.data.push_back({12, 4, ui->lineEdit_materialCap->text()});
    m_report.data.push_back({13, 4, ui->lineEdit_materialSaddle->text()});
    m_report.data.push_back({14, 4, ui->lineEdit_materialBall->text()});
    m_report.data.push_back({15, 4, ui->lineEdit_materialDisk->text()});
    m_report.data.push_back({16, 4, ui->lineEdit_materialPlunger->text()});
    m_report.data.push_back({17, 4, ui->lineEdit_materialShaft->text()});
    m_report.data.push_back({18, 4, ui->lineEdit_materialStock->text()});
    m_report.data.push_back({19, 4, ui->lineEdit_materialGuideSleeve->text()});

    for (const auto &data : dataFromOtherFiles) {
        m_report.data.push_back(data);
    }

    m_report.validation = {
       {"=ЗИП!$A$1:$A$37", "J56:J65"},
       {"=Заключение!$B$1:$B$4", "E42"},
       {"=Заключение!$C$1:$C$3", "E44"},
       {"=Заключение!$E$1:$E$4", "E46"},
       {"=Заключение!$D$1:$D$5", "E48"},
       {"=Заключение!$F$3", "E50"},
    };
}

void MainWindow::on_pushButton_report_generate_clicked() {
    initReport();
    bool ok = m_reportSaver->SaveReport(m_report);
    ui->pushButton_report_open->setEnabled(ok);
}

void MainWindow::on_pushButton_report_open_clicked()
{
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(m_reportSaver->Directory().filePath("report.xlsx")));
}
