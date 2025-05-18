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
    MyChart.cpp \
    NotationWindow.cpp \
    ObjectWindow.cpp \
    OtherTestSettings.cpp \
    Program.cpp \
    Registry.cpp \
    ReportSaver.cpp \
    Sensor.cpp \
    StepTestSettings.cpp \
    ValveWindow.cpp \
    main.cpp \
    src/Mpi/Mpi.cpp \
    src/Mpi/MpiSettings.cpp \
    src/Tests/CyclicTestPositioner.cpp \
    src/Tests/MainTest.cpp \
    src/Tests/OptionTest.cpp \
    src/Tests/StepTest.cpp \
    src/Tests/StrokeTest.cpp \
    src/Tests/Test.cpp \
    src/Uart/Uart.cpp \
    src/Uart/UartMessage.cpp \
    src/Uart/UartReader.cpp \
    src/ValidatorFactory/ValidatorFactory.cpp \
    src/ValveСonfig/ValveDataLoader.cpp \
    src/database/ValveDatabase.cpp

HEADERS += \
    MainTestSettings.h \
    MainWindow.h \
    MyChart.h \
    NotationWindow.h \
    ObjectWindow.h \
    OtherTestSettings.h \
    Program.h \
    Registry.h \
    ReportSaver.h \
    Sensor.h \
    StepTestSettings.h \
    ValveWindow.h \
    src/Mpi/Mpi.h \
    src/Mpi/MpiSettings.h \
    src/Tests/CyclicTestPositioner.h \
    src/Tests/MainTest.h \
    src/Tests/OptionTest.h \
    src/Tests/StepTest.h \
    src/Tests/StrokeTest.h \
    src/Tests/Test.h \
    src/Uart/Uart.h \
    src/Uart/UartMessage.h \
    src/Uart/UartReader.h \
    src/ValidatorFactory/ValidatorFactory.h \
    src/ValveСonfig/ValveDataLoader.h \
    src/database/ValveDatabase.h

FORMS += \
    MainTestSettings.ui \
    MainWindow.ui \
    NotationWindow.ui \
    ObjectWindow.ui \
    OtherTestSettings.ui \
    StepTestSettings.ui \
    ValveWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



RESOURCES += \
    Excel.qrc \
    Translations.qrc \
    db_valveData.qrc
