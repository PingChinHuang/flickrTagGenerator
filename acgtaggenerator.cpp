#include "acgtaggenerator.h"
#include "ui_acgtaggenerator.h"

#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMutexLocker>
#include <QMessageBox>

ACGTagGenerator::ACGTagGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ACGTagGenerator),
    m_ACGDB(new CACGDB("./acg_db.sqlite", "QSQLITE"))
{
    ui->setupUi(this);
    ui->pushButton->hide();
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    //Initialize();
    setAutoFillBackground(true);
    QPalette q = this->palette();
    q.setColor(QPalette::Background, QColor(Qt::white));
    setPalette(q);

    connect(ui->treeWidgetActivity, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));
    connect(ui->treeWidgetLocation, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));
    connect(ui->treeWidgetWork, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));
}

ACGTagGenerator::~ACGTagGenerator()
{
    clearCurrentDom();
    delete m_ACGDB;
    delete ui;
}

void ACGTagGenerator::clearCurrentDom()
{
    m_dom.clear();
    if (QFile::exists(m_TagXmlFileName) && QFile::exists(m_TagXmlFileName + ".tmp"))
    {
        QFile::remove(m_TagXmlFileName);
        QFile::rename(m_TagXmlFileName + ".tmp", m_TagXmlFileName);
    }
}

void ACGTagGenerator::InitializeByXML()
{
    if (m_TagXmlFileName.isEmpty())
        return;

    QFile *file = new QFile(m_TagXmlFileName);
    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_dom.setContent(file);
    }
    file->close();

    ui->treeWidgetCommon->clear();
    QDomNodeList commonTags = m_dom.elementsByTagName("CommonTags");
    for (int i = 0; i < commonTags.count(); i++) {
        QDomNodeList commonTagsItems = commonTags.item(i).toElement().childNodes();
        for (int j = 0; j < commonTagsItems.count(); j++) {
            QDomElement element = commonTagsItems.item(j).toElement();
            m_commonTagList.append(element.text());
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(element.text()));
            ui->treeWidgetCommon->addTopLevelItem(item);
        }
    }

    ui->treeWidgetLocation->clear();
    QDomNodeList locationTags = m_dom.elementsByTagName("LocationTags");
    for (int i = 0; i < locationTags.count(); i++) {
        QDomNodeList countries = locationTags.item(i).toElement().elementsByTagName("country");
        QTreeWidgetItem *item = NULL;
        for (int j = 0; j < countries.count(); j++) {
            QDomNodeList countryNames = countries.item(j).toElement().elementsByTagName("name");
            if (countryNames.count() < 1)
                continue;

            item = new QTreeWidgetItem(QStringList(countryNames.item(0).toElement().text()));
            ui->treeWidgetLocation->addTopLevelItem(item);

            QDomNodeList cities = countries.item(j).toElement().elementsByTagName("city");
            for (int k = 0; k < cities.count(); k++) {
                QDomNodeList cityNames = cities.item(k).toElement().elementsByTagName("name");
                if (cityNames.count() < 1)
                    continue;

                QTreeWidgetItem *itemLV2 = new QTreeWidgetItem(QStringList(cityNames.item(0).toElement().text()));
                item->addChild(itemLV2);

                QDomNodeList places = cities.item(k).toElement().elementsByTagName("place");
                for (int l = 0; l < places.count(); l++) {
                    QDomNodeList placeNames = places.item(l).toElement().elementsByTagName("name");
                    if (placeNames.count() < 1)
                        continue;

                    itemLV2->addChild(new QTreeWidgetItem(QStringList(placeNames.item(0).toElement().text())));
                }
            }
        }
    }

    ui->treeWidgetActivity->clear();
    QDomNodeList activitiesTags = m_dom.elementsByTagName("ActivitiesTags");
    for (int i = 0; i < activitiesTags.count(); i++) {
        QDomNodeList activities = activitiesTags.item(i).toElement().elementsByTagName("activity");
        QTreeWidgetItem *item = NULL;
        for (int j = 0; j < activities.count(); j++) {
            QDomNodeList activityNames = activities.item(j).toElement().elementsByTagName("name");
            if (activityNames.count() < 1)
                continue;

            item = new QTreeWidgetItem(QStringList(activityNames.item(0).toElement().text()));
            ui->treeWidgetActivity->addTopLevelItem(item);
        }
    }

    InitializeWorksTagsTreeByXML();
}

