#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    testing_ = false;

    main_test_settings_ = new MainTestSettings(this);
    step_test_settings_ = new StepTestSettings(this);
    response_test_settings_ = new OtherTestSettings(this);
    resolution_test_settings_ = new OtherTestSettings(this);

    file_saver_ = new FileSaver(this);

    ui->groupBox_DO->setVisible(false);

    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tab_main), false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);

    ui->checkBox_DI1->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_DI1->setFocusPolicy(Qt::NoFocus);
    ui->checkBox_DI2->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_DI2->setFocusPolicy(Qt::NoFocus);

    labels_[TextObjects::Label_status] = ui->label_status;
    labels_[TextObjects::Label_init] = ui->label_init;
    labels_[TextObjects::Label_sensors] = ui->label_sensors;
    labels_[TextObjects::Label_start_value] = ui->label_start_value;
    labels_[TextObjects::Label_end_value] = ui->label_end_value;
    labels_[TextObjects::Label_L_limit] = ui->label_low_limit;
    labels_[TextObjects::Label_H_limit] = ui->label_high_limit;
    labels_[TextObjects::Label_pressure_diff] = ui->label_pressure_diff;
    labels_[TextObjects::Label_friction] = ui->label_friction;
    labels_[TextObjects::Label_friction_percent] = ui->label_friction_percent;
    labels_[TextObjects::Label_din_error_mean] = ui->label_din_error_mean;
    labels_[TextObjects::Label_din_error_mean_percent] = ui->label_din_error_mean_percent;
    labels_[TextObjects::Label_din_error_max] = ui->label_din_error_max;
    labels_[TextObjects::Label_din_error_max_percent] = ui->label_din_error_max_percent;
    labels_[TextObjects::Label_range] = ui->label_range;
    labels_[TextObjects::Label_low_limit] = ui->label_low_limit;
    labels_[TextObjects::Label_high_limit] = ui->label_high_limit;
    labels_[TextObjects::Label_forward] = ui->label_forward;
    labels_[TextObjects::Label_backward] = ui->label_backward;

    line_edits_[TextObjects::LineEdit_linear_sensor] = ui->lineEdit_linear_sensor;
    line_edits_[TextObjects::LineEdit_linear_sensor_percent] = ui->lineEdit_linear_sensor_percent;
    line_edits_[TextObjects::LineEdit_pressure_sensor1] = ui->lineEdit_pressure_sensor1;
    line_edits_[TextObjects::LineEdit_pressure_sensor2] = ui->lineEdit_pressure_sensor2;
    line_edits_[TextObjects::LineEdit_pressure_sensor3] = ui->lineEdit_pressure_sensor3;
    line_edits_[TextObjects::LineEdit_dinamic_error] = ui->lineEdit_dinamic_real;
    line_edits_[TextObjects::LineEdit_stroke] = ui->lineEdit_stroke_real;
    line_edits_[TextObjects::LineEdit_range] = ui->lineEdit_range_real;
    line_edits_[TextObjects::LineEdit_friction] = ui->lineEdit_friction;
    line_edits_[TextObjects::LineEdit_friction_percent] = ui->lineEdit_friction_percent;
    line_edits_[TextObjects::LineEdit_forward] = ui->lineEdit_time_forward;
    line_edits_[TextObjects::LineEdit_backward] = ui->lineEdit_time_backward;
    line_edits_[TextObjects::LineEdit_range_pressure] = ui->lineEdit_range_pressure;

    program_ = new Program;
    program_thread_ = new QThread(this);

    program_->moveToThread(program_thread_);

    connect(ui->pushButton_init, &QPushButton::clicked, program_, &Program::button_init);
    connect(ui->pushButton_set, &QPushButton::clicked, program_, &Program::button_set_position);
    // connect(ui->checkBox_autoinit, &QCheckBox::stateChanged, program_, &Program::checkbox_autoinit);
    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged, program_, &Program::checkbox_autoinit);

    connect(this, &MainWindow::SetDO, program_, &Program::button_DO);

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
            SaveChart(Charts::Main_task);
        } else if (ui->tabWidget_maintest->currentWidget() == ui->tab_pressure) {
            SaveChart(Charts::Main_pressure);
        } else if (ui->tabWidget_maintest->currentWidget() == ui->tab_friction) {
            SaveChart(Charts::Main_friction);
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

    connect(ui->pushButton_open, &QPushButton::clicked, program_, &Program::button_open);
    connect(ui->pushButton_report, &QPushButton::clicked, program_, &Program::button_report);
    connect(ui->pushButton_pixmap1, &QPushButton::clicked, program_, &Program::button_pixmap1);
    connect(ui->pushButton_pixmap2, &QPushButton::clicked, program_, &Program::button_pixmap2);
    connect(ui->pushButton_pixmap3, &QPushButton::clicked, program_, &Program::button_pixmap3);

    connect(this, &MainWindow::StartMainTest, program_, &Program::MainTestStart);
    connect(this, &MainWindow::StartStrokeTest, program_, &Program::StrokeTestStart);
    connect(this, &MainWindow::StartOptionalTest, program_, &Program::OptionalTestStart);
    connect(this, &MainWindow::StopTest, program_, &Program::TerminateTest);

    connect(program_, &Program::StopTest, this, &MainWindow::EndTest);

    connect(program_, &Program::SetText, this, &MainWindow::SetText);
    connect(program_, &Program::SetTextColor, this, &MainWindow::SetTextColor);

    connect(program_, &Program::SetGroupDOVisible, this, [&](bool visible) {
        ui->groupBox_DO->setVisible(visible);
    });

    connect(program_, &Program::SetButtonsDOChecked, this, &MainWindow::SetButtonsDOChecked);
    connect(program_, &Program::SetCheckboxDIChecked, this, &MainWindow::SetCheckboxDIChecked);

    connect(this, &MainWindow::SetDAC, program_, &Program::SetDAC_real);

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

    connect(program_, &Program::SetTask, this, &MainWindow::SetTask);
    connect(program_, &Program::SetSensorNumber, this, &MainWindow::SetSensorsNumber);
    connect(program_, &Program::SetButtonInitEnabled, this, &MainWindow::SetButtonInitEnabled);
    connect(program_, &Program::EnableSetTask, this, &MainWindow::EnableSetTask);
    connect(program_, &Program::SetStepResults, this, &MainWindow::SetStepTestResults);
    connect(program_,
            &Program::GetMainTestParameters,
            this,
            &MainWindow::GetMainTestParameters,
            Qt::BlockingQueuedConnection);

    connect(program_,
            &Program::GetStepTestParameters,
            this,
            &MainWindow::GetStepTestParameters,
            Qt::BlockingQueuedConnection);

    connect(program_,
            &Program::GetResolutionTestParameters,
            this,
            &MainWindow::GetResolutionTestParameters,
            Qt::BlockingQueuedConnection);

    connect(program_,
            &Program::GetResponseTestParameters,
            this,
            &MainWindow::GetResponseTestParameters,
            Qt::BlockingQueuedConnection);

    connect(program_, &Program::Question, this, &MainWindow::Question, Qt::BlockingQueuedConnection);

    connect(file_saver_, &FileSaver::Question, this, &MainWindow::Question, Qt::DirectConnection);
    connect(file_saver_,
            &FileSaver::GetDirectory,
            this,
            &MainWindow::GetDirectory,
            Qt::DirectConnection);

    connect(ui->pushButton_pixmap1, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_pixmap1, &report_.image1);
    });

    connect(ui->pushButton_pixmap2, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_pixmap2, &report_.image2);
    });

    connect(ui->pushButton_pixmap3, &QPushButton::clicked, this, [&] {
        GetImage(ui->label_pixmap3, &report_.image3);
    });

    connect(ui->pushButton_report, &QPushButton::clicked, this, [&] {
        ui->pushButton_open->setEnabled(file_saver_->SaveReport(report_));
    });

    connect(ui->pushButton_open, &QPushButton::clicked, this, [&] {
        QDesktopServices::openUrl(
            QUrl::fromLocalFile(file_saver_->Directory().filePath("report.xlsx")));
    });

    connect(ui->checkBox_autoinit, &QCheckBox::checkStateChanged, this, [&](int state) {
        ui->pushButton_set->setEnabled(state == Qt::Unchecked);
    });

    InitReport();
    SetSensorsNumber(0);

    ui->tableWidget_step_results->setColumnCount(2);
    ui->tableWidget_step_results->setHorizontalHeaderLabels({"T86", "Перерегулирование"});
    ui->tableWidget_step_results->resizeColumnsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
    program_thread_->quit();
    program_thread_->wait();
}

