#include "Program.h"

#include "./Src/Tests/StepTest.h"
#include "./Src/Tests/StrokeTest.h"
#include "./Src/Tests/MainTest.h"
#include "./Src/Runners/MainTestRunner.h"
#include "./Src/Runners/StepTestRunner.h"
#include "./Src/Runners/StrokeTestRunner.h"
#include "./Src/Runners/OptionResponseRunner.h"
#include "./Src/Runners/OptionResolutionRunner.h"

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
        emit setTask(m_mpi.GetDac()->GetValue());

    QVector<Point> points;
    qreal percent = calcPercent(m_mpi.GetDac()->GetValue(),
                                m_registry->getValveInfo()->safePosition != 0);

    quint64 time = QDateTime::currentMSecsSinceEpoch() - m_initTime;

    points.push_back({0, qreal(time), percent});
    points.push_back({1, qreal(time), m_mpi[0]->GetPersent()});

    emit addPoints(Charts::Trend, points);
}

void Program::endTest() {
    m_testing = false;
    emit enableSetTask(true);
    emit setButtonInitEnabled(true);

    emit setTask(m_mpi.GetDac()->GetValue());

    m_activeRunner.reset();
    emit testFinished();
}

void Program::disposeActiveRunnerAsync() {
    m_activeRunner.reset();
}

void Program::setDac_real(qreal value)
{
    m_mpi.SetDAC_Real(value);
}

void Program::setDac_int(quint16 value)
{
    m_mpi.SetDAC_Raw(value);
}

void Program::initialization()
{
    auto &ts = m_telemetryStore;

    ts.init.initStatusText = "";
    ts.init.connectedSensorsText = "";
    ts.init.deviceStatusText = "";
    ts.init.startingPositionText = "";
    ts.init.finalPositionText = "";

    m_timerSensors->stop();
    m_timerDI->stop();
    emit setButtonInitEnabled(false);
    emit setSensorNumber(0);

    if (!connectAndInitDevice()) {
        emit setButtonInitEnabled(true);
        return;
    }

    if (!detectAndReportSensors()) {
        emit setButtonInitEnabled(true);
        return;
    }

    ValveInfo *valveInfo = m_registry->getValveInfo();
    bool normalClosed = (valveInfo->safePosition == 0);

    measureStartPosition(normalClosed);
    measureEndPosition(normalClosed);

    calculateAndApplyCoefficients();

    recordStrokeRange(normalClosed);

    finalizeInitialization();
}

// Подключение и инициализация устройства
bool Program::connectAndInitDevice()
{
    auto &ts = m_telemetryStore;
    bool ok = m_mpi.Connect();
    ts.init.deviceStatusText  = ok
                                   ? QString("Успешное подключение к порту %1").arg(m_mpi.PortName())
                                   : "Ошибка подключения";
    ts.init.deviceStatusColor = ok ? Qt::darkGreen : Qt::red;
    emit telemetryUpdated(ts);

    if (!ok) {
        return false;
    }

    ok = m_mpi.Initialize();
    ts.init.initStatusText  = ok ? "Успешная инициализация" : "Ошибка инициализации";
    ts.init.initStatusColor = ok ? Qt::darkGreen : Qt::red;
    emit telemetryUpdated(ts);

    m_isInitialized = ok;
    return ok;
}

// Обнаружение и отчёт по датчикам
bool Program::detectAndReportSensors()
{
    auto &ts = m_telemetryStore;
    int cnt = m_mpi.SensorCount();

    if (cnt == 0) {
        ts.init.connectedSensorsText = "Датчики не обнаружены";
        ts.init.connectedSensorsColor = Qt::red;
    }
    else if (cnt == 1) {
        ts.init.connectedSensorsText  = "Обнаружен 1 датчик";
        ts.init.connectedSensorsColor = Qt::darkYellow;
    }
    else {
        ts.init.connectedSensorsText = QString("Обнаружено %1 датчика").arg(cnt);
        ts.init.connectedSensorsColor = Qt::darkGreen;
    }

    emit telemetryUpdated(ts);

    return cnt > 0;
}

