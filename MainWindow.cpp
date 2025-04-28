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

    ui->groupBox_DO->setVisible(false);

    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tab_main), false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);

    ui->checkBox_DI1->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_DI1->setFocusPolicy(Qt::NoFocus);
    ui->checkBox_DI2->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_DI2->setFocusPolicy(Qt::NoFocus);

    m_labels[TextObjects::Label_status] = ui->label_status;
    m_labels[TextObjects::Label_init] = ui->label_init;
    m_labels[TextObjects::Label_sensors] = ui->label_sensors;
    m_labels[TextObjects::Label_start_value] = ui->label_start_value;
    m_labels[TextObjects::Label_end_value] = ui->label_end_value;
    m_labels[TextObjects::Label_L_limit] = ui->label_low_limit;
    m_labels[TextObjects::Label_H_limit] = ui->label_high_limit;
    m_labels[TextObjects::Label_pressure_diff] = ui->label_pressure_diff;
    m_labels[TextObjects::Label_friction] = ui->label_friction;
    m_labels[TextObjects::Label_friction_percent] = ui->label_friction_percent;
    m_labels[TextObjects::Label_din_error_mean] = ui->label_din_error_mean;
    m_labels[TextObjects::Label_din_error_mean_percent] = ui->label_din_error_mean_percent;
    m_labels[TextObjects::Label_din_error_max] = ui->label_din_error_max;
    m_labels[TextObjects::Label_din_error_max_percent] = ui->label_din_error_max_percent;
    m_labels[TextObjects::Label_range] = ui->label_range;
    m_labels[TextObjects::Label_low_limit] = ui->label_low_limit;
    m_labels[TextObjects::Label_high_limit] = ui->label_high_limit;
    m_labels[TextObjects::Label_forward] = ui->label_forward;
    m_labels[TextObjects::Label_backward] = ui->label_backward;

    m_lineEdits[TextObjects::LineEdit_linear_sensor] = ui->lineEdit_linear_sensor;
    m_lineEdits[TextObjects::LineEdit_linear_sensor_percent] = ui->lineEdit_linear_sensor_percent;
    m_lineEdits[TextObjects::LineEdit_pressure_sensor1] = ui->lineEdit_pressure_sensor1;
    m_lineEdits[TextObjects::LineEdit_pressure_sensor2] = ui->lineEdit_pressure_sensor2;
    m_lineEdits[TextObjects::LineEdit_pressure_sensor3] = ui->lineEdit_pressure_sensor3;
    m_lineEdits[TextObjects::LineEdit_dinamic_error] = ui->lineEdit_dinamic_real;
    m_lineEdits[TextObjects::LineEdit_stroke] = ui->lineEdit_stroke_real;
    m_lineEdits[TextObjects::LineEdit_range] = ui->lineEdit_range_real;
    m_lineEdits[TextObjects::LineEdit_friction] = ui->lineEdit_friction;
    m_lineEdits[TextObjects::LineEdit_friction_percent] = ui->lineEdit_friction_percent;
    m_lineEdits[TextObjects::LineEdit_forward] = ui->lineEdit_time_forward;
    m_lineEdits[TextObjects::LineEdit_backward] = ui->lineEdit_time_backward;
    m_lineEdits[TextObjects::LineEdit_range_pressure] = ui->lineEdit_range_pressure;

    m_program = new Program;
    m_programThread = new QThread(this);

    m_program->moveToThread(m_programThread);

    connect(ui->pushButton_init, &QPushButton::clicked, m_program, &Program::button_init);
    connect(ui->pushButton_set, &QPushButton::clicked, m_program, &Program::button_set_position);
    // connect(ui->checkBox_autoinit, &QCheckBox::stateChanged, m_program, &Program::checkbox_autoinit);
    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged, m_program, &Program::checkbox_autoinit);

    connect(this, &MainWindow::SetDO, m_program, &Program::button_DO);

    connect(ui->pushButton_DO0, &QPushButton::clicked, this, [&](bool checked) {
        emit SetDO(0, checked);
    });

    connect(ui->pushButton_DO1, &QPushButton::clicked, this, [&](bool checked) {
        emit SetDO(1, checked);
    });

    connect(ui->pushButton_DO2, &QPushButton::clicked, this, [&](bool checked) {
        emit SetDO(2, checked);
    });

    connect(ui->pushButton_DO3, &QPushButton::clicked, this, [&](bool checked) {
        emit SetDO(3, checked);
    });

    connect(ui->pushButton_main_save, &QPushButton::clicked, this, [&] {
        if (ui->tabWidget_maintest->currentWidget() == ui->tab_task) {
            SaveChart(Charts::Task);
        } else if (ui->tabWidget_maintest->currentWidget() == ui->tab_pressure) {
            SaveChart(Charts::Pressure);
        } else if (ui->tabWidget_maintest->currentWidget() == ui->tab_friction) {
            SaveChart(Charts::Friction);
        }
    });

    connect(ui->pushButton_main_start, &QPushButton::clicked, this, &MainWindow::ButtonStartMain);
    connect(ui->pushButton_stroke_start,
            &QPushButton::clicked,
            this,
            &MainWindow::ButtonStartStroke);
    connect(ui->pushButton_tests_start,
            &QPushButton::clicked,
            this,
            &MainWindow::ButtonStartOptional);

    connect(ui->pushButton_stroke_save, &QPushButton::clicked, this, [&] {
        SaveChart(Charts::Stroke);
    });

    connect(ui->pushButton_tests_save, &QPushButton::clicked, this, [&] {
        if (ui->tabWidget_tests->currentWidget() == ui->tab_response) {
            SaveChart(Charts::Response);
        } else if (ui->tabWidget_tests->currentWidget() == ui->tab_resolution) {
            SaveChart(Charts::Resolution);
        } else if (ui->tabWidget_tests->currentWidget() == ui->tab_step) {
            SaveChart(Charts::Step);
        }
    });

    connect(ui->pushButton_open, &QPushButton::clicked, m_program, &Program::button_open);

    connect(ui->pushButton_report, &QPushButton::clicked, this, [&] {
        InitReport();
        bool ok = m_reportSaver->SaveReport(m_report);
        ui->pushButton_open->setEnabled(ok);
    });

    connect(ui->pushButton_pixmap1, &QPushButton::clicked, m_program, &Program::button_pixmap1);
    connect(ui->pushButton_pixmap2, &QPushButton::clicked, m_program, &Program::button_pixmap2);
    connect(ui->pushButton_pixmap3, &QPushButton::clicked, m_program, &Program::button_pixmap3);

    connect(this, &MainWindow::StartMainTest, m_program, &Program::MainTestStart);
    connect(this, &MainWindow::StartStrokeTest, m_program, &Program::StrokeTestStart);
    connect(this, &MainWindow::StartOptionalTest, m_program, &Program::OptionalTestStart);

    connect(this, &MainWindow::StopTest, m_program, &Program::TerminateTest);

    connect(m_program, &Program::StopTest, this, &MainWindow::EndTest);

    connect(m_program, &Program::SetText, this, &MainWindow::SetText);
    connect(m_program, &Program::SetTextColor, this, &MainWindow::SetTextColor);

    connect(m_program, &Program::SetGroupDOVisible, this, [&](bool visible) {
        ui->groupBox_DO->setVisible(visible);
    });

    connect(m_program, &Program::SetButtonsDOChecked, this, &MainWindow::SetButtonsDOChecked);
    connect(m_program, &Program::SetCheckboxDIChecked, this, &MainWindow::SetCheckboxDIChecked);

    connect(this, &MainWindow::SetDAC, m_program, &Program::SetDAC_real);

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

    connect(ui->verticalSlider_task, &QSlider::valueChanged, this, [&](int value) {
        if (qRound(ui->doubleSpinBox_task->value() * 1000) != value) {
            if (ui->doubleSpinBox_task->isEnabled())
                emit SetDAC(value / 1000.0);
            ui->doubleSpinBox_task->setValue(value / 1000.0);
        }
    });

    connect(m_program, &Program::SetTask, this, &MainWindow::SetTask);
    connect(m_program, &Program::SetSensorNumber, this, &MainWindow::SetSensorsNumber);
    connect(m_program, &Program::SetButtonInitEnabled, this, &MainWindow::SetButtonInitEnabled);
    connect(m_program, &Program::EnableSetTask, this, &MainWindow::EnableSetTask);
    connect(m_program, &Program::SetStepResults, this, &MainWindow::SetStepTestResults);
    connect(m_program,
            &Program::GetMainTestParameters,
            this,
            &MainWindow::GetMainTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program,
            &Program::GetStepTestParameters,
            this,
            &MainWindow::GetStepTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program,
            &Program::GetResolutionTestParameters,
            this,
            &MainWindow::GetResolutionTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program,
            &Program::GetResponseTestParameters,
            this,
            &MainWindow::GetResponseTestParameters,
            Qt::BlockingQueuedConnection);

    connect(m_program, &Program::Question, this, &MainWindow::Question, Qt::BlockingQueuedConnection);

    connect(m_reportSaver, &ReportSaver::Question, this, &MainWindow::Question, Qt::DirectConnection);
    connect(m_reportSaver,
            &ReportSaver::GetDirectory,
            this,
            &MainWindow::GetDirectory,
            Qt::DirectConnection);

    connect(ui->pushButton_pixmap1, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_pixmap1, &m_report.image1);
    });

    connect(ui->pushButton_pixmap2, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_pixmap2, &m_report.image2);
    });

    connect(ui->pushButton_pixmap3, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_pixmap3, &m_report.image3);
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

    // InitReport();
    SetSensorsNumber(0);

    ui->tableWidget_step_results->setColumnCount(2);
    ui->tableWidget_step_results->setHorizontalHeaderLabels({"T86", "Перерегулирование"});
    ui->tableWidget_step_results->resizeColumnsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
    m_programThread->quit();
    m_programThread->wait();
}

