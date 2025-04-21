QT       += core gui
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
    FileSaver.cpp \
    MainTestSettings.cpp \
    MainWindow.cpp \
    Mpi.cpp \
    MpiSettings.cpp \
    MyChart.cpp \
    NotationWindow.cpp \
    ObjectWindow.cpp \
    OtherTestSettings.cpp \
    Program.cpp \
    Registry.cpp \
    Sensor.cpp \
    StepTestSettings.cpp \
    Test.cpp \
    Uart.cpp \
    UartReader.cpp \
    ValveWindow.cpp \
    main.cpp \
    src/ValveСonfig/ValveDataLoader.cpp

HEADERS += \
    FileSaver.h \
    MainTestSettings.h \
    MainWindow.h \
    Mpi.h \
    MpiSettings.h \
    MyChart.h \
    NotationWindow.h \
    ObjectWindow.h \
    OtherTestSettings.h \
    Program.h \
    Registry.h \
    Sensor.h \
    StepTestSettings.h \
    Test.h \
    Uart.h \
    UartReader.h \
    ValveWindow.h \
    src/ValveСonfig/ValveDataLoader.h

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
