#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_testing = false;

    ui->tabWidget->setCurrentIndex(0);

    m_durationTimer = new QTimer(this);
    m_durationTimer->setInterval(100);
    connect(m_durationTimer, &QTimer::timeout,
            this, &MainWindow::onCountdownTimeout);

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

    connect(ui->pushButton_init, &QPushButton::clicked,
            m_program, &Program::button_init);

    connect(ui->pushButton_set, &QPushButton::clicked,
            m_program, &Program::button_set_position);

    // connect(ui->checkBox_autoinit, &QCheckBox::stateChanged, m_program, &Program::checkbox_autoinit);

    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged,
            m_program, &Program::checkbox_autoinit);

    connect(this, &MainWindow::SetDO,
            m_program, &Program::button_DO);

    connect(ui->pushButton_mainTest_start, &QPushButton::clicked,
            this, &MainWindow::ButtonStartMain);

    connect(ui->pushButton_mainTest_save, &QPushButton::clicked, this, [&] {
        if (ui->tabWidget_mainTest->currentWidget() == ui->tab_mainTests_task) {
            SaveChart(Charts::Task);
        } else if (ui->tabWidget_mainTest->currentWidget() == ui->tab_mainTests_pressure) {
            SaveChart(Charts::Pressure);
        } else if (ui->tabWidget_mainTest->currentWidget() == ui->tab_mainTests_friction) {
            SaveChart(Charts::Friction);
        }
    });

    connect(ui->pushButton_strokeTest_start, &QPushButton::clicked,
            this, &MainWindow::ButtonStartStroke);

    connect(ui->pushButton_strokeTest_save, &QPushButton::clicked, this, [&] {
        SaveChart(Charts::Stroke);
    });

    connect(ui->pushButton_optionalTests_start, &QPushButton::clicked,
            this, &MainWindow::ButtonStartOptional);

    connect(ui->pushButton_optionalTests_save, &QPushButton::clicked, this, [&] {
        if (ui->tabWidget_optionalTests->currentWidget() == ui->tab_optionalTests_response) {
            SaveChart(Charts::Response);
        } else if (ui->tabWidget_optionalTests->currentWidget() == ui->frame_chartResolution) {
            SaveChart(Charts::Resolution);
        } else if (ui->tabWidget_optionalTests->currentWidget() == ui->tab_optionalTests_step) {
            SaveChart(Charts::Step);
        }
    });

    connect(ui->pushButton_open, &QPushButton::clicked,
            m_program, &Program::button_open);

    connect(ui->pushButton_report, &QPushButton::clicked, this, [&] {
        InitReport();
        bool ok = m_reportSaver->SaveReport(m_report);
        ui->pushButton_open->setEnabled(ok);
    });

    connect(ui->pushButton_imageChartTask, &QPushButton::clicked,
            m_program, &Program::button_pixmap1);

    connect(ui->pushButton_imageChartPressure, &QPushButton::clicked,
            m_program, &Program::button_pixmap2);

    connect(ui->pushButton_imageChartFriction, &QPushButton::clicked,
            m_program, &Program::button_pixmap3);

    connect(this, &MainWindow::StartMainTest,
            m_program, &Program::MainTestStart);

    connect(this, &MainWindow::StartStrokeTest,
            m_program, &Program::StrokeTestStart);

    connect(this, &MainWindow::StartOptionalTest,
            m_program, &Program::OptionalTestStart);

    connect(this, &MainWindow::StopTest,
            m_program, &Program::TerminateTest);

    connect(m_program, &Program::StopTest,
            this, &MainWindow::EndTest);

    connect(m_program, &Program::SetText,
            this, &MainWindow::SetText);

    connect(m_program, &Program::SetTextColor,
            this, &MainWindow::SetTextColor);

    connect(this, &MainWindow::SetDAC,
            m_program, &Program::SetDAC_real);

    connect(ui->doubleSpinBox_task,
            qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,
            [&](double value) {
                if (qRound(value * 1000) != ui->verticalSlider_task->value()) {
                    if (ui->verticalSlider_task->isEnabled())
                        emit SetDAC(value);
                    ui->verticalSlider_task->setValue(qRound(value * 1000));
                }
            });

    connect(ui->verticalSlider_task, &QSlider::valueChanged,
            this, [&](int value) {
        if (qRound(ui->doubleSpinBox_task->value() * 1000) != value) {
            if (ui->doubleSpinBox_task->isEnabled())
                emit SetDAC(value / 1000.0);
            ui->doubleSpinBox_task->setValue(value / 1000.0);
        }
    });

    connect(ui->pushButton_signal_4mA, &QPushButton::clicked,
            this, [this]() {
        ui->doubleSpinBox_task->setValue(4.0);
    });
    connect(ui->pushButton_signal_8mA, &QPushButton::clicked,
            this, [this]() {
        ui->doubleSpinBox_task->setValue(8.0);
    });
    connect(ui->pushButton_signal_12mA, &QPushButton::clicked,
            this, [this]() {
        ui->doubleSpinBox_task->setValue(12.0);
    });
    connect(ui->pushButton_signal_16mA, &QPushButton::clicked,
            this, [this]() {
        ui->doubleSpinBox_task->setValue(16.0);
    });
    connect(ui->pushButton_signal_20mA, &QPushButton::clicked,
            this, [this]() {
        ui->doubleSpinBox_task->setValue(20.0);
    });

    ui->label_arrowUp->setCursor(Qt::PointingHandCursor);
    ui->label_arrowDown->setCursor(Qt::PointingHandCursor);

    ui->label_arrowUp->installEventFilter(this);
    ui->label_arrowDown->installEventFilter(this);

    connect(m_program, &Program::SetTask,
            this, &MainWindow::SetTask);

    connect(m_program, &Program::SetSensorNumber,
            this, &MainWindow::SetSensorsNumber);

    connect(m_program, &Program::SetButtonInitEnabled,
            this, &MainWindow::SetButtonInitEnabled);

    connect(m_program, &Program::EnableSetTask,
            this, &MainWindow::EnableSetTask);

    connect(m_program, &Program::SetStepResults,
            this, &MainWindow::SetStepTestResults);
    connect(m_program, &Program::GetMainTestParameters,
            this, &MainWindow::GetMainTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::GetStepTestParameters,
            this, &MainWindow::GetStepTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program,  &Program::GetResolutionTestParameters,
            this, &MainWindow::GetResolutionTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::GetResponseTestParameters,
            this, &MainWindow::GetResponseTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::Question,
            this, &MainWindow::Question,
            Qt::BlockingQueuedConnection);

    connect(m_reportSaver, &ReportSaver::Question,
            this, &MainWindow::Question,
            Qt::DirectConnection);

    connect(m_reportSaver, &ReportSaver::GetDirectory,
            this, &MainWindow::GetDirectory,
            Qt::DirectConnection);

    connect(ui->pushButton_imageChartTask, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_imageChartTask, &m_report.image1);
    });

    connect(ui->pushButton_imageChartPressure, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_imageChartPressure, &m_report.image2);
    });

    connect(ui->pushButton_imageChartFriction, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_imageChartFriction, &m_report.image3);
    });

    connect(ui->pushButton_report, &QPushButton::clicked, this, [&] {
        ui->pushButton_open->setEnabled(m_reportSaver->SaveReport(m_report));
    });

    connect(ui->pushButton_open, &QPushButton::clicked, this, [&] {
        QDesktopServices::openUrl(
            QUrl::fromLocalFile(m_reportSaver->Directory().filePath("report.xlsx")));
    });

    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged, this, [&](int state) {
        ui->pushButton_set->setEnabled(state == Qt::Unchecked);
    });

    SetSensorsNumber(0);

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

