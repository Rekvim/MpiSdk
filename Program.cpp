#include "Program.h"

#include "./Src/Tests/StepTest.h"
#include "./Src/Tests/StrokeTest.h"
#include "./Src/Tests/MainTest.h"

Program::Program(QObject *parent)
    : QObject{parent}
{
    m_timerSensors = new QTimer(this);
    m_timerSensors->setInterval(200);

    connect(m_timerSensors, &QTimer::timeout,
            this, &Program::UpdateSensors);

    m_testing = false;
    m_dacEventloop = new QEventLoop(this);

    m_timerDI = new QTimer(this);
    m_timerDI->setInterval(1000);
    connect(m_timerDI, &QTimer::timeout, this, [&]() {
        quint8 DI = m_mpi.GetDIStatus();
        emit SetCheckboxDIChecked(DI);
    });
}

void Program::SetRegistry(Registry *registry)
{
    m_registry = registry;
}

void Program::SetDac(quint16 dac, quint32 sleepMs, bool waitForStop, bool waitForStart)
{
    m_stopSetDac = false;

    if (m_mpi.SensorCount() == 0) {
        emit ReleaseBlock();
        return;
    }

    m_mpi.SetDAC_Raw(dac);

    if (waitForStart) {
        QTimer timer;
        timer.setInterval(50);

        QList<quint16> lineSensor;

        connect(&timer, &QTimer::timeout, this, [&]() {
            lineSensor.push_back(m_mpi[0]->GetRawValue());
            if (qAbs(lineSensor.first() - lineSensor.last()) > 10) {
                timer.stop();
                m_dacEventloop->quit();
            }
            if (lineSensor.size() > 50) {
                lineSensor.pop_front();
            }
        });

        timer.start();
        m_dacEventloop->exec();
        timer.stop();
    }

    if (m_stopSetDac) {
        emit ReleaseBlock();
        return;
    }

    if (sleepMs > 20) {
        QTimer timer;
        connect(&timer, &QTimer::timeout, m_dacEventloop, &QEventLoop::quit);
        timer.start(sleepMs);
        m_dacEventloop->exec();
        timer.stop();
    }

    if (m_stopSetDac) {
        emit ReleaseBlock();
        return;
    }

    if (waitForStop) {
        QTimer timer;
        timer.setInterval(50);

        QList<quint16> lineSensor;

        connect(&timer, &QTimer::timeout, this, [&]() {
            lineSensor.push_back(m_mpi[0]->GetRawValue());
            if (lineSensor.size() == 50) {
                if (qAbs(lineSensor.first() - lineSensor.last()) < 10) {
                    timer.stop();
                    m_dacEventloop->quit();
                }
                lineSensor.pop_front();
            }
        });

        timer.start();
        m_dacEventloop->exec();
        timer.stop();
    }

    emit ReleaseBlock();
}

void Program::SetTimeStart()
{
    m_startTime = QDateTime::currentMSecsSinceEpoch();
}

void Program::results_strokeTest(const quint64 forwardTime, const quint64 backwardTime)
{
    QString forwardText = QTime(0, 0).addMSecs(forwardTime).toString("mm:ss.zzz");
    QString backwardText = QTime(0, 0).addMSecs(backwardTime).toString("mm:ss.zzz");
    emit SetText(TextObjects::Label_forward, forwardText);
    emit SetText(TextObjects::LineEdit_forward, forwardText);
    emit SetText(TextObjects::Label_backward, backwardText);
    emit SetText(TextObjects::LineEdit_backward, backwardText);
}

void Program::AddRegression(const QVector<QPointF> &points)
{
    QVector<Point> chartPoints;
    for (QPointF point : points) {
        chartPoints.push_back({1, point.x(), point.y()});
    }
    emit AddPoints(Charts::Pressure, chartPoints);

    //emit SetVisible(Charts::Main_pressure, 1, true);
    emit SetRegressionEnable(true);
}

