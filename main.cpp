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
    Registry registry;

    ObjectWindow objectWindow;
    objectWindow.LoadFromReg(&registry);
    if (objectWindow.exec() != QDialog::Accepted)
        return 0;

    NotationWindow notationWindow;
    notationWindow.SetRegistry(&registry);
    if (notationWindow.exec() != QDialog::Accepted)
        return 0;

    ValveWindow valveWindow;
    valveWindow.SetRegistry(&registry);
    if (valveWindow.exec() != QDialog::Accepted)
        return 0;

    ReportSaver::Report report;

    notationWindow.fillReport(report);
    valveWindow.fillReport(report);

    ReportSaver saver;
    saver.SetRegistry(&registry);
    if (!saver.SaveReport(report)) {
        QMessageBox::warning(nullptr, "Ошибка", "Не удалось сохранить отчет");
    }

    MainWindow mainWindow;
    mainWindow.SetRegistry(&registry);
    mainWindow.setReport(report);
    mainWindow.show();

    return a.exec();
}

