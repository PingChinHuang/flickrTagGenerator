#include "acgtaggenerator.h"
#include "ui_acgtaggenerator.h"

#include <QFile>
#include <QDebug>

ACGTagGenerator::ACGTagGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ACGTagGenerator)
{
    ui->setupUi(this);
    ui->pushButton->hide();
    Initialize();

    connect(ui->treeWidgetActivity, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));
    connect(ui->treeWidgetLocation, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));
    connect(ui->treeWidgetWork, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));
}

ACGTagGenerator::~ACGTagGenerator()
{
    m_dom.clear();
    delete ui;
}

void ACGTagGenerator::Initialize()
{
    QFile *file = new QFile("F:/Qt/Qt5.2.1/User/Audi/build-ACGTagGenerator-Desktop_Qt_5_2_1_MinGW_32bit-Debug/debug/acgtag.xml");
    if (file->open(QIODevice::ReadOnly)) {
        m_dom.setContent(file);
    }
    file->close();

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

    InitializeWorksTagsTree();
}

void ACGTagGenerator::InitializeWorksTagsTree()
{
    ui->treeWidgetWork->clear();

    QDomNodeList worksTags = m_dom.elementsByTagName("WorksTags");
    for (int i = 0; i < worksTags.count(); i++) {
        QDomNodeList works = worksTags.item(i).toElement().elementsByTagName("work");
        QTreeWidgetItem *item = NULL;
        for (int j = 0; j < works.count(); j++) {
            //QDomNodeList workNames = works.item(j).toElement().elementsByTagName("name");
            //if (workNames.count() < 1)
            //    continue;

            QDomNodeList workChildren = works.item(j).childNodes();
            if (workChildren.count() < 1)
                continue;

            bool bWorkNameFound = false;
            for (int k = 0; k < workChildren.count(); k++) {
                if (workChildren.item(k).toElement().tagName() == "name") {
                    if (!bWorkNameFound ) {
                        ui->comboBoxWork->addItem(workChildren.item(k).toElement().text());
                        item = new QTreeWidgetItem(QStringList(workChildren.item(k).toElement().text()));
                        ui->treeWidgetWork->addTopLevelItem(item);
                        bWorkNameFound = true;
                        break;
                    }

                    //ui->comboBoxWorkName->addItem(workChildren.item(k).toElement().text());
                }
            }

            if (!bWorkNameFound)
                continue;
            //item = new QTreeWidgetItem(QStringList(workNames.item(0).toElement().text()));
            //ui->treeWidgetWork->addTopLevelItem(item);

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
        targetParentChildren = targetParentNode.childNodes();
        for (int i = 0; i < targetParentChildren.count(); i ++) {
            QString tagName = targetParentChildren.item(i).toElement().tagName();
            QString content = targetParentChildren.item(i).toElement().text();
            if ( tagName == "name") {
                if (i < ui->tableWidgetWork->rowCount())
                    ui->tableWidgetWork->setItem(i, 0, new QTableWidgetItem(content));
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

void ACGTagGenerator::on_pushButton_clicked()
{
    QTreeWidgetItem *itemPlace = ui->treeWidgetLocation->currentItem();
    QTreeWidgetItem *itemActivity = ui->treeWidgetActivity->currentItem();
    QTreeWidgetItem *itemChar = ui->treeWidgetWork->currentItem();
    QString output;

    if (itemActivity != NULL) {
        QDomNode targetNode;
        findTargetNode(m_dom, "ActivitiesTags", itemActivity->text(0), targetNode);
        traverseParent(targetNode, output);
    }

    if (itemChar != NULL) {
        QDomNode targetNode;
        findTargetNode(m_dom, "WorksTags", itemChar->text(0), targetNode);
        traverseParent(targetNode, output);
    }

    if (itemPlace != NULL) {
        QDomNode targetNode;
        findTargetNode(m_dom, "LocationTags", itemPlace->text(0), targetNode);
        traverseParent(targetNode, output);
    }

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
    if (!this->UpdateWorkCharComboBox(false))
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
    QDomNode targetNode;
    if (findTargetNode(m_dom, "WorksTags", ui->comboBoxWork->currentText(), targetNode)) {
        QDomNode targetParentNode = targetNode.parentNode();
        if (targetParentNode.isNull())
            return; // XML file may have incorrect data format.

        // Remove all work's name.
        QDomNode sibling = targetNode.toElement().nextSibling();
        if (targetNode.toElement().tagName() == "name")
            targetParentNode.removeChild(targetNode);
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

    QFile *file = new QFile("F:/Qt/Qt5.2.1/User/Audi/build-ACGTagGenerator-Desktop_Qt_5_2_1_MinGW_32bit-Debug/debug/acgtag.xml");
    if (file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(file);
        stream.setCodec("UTF-8");
        stream << m_dom.toString();
        file->close();
    }

    this->InitializeWorksTagsTree();
}

void ACGTagGenerator::on_comboBoxWork_currentTextChanged(const QString &arg1)
{
    ui->tableWidgetWork->clear();
    if (!this->UpdateWorkCharComboBox())
        if (!arg1.isEmpty())
             ui->tableWidgetWork->setItem(0, 0, new QTableWidgetItem(arg1));
}