void ACGTagGenerator::InitializeByDB()
{
    if (m_TagXmlFileName.isEmpty())
        return;

    QFile *file = new QFile(m_TagXmlFileName);
    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_dom.setContent(file);
    }
    file->close();

    ui->treeWidgetCommon->clear();
    QDomNodeList commonTags = m_dom.elementsByTagName("CommonTags");
    for (int i = 0; i < commonTags.count(); i++) {
        QDomNodeList commonTagsItems = commonTags.item(i).toElement().childNodes();
        for (int j = 0; j < commonTagsItems.count(); j++) {
            QDomElement element = commonTagsItems.item(j).toElement();
            m_commonTagList.append(element.text());
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(element.text()));
            ui->treeWidgetCommon->addTopLevelItem(item);
        }
    }

    ui->treeWidgetLocation->clear();
    QDomNodeList locationTags = m_dom.elementsByTagName("LocationTags");
    for (int i = 0; i < locationTags.count(); i++) {
        QDomNodeList countries = locationTags.item(i).toElement().elementsByTagName("country");
        QTreeWidgetItem *item = NULL;
        for (int j = 0; j < countries.count(); j++) {
            QDomNodeList countryNames = countries.item(j).toElement().elementsByTagName("name");
            if (countryNames.count() < 1)
                continue;

            item = new QTreeWidgetItem(QStringList(countryNames.item(0).toElement().text()));
            ui->treeWidgetLocation->addTopLevelItem(item);

            QDomNodeList cities = countries.item(j).toElement().elementsByTagName("city");
            for (int k = 0; k < cities.count(); k++) {
                QDomNodeList cityNames = cities.item(k).toElement().elementsByTagName("name");
                if (cityNames.count() < 1)
                    continue;

                QTreeWidgetItem *itemLV2 = new QTreeWidgetItem(QStringList(cityNames.item(0).toElement().text()));
                item->addChild(itemLV2);

                QDomNodeList places = cities.item(k).toElement().elementsByTagName("place");
                for (int l = 0; l < places.count(); l++) {
                    QDomNodeList placeNames = places.item(l).toElement().elementsByTagName("name");
                    if (placeNames.count() < 1)
                        continue;

                    itemLV2->addChild(new QTreeWidgetItem(QStringList(placeNames.item(0).toElement().text())));
                }
            }
        }
    }

    ui->treeWidgetActivity->clear();
    QDomNodeList activitiesTags = m_dom.elementsByTagName("ActivitiesTags");
    for (int i = 0; i < activitiesTags.count(); i++) {
        QDomNodeList activities = activitiesTags.item(i).toElement().elementsByTagName("activity");
        QTreeWidgetItem *item = NULL;
        for (int j = 0; j < activities.count(); j++) {
            QDomNodeList activityNames = activities.item(j).toElement().elementsByTagName("name");
            if (activityNames.count() < 1)
                continue;

            item = new QTreeWidgetItem(QStringList(activityNames.item(0).toElement().text()));
            ui->treeWidgetActivity->addTopLevelItem(item);
        }
    }

    InitializeWorksTagsTreeByDB();
}

void ACGTagGenerator::InitializeWorksTagsTreeByXML()
{
    ui->treeWidgetWork->clear();
    ui->comboBoxWork->clear();

    QDomNodeList worksTags = m_dom.elementsByTagName("WorksTags");
    for (int i = 0; i < worksTags.count(); i++) {
        QDomNodeList works = worksTags.item(i).toElement().elementsByTagName("work");
        QTreeWidgetItem *item = NULL;
        for (int j = 0; j < works.count(); j++) {
            QDomNodeList workChildren = works.item(j).childNodes();
            if (workChildren.count() < 1)
                continue;

            bool bWorkNameFound = false;
            for (int k = 0; k < workChildren.count(); k++) {
                if (workChildren.item(k).toElement().tagName() == "name") {
                    if (!bWorkNameFound && !workChildren.item(k).toElement().text().isEmpty()) {
                        ui->comboBoxWork->addItem(workChildren.item(k).toElement().text());
                        item = new QTreeWidgetItem(QStringList(workChildren.item(k).toElement().text()));
                        ui->treeWidgetWork->addTopLevelItem(item);
                        bWorkNameFound = true;
                        break;
                    }
                }
            }

            if (!bWorkNameFound)
                continue;

            QDomNodeList characters = works.item(j).toElement().elementsByTagName("char");
            for (int k = 0; k < characters.count(); k++) {
                QDomNodeList charNames = characters.item(k).toElement().elementsByTagName("name");
                if (charNames.count() < 1)
                    continue;

                QTreeWidgetItem *itemLV2 = new QTreeWidgetItem(QStringList(charNames.item(0).toElement().text()));
                item->addChild(itemLV2);
            }
        }
    }
}

void ACGTagGenerator::InitializeWorksTagsTreeByDB()
{
    ui->treeWidgetWork->clear();
    ui->comboBoxWork->clear();

    QStringList acgList;
    if (!m_ACGDB->QueryAllACGList(acgList))
        return;

    for (int i = 0; i < acgList.count(); i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(acgList.at(i)));
        ui->treeWidgetWork->addTopLevelItem(item);
        ui->comboBoxWork->addItem(acgList.at(i));

        QStringList charList;
        if (m_ACGDB->QueryCharactersByACG(acgList.at(i), charList)) {
            for (int j = 0; j < charList.count(); j++) {
                QTreeWidgetItem *itemLV2 = new QTreeWidgetItem(QStringList(charList.at(j)));
                item->addChild(itemLV2);
            }
        }
    }
}

