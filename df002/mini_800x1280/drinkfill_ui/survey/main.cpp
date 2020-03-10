#include <iostream>
#include <QApplication>

#include "survey.h"

using namespace std;

int main(int argc, char *argv[])
{
    /*(qputenv) This function sets the value of the environment variable named varName.
     * It will create the variable if it does not exist.
     *  It returns 0 if the variable could not be set.*/


    cout << "Keyboard: " << qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"))<< endl;
    cout << "Layout: " << qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/layouts")<< endl;
    qputenv("QT_VIRTUALKEYBOARD_STYLE", "drinkfill");



    QApplication a(argc, argv);
    survey keyboardTest;
    keyboardTest.showFullScreen();


    return a.exec();
}
