#ifndef ACGTAGGENERATOR_H
#define ACGTAGGENERATOR_H

#include <QWidget>
#include <QDomDocument>

namespace Ui {
class ACGTagGenerator;
}

class ACGTagGenerator : public QWidget
{
    Q_OBJECT

public:
    explicit ACGTagGenerator(QWidget *parent = 0);
    ~ACGTagGenerator();

private:
    Ui::ACGTagGenerator *ui;
    QStringList m_commonTagList;

public:
    void Initialize();
    void findTargetNode(QDomDocument &dom, const QString &tag, const QString &targetTag, QDomNode &targetNode);
    void traverseParent(QDomNode &targetNode, QString &output);

private slots:
    void on_pushButton_clicked();
    void on_treeWidgetActivity_itemSelectionChanged();
    void on_treeWidgetWork_itemSelectionChanged();
    void on_treeWidgetLocation_itemSelectionChanged();
    void on_pushButtonClear_clicked();
    void on_spinBox_editingFinished();
};

#endif // ACGTAGGENERATOR_H
