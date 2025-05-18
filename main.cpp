#include <QApplication>
#include <QTranslator>

#include "MainWindow.h"
#include "NotationWindow.h"
#include "ObjectWindow.h"
#include "Registry.h"
#include "ValveWindow.h"

// #include "./src/Repository/ObjectInfoRepository.h"
// #include "./src/Repository/ValveRepository.h"
// #include "./src/Repository/MaterialsRepository.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Registry registry;
    ValveDatabase database;

    ObjectWindow objectWindow;
    objectWindow.LoadFromReg(&registry);
    if (objectWindow.exec() != QDialog::Accepted)
        return 0;

    NotationWindow notationWindow;
    notationWindow.SetRegistry(&registry);
    if (notationWindow.exec() != QDialog::Accepted)
        return 0;

    ValveWindow valveWindow(database);
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

