#include "drinkfill_ev.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    drinkfill_ev w;
    w.setFixedWidth(610);
    w.setFixedHeight(450);
    w.show();

    return a.exec();
}
