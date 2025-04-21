#include "MainWindow.h"
#include "NotationWindow.h"
#include "ObjectWindow.h"
#include "Registry.h"
#include "ValveWindow.h"

#include <QApplication>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;

    if (qtTranslator.load("qt_ru.qm", ":/translations"))
        a.installTranslator(&qtTranslator);

    //MainWindow w;
    //w.show();
    Registry registry;

    MainWindow main_window;

    ObjectWindow object_window; // Окно данных об объекте
    object_window.LoadFromReg(&registry);
    if (object_window.exec() == QDialog::Rejected)
        return 0;

    NotationWindow notationWindow;
    if (notationWindow.exec() == QDialog::Rejected) {
        return 0;
    }
    notationWindow.fillReport();

    ValveWindow valve_window;
    valve_window.SetRegistry(&registry);
    FileSaver::Report report = notationWindow.getReport();

    if (valve_window.exec() == QDialog::Rejected)
        return 0;

    main_window.setReport(report);
    main_window.SetRegistry(&registry);
    main_window.show();

    return a.exec();
}
