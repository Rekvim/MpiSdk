#include <QApplication>
#include <QTranslator>
#include <QSqlDatabase>

#include "MainWindow.h"
#include "NotationWindow.h"
#include "ObjectWindow.h"
#include "Registry.h"
#include "ValveWindow.h"
#include "./Src/Database/ValveDatabase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator qtTranslator;

    if (qtTranslator.load("qt_ru.qm", ":/translations"))
        a.installTranslator(&qtTranslator);

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");

    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(writablePath);
    QString dbPath = writablePath + "/Database.db";

    if (!QFile::exists(dbPath)) {
        if (!QFile::copy(":/Database/Database.db", dbPath)) {
            qCritical() << "Не удалось скопировать БД";
            return -1;
        }
        QFile::setPermissions(dbPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }

    database.setDatabaseName(dbPath);

    if (!database.open()) {
        qCritical() << "Не могу открыть базу данных:";
        return -1;
    }

    Registry registry;
    ValveDatabase valveDatabase(database);

    ObjectWindow objectWindow(registry);
    if (objectWindow.exec() != QDialog::Accepted)
        return 0;

    NotationWindow notationWindow(registry);
    if (notationWindow.exec() != QDialog::Accepted)
        return 0;

    ValveWindow valveWindow(registry, valveDatabase);
    if (valveWindow.exec() != QDialog::Accepted)
        return 0;

    MainWindow mainWindow;
    mainWindow.setRegistry(&registry);
    mainWindow.show();

    return a.exec();
}