void ACGTagGenerator::TransferXMLtoDatabase()
{
    QDomNodeList worksTags = m_dom.elementsByTagName("WorksTags");
    for (int i = 0; i < worksTags.count(); i++) {
        QDomNodeList works = worksTags.item(i).toElement().elementsByTagName("work");
        for (int j = 0; j < works.count(); j++) {
            QDomNodeList workChildren = works.item(j).childNodes();
            if (workChildren.isEmpty())
                continue;

            QStringList workAliasList;
            bool bACGExist = false;
            int acgID = ACGID_INVALID;
            for (int k = 0; k < workChildren.count(); k++) {
                if (workChildren.item(k).toElement().tagName() == "name") {
                    if (!workChildren.item(k).toElement().text().isEmpty()) {
                        if (m_ACGDB->IsACGExist(workChildren.item(k).toElement().text(), acgID)) {
                            if (acgID > ACGID_INVALID) {
                                bACGExist = true;
                                break;
                            }
                        }

                        workAliasList.push_back(workChildren.item(k).toElement().text());
                    }
                }
            }

            if (!bACGExist) {
                if (m_ACGDB->AddACG(workAliasList)) {
                    acgID = -1;
                    if (m_ACGDB->IsACGExist(workAliasList.at(0), acgID)) {
                        if (acgID == ACGID_INVALID) {
                            QMessageBox msgBox(QMessageBox::Warning,
                                               "Error",
                                               QString("%1 cannot add into the database.\n It's characters will not be handled.").arg(workAliasList.at(0)),
                                               QMessageBox::Ok,
                                               NULL);
                            msgBox.exec();
                            continue;
                        }
                    }
                } else {
                    QMessageBox msgBox(QMessageBox::Warning,
                                       "Error",
                                       QString("%1 cannot add into the database.\n It's characters will not be handled.").arg(workAliasList.at(0)),
                                       QMessageBox::Ok,
                                       NULL);
                    msgBox.exec();
                    continue;
                }
            }

            QDomNodeList characters = works.item(j).toElement().elementsByTagName("char");
            for (int k = 0; k < characters.count(); k++) {
                QDomNodeList charNames = characters.item(k).toElement().elementsByTagName("name");
                if (charNames.isEmpty())
                    continue;

                QStringList charAliasList;
                QString queryResult;
                bool bCharExist = false;
                for (int l = 0; l < charNames.count(); l++) {
                    if (!charNames.item(l).toElement().text().isEmpty()) {
                        if (m_ACGDB->IsCharacterExist(charNames.item(l).toElement().text(), queryResult)) {
                            if (!queryResult.isEmpty()) {
                                bCharExist = true;
                                break;
                            }
                        }

                        charAliasList.push_back(charNames.item(l).toElement().text());
                    }
                }

                if (!bCharExist) {
                    if (m_ACGDB->AddCharacter(acgID, charAliasList)) {
                        queryResult = "";
                        if (m_ACGDB->IsCharacterExist(charAliasList.at(0), queryResult)) {
                            if (queryResult.isEmpty()) {
                                QMessageBox msgBox(QMessageBox::Warning,
                                                   "Error",
                                                   QString("%1 cannot be add into the database.").arg(charAliasList.at(0)),
                                                   QMessageBox::Ok,
                                                   NULL);
                                msgBox.exec();
                                continue;
                            }
                        }

                    } else {
                        QMessageBox msgBox(QMessageBox::Warning,
                                           "Error",
                                           QString("%1 cannot be add into the database.").arg(charAliasList.at(0)),
                                           QMessageBox::Ok,
                                           NULL);
                        msgBox.exec();
                        continue;
                    }
                }
            }
        }
    }
}

bool ACGTagGenerator::UpdateWorkCharComboBox(bool bWork)
{
    QDomNode targetNode;
    QDomNode targetParentNode;
    QDomNodeList targetParentChildren;

    if (bWork) {
        ui->comboBoxChar->clear();
        if (!findTargetNode(m_dom, "WorksTags", ui->comboBoxWork->currentText(), targetNode))
            return false;
        targetParentNode = targetNode.parentNode();
        if (targetParentNode.isNull())
            return false;

        int rowCounter = 0;
        targetParentChildren = targetParentNode.childNodes();
        for (int i = 0; i < targetParentChildren.count(); i ++) {
            QString tagName = targetParentChildren.item(i).toElement().tagName();
            QString content = targetParentChildren.item(i).toElement().text();
            if ( tagName == "name") {
                if (i < ui->tableWidgetWork->rowCount() && rowCounter < ui->tableWidgetWork->rowCount() && !content.isEmpty())
                    ui->tableWidgetWork->setItem(rowCounter++, 0, new QTableWidgetItem(content));
            } else if (tagName == "char") {
                QDomNodeList charChildren = targetParentChildren.item(i).toElement().elementsByTagName("name");
                for (int j = 0; j < charChildren.count(); j++) {
                    ui->comboBoxChar->addItem(charChildren.item(j).toElement().text());
                    break;
                }
            }
        }
    } else {
        if (!findTargetNode(m_dom, "WorksTags", ui->comboBoxChar->currentText(), targetNode))
            return false;
        targetParentNode = targetNode.parentNode();
        if (targetParentNode.isNull())
            return false;
        targetParentChildren = targetParentNode.toElement().elementsByTagName("name");
        for (int i = 0; i < targetParentChildren.count(); i++) {
            if (i < ui->tableWidgetChar->rowCount())
                ui->tableWidgetChar->setItem(i, 0,
                                             new QTableWidgetItem(targetParentChildren.item(i).toElement().text()));
        }
    }

    return true;
}

bool ACGTagGenerator::UpdateWorkCharComboBoxByDB(bool bWork)
{
    if (bWork) {
        ui->comboBoxChar->clear();

        QStringList acgAliasList;
        QStringList acgFieldList;
        if (m_ACGDB->QueryACG(ui->comboBoxWork->currentText(), acgAliasList, acgFieldList)) {
            int rowCounter = 0;
            for (int i = 0; i < acgAliasList.count(); i++) {
                ui->tableWidgetWork->setItem(rowCounter++, 0, new QTableWidgetItem(acgAliasList.at(i)));
            }

            QStringList charList;
            if (m_ACGDB->QueryCharactersByACG(ui->comboBoxWork->currentText(), charList)) {
                for (int j = 0; j < charList.count(); j++) {
                    ui->comboBoxChar->addItem(charList.at(j));
                }
            } else
                return false;
        } else
            return false;
    } else {
        QStringList acgFieldList;
        QStringList acgAliasList;
        QStringList charFieldList;
        QStringList charAliasList;

        if (m_ACGDB->QueryCharacter(ui->comboBoxChar->currentText(),
                                charAliasList, acgAliasList,
                                charFieldList, acgFieldList)) {
            int rowCounter = 0;
            for (int i = 1; i < charAliasList.count(); i++) { // charAliasList.at(0) is ACG ID. Ignore it.
                ui->tableWidgetChar->setItem(rowCounter++, 0, new QTableWidgetItem(charAliasList.at(i)));
            }
        } else
            return false;
    }

    return true;
}