void MainWindow::SetRegistry(Registry *registry)
{
    m_registry = registry;

    m_reportSaver->SetRegistry(registry);

    ObjectInfo *object_info = m_registry->GetObjectInfo();
    ValveInfo *valveInfo = m_registry->GetValveInfo();
    auto m = m_registry->GetMaterialsOfComponentParts();

    OtherParameters *other_parameters = m_registry->GetOtherParameters();

    ui->lineEdit_object->setText(object_info->object);
    ui->lineEdit_manufacture->setText(object_info->manufactory);
    ui->lineEdit_department->setText(object_info->department);
    ui->lineEdit_FIO->setText(object_info->FIO);
    ui->lineEdit_data->setText(other_parameters->data);

    ui->lineEdit_positionNumber->setText(valveInfo->positionNumber);
    ui->lineEdit_serialNumber->setText(valveInfo->serialNumber);
    ui->lineEdit_valveModel->setText(valveInfo->valveModel);
    ui->lineEdit_manufacturer->setText(valveInfo->manufacturer);
    ui->lineEdit_DNPN->setText(valveInfo->DN + "/" + valveInfo->PN);
    ui->lineEdit_positioner->setText(valveInfo->positioner);
    ui->lineEdit_safePosition->setText(other_parameters->safePosition);
    ui->lineEdit_modelDrive->setText(valveInfo->modelDrive);
    ui->lineEdit_movement->setText(other_parameters->movement);

    ui->lineEdit_dinamicRecomend->setText(valveInfo->dinamicError);
    ui->lineEdit_stroke_recomend->setText(valveInfo->stroke);
    ui->lineEdit_range_recomend->setText(valveInfo->range);

    ui->lineEdit_materialStuffingBoxSeal->setText(m->materialStuffingBoxSeal);

    ui->lineEdit_materialCorpus->       setText(m->materialCorpus);
    ui->lineEdit_materialCap->          setText(m->materialCap);
    ui->lineEdit_materialSaddle->       setText(m->materialSaddle);
    ui->lineEdit_materialBall->         setText(m->materialBall);
    ui->lineEdit_materialDisk->         setText(m->materialDisk);
    ui->lineEdit_materialPlunger->      setText(m->materialPlunger);
    ui->lineEdit_materialShaft->        setText(m->materialShaft);
    ui->lineEdit_materialStock->        setText(m->materialStock);
    ui->lineEdit_materialGuideSleeve->  setText(m->materialGuideSleeve);

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
    ui->tableWidget_step_results->setHorizontalHeaderLabels(
        {QString("T%1").arg(T_value), "Перерегулирование"});

    ui->tableWidget_step_results->setRowCount(results.size());
    QStringList row_names;
    for (int i = 0; i < results.size(); ++i) {
        QString time = results.at(i).T_value == 0
                           ? "Ошибка"
                           : QTime(0, 0).addMSecs(results.at(i).T_value).toString("m:ss.zzz");
        ui->tableWidget_step_results->setItem(i, 0, new QTableWidgetItem(time));
        QString overshoot = QString("%1%").arg(results.at(i).overshoot, 4, 'f', 2);
        ui->tableWidget_step_results->setItem(i, 1, new QTableWidgetItem(overshoot));
        QString row_name = QString("%1-%2").arg(results.at(i).from).arg(results.at(i).to);
        row_names << row_name;
    }
    ui->tableWidget_step_results->setVerticalHeaderLabels(row_names);
    ui->tableWidget_step_results->resizeColumnsToContents();
}