void MainWindow::onCountdownTimeout()
{
    qint64 elapsedMs = m_elapsedTimer.elapsed();

    qint64 remainingMs = m_totalTestMs - elapsedMs;
    if (remainingMs <= 0) {
        remainingMs = 0;
    }

    QTime t(0, 0);
    t = t.addMSecs(remainingMs);

    ui->lineEdit_testDuration->setText(t.toString("hh:mm:ss.zzz"));

    if (remainingMs == 0) {
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
            ui->label_arrowUp->setPixmap(QPixmap(":/src/img/arrowUpHover.png"));
            return true;
        }
        if (event->type() == QEvent::Leave) {
            ui->label_arrowUp->setPixmap(QPixmap(":/src/img/arrowUp.png"));
            return true;
        }
    }
    if (watched == ui->label_arrowDown) {
        if (event->type() == QEvent::Enter) {
            ui->label_arrowDown->setPixmap(QPixmap(":/src/img/arrowDownHover.png"));
            return true;
        }
        if (event->type() == QEvent::Leave) {
            ui->label_arrowDown->setPixmap(QPixmap(":/src/img/arrowDown.png"));
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::SetRegistry(Registry *registry)
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

    InitCharts();

    m_program->SetRegistry(registry);
    m_programThread->start();

    m_reportSaver->SetRegistry(registry);
    // InitReport();
}

void MainWindow::SetText(TextObjects object, const QString &text)
{
    if (m_lineEdits.contains(object)) {
        m_lineEdits[object]->setText(text);
    }

    if (m_labels.contains(object)) {
        m_labels[object]->setText(text);
    }
}

void MainWindow::SetTask(qreal task)
{
    quint16 i_task = qRound(task * 1000);

    if (ui->doubleSpinBox_task->value() != i_task / 1000.0) {
        ui->doubleSpinBox_task->setValue(i_task / 1000.0);
    }

    if (ui->verticalSlider_task->value() != i_task) {
        ui->verticalSlider_task->setSliderPosition(i_task);
    }
}

void MainWindow::SetTextColor(TextObjects object, const QColor color)
{
    if (m_labels.contains(object)) {
        m_labels[object]->setStyleSheet("color:" + color.name(QColor::HexRgb));
    }
}

void MainWindow::SetStepTestResults(QVector<StepTest::TestResult> results, quint32 T_value)
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

void MainWindow::SetSensorsNumber(quint8 num)
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

    if (num > 0) {
        ui->checkBox_showCurve_task->setVisible(num > 1);
        ui->checkBox_showCurve_moving->setVisible(num > 1);
        ui->checkBox_showCurve_pressure_1->setVisible(num > 1);
        ui->checkBox_showCurve_pressure_2->setVisible(num > 2);
        ui->checkBox_showCurve_pressure_3->setVisible(num > 3);

        ui->groupBox_SettingCurrentSignal->setEnabled(num > 1);

        ui->checkBox_showCurve_task->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_moving->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_pressure_1->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_pressure_2->setCheckState(num > 2 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_showCurve_pressure_3->setCheckState(num > 3 ? Qt::Checked : Qt::Unchecked);
    }
}

void MainWindow::SetButtonInitEnabled(bool enable)
{
    ui->pushButton_init->setEnabled(enable);
}

void MainWindow::AddPoints(Charts chart, QVector<Point> points)
{
    for (const auto point : points)
        m_charts[chart]->addPoint(point.seriesNum, point.X, point.Y);
}

void MainWindow::ClearPoints(Charts chart)
{
    m_charts[chart]->clear();
}

void MainWindow::SetChartVisible(Charts chart, quint16 series, bool visible)
{
    m_charts[chart]->visible(series, visible);
}

void MainWindow::ShowDots(bool visible)
{
    m_charts[Charts::Task]->showdots(visible);
    m_charts[Charts::Pressure]->showdots(visible);
}

void MainWindow::DublSeries()
{
    m_charts[Charts::Task]->dublSeries(1);
    m_charts[Charts::Task]->dublSeries(2);
    m_charts[Charts::Task]->dublSeries(3);
    m_charts[Charts::Task]->dublSeries(4);
    m_charts[Charts::Pressure]->dublSeries(0);
}

void MainWindow::EnableSetTask(bool enable)
{
    ui->verticalSlider_task->setEnabled(enable);
    ui->doubleSpinBox_task->setEnabled(enable);
}

void MainWindow::GetPoints(QVector<QVector<QPointF>> &points, Charts chart)
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

void MainWindow::SetRegressionEnable(bool enable)
{
    ui->checkBox_regression->setEnabled(enable);
    ui->checkBox_regression->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
}

void MainWindow::GetMainTestParameters(MainTestSettings::TestParameters &parameters)
{
    if (m_mainTestSettings->exec() == QDialog::Accepted) {
        parameters = m_mainTestSettings->getParameters();

        qint64 totalMs = m_mainTestSettings->totalTestTimeMillis();
        m_totalTestMs = totalMs;
        m_elapsedTimer.start();
        m_durationTimer->start();
        QTime t(0, 0);
        t = t.addMSecs(totalMs);
        ui->lineEdit_testDuration->setText(t.toString("hh:mm:ss.zzz"));

        return;
    }

    parameters.delay = 0;
    return;
}

void MainWindow::GetStepTestParameters(StepTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (m_stepTestSettings->exec() == QDialog::Accepted) {
        parameters = m_stepTestSettings->getParameters();
    }
}

void MainWindow::GetResolutionTestParameters(OtherTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (m_resolutionTestSettings->exec() == QDialog::Accepted) {
        parameters = m_resolutionTestSettings->getParameters();
    }
}

void MainWindow::GetResponseTestParameters(OtherTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (m_responseTestSettings->exec() == QDialog::Accepted) {
        parameters = m_responseTestSettings->getParameters();
    }
}

void MainWindow::Question(QString title, QString text, bool &result)
{
    result = (QMessageBox::question(NULL, title, text) == QMessageBox::Yes);
}

void MainWindow::GetDirectory(QString currentPath, QString &result)
{
    result = QFileDialog::getExistingDirectory(this,
        "Выберите папку для сохранения изображений",
        currentPath);
}

void MainWindow::StartTest()
{
    m_testing = true;
    ui->statusbar->showMessage("Тест в процессе");
}

void MainWindow::EndTest()
{
    m_testing = false;
    ui->statusbar->showMessage("Тест завершен");

    m_durationTimer->stop();
}

void MainWindow::ButtonStartMain()
{
    if (m_testing) {
        if (QMessageBox::question(this, "Внимание!", "Вы действительно хотите завершить тест")
            == QMessageBox::Yes) {
            emit StopTest();
        }
    } else {
        emit StartMainTest();
        StartTest();
    }
}

void MainWindow::ButtonStartStroke()
{
    if (m_testing) {
        if (QMessageBox::question(this, "Внимание!", "Вы действительно хотите завершить тест")
            == QMessageBox::Yes) {
            emit StopTest();
        }
    } else {
        emit StartStrokeTest();
        StartTest();
    }
}

void MainWindow::ButtonStartOptional()
{
    if (m_testing) {
        if (QMessageBox::question(this, "Внимание!", "Вы действительно хотите завершить тест")
            == QMessageBox::Yes) {
            emit StopTest();
        }
    } else {
        emit StartOptionalTest(ui->tabWidget_optionalTests->currentIndex());
        StartTest();
    }
}

void MainWindow::InitCharts()
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


    // m_charts[Charts::Cyclic]->useTimeaxis(true);
    // m_charts[Charts::Cyclic]->addAxis("%.2f%%");
    // m_charts[Charts::Cyclic]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    // m_charts[Charts::Cyclic]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));


    connect(m_program, &Program::AddPoints, this, &MainWindow::AddPoints);
    connect(m_program, &Program::ClearPoints, this, &MainWindow::ClearPoints);
    connect(m_program, &Program::DublSeries, this, &MainWindow::DublSeries);
    connect(m_program, &Program::SetVisible, this, &MainWindow::SetChartVisible);
    connect(m_program, &Program::SetRegressionEnable, this, &MainWindow::SetRegressionEnable);
    connect(m_program, &Program::ShowDots, this, &MainWindow::ShowDots);

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

    connect(
        m_program,
        &Program::GetPoints,
        this,
        &MainWindow::GetPoints,
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

void MainWindow::SaveChart(Charts chart)
{
    m_reportSaver->SaveImage(m_charts[chart]);

    QPixmap pix = m_charts[chart]->grab();

    switch (chart) {
    case Charts::Task:
        ui->label_imageChartTask->setPixmap(pix);
        break;
    case Charts::Stroke:
        break;
    case Charts::Response:
        break;
    case Charts::Resolution:
        break;
    case Charts::Step:
        break;
    case Charts::Pressure:
        ui->label_imageChartPressure->setPixmap(pix);
        break;
    case Charts::Friction:
        ui->label_imageChartFriction->setPixmap(pix);
        break;
    case Charts::Trend:
        break;
    case Charts::Solenoid:
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

void MainWindow::GetImage(QLabel *label, QImage *image)
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

void MainWindow::InitReport()
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