bool ACGTagGenerator::findTargetNode(QDomDocument &dom, const QString &tag, const QString &targetTag, QDomNode &targetNode)
{
    QDomNodeList list = dom.elementsByTagName(tag);
    for (int i = 0; i < list.count(); i++) {
        QDomNodeList names = list.item(i).toElement().elementsByTagName("name");
        if (names.count() < 1) // No names exists.
            continue;

        for (int j = 0; j < names.count(); j++) {
            //qDebug() << names.item(j).toElement().text();
            if (names.item(j).toElement().text() == targetTag) {
                targetNode = names.item(j);
                return true;
            }
        }
    }

    return false;
}

void ACGTagGenerator::traverseParent(QDomNode &targetNode, QString &output)
{
    QDomNode parent = targetNode.parentNode();
    while (!parent.isNull()) {
        QDomNodeList children = parent.childNodes();
        for (int i = 0; i < children.count(); i++) {
            if (children.item(i).toElement().tagName() == "name") {
                output.append("\"" + children.item(i).toElement().text());
                if (parent.toElement().tagName() == "activity" && ui->spinBox->value() > -1)
                    output.append(QString("%1").arg(ui->spinBox->value()));
                output.append("\" ");
            }
        }
        parent = parent.parentNode();
    }
}

void ACGTagGenerator::GetACGCharOutputByDB(QString &output)
{
    QTreeWidgetItem *itemChar = ui->treeWidgetWork->currentItem();
    if (itemChar != NULL) {
        if (itemChar->parent() == NULL) { // ACG
            QStringList acgFieldList;
            QStringList acgAliasList;

            if (m_ACGDB->QueryACG(itemChar->text(0), acgAliasList, acgFieldList)) {
                for (int i = 0; i < acgAliasList.count() && !acgAliasList.at(i).isEmpty(); i++) {
                    output += QString("\"%1\" ").arg(acgAliasList.at(i));
                    if (i == 0)
                        ui->comboBoxWork->setCurrentText(acgAliasList.at(i));
                }
            } else
                return;
        } else { // Character
            QStringList acgFieldList;
            QStringList acgAliasList;
            QStringList charFieldList;
            QStringList charAliasList;

            if (m_ACGDB->QueryCharacter(itemChar->text(0),
                                    charAliasList, acgAliasList,
                                    charFieldList, acgFieldList)) {
                for (int i = 0; i < acgAliasList.count() && !acgAliasList.at(i).isEmpty(); i++) {
                    output += QString("\"%1\" ").arg(acgAliasList.at(i));
                    if (i == 0)
                        ui->comboBoxWork->setCurrentText(acgAliasList.at(i));
                }
                for (int i = 1; i < charAliasList.count() && !charAliasList.at(i).isEmpty(); i++) {
                    output += QString("\"%1\" ").arg(charAliasList.at(i));
                    if (i == 1)
                        ui->comboBoxChar->setCurrentText(charAliasList.at(i));
                }
            } else
                return;
        }
    }
}

void ACGTagGenerator::GetACGCharOutputByXML(QString &output)
{
     QTreeWidgetItem *itemChar = ui->treeWidgetWork->currentItem();
     if (itemChar != NULL) {
         QDomNode targetNode;
         findTargetNode(m_dom, "WorksTags", itemChar->text(0), targetNode);
         traverseParent(targetNode, output);

         if (!targetNode.isNull()) {
             QDomNode targetParentNode = targetNode.parentNode();
             if (!targetParentNode.isNull() && targetParentNode.toElement().tagName() == "char") {
                 QDomNode targetGrandParentNode = targetParentNode.parentNode();
                 if (!targetGrandParentNode.isNull() && targetGrandParentNode.toElement().tagName() == "work") {
                     QDomNode targetNodeSibling = targetGrandParentNode.firstChild();
                     while (!targetNodeSibling.isNull()) {
                         if (targetNodeSibling.toElement().tagName() == "name") {
                             ui->comboBoxWork->setCurrentText(targetNodeSibling.toElement().text());
                             break;
                         }
                         targetNodeSibling = targetNodeSibling.nextSibling();
                     }

                     if (!targetNodeSibling.isNull())
                         ui->comboBoxChar->setCurrentText(targetNode.toElement().text());
                 }
             } else if (!targetParentNode.isNull() && targetParentNode.toElement().tagName() == "work") {
                 QDomNode targetNodeSibling = targetParentNode.firstChild();
                 while (!targetNodeSibling.isNull()) {
                     if (targetNodeSibling.toElement().tagName() == "name") {
                         ui->comboBoxWork->setCurrentText(targetNodeSibling.toElement().text());
                         break;
                     }
                     targetNodeSibling = targetNodeSibling.nextSibling();
                 }
             }
         }
     }
}

