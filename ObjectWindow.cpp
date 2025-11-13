#include "ObjectWindow.h"

#include "ui_ObjectWindow.h"

ObjectWindow::ObjectWindow(Registry &registry, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ObjectWindow)
    , m_registry(registry)
{
    ui->setupUi(this);

    loadFromRegistry();
    syncUIFromObjectInfo();

    QRect scr = QApplication::primaryScreen()->geometry();
    move(scr.center() - rect().center());

    ui->dateEdit->setDate(QDate::currentDate());

    QRegularExpression regular;
    regular.setPattern("[^/?*:;{}\\\\]+");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regular, this);

    ui->lineEdit_object->setValidator(validator);
    ui->lineEdit_manufactory->setValidator(validator);
    ui->lineEdit_department->setValidator(validator);
}

void ObjectWindow::loadFromRegistry()
{
    m_objectInfo = m_registry.getObjectInfo();
}

void ObjectWindow::syncUIFromObjectInfo()
{
    ui->lineEdit_object->setText(m_objectInfo->object);
    ui->lineEdit_manufactory->setText(m_objectInfo->manufactory);
    ui->lineEdit_department->setText(m_objectInfo->department);
    ui->lineEdit_FIO->setText(m_objectInfo->FIO);
}

void ObjectWindow::SaveObjectInfo()
{
    m_objectInfo->object = ui->lineEdit_object->text();
    m_objectInfo->manufactory = ui->lineEdit_manufactory->text();
    m_objectInfo->department = ui->lineEdit_department->text();
    m_objectInfo->FIO = ui->lineEdit_FIO->text();

    m_registry.saveObjectInfo();
}

void ObjectWindow::on_pushButton_clicked()
{
    if (ui->lineEdit_object->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите наименование объекта");
        return;
    }

    if (ui->lineEdit_manufactory->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите наименование цеха");
        return;
    }

    if (ui->lineEdit_department->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите наименование отделение/установки");
        return;
    }

    if (!m_registry.checkObject(ui->lineEdit_object->text())) {
        QMessageBox::StandardButton button
            = QMessageBox::question(this,
                                    "Предупреждение",
                                    "Объекта нет в базе. Вы действительно хотите продождить?");

        if (button == QMessageBox::StandardButton::Yes) {
            SaveObjectInfo();

            accept();
        }

        return;
    }

    m_objectInfo->object = ui->lineEdit_object->text();

    if (!m_registry.checkManufactory(ui->lineEdit_manufactory->text())) {
        QMessageBox::StandardButton button
            = QMessageBox::question(this,
                                    "Предупреждение",
                                    "Цеха нет в базе. Вы действительно хотите продождить?");

        if (button == QMessageBox::StandardButton::Yes) {
            SaveObjectInfo();

            accept();
        }

        return;
    }

    m_objectInfo->manufactory = ui->lineEdit_manufactory->text();

    if (!m_registry.checkDepartment(ui->lineEdit_department->text())) {
        QMessageBox::StandardButton button = QMessageBox::question(
            this,
            "Предупреждение",
            "Отделения/установки нет в базе. Вы действительно хотите продождить?");

        if (button == QMessageBox::StandardButton::Yes) {
            SaveObjectInfo();

            accept();
        }

        return;
    }

    OtherParameters *otherParameters = m_registry.getOtherParameters();
    otherParameters->date = ui->dateEdit->date().toString("dd.MM.yyyy");

    SaveObjectInfo();
    accept();
}