void MainWindow::SetRegistry(Registry *registry)
{
    registry_ = registry;

    ObjectInfo *object_info = registry_->GetObjectInfo();
    ValveInfo *valve_info = registry_->GetValveInfo();
    OtherParameters *other_parameters = registry_->GetOtherParameters();

    ui->lineEdit_object->setText(object_info->object);
    ui->lineEdit_manufacture->setText(object_info->manufactory);
    ui->lineEdit_department->setText(object_info->department);
    ui->lineEdit_FIO->setText(object_info->FIO);
    ui->lineEdit_data->setText(other_parameters->data);

    ui->lineEdit_position->setText(valve_info->positionNumber);
    ui->lineEdit_serial->setText(valve_info->serialNumber);
    ui->lineEdit_valveModel->setText(valve_info->valveModel);
    ui->lineEdit_manufacturer->setText(valve_info->manufacturer);
    ui->lineEdit_DNPN->setText(valve_info->DN + "/" + valve_info->PN);
    ui->lineEdit_positioner->setText(valve_info->positioner);
    ui->lineEdit_safePosition->setText(other_parameters->safePosition);
    ui->lineEdit_modelDrive->setText(valve_info->modelDrive);
    ui->lineEdit_movement->setText(other_parameters->movement);
    ui->lineEdit_materialStuffingBoxSeal->setText(valve_info->materialStuffingBoxSeal);

    ui->lineEdit_dinamicRecomend->setText(valve_info->dinamicError);
    ui->lineEdit_stroke_recomend->setText(valve_info->stroke);
    ui->lineEdit_range_recomend->setText(valve_info->range);

    ui->lineEdit_material_1->setText(valve_info->material1);
    ui->lineEdit_material_2->setText(valve_info->material2);
    //  ui->lineEdit_material_3->setText(valve_info->material3);
    ui->lineEdit_material_4->setText(valve_info->material4);
    ui->lineEdit_material_5->setText(valve_info->material5);
    ui->lineEdit_material_6->setText(valve_info->material6);
    ui->lineEdit_material_7->setText(valve_info->material7);
    ui->lineEdit_material_8->setText(valve_info->material8);
    ui->lineEdit_material_9->setText(valve_info->material9);

    if (valve_info->safePosition != 0) {
        step_test_settings_->reverse();
        response_test_settings_->reverse();
        resolution_test_settings_->reverse();
    }

    InitCharts();

    program_->SetRegistry(registry);
    program_thread_->start();

    file_saver_->SetRegistry(registry);
}

