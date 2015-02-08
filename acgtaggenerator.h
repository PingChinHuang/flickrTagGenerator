#ifndef ACGTAGGENERATOR_H
#define ACGTAGGENERATOR_H

#include "databasedialog.h"

#include <QWidget>
#include <QStringList>
#include <QDomDocument>
#include <QtSql/QtSql>
#include <QMutex>

#define ACGID_INVALID (-1)

namespace Ui {
class ACGTagGenerator;
}

struct ForeignKeyDefinition {
    QString refDB;
    QString refField;
};

struct DBTableDefinition {
    QStringList fieldNames;
    QStringList fieldTypes;
    QList<bool> fieldUnique;
    QList<bool> fieldForeign;
    QList<bool> fieldNotNULL;
    QList<ForeignKeyDefinition> fieldForeignDefinition;
    unsigned    fieldPrimary;
};

class CDBControl {
private:
    QSqlDatabase    m_db;
    QMutex          m_refLock;
    int             m_ref;

    explicit CDBControl();

protected:
    QString         m_dbPath;
    QString         m_dbType;

    bool IsOpen();
    bool GetTableList(QStringList &tableList);
    void IncRef();
    void DecRef();

public:
    explicit CDBControl(const QString& dbPath, const QString& dbType);
    ~CDBControl();

    bool OpenDB();
    void CloseDB();
    bool CreateTable(const QString& name, struct DBTableDefinition& definition);
    bool QueryDB(const QString& query);
    bool QueryDB(const QString& query, QSqlQuery &requester);
};

/*class CWorkDB: public CDBControl {
public:
    explicit CWorkDB(const QString& dbFilePath);
    virtual ~CWorkDB();

    bool CreateWorkTable();
    bool QueryWork(const QString& name, QStringList& aliasList);
    bool AddWork(QStringList& aliasList);
};*/

class CACGDB : public CDBControl {
private:
    QString         m_acgTableName;
    QString         m_charTableName;
    QString         m_activityTableName;

    bool            m_hasACGTable;
    bool            m_hasCharTable;
    bool            m_hasActivityTable;
    bool            m_isWorkable;
    QMutex          m_isWorkableLock;

private:
    explicit CACGDB();

    bool CreateACGTable();
    bool CreateCharTable();
    bool CreateActivityTable();

public:
    explicit CACGDB(const QString& dbPath, const QString &dbType);
    virtual ~CACGDB();

    bool IsWorkable();

    bool IsCharacterExist(const QString& name, QString& queryResult);
    bool QueryCharacter(const QString& name,
                        QStringList& charAliasList, QStringList& workAliasList,
                        QStringList &charFieldList, QStringList &workFieldList);
    bool AddCharacter(int workId, QStringList& aliasList);
    bool RemoveCharacter(const QString& name);
    bool ModifyCharacter(int acgID, QStringList& aliasList);
    bool QueryAllCharacterList(QStringList &charList);

    bool IsACGExist(const QString &name, int &acgID);
    bool QueryACG(const QString& name, QStringList &aliasList, QStringList &fieldList);
    bool AddACG(QStringList& aliasList);
    bool RemoveACG(const QString& name);
    bool ModifyACG(QStringList& aliasList);
    bool QueryAllACGList(QStringList &acgList);

    bool IsActivityExist(const QString &name, int &id);
    bool QueryActivity(const QString &name, QStringList &aliasList, QStringList &fieldList);
    bool AddActivity(QStringList &aliasList);
    bool RemoveActivity(QStringList &aliasList);
    bool ModifyActivity(QStringList &aliasList);
    bool QueryAllActivityList(QStringList &list);

    bool QueryCharactersByACG(const QString &name, QStringList &charList);
};

class ACGTagGenerator : public QWidget
{
    Q_OBJECT

public:
    explicit ACGTagGenerator(QWidget *parent = 0);
    ~ACGTagGenerator();

private:
    Ui::ACGTagGenerator *ui;
    QStringList m_commonTagList;
    QDomDocument m_dom;
    QString m_TagXmlFileName;
    CACGDB *m_ACGDB;
    DatabaseDialog m_dbDialog;
    bool m_useDB;

public:
    void InitializeByXML();
    void InitializeByDB();
    bool UpdateWorkCharComboBox(bool bWork = true);
    bool UpdateWorkCharComboBoxByDB(bool bWork = true);
    bool UpdateActivityComboBoxByDB();
    bool findTargetNode(QDomDocument &dom, const QString &tag, const QString &targetTag, QDomNode &targetNode);
    void traverseParent(QDomNode &targetNode, QString &output);
    bool createNewCharNode(QDomNode &newChar);
    bool addWorkNameNode(QDomNode &targetNode);

private:
    void InitializeWorksTagsTreeByXML();
    void InitializeWorksTagsTreeByDB();
    void InitializeActivityTreeByXML();
    void InitializeActivityTreeByDB();
    void TransferXMLtoDatabase();
    void clearCurrentDom();

    void GetACGCharOutputByXML(QString &output);
    void GetACGCharOutputByDB(QString &output, QString &charOutput);
    void GetActivityOutputByXML(QString &output);
    void GetActivityOutputByDB(QString &output);

private slots:
    void on_pushButton_clicked();
    void on_itemSelectionChanged();
    void on_pushButtonClear_clicked();
    void on_spinBox_editingFinished();
    void on_comboBoxChar_currentTextChanged(const QString &arg1);
    void on_pushButtonApply_clicked();
    void on_comboBoxWork_currentTextChanged(const QString &arg1);
    void on_pushButtonOpen_clicked();
    void on_pushButton_2_clicked();
    void on_pushButtonXMLtoDB_clicked();
    void on_actionTriggered();
    void on_pushButtonReset_clicked();
    void on_lineEditOtherTags_returnPressed();
    void on_comboBoxActivity_currentTextChanged(const QString &arg1);
};

#endif // ACGTAGGENERATOR_H