void Program::AddFriction(const QVector<QPointF> &points)
{
    QVector<Point> chartPoints;

    ValveInfo *valveInfo = m_registry->GetValveInfo();

    qreal k = 5 * M_PI * valveInfo->diameter * valveInfo->diameter / 4;

    for (QPointF point : points) {
        chartPoints.push_back({0, point.x(), point.y() * k});
    }
    emit AddPoints(Charts::Friction, chartPoints);
}

void Program::UpdateSensors()
{
    for (quint8 i = 0; i < m_mpi.SensorCount(); ++i) {
        switch (i) {
        case 0:
            emit SetText(TextObjects::LineEdit_linearSensor, m_mpi[i]->GetFormatedValue());
            emit SetText(TextObjects::LineEdit_linearSensorPercent, m_mpi[i]->GetPersentFormated());
            break;
        case 1:
            emit SetText(TextObjects::LineEdit_pressureSensor_1, m_mpi[i]->GetFormatedValue());
            break;
        case 2:
            emit SetText(TextObjects::LineEdit_pressureSensor_2, m_mpi[i]->GetFormatedValue());
            break;
        case 3:
            emit SetText(TextObjects::LineEdit_pressureSensor_3, m_mpi[i]->GetFormatedValue());
            break;
        }
    }
    if (m_testing)
        emit SetTask(m_mpi.GetDAC()->GetValue());

    QVector<Point> points;
    qreal percent = ((m_mpi.GetDAC()->GetValue() - 4) / 16) * 100;
    percent = qMin(qMax(percent, 0.0), 100.0);

    ValveInfo *valveInfo = m_registry->GetValveInfo();
    if (valveInfo->safePosition != 0) {
        percent = 100 - percent;
    }

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_initTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit AddPoints(Charts::Trend, points);
}

void Program::UpdateCharts_maintest()
{
    QVector<Point> points;
    qreal percent = ((m_mpi.GetDAC()->GetValue() - 4) / 16) * 100;
    percent = qMin(qMax(percent, 0.0), 100.0);

    ValveInfo *valveInfo = m_registry->GetValveInfo();
    if (valveInfo->safePosition != 0) {
        percent = 100 - percent;
    }

    qreal task = m_mpi[0]->GetValueFromPercent(percent);
    qreal X = m_mpi.GetDAC()->GetValue();
    points.push_back({0, X, task});

    for (quint8 i = 0; i < m_mpi.SensorCount(); ++i) {
        points.push_back({static_cast<quint8>(i + 1), X, m_mpi[i]->GetValue()});
    }

    emit AddPoints(Charts::Task, points);

    points.clear();
    points.push_back({0, m_mpi[1]->GetValue(), m_mpi[0]->GetValue()});

    emit AddPoints(Charts::Pressure, points);
}

void Program::UpdateCharts_stroketest()
{
    QVector<Point> points;

    qreal percent = ((m_mpi.GetDAC()->GetValue() - 4) / 16) * 100;
    percent = qMin(qMax(percent, 0.0), 100.0);

    ValveInfo *valveInfo = m_registry->GetValveInfo();
    if (valveInfo->safePosition != 0) {
        percent = 100 - percent;
    }

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_startTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit AddPoints(Charts::Stroke, points);
}

void Program::UpdateCharts_optiontest(Charts chart)
{
    QVector<Point> points;

    qreal percent = ((m_mpi.GetDAC()->GetValue() - 4) / 16) * 100;
    percent = qMin(qMax(percent, 0.0), 100.0);

    ValveInfo *valveInfo = m_registry->GetValveInfo();
    if (valveInfo->safePosition != 0) {
        percent = 100 - percent;
    }

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_startTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit AddPoints(chart, points);
}