void MainWindow::SetSensorsNumber(quint8 num)
{
    bool no_sensors = num == 0;

    ui->verticalSlider_task->setEnabled(!no_sensors);
    ui->doubleSpinBox_task->setEnabled(!no_sensors);

    ui->pushButton_stroke_start->setEnabled(!no_sensors);
    ui->pushButton_tests_start->setEnabled(!no_sensors);
    ui->pushButton_main_start->setEnabled(num > 1);

    ui->tabWidget->setTabEnabled(1, num > 1);
    ui->tabWidget->setTabEnabled(2, !no_sensors);
    ui->tabWidget->setTabEnabled(3, !no_sensors);

    if (num > 0) {
        ui->checkBox_task->setVisible(num > 1);
        ui->checkBox_line->setVisible(num > 1);
        ui->checkBox_pressure1->setVisible(num > 1);
        ui->checkBox_pressure2->setVisible(num > 2);
        ui->checkBox_pressure3->setVisible(num > 3);

        ui->checkBox_task->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_line->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_pressure1->setCheckState(num > 1 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_pressure2->setCheckState(num > 2 ? Qt::Checked : Qt::Unchecked);
        ui->checkBox_pressure3->setCheckState(num > 3 ? Qt::Checked : Qt::Unchecked);
    }
}

void MainWindow::SetButtonInitEnabled(bool enable)
{
    ui->pushButton_init->setEnabled(enable);
}

void MainWindow::AddPoints(Charts chart, QVector<Point> points)
{
    for (const auto point : points)
        m_charts[chart]->addPoint(point.series_num, point.X, point.Y);
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
        QPair<QList<QPointF>, QList<QPointF>> points_linear = m_charts[Charts::Task]->getpoints(
            1);
        QPair<QList<QPointF>, QList<QPointF>> points_pressure = m_charts[Charts::Pressure]
                                                                    ->getpoints(0);

        points.push_back({points_linear.first.begin(), points_linear.first.end()});
        points.push_back({points_linear.second.begin(), points_linear.second.end()});
        points.push_back({points_pressure.first.begin(), points_pressure.first.end()});
        points.push_back({points_pressure.second.begin(), points_pressure.second.end()});
    }
    if (chart == Charts::Step) {
        QPair<QList<QPointF>, QList<QPointF>> points_linear = m_charts[Charts::Step]->getpoints(1);
        QPair<QList<QPointF>, QList<QPointF>> points_task = m_charts[Charts::Step]->getpoints(0);

        points.clear();
        points.push_back({points_linear.first.begin(), points_linear.first.end()});
        points.push_back({points_task.first.begin(), points_task.first.end()});
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

void MainWindow::GetDirectory(QString current_path, QString &result)
{
    result = QFileDialog::getExistingDirectory(this,
                                               "Выберите папку для сохранения изображений",
                                               current_path);
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
        emit StartOptionalTest(ui->tabWidget_tests->currentIndex());
        StartTest();
    }
}

void MainWindow::SetButtonsDOChecked(quint8 status)
{
    ui->pushButton_DO0->blockSignals(true);
    ui->pushButton_DO1->blockSignals(true);
    ui->pushButton_DO2->blockSignals(true);
    ui->pushButton_DO3->blockSignals(true);

    ui->pushButton_DO0->setChecked((status & (1 << 0)) != 0);
    ui->pushButton_DO1->setChecked((status & (1 << 1)) != 0);
    ui->pushButton_DO2->setChecked((status & (1 << 2)) != 0);
    ui->pushButton_DO3->setChecked((status & (1 << 3)) != 0);

    ui->pushButton_DO0->blockSignals(false);
    ui->pushButton_DO1->blockSignals(false);
    ui->pushButton_DO2->blockSignals(false);
    ui->pushButton_DO3->blockSignals(false);
}

void MainWindow::SetCheckboxDIChecked(quint8 status)
{
    ui->checkBox_DI1->setChecked((status & (1 << 0)) != 0);
    ui->checkBox_DI2->setChecked((status & (1 << 1)) != 0);
}

void MainWindow::InitCharts()
{
    ValveInfo *valve_info = m_registry->GetValveInfo();
    bool rotate = (valve_info->strokeMovement != 0);

    m_charts[Charts::Task] = ui->Chart_task;
    m_charts[Charts::Friction] = ui->Chart_friction;
    m_charts[Charts::Pressure] = ui->Chart_pressure;
    m_charts[Charts::Resolution] = ui->Chart_resolution;
    m_charts[Charts::Response] = ui->Chart_response;
    m_charts[Charts::Stroke] = ui->Chart_stroke;
    m_charts[Charts::Step] = ui->Chart_step;
    m_charts[Charts::Trend] = ui->Chart_trend;
    m_charts[Charts::Cyclic] = ui->Chart_cyclic;

    m_charts[Charts::Task]->setname("Task");
    m_charts[Charts::Friction]->setname("Friction");
    m_charts[Charts::Pressure]->setname("Pressure");
    m_charts[Charts::Resolution]->setname("Resolution");
    m_charts[Charts::Response]->setname("Response");
    m_charts[Charts::Stroke]->setname("Stroke");
    m_charts[Charts::Step]->setname("Step");
    m_charts[Charts::Cyclic]->setname("Cyclic");

    m_charts[Charts::Task]->useTimeaxis(false);
    m_charts[Charts::Task]->addAxis("%.2f bar");

    if (!rotate)
        m_charts[Charts::Task]->addAxis("%.2f mm");
    else
        m_charts[Charts::Task]->addAxis("%.2f deg");

    m_charts[Charts::Task]->addSeries(1, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Task]->addSeries(1,
                                          "Датчик линейных перемещений",
                                          QColor::fromRgb(255, 0, 0));
    m_charts[Charts::Task]->addSeries(0, "Датчик давления 1", QColor::fromRgb(0, 0, 255));
    m_charts[Charts::Task]->addSeries(0, "Датчик давления 2", QColor::fromRgb(0, 200, 0));
    m_charts[Charts::Task]->addSeries(0, "Датчик давления 3", QColor::fromRgb(150, 0, 200));

    m_charts[Charts::Pressure]->useTimeaxis(false);
    m_charts[Charts::Pressure]->setLabelXformat("%.2f bar");
    if (!rotate)
        m_charts[Charts::Pressure]->addAxis("%.2f mm");
    else
        m_charts[Charts::Pressure]->addAxis("%.2f deg");

    m_charts[Charts::Pressure]->addSeries(0,
                                              "Перемещение от давления",
                                              QColor::fromRgb(255, 0, 0));

    m_charts[Charts::Pressure]->addSeries(0, "Линейная регрессия", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Pressure]->visible(1, false);

    m_charts[Charts::Stroke]->useTimeaxis(true);
    m_charts[Charts::Stroke]->addAxis("%.2f%%");
    m_charts[Charts::Stroke]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Stroke]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));

    m_charts[Charts::Step]->useTimeaxis(true);
    m_charts[Charts::Step]->addAxis("%.2f%%");
    m_charts[Charts::Step]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Step]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));

    m_charts[Charts::Response]->useTimeaxis(true);
    m_charts[Charts::Response]->addAxis("%.2f%%");
    m_charts[Charts::Response]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Response]->addSeries(0,
                                         "Датчик линейных перемещений",
                                         QColor::fromRgb(255, 0, 0));

    m_charts[Charts::Resolution]->useTimeaxis(true);
    m_charts[Charts::Resolution]->addAxis("%.2f%%");
    m_charts[Charts::Resolution]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Resolution]->addSeries(0,
                                           "Датчик линейных перемещений",
                                           QColor::fromRgb(255, 0, 0));

    m_charts[Charts::Trend]->useTimeaxis(true);
    m_charts[Charts::Trend]->addAxis("%.2f%%");
    m_charts[Charts::Trend]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Trend]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));
    m_charts[Charts::Trend]->setMaxRange(60000);

    m_charts[Charts::Cyclic]->useTimeaxis(true);
    m_charts[Charts::Cyclic]->addAxis("%.2f%%");
    m_charts[Charts::Cyclic]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    m_charts[Charts::Cyclic]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));

    connect(m_program, &Program::AddPoints, this, &MainWindow::AddPoints);
    connect(m_program, &Program::ClearPoints, this, &MainWindow::ClearPoints);
    connect(m_program, &Program::DublSeries, this, &MainWindow::DublSeries);
    connect(m_program, &Program::SetVisible, this, &MainWindow::SetChartVisible);
    connect(m_program, &Program::SetRegressionEnable, this, &MainWindow::SetRegressionEnable);
    connect(m_program, &Program::ShowDots, this, &MainWindow::ShowDots);

    connect(ui->checkBox_task, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(0, k != 0);
    });

    connect(ui->checkBox_line, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(1, k != 0);
    });

    connect(ui->checkBox_pressure1, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(2, k != 0);
    });

    connect(ui->checkBox_pressure2, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(3, k != 0);
    });

    connect(ui->checkBox_pressure3, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Task]->visible(4, k != 0);
    });

    connect(ui->checkBox_regression, &QCheckBox::checkStateChanged, this, [&](int k) {
        m_charts[Charts::Pressure]->visible(1, k != 0);
    });

    connect(m_program,
            &Program::GetPoints,
            this,
            &MainWindow::GetPoints,
            Qt::BlockingQueuedConnection);

    m_charts[Charts::Friction]->addAxis("%.2f H");
    m_charts[Charts::Friction]->addSeries(0,
                                              "Трение от перемещения",
                                              QColor::fromRgb(255, 0, 0));
    if (!rotate)
        m_charts[Charts::Friction]->setLabelXformat("%.2f mm");
    else
        m_charts[Charts::Friction]->setLabelXformat("%.2f deg");

    ui->checkBox_task->setCheckState(Qt::Unchecked);
    ui->checkBox_line->setCheckState(Qt::Unchecked);
    ui->checkBox_pressure1->setCheckState(Qt::Unchecked);
    ui->checkBox_pressure2->setCheckState(Qt::Unchecked);
    ui->checkBox_pressure3->setCheckState(Qt::Unchecked);

    ui->checkBox_task->setVisible(false);
    ui->checkBox_line->setVisible(false);
    ui->checkBox_pressure1->setVisible(false);
    ui->checkBox_pressure2->setVisible(false);
    ui->checkBox_pressure3->setVisible(false);
}