void ACGTagGenerator::on_pushButton_clicked()
{
    QTreeWidgetItem *itemPlace = ui->treeWidgetLocation->currentItem();
    QTreeWidgetItem *itemActivity = ui->treeWidgetActivity->currentItem();
    QString output;

    if (itemActivity != NULL) {
        QDomNode targetNode;
        findTargetNode(m_dom, "ActivitiesTags", itemActivity->text(0), targetNode);
        traverseParent(targetNode, output);
    }

    if (itemPlace != NULL) {
        QDomNode targetNode;
        findTargetNode(m_dom, "LocationTags", itemPlace->text(0), targetNode);
        traverseParent(targetNode, output);
    }

    //GetACGCharOutputByXML(output);
    GetACGCharOutputByDB(output);

    for (int i = 0; i < m_commonTagList.count(); i++) {
        output.append("\"" + m_commonTagList[i] + "\" ");
    }
    ui->plainTextEdit->setPlainText(output);
}

void ACGTagGenerator::on_itemSelectionChanged()
{
    on_pushButton_clicked();
}

void ACGTagGenerator::on_pushButtonClear_clicked()
{
    ui->plainTextEdit->clear();
}

void ACGTagGenerator::on_spinBox_editingFinished()
{
    if (NULL != ui->treeWidgetActivity->currentItem())
        on_pushButton_clicked();
}

void ACGTagGenerator::on_comboBoxChar_currentTextChanged(const QString &arg1)
{
    ui->tableWidgetChar->clear();
    if (!this->UpdateWorkCharComboBoxByDB(false))
        if (!arg1.isEmpty())
             ui->tableWidgetChar->setItem(0, 0, new QTableWidgetItem(arg1));
}

bool ACGTagGenerator::createNewCharNode(QDomNode &newChar)
{
    newChar = m_dom.createElement("char");
    bool bHasNewName = false;
    for (int i = 0; i < ui->tableWidgetChar->rowCount(); i++) {
        if (NULL == ui->tableWidgetChar->item(i, 0))
            continue;

        if (!ui->tableWidgetChar->item(i, 0)->text().isEmpty()) {
            QDomNode newCharName = m_dom.createElement("name");
            QDomNode newCharNameContent = m_dom.createTextNode(ui->tableWidgetChar->item(i, 0)->text());
            newCharName.appendChild(newCharNameContent);
            newChar.appendChild(newCharName);
            bHasNewName = true;
        }
    }

    return bHasNewName;
}

bool ACGTagGenerator::addWorkNameNode(QDomNode &targetNode)
{
    bool bHasNewName = false;
    for (int i = 0; i < ui->tableWidgetWork->rowCount(); i++) {
        if (NULL == ui->tableWidgetWork->item(i, 0))
            continue;

        if (!ui->tableWidgetWork->item(i, 0)->text().isEmpty()) {
            QDomNode newWorkName = m_dom.createElement("name");
            QDomNode newWorkNameContent = m_dom.createTextNode(ui->tableWidgetWork->item(i, 0)->text());
            newWorkName.appendChild(newWorkNameContent);
            targetNode.appendChild(newWorkName);
            bHasNewName = true;
        }
    }

    return bHasNewName;
}

void ACGTagGenerator::on_pushButtonApply_clicked()
{
    if (m_TagXmlFileName.isEmpty())
        return;

    QDomNode targetNode;
    if (findTargetNode(m_dom, "WorksTags", ui->comboBoxWork->currentText(), targetNode)) {
        QDomNode targetParentNode = targetNode.parentNode();
        if (targetParentNode.isNull())
            return; // XML file may have incorrect data format.

        // Remove all work's name.
        QDomNode sibling = targetParentNode.firstChild();
        //if (targetNode.toElement().tagName() == "name")
        //    targetParentNode.removeChild(targetNode);
        while (!sibling.isNull()) {
            QDomNode removeCandidate = sibling;
            sibling = sibling.toElement().nextSibling();
            if (removeCandidate.toElement().tagName() == "name")
                targetParentNode.removeChild(removeCandidate);
        }

        // Add by the content in the table widget.
        addWorkNameNode(targetParentNode);

        QDomNode targetCharNode;
        if (!findTargetNode(m_dom, "WorksTags", ui->comboBoxChar->currentText(), targetCharNode)) {
            QDomNode newChar;
            if (createNewCharNode(newChar))
                targetParentNode.appendChild(newChar);
        } else {
            targetParentNode.removeChild(targetCharNode.parentNode());
            QDomNode newChar;
            if (createNewCharNode(newChar))
                targetParentNode.toElement().appendChild(newChar);
        }
    } else {
        QDomNodeList worksTags = m_dom.elementsByTagName("WorksTags");
        QDomNode newWorksTags;
        if (worksTags.count() > 0) {
            newWorksTags = worksTags.item(0);
        } else {
            newWorksTags = m_dom.createElement("WorksTags");
            m_dom.appendChild(newWorksTags);
        }

        QDomNode newWork = m_dom.createElement("work");
        QDomNode newChar = m_dom.createElement("char");
        bool bAddWorkNameRet = false;
        bool bCreateNewCharNodeRet = false;

        bAddWorkNameRet = addWorkNameNode(newWork);
        bCreateNewCharNodeRet = createNewCharNode(newChar);
        if (bCreateNewCharNodeRet)
            newWork.appendChild(newChar);
        if (bCreateNewCharNodeRet || bAddWorkNameRet) // New work has at least one "char" or "name" child node.
            newWorksTags.appendChild(newWork);
    }

    QFile *file = new QFile(m_TagXmlFileName + ".tmp");
    if (file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(file);
        stream.setCodec("UTF-8");
        stream << m_dom.toString();
        file->close();
    }

    this->InitializeWorksTagsTreeByXML();
}

void ACGTagGenerator::on_comboBoxWork_currentTextChanged(const QString &arg1)
{
    ui->tableWidgetWork->clear();
    if (!this->UpdateWorkCharComboBoxByDB())
        if (!arg1.isEmpty())
             ui->tableWidgetWork->setItem(0, 0, new QTableWidgetItem(arg1));
}

