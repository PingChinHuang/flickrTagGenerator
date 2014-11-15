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
}

ACGTagGenerator::~ACGTagGenerator()
{
    m_dom.clear();
    delete ui;
}

void ACGTagGenerator::Initialize()
{
    QFile *file = new QFile("F:/Qt/Qt5.2.1/User/Audi/build-ACGTagGenerator-Desktop_Qt_5_2_1_MinGW_32bit-Debug/debug/acgtag.xml");
    if (file->open(QIODevice::ReadWrite)) {
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

    //UpdateWorkCharComboBox();
    /*QDomNode targetNode;
    findTargetNode(dom, "WorksTags", ui->comboBoxWork->currentText(), targetNode);
    QDomNode targetParentNode = targetNode.parentNode();
    QDomNodeList targetParentChildren = targetParentNode.childNodes();
    for (int i = 0; i < targetParentChildren.count(); i ++) {
        QString tagName = targetParentChildren.item(i).toElement().tagName();
        QString content = targetParentChildren.item(i).toElement().text();
        if ( tagName == "name") {
            ui->comboBoxWorkName->addItem(content);
        } else if (tagName == "char") {
            QDomNodeList charChildren = targetParentChildren.item(i).toElement().elementsByTagName("name");
            for (int j = 0; j < charChildren.count(); j++) {
                ui->comboBoxChar->addItem(charChildren.item(j).toElement().text());
                break;
            }
        }
    }

    findTargetNode(dom, "WorksTags", ui->comboBoxChar->currentText(), targetNode);
    targetParentNode = targetNode.parentNode();
    targetParentChildren = targetParentNode.toElement().elementsByTagName("name");
    for (int i = 0; i < targetParentChildren.count(); i++) {
        ui->comboBoxCharName->addItem(targetParentChildren.item(i).toElement().text());
    }*/
}

void ACGTagGenerator::UpdateWorkCharComboBox(bool bWork)
{
    QDomNode targetNode;
    QDomNode targetParentNode;
    QDomNodeList targetParentChildren;

    if (bWork) {
        ui->comboBoxChar->clear();
        findTargetNode(m_dom, "WorksTags", ui->comboBoxWork->currentText(), targetNode);
        targetParentNode = targetNode.parentNode();
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
        findTargetNode(m_dom, "WorksTags", ui->comboBoxChar->currentText(), targetNode);
        targetParentNode = targetNode.parentNode();
        targetParentChildren = targetParentNode.toElement().elementsByTagName("name");
        for (int i = 0; i < targetParentChildren.count(); i++) {
            if (i < ui->tableWidgetChar->rowCount())
                ui->tableWidgetChar->setItem(i, 0,
                                             new QTableWidgetItem(targetParentChildren.item(i).toElement().text()));
        }
    }
}

void ACGTagGenerator::findTargetNode(QDomDocument &dom, const QString &tag, const QString &targetTag, QDomNode &targetNode)
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
                return;
            }
        }
    }
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

void ACGTagGenerator::on_treeWidgetActivity_itemSelectionChanged()
{
    on_pushButton_clicked();
}

void ACGTagGenerator::on_treeWidgetWork_itemSelectionChanged()
{
    on_pushButton_clicked();
}

void ACGTagGenerator::on_treeWidgetLocation_itemSelectionChanged()
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

void ACGTagGenerator::on_comboBoxWork_currentIndexChanged(const QString &arg1)
{
    ui->tableWidgetWork->clear();
    this->UpdateWorkCharComboBox();
}

void ACGTagGenerator::on_comboBoxChar_currentTextChanged(const QString &arg1)
{
    ui->tableWidgetChar->clear();
    this->UpdateWorkCharComboBox(false);
}
