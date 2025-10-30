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
            this, &Program::updateSensors);

    m_testing = false;
    m_dacEventloop = new QEventLoop(this);

    m_timerDI = new QTimer(this);
    m_timerDI->setInterval(1000);
    connect(m_timerDI, &QTimer::timeout, this, [&]() {
        quint8 DI = m_mpi.GetDIStatus();
        emit setCheckboxDIChecked(DI);
    });
}

void Program::setRegistry(Registry *registry)
{
    m_registry = registry;
}

void Program::setDac(quint16 dac, quint32 sleepMs, bool waitForStop, bool waitForStart)
{
    m_stopSetDac = false;

    if (m_mpi.SensorCount() == 0) {
        emit releaseBlock();
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
        emit releaseBlock();
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
        emit releaseBlock();
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

    emit releaseBlock();
}

void Program::setTimeStart()
{
    m_startTime = QDateTime::currentMSecsSinceEpoch();
}

void Program::updateSensors()
{
    for (quint8 i = 0; i < m_mpi.SensorCount(); ++i) {
        switch (i) {
        case 0:
            emit setText(TextObjects::LineEdit_linearSensor, m_mpi[i]->GetFormatedValue());
            emit setText(TextObjects::LineEdit_linearSensorPercent, m_mpi[i]->GetPersentFormated());
            break;
        case 1:
            emit setText(TextObjects::LineEdit_pressureSensor_1, m_mpi[i]->GetFormatedValue());
            break;
        case 2:
            emit setText(TextObjects::LineEdit_pressureSensor_2, m_mpi[i]->GetFormatedValue());
            break;
        case 3:
            emit setText(TextObjects::LineEdit_pressureSensor_3, m_mpi[i]->GetFormatedValue());
            break;
        }
    }
    if (m_testing)
        emit setTask(m_mpi.GetDAC()->GetValue());

    QVector<Point> points;
    qreal percent = calcPercent(m_mpi.GetDAC()->GetValue(),
                                m_registry->getValveInfo()->safePosition != 0);

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_initTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit addPoints(Charts::Trend, points);
}

void Program::endTest()
{
    m_testing = false;
    emit enableSetTask(true);
    emit setButtonInitEnabled(true);
    setDac_int(0);
    emit stopTest();
    emit setTask(m_mpi.GetDAC()->GetValue());
}

void Program::setDac_real(qreal value)
{
    m_mpi.SetDAC_Real(value);
}

void Program::setDac_int(quint16 value)
{
    m_mpi.SetDAC_Raw(value);
}

void Program::initialize()
{
    m_timerSensors->stop();
    m_timerDI->stop();

    emit setButtonInitEnabled(false);
    emit setSensorNumber(0);

    emit setGroupDOVisible(false);

    emit setText(TextObjects::Label_deviceStatusValue, "");
    emit setText(TextObjects::Label_deviceInitValue, "");
    emit setText(TextObjects::Label_connectedSensorsNumber, "");
    emit setText(TextObjects::Label_startingPositionValue, "");
    emit setText(TextObjects::Label_finalPositionValue, "");

    if (!m_mpi.Connect()) {
        emit setText(TextObjects::Label_deviceStatusValue, "Ошибка подключения");
        emit setTextColor(TextObjects::Label_deviceStatusValue, Qt::red);
        emit setButtonInitEnabled(true);
        return;
    }

    emit setText(TextObjects::Label_deviceStatusValue, "Успешное подключение к порту " + m_mpi.PortName());
    emit setTextColor(TextObjects::Label_deviceStatusValue, Qt::darkGreen);

    if (!m_mpi.Initialize()) {
        emit setText(TextObjects::Label_deviceInitValue, "Ошибка инициализации");
        emit setTextColor(TextObjects::Label_deviceStatusValue, Qt::red);
        emit setButtonInitEnabled(true);
        return;
    }

    if ((m_mpi.Version() & 0x40) != 0) {
        emit setGroupDOVisible(true);
        emit setButtonsDOChecked(m_mpi.GetDOStatus());
        m_timerDI->start();
    }

    emit setText(TextObjects::Label_deviceInitValue, "Успешная инициализация");
    emit setTextColor(TextObjects::Label_deviceInitValue, Qt::darkGreen);

    switch (m_mpi.SensorCount()) {
    case 0:
        emit setText(TextObjects::Label_connectedSensorsNumber, "Дачики не обнаружены");
        emit setTextColor(TextObjects::Label_connectedSensorsNumber, Qt::red);
        // emit setButtonInitEnabled(true);
        // return;
    case 1:
        emit setText(TextObjects::Label_connectedSensorsNumber, "Обнаружен 1 датчик");
        emit setTextColor(TextObjects::Label_connectedSensorsNumber, Qt::darkYellow);
        break;
    default:
        emit setText(TextObjects::Label_connectedSensorsNumber,
                     "Обнаружено " + QString::number(m_mpi.SensorCount()) + " датчика");
        emit setTextColor(TextObjects::Label_connectedSensorsNumber, Qt::darkGreen);
    }

    ValveInfo *valveInfo = m_registry->getValveInfo();
    bool normalClosed = (valveInfo->safePosition == 0);

    emit setText(TextObjects::Label_startingPositionValue, "Измерение");
    emit setTextColor(TextObjects::Label_startingPositionValue, Qt::darkYellow);

    setDac(0, 10000, true);

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

    emit setText(TextObjects::Label_startingPositionValue, m_mpi[0]->GetFormatedValue());
    emit setTextColor(TextObjects::Label_startingPositionValue, Qt::darkGreen);

    emit setText(TextObjects::Label_finalPositionValue, "Измерение");
    emit setTextColor(TextObjects::Label_finalPositionValue, Qt::darkYellow);

    setDac(0xFFFF, 10000, true);

    timer.start(50);
    m_dacEventloop->exec();
    timer.stop();

    if (normalClosed)
        m_mpi[0]->SetMax();
    else
        m_mpi[0]->SetMin();

    emit setText(TextObjects::Label_finalPositionValue, m_mpi[0]->GetFormatedValue());
    emit setTextColor(TextObjects::Label_finalPositionValue, Qt::darkGreen);

    qreal correctCoefficient = 1;
    if (valveInfo->strokeMovement != 0) {
        correctCoefficient = qRadiansToDegrees(2 / valveInfo->pulley);
        m_mpi[0]->SetUnit("°");
    }
    m_mpi[0]->CorrectCoefficients(correctCoefficient);

    if (normalClosed) {
        emit setText(TextObjects::Label_valveStroke_range, m_mpi[0]->GetFormatedValue());
        emit setText(TextObjects::LineEdit_stroke, QString::asprintf("%.2f", m_mpi[0]->GetValue()));
        setDac(0);
    } else {
        setDac(0, 10000, true);
        emit setText(TextObjects::Label_valveStroke_range, m_mpi[0]->GetFormatedValue());
        emit setText(TextObjects::LineEdit_stroke, QString::asprintf("%.2f", m_mpi[0]->GetValue()));
    }

    emit setTask(m_mpi.GetDAC()->GetValue());

    emit clearPoints(Charts::Trend);
    m_initTime = QDateTime::currentMSecsSinceEpoch();

    emit setSensorNumber(m_mpi.SensorCount());
    emit setButtonInitEnabled(true);
    m_timerSensors->start();
}

void Program::runningMainTest()
{
    MainTestSettings::TestParameters parameters;
    emit getParameters_mainTest(parameters);

    if (parameters.delay == 0) {
        emit stopTest();
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
                            + (pn + 1) * response
                            + 10000ULL;
    emit totalTestTimeMs(totalMs);

    parameters.dac_min = qMax(m_mpi.GetDAC()->GetRawFromValue(parameters.signal_min),
                              m_mpi.GetDac_Min());
    parameters.dac_max = qMin(m_mpi.GetDAC()->GetRawFromValue(parameters.signal_max),
                              m_mpi.GetDac_Max());

    emit setButtonInitEnabled(false);

    MainTest *mainTest = new MainTest;

    mainTest->SetParameters(parameters);

    QThread *threadTest = new QThread(this);
    mainTest->moveToThread(threadTest);

    connect(threadTest, &QThread::started,
            mainTest, &MainTest::Process);

    connect(mainTest, &MainTest::EndTest,
            threadTest, &QThread::quit);

    connect(this, &Program::stopTest,
            mainTest, &MainTest::Stop);

    connect(threadTest, &QThread::finished,
            threadTest, &QThread::deleteLater);

    connect(threadTest, &QThread::finished,
            mainTest, &MainTest::deleteLater);

    connect(mainTest, &MainTest::EndTest,
            this, &Program::endTest);

    connect(mainTest, &MainTest::EndTest,
            this, &Program::mainTestFinished);

    connect(mainTest, &MainTest::UpdateGraph,
            this, &Program::updateCharts_mainTest);

    connect(mainTest, &MainTest::SetDAC,
            this, &Program::setDac);

    connect(mainTest, &MainTest::DublSeries,
            this, [&] { emit dublSeries(); });

    connect(mainTest, &MainTest::GetPoints,
            this, &Program::getPoints_maintest,
            Qt::BlockingQueuedConnection);

    connect(mainTest, &MainTest::AddRegression,
            this, &Program::addRegression);

    connect(mainTest, &MainTest::AddFriction,
            this, &Program::addFriction);

    connect(this, &Program::releaseBlock,
            mainTest, &MainTest::ReleaseBlock);

    connect(mainTest, &MainTest::Results,
            this, &Program::results_mainTest);

    connect(mainTest, &MainTest::ShowDots,
            this, [&](bool visible) { emit showDots(visible); });

    connect(mainTest, &MainTest::ClearGraph,
            this, [&] {
        emit clearPoints(Charts::Task);
        emit clearPoints(Charts::Pressure);
        emit clearPoints(Charts::Friction);
        emit setRegressionEnable(false);
    });

    m_testing = true;
    emit enableSetTask(false);
    threadTest->start();
}

void Program::addFriction(const QVector<QPointF> &points)
{
    QVector<Point> chartPoints;

    ValveInfo *valveInfo = m_registry->getValveInfo();

    qreal k = 5 * M_PI * valveInfo->diameter * valveInfo->diameter / 4;

    for (QPointF point : points) {
        chartPoints.push_back({0, point.x(), point.y() * k});
    }
    emit addPoints(Charts::Friction, chartPoints);
}

void Program::addRegression(const QVector<QPointF> &points)
{
    QVector<Point> chartPoints;
    for (QPointF point : points) {
        chartPoints.push_back({1, point.x(), point.y()});
    }
    emit addPoints(Charts::Pressure, chartPoints);

    //emit SetVisible(Charts::Main_pressure, 1, true);
    emit setRegressionEnable(true);
}

void Program::updateCharts_mainTest()
{
    QVector<Point> points;
    qreal percent = calcPercent(m_mpi.GetDAC()->GetValue(),
                                m_registry->getValveInfo()->safePosition != 0);

    qreal task = m_mpi[0]->GetValueFromPercent(percent);
    qreal X = m_mpi.GetDAC()->GetValue();
    points.push_back({0, X, task});

    for (quint8 i = 0; i < m_mpi.SensorCount(); ++i) {
        points.push_back({static_cast<quint8>(i + 1), X, m_mpi[i]->GetValue()});
    }

    emit addPoints(Charts::Task, points);

    points.clear();
    points.push_back({0, m_mpi[1]->GetValue(), m_mpi[0]->GetValue()});

    emit addPoints(Charts::Pressure, points);
}

void Program::getPoints_maintest(QVector<QVector<QPointF>> &points)
{
    emit getPoints(points, Charts::Task);
}

void Program::results_mainTest(const MainTest::TestResults &results)
{
    ValveInfo *valveInfo = m_registry->getValveInfo();

    qreal k = 5 * M_PI * valveInfo->diameter * valveInfo->diameter / 4;

    emit setText(TextObjects::Label_pressureDifferenceValue,
                 QString::asprintf("%.3f bar", results.pressureDiff));
    emit setText(TextObjects::Label_frictionForceValue,
                 QString::asprintf("%.3f H", results.pressureDiff * k));
    emit setText(TextObjects::Label_frictionPercentValue,
                 QString::asprintf("%.2f %%", results.friction));
    emit setText(TextObjects::Label_dynamicErrorMax,
                 QString::asprintf("%.3f mA", results.dinErrorMean));
    emit setText(TextObjects::Label_dynamicErrorMax,
                 QString::asprintf("%.2f %%", results.dinErrorMean / 0.16));
    emit setText(TextObjects::Label_dynamicErrorMean,
                 QString::asprintf("%.3f mA", results.dinErrorMax));
    emit setText(TextObjects::Label_dynamicErrorMeanPercent,
                 QString::asprintf("%.2f %%", results.dinErrorMax / 0.16));

    emit setText(TextObjects::Label_lowLimitValue, QString::asprintf("%.2f bar", results.lowLimit));
    emit setText(TextObjects::Label_highLimitValue, QString::asprintf("%.2f bar", results.highLimit));

    emit setText(TextObjects::LineEdit_dinamic_error,
                 QString::asprintf("%.2f", results.dinErrorMean / 0.16));
    emit setText(TextObjects::LineEdit_range_pressure,
                 QString::asprintf("%.2f - %.2f", results.lowLimit, results.highLimit));
    emit setText(TextObjects::LineEdit_range,
                 QString::asprintf("%.2f - %.2f", results.springLow, results.springHigh));

    emit setText(TextObjects::LineEdit_friction,
                 QString::asprintf("%.3f", results.pressureDiff * k));
    emit setText(TextObjects::LineEdit_friction_percent,
                 QString::asprintf("%.2f", results.friction));
}

void Program::runningStrokeTest()
{
    emit setButtonInitEnabled(false);
    emit clearPoints(Charts::Stroke);

    StrokeTest *strokeTest = new StrokeTest;
    QThread *threadTest = new QThread(this);
    strokeTest->moveToThread(threadTest);

    connect(threadTest, &QThread::started,
            strokeTest, &StrokeTest::Process);

    connect(strokeTest, &StrokeTest::EndTest,
            threadTest, &QThread::quit);

    connect(this, &Program::stopTest,
            strokeTest, &StrokeTest::Stop);

    connect(threadTest, &QThread::finished,
            threadTest, &QThread::deleteLater);

    connect(threadTest, &QThread::finished,
            strokeTest, &StrokeTest::deleteLater);

    connect(this, &Program::releaseBlock,
            strokeTest, &MainTest::ReleaseBlock);

    connect(strokeTest, &StrokeTest::EndTest,
            this, &Program::endTest);

    connect(strokeTest, &StrokeTest::UpdateGraph,
            this, &Program::updateCharts_strokeTest);

    connect(strokeTest, &StrokeTest::SetDAC,
            this, &Program::setDac);

    connect(strokeTest, &StrokeTest::SetStartTime,
            this, &Program::setTimeStart);

    connect(strokeTest, &StrokeTest::Results,
            this, &Program::results_strokeTest);

    m_testing = true;
    emit enableSetTask(false);
    threadTest->start();
}

void Program::updateCharts_strokeTest()
{
    QVector<Point> points;

    qreal percent = calcPercent(m_mpi.GetDAC()->GetValue(),
                                m_registry->getValveInfo()->safePosition != 0);

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_startTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit addPoints(Charts::Stroke, points);
}

void Program::results_strokeTest(const quint64 forwardTime, const quint64 backwardTime)
{
    QString forwardText = QTime(0, 0).addMSecs(forwardTime).toString("mm:ss.zzz");
    QString backwardText = QTime(0, 0).addMSecs(backwardTime).toString("mm:ss.zzz");
    emit setText(TextObjects::Label_forward, forwardText);
    emit setText(TextObjects::LineEdit_forward, forwardText);
    emit setText(TextObjects::Label_backward, backwardText);
    emit setText(TextObjects::LineEdit_backward, backwardText);
}

void Program::runningOptionalTest(const quint8 testNum)
{
    OptionTest::Task task;
    OptionTest *optionalTest;

    switch (testNum) {
    case 0: {
        optionalTest = new OptionTest;

        OtherTestSettings::TestParameters parameters;
        emit getParameters_responseTest(parameters);

        if (parameters.points.empty()) {
            delete optionalTest;
            emit stopTest();
            return;
        }

        const quint64 P = static_cast<quint64>(parameters.points.size());
        const quint64 S = static_cast<quint64>(parameters.steps.size());
        const quint64 delay = static_cast<quint64>(parameters.delay);

        const quint64 N_values = 1 + 2 * P * (1 + S);
        const quint64 totalMs = 10000ULL + N_values * delay + 10000ULL;
        emit totalTestTimeMs(totalMs);

        task.delay = parameters.delay;

        ValveInfo *valveInfo = m_registry->getValveInfo();

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

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(4.0));

        optionalTest->SetTask(task);

        connect(optionalTest, &OptionTest::UpdateGraph,
                this, [&] {
            updateCharts_optionTest(Charts::Response);
        });

        emit clearPoints(Charts::Response);

        break;
    }
    case 1: {
        optionalTest = new OptionTest;
        OtherTestSettings::TestParameters parameters;
        emit getParameters_resolutionTest(parameters);

        if (parameters.points.empty()) {
            delete optionalTest;
            emit stopTest();
            return;
        }

        const quint64 P = static_cast<quint64>(parameters.points.size());
        const quint64 S = static_cast<quint64>(parameters.steps.size());
        const quint64 delay = static_cast<quint64>(parameters.delay);

        const quint64 N_values = 1 + P * (2 * S);
        const quint64 totalMs = 10000ULL + N_values * delay + 10000ULL;
        emit totalTestTimeMs(totalMs);

        task.delay = parameters.delay;

        ValveInfo *valveInfo = m_registry->getValveInfo();

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

        task.value.push_back(m_mpi.GetDAC()->GetRawFromValue(4.0));

        optionalTest->SetTask(task);

        connect(optionalTest, &OptionTest::UpdateGraph,
                this, [&] {
            updateCharts_optionTest(Charts::Resolution);
        });

        emit clearPoints(Charts::Resolution);

        break;
    }

    case 2: {
        optionalTest = new StepTest;
        StepTestSettings::TestParameters parameters;
        emit getParameters_stepTest(parameters);

        if (parameters.points.empty()) {
            delete optionalTest;
            emit stopTest();
            return;
        }

        const quint64 P = static_cast<quint64>(parameters.points.size());
        const quint64 delay = static_cast<quint64>(parameters.delay);
        // N_values = 1 старт + P прямой + 1 конец + P обратный + 1 возврат = 3 + 2*P
        const quint64 N_values = 3 + 2 * P;
        const quint64 totalMs = 10000ULL + N_values * delay;
        emit totalTestTimeMs(totalMs);

        task.delay = parameters.delay;
        ValveInfo *valveInfo = m_registry->getValveInfo();

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
            updateCharts_optionTest(Charts::Step);
        });
        connect(dynamic_cast<StepTest *>(optionalTest), &StepTest::GetPoints,
                this, &Program::getPoints_steptest,
                Qt::BlockingQueuedConnection);

        connect(dynamic_cast<StepTest *>(optionalTest), &StepTest::Results,
                this, &Program::results_stepTest);

        emit clearPoints(Charts::Step);

        break;
    }
    default:
        emit stopTest();
        return;
    }

    QThread *threadTest = new QThread(this);
    optionalTest->moveToThread(threadTest);

    emit setButtonInitEnabled(false);

    connect(threadTest, &QThread::started,
            optionalTest, &OptionTest::Process);

    connect(optionalTest, &OptionTest::EndTest,
            threadTest, &QThread::quit);

    connect(this, &Program::stopTest,
            optionalTest, &OptionTest::Stop);

    connect(threadTest, &QThread::finished,
            threadTest, &QThread::deleteLater);

    connect(threadTest, &QThread::finished,
            optionalTest, &OptionTest::deleteLater);

    connect(this, &Program::releaseBlock,
            optionalTest, &MainTest::ReleaseBlock);

    connect(optionalTest, &OptionTest::EndTest,
            this, &Program::endTest);

    connect(optionalTest, &OptionTest::SetDAC,
            this, &Program::setDac);

    connect(optionalTest, &OptionTest::SetStartTime,
            this, &Program::setTimeStart);

    m_testing = true;
    emit enableSetTask(false);
    threadTest->start();
}

void Program::results_stepTest(const QVector<StepTest::TestResult> &results, quint32 T_value)
{
    emit setStepResults(results, T_value);
}

void Program::getPoints_steptest(QVector<QVector<QPointF>> &points)
{
    emit getPoints(points, Charts::Step);
}

void Program::updateCharts_optionTest(Charts chart)
{
    QVector<Point> points;

    qreal percent = calcPercent(m_mpi.GetDAC()->GetValue(),
                                m_registry->getValveInfo()->safePosition != 0);

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_startTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit addPoints(chart, points);
}

void Program::terminateTest()
{
    m_stopSetDac = true;
    m_dacEventloop->quit();
    emit stopTest();
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
