#include <QApplication>
#include <QTranslator>

#include "MainWindow.h"
#include "NotationWindow.h"
#include "ObjectWindow.h"
#include "Registry.h"
#include "ValveWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator qtTranslator;

    if (qtTranslator.load("qt_ru.qm", ":/translations"))
        a.installTranslator(&qtTranslator);

    Registry registry;
    ValveDatabase database;

    ObjectWindow objectWindow;
    objectWindow.LoadFromReg(&registry);
    if (objectWindow.exec() != QDialog::Accepted)
        return 0;

    NotationWindow notationWindow(registry);
    if (notationWindow.exec() != QDialog::Accepted)
        return 0;

    ValveWindow valveWindow(database);
    valveWindow.setRegistry(&registry);
    if (valveWindow.exec() != QDialog::Accepted)
        return 0;

    MainWindow mainWindow;
    mainWindow.setRegistry(&registry);
    mainWindow.show();

    return a.exec();
}