void ACGTagGenerator::on_pushButtonOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Tag XML"), "./", tr("XML Files (*.xml *.XML);;All Files (*.*)"));

    if (fileName != m_TagXmlFileName) {
        clearCurrentDom();
        m_TagXmlFileName = fileName;
    }

    InitializeByDB();
    ui->plainTextEdit->clear();
}

CDBControl::CDBControl(const QString& dbPath, const QString& dbType)
    : m_ref(0),
      m_dbPath(dbPath),
      m_dbType(dbType)
{
    m_db = QSqlDatabase::addDatabase(m_dbType, QString("con%1").arg(m_dbPath));
    m_db.setDatabaseName(m_dbPath);
    OpenDB();
}

CDBControl::~CDBControl()
{
    CloseDB();
    m_db.removeDatabase(QString("con%1").arg(m_dbPath));
}

bool CDBControl::OpenDB()
{
    m_db = QSqlDatabase::database(QString("con%1").arg(m_dbPath));
    if (!m_db.open()) {
        qDebug() << m_db.lastError();
        return false;
    }

    return true;
}

void CDBControl::CloseDB()
{
    if (m_db.isOpen() && m_ref == 0) {
        m_db.close();
    }
}

bool CDBControl::QueryDB(const QString &query)
{
    bool ret = false;

    //if (!OpenDB())
    //    return false;

    qDebug() << query;
    QSqlQuery qry(m_db);
    qry.prepare(query);
    if (!(ret = qry.exec())) {
        QMessageBox message(QMessageBox::Warning,"Error",qry.lastError().text(),QMessageBox::Ok,NULL);
        message.exec();
    }

    //CloseDB();

    return ret;
}

bool CDBControl::QueryDB(const QString& query, QSqlQuery& requester)
{
    bool ret = false;

    //if (!OpenDB())
    //    return false;
    qDebug() << query;
    QSqlQuery qry(m_db);
    qry.prepare(query);
    if (!(ret = qry.exec())) {
        QMessageBox message(QMessageBox::Warning,"Error",qry.lastError().text(),QMessageBox::Ok,NULL);
        message.exec();
    }

    requester = qry;

    //CloseDB();

    return ret;
}

bool CDBControl::CreateTable(const QString &name, struct DBTableDefinition& definition)
{
    //if (IsOpen()) {
        QString query = "CREATE TABLE IF NOT EXISTS " + name;
        QString table_definition = " (";

        // "CREATE TABLE IF NOT EXISTS names (id INTEGER UNIQUE PRIMARY KEY, firstname VARCHAR(30), lastname VARCHAR(30))"
        for (unsigned i = 0; i < definition.fieldNames.count(); i++) {
            table_definition += definition.fieldNames.at(i);
            table_definition += " ";
            table_definition += definition.fieldTypes.at(i);
            if (i == definition.fieldPrimary) {
                table_definition += " UNIQUE PRIMARY KEY";
            } else if (definition.fieldUnique.at(i)) {
                table_definition += " UNIQUE";
            } else if (definition.fieldForeign.at(i)) {
                table_definition += QString(" REFERENCES %1(%2)").arg(definition.fieldForeignDefinition.at(i).refDB).arg(definition.fieldForeignDefinition.at(i).refField);
            }

            if (i != definition.fieldNames.count() - 1)
                table_definition += ", ";
        }
        table_definition += ")";

        query += table_definition;
        //query += " WITHOUT ROWID"; // I want use my own primary key.
        return QueryDB(query);
    /*} else {
        qDebug() << "DB is not opened.";
    }*/

    //return false;
}

bool CDBControl::IsOpen()
{
    return m_db.isOpen();
}

void CDBControl::DecRef()
{
    QMutexLocker _l(&m_refLock);
    if (m_ref == 0)
        return;
    else
        m_ref--;
}

void CDBControl::IncRef()
{
    QMutexLocker _l(&m_refLock);
    m_ref++;
}

bool CDBControl::GetTableList(QStringList& tableList)
{
    //if (!IsOpen())
        //return false;
    m_db = QSqlDatabase::database(QString("con%1").arg(m_dbPath));
    tableList = m_db.tables();
    if (tableList.empty())
        qDebug() << "There is no table in db.";

    return true;
}

CACGDB::CACGDB (const QString &dbPath, const QString& dbType)
    : CDBControl(dbPath, dbType),
      m_acgTableName("ACG"),
      m_charTableName("Character")
{
    QStringList tableList;
    if (GetTableList(tableList) && !tableList.empty()) {
        for (int i = 0; i < tableList.count() && (!m_hasACGTable || !m_hasCharTable); i++) {
            if (tableList.at(i) == m_acgTableName)
                m_hasACGTable = true;
            else if (tableList.at(i) == m_charTableName)
                m_hasCharTable = true;
        }
    }

    if (!m_hasACGTable)
        m_hasACGTable = CreateACGTable();

    if (!m_hasCharTable)
        m_hasCharTable = CreateCharTable();

    //m_isWorkable = IsOpen();
}

CACGDB::~CACGDB()
{
    QMutexLocker _l(&m_isWorkableLock);
    m_isWorkable = false;
}