void Program::results_mainTest(const MainTest::TestResults &results)
{
    ValveInfo *valveInfo = m_registry->GetValveInfo();

    qreal k = 5 * M_PI * valveInfo->diameter * valveInfo->diameter / 4;

    emit SetText(TextObjects::Label_pressureDifferenceValue,
                 QString::asprintf("%.3f bar", results.pressureDiff));
    emit SetText(TextObjects::Label_frictionForceValue,
                 QString::asprintf("%.3f H", results.pressureDiff * k));
    emit SetText(TextObjects::Label_frictionPercentValue,
                 QString::asprintf("%.2f %%", results.friction));
    emit SetText(TextObjects::Label_dynamicErrorMax,
                 QString::asprintf("%.3f mA", results.dinErrorMean));
    emit SetText(TextObjects::Label_dynamicErrorMax,
                 QString::asprintf("%.2f %%", results.dinErrorMean / 0.16));
    emit SetText(TextObjects::Label_dynamicErrorMean,
                 QString::asprintf("%.3f mA", results.dinErrorMax));
    emit SetText(TextObjects::Label_dynamicErrorMeanPercent,
                 QString::asprintf("%.2f %%", results.dinErrorMax / 0.16));

    emit SetText(TextObjects::Label_lowLimitValue, QString::asprintf("%.2f bar", results.lowLimit));
    emit SetText(TextObjects::Label_highLimitValue, QString::asprintf("%.2f bar", results.highLimit));

    emit SetText(TextObjects::LineEdit_dinamic_error,
                 QString::asprintf("%.2f", results.dinErrorMean / 0.16));
    emit SetText(TextObjects::LineEdit_range_pressure,
                 QString::asprintf("%.2f - %.2f", results.lowLimit, results.highLimit));
    emit SetText(TextObjects::LineEdit_range,
                 QString::asprintf("%.2f - %.2f", results.springLow, results.springHigh));

    emit SetText(TextObjects::LineEdit_friction,
                 QString::asprintf("%.3f", results.pressureDiff * k));
    emit SetText(TextObjects::LineEdit_friction_percent,
                 QString::asprintf("%.2f", results.friction));
}

void Program::results_stepTest(const QVector<StepTest::TestResult> &results, quint32 T_value)
{
    emit SetStepResults(results, T_value);
}

void Program::GetPoints_maintest(QVector<QVector<QPointF>> &points)
{
    emit GetPoints(points, Charts::Task);
}

void Program::GetPoints_steptest(QVector<QVector<QPointF>> &points)
{
    emit GetPoints(points, Charts::Step);
}

void Program::EndTest()
{
    m_testing = false;
    emit EnableSetTask(true);
    emit SetButtonInitEnabled(true);
    SetDAC_int(0);
    emit StopTest();
    emit SetTask(m_mpi.GetDAC()->GetValue());
}

void Program::SetDAC_real(qreal value)
{
    m_mpi.SetDAC_Real(value);
}

void Program::SetDAC_int(quint16 value)
{
    m_mpi.SetDAC_Raw(value);
}