void Program::waitForDacCycle()
{
    QTimer timer(this);
    connect(&timer, &QTimer::timeout, this, [&] {
        if (!m_waitForButton || m_stopSetDac)
            m_dacEventloop->quit();
    });
    timer.start(50);
    m_dacEventloop->exec();
    timer.stop();
}

void Program::measureStartPosition(bool normalClosed)
{
    auto &ts = m_telemetryStore;
    ts.init.startingPositionText = "Измерение";
    ts.init.startingPositionColor = Qt::darkYellow;
    emit telemetryUpdated(ts);

    setDac(0, 10000, true);
    waitForDacCycle();

    if (normalClosed) m_mpi[0]->SetMin();
    else m_mpi[0]->SetMax();

    ts.init.startingPositionText = m_mpi[0]->GetFormatedValue();
    ts.init.startingPositionColor = Qt::darkGreen;
    emit telemetryUpdated(ts);
}

void Program::measureEndPosition(bool normalClosed)
{
    auto &ts = m_telemetryStore;
    ts.init.finalPositionText = "Измерение";
    ts.init.finalPositionColor = Qt::darkYellow;
    emit telemetryUpdated(ts);

    setDac(0xFFFF, 10000, true);
    waitForDacCycle();

    if (normalClosed) m_mpi[0]->SetMax();
    else m_mpi[0]->SetMin();

    ts.init.finalPositionText = m_mpi[0]->GetFormatedValue();
    ts.init.finalPositionColor = Qt::darkGreen;
    emit telemetryUpdated(ts);
}

void Program::calculateAndApplyCoefficients()
{
    ValveInfo *valveInfo = m_registry->getValveInfo();
    qreal coeff = 1.0;

    if (valveInfo->strokeMovement != 0) {
        coeff = qRadiansToDegrees(2.0 / valveInfo->diameter);
        m_mpi[0]->SetUnit("°");
    }

    m_mpi[0]->CorrectCoefficients(coeff);
}

void Program::recordStrokeRange(bool normalClosed)
{
    auto &s = m_telemetryStore;

    if (normalClosed) {
        s.valveStrokeRecord.range = m_mpi[0]->GetFormatedValue();
        s.valveStrokeRecord.real = m_mpi[0]->GetValue();
        setDac(0);
    } else {
        setDac(0, 10000, true);
        s.valveStrokeRecord.range = m_mpi[0]->GetFormatedValue();
        s.valveStrokeRecord.real = m_mpi[0]->GetValue();
    }

    emit telemetryUpdated(s);
    emit setTask(m_mpi.GetDac()->GetValue());
}

void Program::finalizeInitialization()
{
    emit clearPoints(Charts::Trend);
    m_initTime = QDateTime::currentMSecsSinceEpoch();

    emit setSensorNumber(m_mpi.SensorCount());
    emit setButtonInitEnabled(true);
    m_timerSensors->start();
}

bool Program::isInitialized() const {
    return m_isInitialized;
}

void Program::receivedPoints_mainTest(QVector<QVector<QPointF>> &points)
{
    emit getPoints_mainTest(points, Charts::Task);
}

void Program::runningMainTest()
{
    auto runner = std::make_unique<MainTestRunner>(m_mpi, *m_registry, this);

    connect(runner.get(), &ITestRunner::requestSetDac,
            this, &Program::setDac);

    connect(this, &Program::releaseBlock,
            runner.get(), &ITestRunner::releaseBlock);

    connect(runner.get(), &MainTestRunner::getParameters_mainTest,
            this, &Program::forwardGetParameters_mainTest);

    connect(runner.get(), &ITestRunner::totalTestTimeMs,
            this, &Program::totalTestTimeMs);

    connect(runner.get(), &ITestRunner::endTest,
            this, &Program::endTest);

    connect(this, &Program::stopTheTest, runner.get(), &ITestRunner::stop);

    emit setButtonInitEnabled(false);
    m_testing = true;
    emit enableSetTask(false);

    m_activeRunner = std::move(runner);
    m_activeRunner->start();
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
    qreal percent = calcPercent(m_mpi.GetDac()->GetValue(),
                                m_registry->getValveInfo()->safePosition != 0);

    qreal task = m_mpi[0]->GetValueFromPercent(percent);
    qreal X = m_mpi.GetDac()->GetValue();
    points.push_back({0, X, task});

    for (quint8 i = 0; i < m_mpi.SensorCount(); ++i) {
        points.push_back({static_cast<quint8>(i + 1), X, m_mpi[i]->GetValue()});
    }

    emit addPoints(Charts::Task, points);

    points.clear();
    points.push_back({0, m_mpi[1]->GetValue(), m_mpi[0]->GetValue()});

    emit addPoints(Charts::Pressure, points);
}

