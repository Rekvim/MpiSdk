QT += core gui sql
QT += serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QXLSX_PARENTPATH=./
QXLSX_HEADERPATH=./header/
QXLSX_SOURCEPATH=./source/
include(./QXlsx.pri)

SOURCES += \
    MainTestSettings.cpp \
    MainWindow.cpp \
    NotationWindow.cpp \
    ObjectWindow.cpp \
    OtherTestSettings.cpp \
    Program.cpp \
    Registry.cpp \
    Sensor.cpp \
    Src/Report/ReportBuilder.cpp \
    Src/Report/ReportSaver.cpp \
    Src/Runners/BaseRunner.cpp \
    Src/Runners/MainTestRunner.cpp \
    Src/Runners/OptionResolutionRunner.cpp \
    Src/Runners/OptionResponseRunner.cpp \
    Src/Runners/RunnerFactory.cpp \
    Src/Runners/StepTestRunner.cpp \
    Src/Runners/StrokeTestRunner.cpp \
    StepTestSettings.cpp \
    ValveWindow.cpp \
    main.cpp \
    Src/CustomChart/MyChart.cpp \
    Src/CustomChart/MySeries.cpp \
    Src/Mpi/Mpi.cpp \
    Src/Mpi/MpiSettings.cpp \
    Src/Telemetry/TelemetryStore.cpp \
    Src/Tests/CyclicTestPositioner.cpp \
    Src/Tests/MainTest.cpp \
    Src/Tests/OptionTest.cpp \
    Src/Tests/StepTest.cpp \
    Src/Tests/StrokeTest.cpp \
    Src/Tests/Test.cpp \
    Src/Uart/Uart.cpp \
    Src/Uart/UartMessage.cpp \
    Src/Uart/UartReader.cpp \
    Src/ValidatorFactory/ValidatorFactory.cpp \
    Src/Database/ValveDatabase.cpp

HEADERS += \
    MainTestSettings.h \
    MainWindow.h \
    NotationWindow.h \
    ObjectWindow.h \
    OtherTestSettings.h \
    Program.h \
    Registry.h \
    Sensor.h \
    Src/Report/ReportBuilder.h \
    Src/Report/ReportSaver.h \
    Src/Runners/BaseRunner.h \
    Src/Runners/ITestRunner.h \
    Src/Runners/MainTestRunner.h \
    Src/Runners/OptionResolutionRunner.h \
    Src/Runners/OptionResponseRunner.h \
    Src/Runners/RunnerFactory.h \
    Src/Runners/StepTestRunner.h \
    Src/Runners/StrokeTestRunner.h \
    StepTestSettings.h \
    ValveWindow.h \
    Src/CustomChart/MyChart.h \
    Src/CustomChart/MySeries.h \
    Src/Mpi/Mpi.h \
    Src/Mpi/MpiSettings.h \
    Src/Telemetry/TelemetryStore.h \
    Src/Tests/CyclicTestPositioner.h \
    Src/Tests/MainTest.h \
    Src/Tests/OptionTest.h \
    Src/Tests/StepTest.h \
    Src/Tests/StrokeTest.h \
    Src/Tests/Test.h \
    Src/Uart/Uart.h \
    Src/Uart/UartMessage.h \
    Src/Uart/UartReader.h \
    Src/ValidatorFactory/RegexPatterns.h \
    Src/ValidatorFactory/ValidatorFactory.h \
    Src/Database/ValveDatabase.h

FORMS += \
    MainTestSettings.ui \
    MainWindow.ui \
    NotationWindow.ui \
    ObjectWindow.ui \
    OtherTestSettings.ui \
    StepTestSettings.ui \
    ValveWindow.ui

INCLUDEPATH += ./Src/CustomChart

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



RESOURCES += \
    Database.qrc \
    Excel.qrc \
    Translations.qrc \
    img.qrc