bool CACGDB::CreateACGTable()
{
    //QMutexLocker _l(&m_isWorkableLock);
    //if (!m_isWorkable)
    //    return false;

    struct DBTableDefinition definition;

    definition.fieldNames.push_back("ID");
    definition.fieldTypes.push_back("INTEGER");
    definition.fieldForeign.push_back(false);
    definition.fieldUnique.push_back(false);
    definition.fieldPrimary = 0;
    for (int i = 0; i < 10; i++) {
        definition.fieldNames.push_back(QString("NAME%1").arg(i));
        definition.fieldTypes.push_back("TEXT");
        if (i == 0)
            definition.fieldUnique.push_back(true);
        else
            definition.fieldUnique.push_back(false);
        definition.fieldForeign.push_back(false);
    }

    return CreateTable(m_acgTableName, definition);
}

bool CACGDB::CreateCharTable()
{
    //QMutexLocker _l(&m_isWorkableLock);
    //if (!m_isWorkable)
    //    return false;

    struct DBTableDefinition definition;
    struct ForeignKeyDefinition fkDefinition;

    definition.fieldNames.push_back("ACG");
    definition.fieldTypes.push_back("INTEGER");
    definition.fieldForeign.push_back(true);
    definition.fieldUnique.push_back(false);
    fkDefinition.refDB = m_acgTableName;
    fkDefinition.refField = "ID";
    definition.fieldForeignDefinition.push_back(fkDefinition);
    definition.fieldPrimary = 1; //Name0
    fkDefinition.refDB = "";
    fkDefinition.refField = "";
    for (int i = 0; i < 10; i++) {
        definition.fieldNames.push_back(QString("NAME%1").arg(i));
        definition.fieldTypes.push_back("TEXT");
        definition.fieldUnique.push_back(false);
        definition.fieldForeign.push_back(false);
        definition.fieldForeignDefinition.push_back(fkDefinition);
    }

    return CreateTable(m_charTableName, definition);
}

bool CACGDB::IsACGExist(const QString &name, int& acgID)
{
    QString sqlCmd = QString("SELECT ID FROM %1 WHERE ").arg(m_acgTableName);
    QString condition;
    QSqlQuery requester;
    bool ret;

    for (int i = 0; i < 10; i++) {
        condition += QString("NAME%1 = \"%2\"").arg(i).arg(name);
        if (i != 9)
            condition += " OR ";
    }

    sqlCmd += condition;
    if (ret = QueryDB(sqlCmd, requester)) {
        for (int r = 0; requester.next(); r++) {
            acgID = requester.value(0).toInt();
        }
    }

    return ret;
}

bool CACGDB::QueryCharacter(const QString &name,
                            QStringList &charAliasList, QStringList &workAliasList,
                            QStringList &charFieldList, QStringList &workFieldList)
{
    QString queryACGFields;
    QString queryCharFields = QString("%1.%2, ").arg(m_charTableName).arg("ACG");
    QString queryConditions;
    QString sqlCmd;
    QSqlQuery requester;
    bool ret;


    for (int i = 0; i < 10; i ++) {
        queryACGFields += QString("%1.%2").arg(m_acgTableName).arg(QString("NAME%1").arg(i));
        queryCharFields += QString("%1.%2").arg(m_charTableName).arg(QString("NAME%1").arg(i));

        if (i != 9) {
            queryACGFields += ", ";
            queryCharFields += ", ";
        }
    }

    queryConditions = QString("%1.ACG = %2.ID AND %3.NAME0 = \"%4\"").arg(m_charTableName).arg(m_acgTableName).arg(m_charTableName).arg(name);

    sqlCmd = QString("SELECT %1, %2 FROM %3, %4 WHERE %5").arg(queryACGFields).arg(queryCharFields).arg(m_acgTableName).arg(m_charTableName).arg(queryConditions);
    if (ret = QueryDB(sqlCmd, requester)) {
        QSqlRecord rec = requester.record();
        for (int c = 0; c < rec.count(); c++) {
            if (c < 10)
                workFieldList.push_back(rec.fieldName(c));
            else
                charFieldList.push_back(rec.fieldName(c));
        }

        for (int r = 0; requester.next(); r++) {
            for (int c = 0; c < rec.count(); c++) {
                if (c < 10)
                    workAliasList.push_back(requester.value(c).toString());
                else
                    charAliasList.push_back(requester.value(c).toString());
            }
        }
    }

    return ret;
}

bool CACGDB::QueryAllCharacterList(QStringList &charList)
{
    QString sqlCmd = QString("SELECT NAME0 FROM %1").arg(m_charTableName);
    QSqlQuery requester;
    bool ret = false;

    if (ret = QueryDB(sqlCmd, requester)) {
        QSqlRecord rec = requester.record();
        for (int r = 0; requester.next(); r++) {
            for (int c = 0; c < rec.count(); c++) {
                charList.push_back(requester.value(c).toString());
            }
        }
    }

    return ret;
}

bool CACGDB::QueryACG(const QString &name, QStringList &aliasList, QStringList &fieldList)
{
    QString queryACGFields;
    QString queryConditions;
    QString sqlCmd;
    QSqlQuery requester;
    bool ret;

    for (int i = 0; i < 10; i ++) {
        queryACGFields += QString("%1").arg(QString("NAME%1").arg(i));

        if (i != 9) {
            queryACGFields += ", ";
        }
    }

    queryConditions = QString("NAME0 = \"%1\"").arg(name);

    sqlCmd = QString("SELECT %1 FROM %2 WHERE %3").arg(queryACGFields).arg(m_acgTableName).arg(queryConditions);
    if (ret = QueryDB(sqlCmd, requester)) {
        QSqlRecord rec = requester.record();
        for (int c = 0; c < rec.count(); c++) {
                fieldList.push_back(rec.fieldName(c));
        }

        for (int r = 0; requester.next(); r++) {
            for (int c = 0; c < rec.count(); c++) {
                aliasList.push_back(requester.value(c).toString());
            }
        }
    }

    return ret;
}