void Program::results_mainTest(const MainTest::TestResults &results)
{
    ValveInfo *valveInfo = m_registry->getValveInfo();

    qreal k = 5 * M_PI * valveInfo->diameter * valveInfo->diameter / 4;

    auto &s = m_telemetryStore.mainTestRecord;

    s.pressureDifference = results.pressureDiff;

    s.frictionForce = results.pressureDiff * k;
    s.frictionPercent = results.friction;

    s.dynamicError_mean = results.dinErrorMean;
    s.dynamicError_meanPercent = results.dinErrorMean / 0.16;
    s.dynamicError_max = results.dinErrorMax;
    s.dynamicError_maxPercent = results.dinErrorMax / 0.16;
    s.dynamicErrorReal = results.dinErrorMean / 0.16;

    s.lowLimitPressure = results.lowLimit;
    s.highLimitPressure = results.highLimit;

    s.springLow = results.springLow;
    s.springHigh = results.springHigh;

    emit telemetryUpdated(m_telemetryStore);
}

void Program::runningStrokeTest() {
    auto r = std::make_unique<StrokeTestRunner>(m_mpi, *m_registry, this);
    startRunner(std::move(r));
}

void Program::updateCharts_strokeTest()
{
    QVector<Point> points;

    qreal percent = calcPercent(m_mpi.GetDac()->GetValue(),
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

    m_telemetryStore.strokeTestRecord.timeForwardMs = forwardText;
    m_telemetryStore.strokeTestRecord.timeBackwardMs = backwardText;

    emit telemetryUpdated(m_telemetryStore);
}

void Program::runningOptionalTest(quint8 testNum)
{
    switch (testNum) {
    case 0: {
        auto r = std::make_unique<OptionResponseRunner>(m_mpi, *m_registry, this);
        connect(r.get(), &OptionResponseRunner::getParameters_responseTest,
                this, [&](OtherTestSettings::TestParameters& p){
                    emit getParameters_responseTest(p);
                });
        startRunner(std::move(r));
        break;
    }
    case 1: {
        auto r = std::make_unique<OptionResolutionRunner>(m_mpi, *m_registry, this);
        connect(r.get(), &OptionResolutionRunner::getParameters_resolutionTest,
                this, [&](OtherTestSettings::TestParameters& p){
                    emit getParameters_resolutionTest(p);
                });
        startRunner(std::move(r));
        break;
    }
    case 2: {
        auto r = std::make_unique<StepTestRunner>(m_mpi, *m_registry, this);
        startRunner(std::move(r));
        break;
    }
    default:
        emit stopTheTest();
        break;
    }
}

void Program::results_stepTest(const QVector<StepTest::TestResult> &results, quint32 T_value)
{
    m_telemetryStore.stepResults.clear();
    for (auto &r : results) {
        StepTestRecord rec;
        rec.from = r.from;
        rec.to = r.to;
        rec.T_value = r.T_value;
        rec.overshoot = r.overshoot;
        m_telemetryStore.stepResults.push_back(rec);
    }

    emit telemetryUpdated(m_telemetryStore);

    emit setStepResults(results, T_value);
}

void Program::receivedPoints_stepTest(QVector<QVector<QPointF>> &points)
{
    emit getPoints_stepTest(points, Charts::Step);
}

void Program::updateCharts_optionTest(Charts chart)
{
    QVector<Point> points;

    qreal percent = calcPercent(m_mpi.GetDac()->GetValue(),
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