void Program::initialize()
{
    m_timerSensors->stop();
    m_timerDI->stop();

    emit SetButtonInitEnabled(false);
    emit SetSensorNumber(0);

    emit SetGroupDOVisible(false);

    emit SetText(TextObjects::Label_deviceStatusValue, "");
    emit SetText(TextObjects::Label_deviceInitValue, "");
    emit SetText(TextObjects::Label_connectedSensorsNumber, "");
    emit SetText(TextObjects::Label_startingPositionValue, "");
    emit SetText(TextObjects::Label_finalPositionValue, "");

    if (!m_mpi.Connect()) {
        emit SetText(TextObjects::Label_deviceStatusValue, "Ошибка подключения");
        emit SetTextColor(TextObjects::Label_deviceStatusValue, Qt::red);
        emit SetButtonInitEnabled(true);
        return;
    }

    emit SetText(TextObjects::Label_deviceStatusValue, "Успешное подключение к порту " + m_mpi.PortName());
    emit SetTextColor(TextObjects::Label_deviceStatusValue, Qt::darkGreen);

    if (!m_mpi.Initialize()) {
        emit SetText(TextObjects::Label_deviceInitValue, "Ошибка инициализации");
        emit SetTextColor(TextObjects::Label_deviceStatusValue, Qt::red);
        emit SetButtonInitEnabled(true);
        return;
    }

    if ((m_mpi.Version() & 0x40) != 0) {
        emit SetGroupDOVisible(true);
        emit SetButtonsDOChecked(m_mpi.GetDOStatus());
        m_timerDI->start();
    }

    emit SetText(TextObjects::Label_deviceInitValue, "Успешная инициализация");
    emit SetTextColor(TextObjects::Label_deviceInitValue, Qt::darkGreen);

    switch (m_mpi.SensorCount()) {
    case 0:
        emit SetText(TextObjects::Label_connectedSensorsNumber, "Дачики не обнаружены");
        emit SetTextColor(TextObjects::Label_connectedSensorsNumber, Qt::red);
        // emit SetButtonInitEnabled(true);
        // return;
    case 1:
        emit SetText(TextObjects::Label_connectedSensorsNumber, "Обнаружен 1 датчик");
        emit SetTextColor(TextObjects::Label_connectedSensorsNumber, Qt::darkYellow);
        break;
    default:
        emit SetText(TextObjects::Label_connectedSensorsNumber,
                     "Обнаружено " + QString::number(m_mpi.SensorCount()) + " датчика");
        emit SetTextColor(TextObjects::Label_connectedSensorsNumber, Qt::darkGreen);
    }

    ValveInfo *valveInfo = m_registry->GetValveInfo();
    bool normalClosed = (valveInfo->safePosition == 0);

    emit SetText(TextObjects::Label_startingPositionValue, "Измерение");
    emit SetTextColor(TextObjects::Label_startingPositionValue, Qt::darkYellow);

    SetDac(0, 10000, true);

    QTimer timer(this);
    connect(&timer, &QTimer::timeout, this, [&] {
        if (!m_waitForButton || m_stopSetDac) {
            m_dacEventloop->quit();
        }
    });

    timer.start(50);
    m_dacEventloop->exec();
    timer.stop();

    if (normalClosed)
        m_mpi[0]->SetMin();
    else
        m_mpi[0]->SetMax();

    emit SetText(TextObjects::Label_startingPositionValue, m_mpi[0]->GetFormatedValue());
    emit SetTextColor(TextObjects::Label_startingPositionValue, Qt::darkGreen);

    emit SetText(TextObjects::Label_finalPositionValue, "Измерение");
    emit SetTextColor(TextObjects::Label_finalPositionValue, Qt::darkYellow);

    SetDac(0xFFFF, 10000, true);

    timer.start(50);
    m_dacEventloop->exec();
    timer.stop();

    if (normalClosed)
        m_mpi[0]->SetMax();
    else
        m_mpi[0]->SetMin();

    emit SetText(TextObjects::Label_finalPositionValue, m_mpi[0]->GetFormatedValue());
    emit SetTextColor(TextObjects::Label_finalPositionValue, Qt::darkGreen);

    qreal correctCoefficient = 1;
    if (valveInfo->strokeMovement != 0) {
        correctCoefficient = qRadiansToDegrees(2 / valveInfo->pulley);
        m_mpi[0]->SetUnit("°");
    }
    m_mpi[0]->CorrectCoefficients(correctCoefficient);

    if (normalClosed) {
        emit SetText(TextObjects::Label_valveStroke_range, m_mpi[0]->GetFormatedValue());
        emit SetText(TextObjects::LineEdit_stroke, QString::asprintf("%.2f", m_mpi[0]->GetValue()));
        SetDac(0);
    } else {
        SetDac(0, 10000, true);
        emit SetText(TextObjects::Label_valveStroke_range, m_mpi[0]->GetFormatedValue());
        emit SetText(TextObjects::LineEdit_stroke, QString::asprintf("%.2f", m_mpi[0]->GetValue()));
    }

    emit SetTask(m_mpi.GetDAC()->GetValue());

    emit ClearPoints(Charts::Trend);
    m_initTime = QDateTime::currentMSecsSinceEpoch();

    emit SetSensorNumber(m_mpi.SensorCount());
    emit SetButtonInitEnabled(true);
    m_timerSensors->start();
}