void MainWindow::SetText(TextObjects object, const QString &text)
{
    if (line_edits_.contains(object)) {
        line_edits_[object]->setText(text);
    }

    if (labels_.contains(object)) {
        labels_[object]->setText(text);
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
    if (labels_.contains(object)) {
        labels_[object]->setStyleSheet("color:" + color.name(QColor::HexRgb));
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
        charts_[chart]->addPoint(point.series_num, point.X, point.Y);
}

void MainWindow::ClearPoints(Charts chart)
{
    charts_[chart]->clear();
}

void MainWindow::SetChartVisible(Charts chart, quint16 series, bool visible)
{
    charts_[chart]->visible(series, visible);
}

void MainWindow::ShowDots(bool visible)
{
    charts_[Charts::Main_task]->showdots(visible);
    charts_[Charts::Main_pressure]->showdots(visible);
}

void MainWindow::DublSeries()
{
    charts_[Charts::Main_task]->dublSeries(1);
    charts_[Charts::Main_task]->dublSeries(2);
    charts_[Charts::Main_task]->dublSeries(3);
    charts_[Charts::Main_task]->dublSeries(4);
    charts_[Charts::Main_pressure]->dublSeries(0);
}

void MainWindow::EnableSetTask(bool enable)
{
    ui->verticalSlider_task->setEnabled(enable);
    ui->doubleSpinBox_task->setEnabled(enable);
}

void MainWindow::GetPoints(QVector<QVector<QPointF>> &points, Charts chart)
{
    points.clear();
    if (chart == Charts::Main_task) {
        QPair<QList<QPointF>, QList<QPointF>> points_linear = charts_[Charts::Main_task]->getpoints(
            1);
        QPair<QList<QPointF>, QList<QPointF>> points_pressure = charts_[Charts::Main_pressure]
                                                                    ->getpoints(0);

        points.push_back({points_linear.first.begin(), points_linear.first.end()});
        points.push_back({points_linear.second.begin(), points_linear.second.end()});
        points.push_back({points_pressure.first.begin(), points_pressure.first.end()});
        points.push_back({points_pressure.second.begin(), points_pressure.second.end()});
    }
    if (chart == Charts::Step) {
        QPair<QList<QPointF>, QList<QPointF>> points_linear = charts_[Charts::Step]->getpoints(1);
        QPair<QList<QPointF>, QList<QPointF>> points_task = charts_[Charts::Step]->getpoints(0);

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
    if (main_test_settings_->exec() == QDialog::Accepted) {
        parameters = main_test_settings_->getParameters();
        return;
    }

    parameters.delay = 0;
    return;
}

void MainWindow::GetStepTestParameters(StepTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (step_test_settings_->exec() == QDialog::Accepted) {
        parameters = step_test_settings_->getParameters();
    }
}

void MainWindow::GetResolutionTestParameters(OtherTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (resolution_test_settings_->exec() == QDialog::Accepted) {
        parameters = resolution_test_settings_->getParameters();
    }
}

void MainWindow::GetResponseTestParameters(OtherTestSettings::TestParameters &parameters)
{
    parameters.points.clear();

    if (response_test_settings_->exec() == QDialog::Accepted) {
        parameters = response_test_settings_->getParameters();
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
    testing_ = true;
    ui->statusbar->showMessage("Тест в процессе");
}

void MainWindow::EndTest()
{
    testing_ = false;
    ui->statusbar->showMessage("Тест завершен");
}

void MainWindow::ButtonStartMain()
{
    if (testing_) {
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
    if (testing_) {
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
    if (testing_) {
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
    ValveInfo *valve_info = registry_->GetValveInfo();
    bool rotate = (valve_info->strokeMovement != 0);

    charts_[Charts::Main_task] = ui->Chart_task;
    charts_[Charts::Main_friction] = ui->Chart_friction;
    charts_[Charts::Main_pressure] = ui->Chart_pressure;
    charts_[Charts::Resolution] = ui->Chart_resolution;
    charts_[Charts::Response] = ui->Chart_response;
    charts_[Charts::Stroke] = ui->Chart_stroke;
    charts_[Charts::Step] = ui->Chart_step;
    charts_[Charts::Trend] = ui->Chart_trend;
    charts_[Charts::Cyclic] = ui->Chart_cyclic;

    charts_[Charts::Main_task]->setname("Task");
    charts_[Charts::Main_friction]->setname("Friction");
    charts_[Charts::Main_pressure]->setname("Pressure");
    charts_[Charts::Resolution]->setname("Resolution");
    charts_[Charts::Response]->setname("Response");
    charts_[Charts::Stroke]->setname("Stroke");
    charts_[Charts::Step]->setname("Step");
    charts_[Charts::Cyclic]->setname("Cyclic");

    charts_[Charts::Main_task]->useTimeaxis(false);
    charts_[Charts::Main_task]->addAxis("%.2f bar");

    if (!rotate)
        charts_[Charts::Main_task]->addAxis("%.2f mm");
    else
        charts_[Charts::Main_task]->addAxis("%.2f deg");

    charts_[Charts::Main_task]->addSeries(1, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Main_task]->addSeries(1,
                                          "Датчик линейных перемещений",
                                          QColor::fromRgb(255, 0, 0));
    charts_[Charts::Main_task]->addSeries(0, "Датчик давления 1", QColor::fromRgb(0, 0, 255));
    charts_[Charts::Main_task]->addSeries(0, "Датчик давления 2", QColor::fromRgb(0, 200, 0));
    charts_[Charts::Main_task]->addSeries(0, "Датчик давления 3", QColor::fromRgb(150, 0, 200));

    charts_[Charts::Main_pressure]->useTimeaxis(false);
    charts_[Charts::Main_pressure]->setLabelXformat("%.2f bar");
    if (!rotate)
        charts_[Charts::Main_pressure]->addAxis("%.2f mm");
    else
        charts_[Charts::Main_pressure]->addAxis("%.2f deg");

    charts_[Charts::Main_pressure]->addSeries(0,
                                              "Перемещение от давления",
                                              QColor::fromRgb(255, 0, 0));

    charts_[Charts::Main_pressure]->addSeries(0, "Линейная регрессия", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Main_pressure]->visible(1, false);

    charts_[Charts::Stroke]->useTimeaxis(true);
    charts_[Charts::Stroke]->addAxis("%.2f%%");
    charts_[Charts::Stroke]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Stroke]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));

    charts_[Charts::Step]->useTimeaxis(true);
    charts_[Charts::Step]->addAxis("%.2f%%");
    charts_[Charts::Step]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Step]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));

    charts_[Charts::Response]->useTimeaxis(true);
    charts_[Charts::Response]->addAxis("%.2f%%");
    charts_[Charts::Response]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Response]->addSeries(0,
                                         "Датчик линейных перемещений",
                                         QColor::fromRgb(255, 0, 0));

    charts_[Charts::Resolution]->useTimeaxis(true);
    charts_[Charts::Resolution]->addAxis("%.2f%%");
    charts_[Charts::Resolution]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Resolution]->addSeries(0,
                                           "Датчик линейных перемещений",
                                           QColor::fromRgb(255, 0, 0));

    charts_[Charts::Trend]->useTimeaxis(true);
    charts_[Charts::Trend]->addAxis("%.2f%%");
    charts_[Charts::Trend]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Trend]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));
    charts_[Charts::Trend]->setMaxRange(60000);

    charts_[Charts::Cyclic]->useTimeaxis(true);
    charts_[Charts::Cyclic]->addAxis("%.2f%%");
    charts_[Charts::Cyclic]->addSeries(0, "Задание", QColor::fromRgb(0, 0, 0));
    charts_[Charts::Cyclic]->addSeries(0, "Датчик линейных перемещений", QColor::fromRgb(255, 0, 0));

    connect(program_, &Program::AddPoints, this, &MainWindow::AddPoints);
    connect(program_, &Program::ClearPoints, this, &MainWindow::ClearPoints);
    connect(program_, &Program::DublSeries, this, &MainWindow::DublSeries);
    connect(program_, &Program::SetVisible, this, &MainWindow::SetChartVisible);
    connect(program_, &Program::SetRegressionEnable, this, &MainWindow::SetRegressionEnable);
    connect(program_, &Program::ShowDots, this, &MainWindow::ShowDots);

    connect(ui->checkBox_task, &QCheckBox::checkStateChanged, this, [&](int k) {
        charts_[Charts::Main_task]->visible(0, k != 0);
    });

    connect(ui->checkBox_line, &QCheckBox::checkStateChanged, this, [&](int k) {
        charts_[Charts::Main_task]->visible(1, k != 0);
    });

    connect(ui->checkBox_pressure1, &QCheckBox::checkStateChanged, this, [&](int k) {
        charts_[Charts::Main_task]->visible(2, k != 0);
    });

    connect(ui->checkBox_pressure2, &QCheckBox::checkStateChanged, this, [&](int k) {
        charts_[Charts::Main_task]->visible(3, k != 0);
    });

    connect(ui->checkBox_pressure3, &QCheckBox::checkStateChanged, this, [&](int k) {
        charts_[Charts::Main_task]->visible(4, k != 0);
    });

    connect(ui->checkBox_regression, &QCheckBox::checkStateChanged, this, [&](int k) {
        charts_[Charts::Main_pressure]->visible(1, k != 0);
    });

    connect(program_,
            &Program::GetPoints,
            this,
            &MainWindow::GetPoints,
            Qt::BlockingQueuedConnection);

    charts_[Charts::Main_friction]->addAxis("%.2f H");
    charts_[Charts::Main_friction]->addSeries(0,
                                              "Трение от перемещения",
                                              QColor::fromRgb(255, 0, 0));
    if (!rotate)
        charts_[Charts::Main_friction]->setLabelXformat("%.2f mm");
    else
        charts_[Charts::Main_friction]->setLabelXformat("%.2f deg");

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
    file_saver_->SaveImage(charts_[chart]);
}