void MainWindow::SaveChart(Charts chart)
{
    m_reportSaver->SaveImage(m_charts[chart]);
}

void MainWindow::setReport(const ReportSaver::Report &report)
{
    m_report = report;

    InitReport();
}

void MainWindow::GetImage(QLabel *label, QImage *image)
{
    QString img_path = QFileDialog::getOpenFileName(this,
                                                    "Выберите файл",
                                                    m_reportSaver->Directory().absolutePath(),
                                                    "Изображения (*.jpg *.png *.bmp)");

    if (!img_path.isEmpty()) {
        QImage img(img_path);
        *image = img.scaled(1000, 430, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::InitReport()
{

    auto oldData = m_report.data;

    // Общие поля
    m_report.data.push_back({5, 4, ui->lineEdit_object->text()});
    m_report.data.push_back({6, 4, ui->lineEdit_manufacture->text()});
    m_report.data.push_back({7, 4, ui->lineEdit_department->text()});

    // Параметры клапана
    m_report.data.push_back({5, 13, ui->lineEdit_positionNumber->text()});
    m_report.data.push_back({6, 13, ui->lineEdit_serialNumber->text()});
    m_report.data.push_back({7, 13, ui->lineEdit_valveModel->text()});
    m_report.data.push_back({8, 13, ui->lineEdit_manufacturer->text()});
    m_report.data.push_back({9, 13, ui->lineEdit_DNPN->text()});
    m_report.data.push_back({10, 13, ui->lineEdit_positioner->text()});
    m_report.data.push_back({11, 13, ui->lineEdit_pressure->text()});
    m_report.data.push_back({12, 13, ui->lineEdit_safePosition->text()});
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
    m_report.data.push_back({66, 12, ui->lineEdit_data->text()});
    m_report.data.push_back({159, 12, ui->lineEdit_data->text()});

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

    for (const auto &f : oldData) {
        m_report.data.push_back(f);
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