void Program::runningMainTest()
{
    MainTestSettings::TestParameters parameters;
    emit GetMainTestParameters(parameters);

    if (parameters.delay == 0) {
        emit StopTest();
        return;
    }

    const quint64 delay = static_cast<quint64>(parameters.delay);
    const quint64 response = static_cast<quint64>(parameters.response);
    const quint64 pn = static_cast<quint64>(
        parameters.pointNumbers * parameters.delay / parameters.response
        );
    const quint64 totalMs = 10000ULL
                            + delay
                            + (pn + 1) * response
                            + delay
                            + (pn + 1) * response;
    emit TotalTestTimeMs(totalMs);

    parameters.dac_min = qMax(m_mpi.GetDAC()->GetRawFromValue(parameters.signal_min),
                              m_mpi.GetDac_Min());
    parameters.dac_max = qMin(m_mpi.GetDAC()->GetRawFromValue(parameters.signal_max),
                              m_mpi.GetDac_Max());

    emit SetButtonInitEnabled(false);

    MainTest *mainTest = new MainTest;

    mainTest->SetParameters(parameters);

    QThread *threadTest = new QThread(this);
    mainTest->moveToThread(threadTest);

    connect(threadTest, &QThread::started,
            mainTest, &MainTest::Process);

    connect(mainTest, &MainTest::EndTest,
            threadTest, &QThread::quit);

    connect(this, &Program::StopTest,
            mainTest, &MainTest::Stop);

    connect(threadTest, &QThread::finished,
            threadTest, &QThread::deleteLater);

    connect(threadTest, &QThread::finished,
            mainTest, &MainTest::deleteLater);

    connect(mainTest, &MainTest::EndTest,
            this, &Program::EndTest);

    connect(mainTest, &MainTest::UpdateGraph,
            this, &Program::UpdateCharts_maintest);

    connect(mainTest, &MainTest::SetDAC,
            this, &Program::SetDac);

    connect(mainTest, &MainTest::DublSeries,
            this, [&] { emit DublSeries(); });

    connect(mainTest, &MainTest::GetPoints,
            this, &Program::GetPoints_maintest,
            Qt::BlockingQueuedConnection);

    connect(mainTest, &MainTest::AddRegression,
            this, &Program::AddRegression);

    connect(mainTest, &MainTest::AddFriction,
            this, &Program::AddFriction);

    connect(this, &Program::ReleaseBlock,
            mainTest, &MainTest::ReleaseBlock);

    connect(mainTest, &MainTest::Results,
            this, &Program::results_mainTest);

    connect(mainTest, &MainTest::ShowDots,
            this, [&](bool visible) { emit ShowDots(visible); });

    connect(mainTest, &MainTest::ClearGraph,
            this, [&] {
        emit ClearPoints(Charts::Task);
        emit ClearPoints(Charts::Pressure);
        emit ClearPoints(Charts::Friction);
        emit SetRegressionEnable(false);
    });

    m_testing = true;
    emit EnableSetTask(false);
    threadTest->start();
}

void Program::runningStrokeTest()
{
    emit SetButtonInitEnabled(false);
    emit ClearPoints(Charts::Stroke);

    emit TotalTestTimeMs(15000ULL);

    StrokeTest *strokeTest = new StrokeTest;
    QThread *threadTest = new QThread(this);
    strokeTest->moveToThread(threadTest);

    connect(threadTest, &QThread::started,
            strokeTest, &StrokeTest::Process);

    connect(strokeTest, &StrokeTest::EndTest,
            threadTest, &QThread::quit);

    connect(this, &Program::StopTest,
            strokeTest, &StrokeTest::Stop);

    connect(threadTest, &QThread::finished,
            threadTest, &QThread::deleteLater);

    connect(threadTest, &QThread::finished,
            strokeTest, &StrokeTest::deleteLater);

    connect(this, &Program::ReleaseBlock,
            strokeTest, &MainTest::ReleaseBlock);

    connect(strokeTest, &StrokeTest::EndTest,
            this, &Program::EndTest);

    connect(strokeTest, &StrokeTest::UpdateGraph,
            this, &Program::UpdateCharts_stroketest);

    connect(strokeTest, &StrokeTest::SetDAC, this,
            &Program::SetDac);
    connect(strokeTest, &StrokeTest::SetStartTime,
            this, &Program::SetTimeStart);

    connect(strokeTest, &StrokeTest::Results,
            this, &Program::results_strokeTest);

    m_testing = true;
    emit EnableSetTask(false);
    threadTest->start();
}

