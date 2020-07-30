#ifndef INCLUDEFILES_H
#define INCLUDEFILES_H

#include <QDebug>
#include <stdint.h>
#include <string.h>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QMessageBox>
#include <QAbstractSocket>

using namespace std;

struct DrinkSelection{
    const char* brandName;
    const char* brandImageRef;
    const char* drinkType;
    const char* drinkFlavor;
    const char* drinkImageRef;
    int optionNumber;
    const char* drinkSubtitle;
    const char* drinktDescription;
    double drinkML;
    double drinkPrice;
};

#endif // INCLUDEFILES_H