bool CACGDB::QueryAllACGList(QStringList &acgList)
{
    QString sqlCmd = QString("SELECT NAME0 FROM %1").arg(m_acgTableName);
    QSqlQuery requester;
    bool ret = false;
    if (ret = QueryDB(sqlCmd, requester)) {
        QSqlRecord rec = requester.record();
        for (int r = 0; requester.next(); r++) {
            for (int c = 0; c < rec.count(); c++) {
                acgList.push_back(requester.value(c).toString());
            }
        }
    }

    return ret;
}

bool CACGDB::AddACG(QStringList &aliasList)
{
    //QMutexLocker _l(&m_isWorkableLock);
    //if (!m_isWorkable)
    //    return false;

    if (!m_hasACGTable)
        if (!(m_hasACGTable = CreateACGTable()))
            return false;

    QString sqlCmd = QString("INSERT INTO %1 ").arg(m_acgTableName);
    QString insertFields = "(";
    QString insertValues = " VALUES (";

    for (int i = 0; i < 10; i++) {
        if (i != 9)
            insertFields += QString("NAME%1, ").arg(i);
        else
            insertFields += QString("NAME%1)").arg(i);
    }

    for (int i = 0; i < 10; i++) {
        if (i < aliasList.count())
            insertValues += QString("\"%1\"").arg(aliasList.at(i));
        else
            insertValues += "\"\"";

        if (i != 9)
            insertValues += ", ";
        else
            insertValues += ")";
    }

    sqlCmd += insertFields;
    sqlCmd += insertValues;
    return QueryDB(sqlCmd);
}

bool CACGDB::IsCharacterExist(const QString &name, QString &queryResult)
{
    QString sqlCmd = QString("SELECT NAME0 FROM %1 WHERE ").arg(m_charTableName);
    QString condition;
    QSqlQuery requester;
    bool ret;

    for (int i = 0; i < 10; i++) {
        condition += QString("NAME%1 = \"%2\"").arg(i).arg(name);
        if (i != 9)
            condition += " OR ";
    }

    sqlCmd += condition;
    if (ret = QueryDB(sqlCmd, requester)) {
        for (int r = 0; requester.next(); r++) {
            queryResult = requester.value(0).toString();
        }
    }

    return ret;
}

bool CACGDB::AddCharacter(int workId, QStringList &aliasList)
{
    //QMutexLocker _l(&m_isWorkableLock);
    //if (!m_isWorkable)
    //    return false;

    if (!m_hasCharTable)
        if (!(m_hasCharTable = CreateCharTable()))
            return false;

    QString sqlCmd = QString("INSERT INTO %1 ").arg(m_charTableName);
    QString insertFields = QString("(ACG, ");
    QString insertValues = QString(" VALUES (%1, ").arg(workId);

    for (int i = 0; i < 10; i++) {
        if (i != 9)
            insertFields += QString("NAME%1, ").arg(i);
        else
            insertFields += QString("NAME%1)").arg(i);
    }

    for (int i = 0; i < 10; i++) {
        if (i < aliasList.count())
            insertValues += QString("\"%1\"").arg(aliasList.at(i));
        else
            insertValues += "\"\"";

        if (i != 9)
            insertValues += ", ";
        else
            insertValues += ")";
    }

    sqlCmd += insertFields;
    sqlCmd += insertValues;
    return QueryDB(sqlCmd);
}

bool CACGDB::QueryCharactersByACG(const QString &name, QStringList &charList)
{
    int acgID = ACGID_INVALID;
    if (!IsACGExist(name, acgID))
        return false;

    if (acgID == ACGID_INVALID)
        return false;

    QString sqlCmd = QString("SELECT %1.NAME0 FROM %1, %2 WHERE %1.ACG = %2.ID AND %2.ID = %3").arg(m_charTableName).arg(m_acgTableName).arg(acgID);
    QSqlQuery requester;
    if (QueryDB(sqlCmd, requester)) {
        QSqlRecord rec = requester.record();
        for (int r = 0; requester.next(); r++) {
            for (int c = 0; c < rec.count(); c++) {
                qDebug() << requester.value(c).toString();
                charList.push_back(requester.value(c).toString());
            }
        }
    } else
        return false;

    return true;
}


void ACGTagGenerator::on_pushButton_2_clicked()
{
    QStringList acgList;
    m_ACGDB->QueryAllACGList(acgList);
    m_dbDialog.ClearAcgTable();
    for (int i = 0; i < acgList.count(); i++) {
        QStringList acgFieldList;
        QStringList acgAliasList;

        m_ACGDB->QueryACG(acgList.at(i), acgAliasList, acgFieldList);
        m_dbDialog.SetACGTable(acgFieldList, acgAliasList);
    }

    QStringList charList;
    m_ACGDB->QueryAllCharacterList(charList);
    m_dbDialog.ClearCharacterTable();
    for (int i = 0; i < charList.count(); i++) {
        QStringList acgFieldList;
        QStringList acgAliasList;
        QStringList charFieldList;
        QStringList charAliasList;

        m_ACGDB->QueryCharacter(charList.at(i),
                                charAliasList, acgAliasList,
                                charFieldList, acgFieldList);
        m_dbDialog.SetCharacterTable(charFieldList, charAliasList);
    }

    m_dbDialog.exec();
}

void ACGTagGenerator::on_pushButtonXMLtoDB_clicked()
{
    TransferXMLtoDatabase();
}