void Program::runningOptionalTest(const quint8 testNum)
{
    OptionTest::Task task;
    OptionTest *optionalTest;

    switch (testNum) {
    case 0: {
        optionalTest = new OptionTest;

        OtherTestSettings::TestParameters parameters;
        emit GetResponseTestParameters(parameters);

        if (parameters.points.empty()) {
            delete optionalTest;
            emit StopTest();
            return;
        }

        const quint64 P = static_cast<quint64>(parameters.points.size());
        const quint64 S = static_cast<quint64>(parameters.steps.size());
        const quint64 delay = static_cast<quint64>(parameters.delay);

        const quint64 N_values = 1 + 2 * P * (1 + S);
        const quint64 totalMs = 10000ULL + N_values * delay;
        emit TotalTestTimeMs(totalMs);

        task.delay = parameters.delay;

        ValveInfo *valveInfo = m_registry->GetValveInfo();

        bool normalOpen = (valveInfo->safePosition != 0);

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(4.0));

        for (auto it = parameters.points.begin(); it != parameters.points.end(); ++it) {
            for (quint8 i = 0; i < 2; i++) {
                qreal current = 16.0 * (normalOpen ? 100 - *it : *it) / 100 + 4.0;
                qreal dac_value = m_mpi.GetDAC()->GetRawFromValue(current);
                task.value.push_back(dac_value);

                for (auto it_s = parameters.steps.begin(); it_s < parameters.steps.end(); ++it_s) {
                    current += (16 * *it_s / 100) * (i == 0 ? 1 : -1) * (normalOpen ? -1 : 1);
                    dac_value = m_mpi.GetDAC()->GetRawFromValue(current);
                    task.value.push_back(dac_value);
                }
            }
        }

        optionalTest->SetTask(task);

        connect(optionalTest, &OptionTest::UpdateGraph,
                this, [&] {
            UpdateCharts_optiontest(Charts::Response);
        });

        emit ClearPoints(Charts::Response);

        break;
    }
    case 1: {
        optionalTest = new OptionTest;
        OtherTestSettings::TestParameters parameters;
        emit GetResolutionTestParameters(parameters);

        if (parameters.points.empty()) {
            delete optionalTest;
            emit StopTest();
            return;
        }

        const quint64 P = static_cast<quint64>(parameters.points.size());
        const quint64 S = static_cast<quint64>(parameters.steps.size());
        const quint64 delay = static_cast<quint64>(parameters.delay);

        const quint64 N_values = 1 + P * (2 * S);
        const quint64 totalMs = 10000ULL + N_values * delay;
        emit TotalTestTimeMs(totalMs);

        task.delay = parameters.delay;

        ValveInfo *valveInfo = m_registry->GetValveInfo();

        bool normalOpen = (valveInfo->safePosition != 0);

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(4.0));

        for (auto it = parameters.points.begin(); it != parameters.points.end(); ++it) {
            qreal current = 16.0 * (normalOpen ? 100 - *it : *it) / 100 + 4.0;
            qreal dac_value = m_mpi.GetDAC()->GetRawFromValue(current);

            for (auto it_s = parameters.steps.begin(); it_s < parameters.steps.end(); ++it_s) {
                task.value.push_back(dac_value);
                current = (16 * (normalOpen ? 100 - *it - *it_s : *it + *it_s) / 100 + 4.0);
                qreal dac_value_step = m_mpi.GetDAC()->GetRawFromValue(current);
                task.value.push_back(dac_value_step);
            }
        }

        optionalTest->SetTask(task);

        connect(optionalTest, &OptionTest::UpdateGraph,
                this, [&] {
            UpdateCharts_optiontest(Charts::Resolution);
        });

        emit ClearPoints(Charts::Resolution);

        break;
    }

    case 2: {
        optionalTest = new StepTest;
        StepTestSettings::TestParameters parameters;
        emit GetStepTestParameters(parameters);

        if (parameters.points.empty()) {
            delete optionalTest;
            emit StopTest();
            return;
        }

        const quint64 P = static_cast<quint64>(parameters.points.size());
        const quint64 delay = static_cast<quint64>(parameters.delay);
        // N_values = 1 старт + P прямой + 1 конец + P обратный + 1 возврат = 3 + 2*P
        const quint64 N_values = 3 + 2 * P;
        const quint64 totalMs = 10000ULL + N_values * delay;
        emit TotalTestTimeMs(totalMs);

        task.delay = parameters.delay;
        ValveInfo *valveInfo = m_registry->GetValveInfo();

        qreal start_value = 4.0;
        qreal end_value = 20.0;

        bool normalOpen = (valveInfo->safePosition != 0);

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(start_value));

        for (auto it = parameters.points.begin(); it != parameters.points.end(); ++it) {
            qreal current = 16.0 * (normalOpen ? 100 - *it : *it) / 100 + 4.0;
            qreal dac_value = m_mpi.GetDAC()->GetRawFromValue(current);
            task.value.push_back(dac_value);
        }

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(end_value));

        for (auto it = parameters.points.rbegin(); it != parameters.points.rend(); ++it) {
            qreal current = 16.0 * (normalOpen ? 100 - *it : *it) / 100 + 4.0;
            qreal dac_value = m_mpi.GetDAC()->GetRawFromValue(current);
            task.value.push_back(dac_value);
        }

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(start_value));

        optionalTest->SetTask(task);
        dynamic_cast<StepTest *>(optionalTest)->Set_T_value(parameters.test_value);

        connect(optionalTest, &OptionTest::UpdateGraph,
                this, [&] {
            UpdateCharts_optiontest(Charts::Step);
        });
        connect(dynamic_cast<StepTest *>(optionalTest), &StepTest::GetPoints,
                this, &Program::GetPoints_steptest,
                Qt::BlockingQueuedConnection);

        connect(dynamic_cast<StepTest *>(optionalTest), &StepTest::Results,
                this, &Program::results_stepTest);

        emit ClearPoints(Charts::Step);

        break;
    }
    default:
        emit StopTest();
        return;
    }

    QThread *threadTest = new QThread(this);
    optionalTest->moveToThread(threadTest);

    emit SetButtonInitEnabled(false);

    connect(threadTest, &QThread::started,
            optionalTest, &OptionTest::Process);

    connect(optionalTest, &OptionTest::EndTest,
            threadTest, &QThread::quit);

    connect(this, &Program::StopTest,
            optionalTest, &OptionTest::Stop);

    connect(threadTest, &QThread::finished,
            threadTest, &QThread::deleteLater);

    connect(threadTest, &QThread::finished,
            optionalTest, &OptionTest::deleteLater);

    connect(this, &Program::ReleaseBlock,
            optionalTest, &MainTest::ReleaseBlock);

    connect(optionalTest, &OptionTest::EndTest,
            this, &Program::EndTest);

    connect(optionalTest, &OptionTest::SetDAC,
            this, &Program::SetDac);

    connect(optionalTest, &OptionTest::SetStartTime,
            this, &Program::SetTimeStart);

    m_testing = true;
    emit EnableSetTask(false);
    threadTest->start();
}

void Program::TerminateTest()
{
    m_stopSetDac = true;
    m_dacEventloop->quit();
    emit StopTest();
}

void Program::button_set_position()
{
    m_stopSetDac = true;
    m_dacEventloop->quit();
}

void Program::button_DO(quint8 DO_num, bool state)
{
    m_mpi.SetDiscreteOutput(DO_num, state);
}

void Program::checkbox_autoinit(int state)
{
    m_waitForButton = (state == 0);
}
