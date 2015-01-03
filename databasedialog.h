#ifndef DATABASEDIALOG_H
#define DATABASEDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class DatabaseDialog;
}

class DatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseDialog(QWidget *parent = 0);
    ~DatabaseDialog();

    void ClearAcgTable();
    void ClearCharacterTable();
    void ClearActivityTable();
    void SetACGTable(QStringList &fieldList, QStringList &aliasList);
    void SetCharacterTable(QStringList &fieldList, QStringList &aliasList);
    void SetActivityTable(QStringList &fieldList, QStringList &aliasList);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::DatabaseDialog *ui;
};

#endif // DATABASEDIALOG_H
