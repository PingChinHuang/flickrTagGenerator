#include "databasedialog.h"
#include "ui_databasedialog.h"

DatabaseDialog::DatabaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseDialog)
{
    ui->setupUi(this);
    setWindowTitle("ACG Database Overview");
}

DatabaseDialog::~DatabaseDialog()
{
    delete ui;
}

void DatabaseDialog::ClearAcgTable()
{
    ui->tableWidget_acg->clear();
    ui->tableWidget_acg->setRowCount(0);
}

void DatabaseDialog::ClearCharacterTable()
{
    ui->tableWidget_characters->clear();
    ui->tableWidget_characters->setRowCount(0);
}

void DatabaseDialog::ClearActivityTable()
{
    ui->tableWidget_activity->clear();
    ui->tableWidget_activity->setRowCount(0);
}

void DatabaseDialog::SetACGTable(QStringList &fieldList, QStringList &aliasList)
{
    ui->tableWidget_acg->setHorizontalHeaderLabels(fieldList);
    ui->tableWidget_acg->insertRow(ui->tableWidget_acg->rowCount());
    for (int i = 0; i < aliasList.count(); i++) {
        ui->tableWidget_acg->setItem(ui->tableWidget_acg->rowCount() - 1, i, new QTableWidgetItem(aliasList.at(i)));
    }
}

void DatabaseDialog::SetCharacterTable(QStringList &fieldList, QStringList &aliasList)
{
    ui->tableWidget_characters->setHorizontalHeaderLabels(fieldList);
    ui->tableWidget_characters->insertRow(ui->tableWidget_characters->rowCount());
    for (int i = 0; i < aliasList.count(); i++) {
        ui->tableWidget_characters->setItem(ui->tableWidget_characters->rowCount() - 1, i, new QTableWidgetItem(aliasList.at(i)));
    }
}

void DatabaseDialog::SetActivityTable(QStringList &fieldList, QStringList &aliasList)
{
    ui->tableWidget_activity->setHorizontalHeaderLabels(fieldList);
    ui->tableWidget_activity->insertRow(ui->tableWidget_activity->rowCount());
    for (int i = 0; i < aliasList.count(); i++) {
        ui->tableWidget_activity->setItem(ui->tableWidget_activity->rowCount() - 1, i, new QTableWidgetItem(aliasList.at(i)));
    }
}

void DatabaseDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    close();
}