void MainWindow::setReport(const FileSaver::Report &report)
{
    currentReport_ = report;

    for (const auto &excelData : std::as_const(currentReport_.data)) {
        // Предполагаем, что line_edit содержит введённый пользователем текст
        qDebug() << "Ячейка (" << excelData.x << ", " << excelData.y
                 << "): " << excelData.line_edit->text();
        report_.data.push_back({excelData.x, excelData.y, excelData.line_edit});

    }

}

void MainWindow::GetImage(QLabel *label, QImage *image)
{
    QString img_path = QFileDialog::getOpenFileName(this,
                                                    "Выберите файл",
                                                    file_saver_->Directory().absolutePath(),
                                                    "Изображения (*.jpg *.png *.bmp)");

    if (!img_path.isEmpty()) {
        QImage img(img_path);
        *image = img.scaled(1000, 430, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::InitReport()
{
    report_.data.push_back({5, 4, ui->lineEdit_object});
    report_.data.push_back({6, 4, ui->lineEdit_manufacture});
    report_.data.push_back({7, 4, ui->lineEdit_department});

    report_.data.push_back({5, 13, ui->lineEdit_position});
    report_.data.push_back({6, 13, ui->lineEdit_serial});
    report_.data.push_back({7, 13, ui->lineEdit_valveModel});
    report_.data.push_back({8, 13, ui->lineEdit_manufacturer});
    report_.data.push_back({9, 13, ui->lineEdit_DNPN});
    report_.data.push_back({10, 13, ui->lineEdit_positioner});
    report_.data.push_back({11, 13, ui->lineEdit_pressure});
    report_.data.push_back({12, 13, ui->lineEdit_safePosition});
    report_.data.push_back({13, 13, ui->lineEdit_modelDrive});
    report_.data.push_back({14, 13, ui->lineEdit_movement});
    report_.data.push_back({15, 13, ui->lineEdit_materialStuffingBoxSeal});

    report_.data.push_back({26, 5, ui->lineEdit_dinamic_real});
    report_.data.push_back({26, 8, ui->lineEdit_dinamicRecomend});
    report_.data.push_back({28, 5, ui->lineEdit_dinamic_ip_real});
    report_.data.push_back({28, 8, ui->lineEdit_dinamic_ip_recomend});
    report_.data.push_back({30, 5, ui->lineEdit_stroke_real});
    report_.data.push_back({30, 8, ui->lineEdit_stroke_recomend});
    report_.data.push_back({32, 5, ui->lineEdit_range_real});
    report_.data.push_back({32, 8, ui->lineEdit_range_recomend});
    report_.data.push_back({34, 5, ui->lineEdit_range_pressure});
    report_.data.push_back({36, 5, ui->lineEdit_friction_percent});
    report_.data.push_back({38, 5, ui->lineEdit_friction});

    report_.data.push_back({52, 5, ui->lineEdit_time_forward});
    report_.data.push_back({52, 8, ui->lineEdit_time_backward});

    report_.data.push_back({74, 4, ui->lineEdit_FIO});

    report_.data.push_back({66, 12, ui->lineEdit_data});
    report_.data.push_back({159, 12, ui->lineEdit_data});

    report_.data.push_back({11, 4, ui->lineEdit_material_1});
    report_.data.push_back({12, 4, ui->lineEdit_material_2});
    report_.data.push_back({13, 4, ui->lineEdit_material_3});
    report_.data.push_back({14, 4, ui->lineEdit_material_4});
    report_.data.push_back({15, 4, ui->lineEdit_material_5});
    report_.data.push_back({16, 4, ui->lineEdit_material_6});
    report_.data.push_back({17, 4, ui->lineEdit_material_7});
    report_.data.push_back({18, 4, ui->lineEdit_material_8});
    report_.data.push_back({19, 4, ui->lineEdit_material_9});

    report_.validation.push_back({"=ЗИП!$A$1:$A$37", "J56:J65"});
    report_.validation.push_back({"=Заключение!$B$1:$B$4", "E42"});
    report_.validation.push_back({"=Заключение!$C$1:$C$3", "E44"});
    report_.validation.push_back({"=Заключение!$E$1:$E$4", "E46"});
    report_.validation.push_back({"=Заключение!$D$1:$D$5", "E48"});
    report_.validation.push_back({"=Заключение!$F$3", "E50"});
}
