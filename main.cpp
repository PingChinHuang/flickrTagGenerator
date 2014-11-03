#include "acgtaggenerator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ACGTagGenerator w;
    w.show();

    return a.exec();
}
