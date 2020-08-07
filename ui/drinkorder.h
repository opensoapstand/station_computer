#ifndef DRINKORDER_H
#define DRINKORDER_H

#include "df_util.h"

struct DrinkSelection{
    int optionNumber;
    double drinkML;
    double drinkPrice;
};

struct QTProductFacing {
    int optionNumber;
    QString brandName;
    QString brandImageRef;
    QString drinkType;
    QString drinkFlavor;
    QString drinkImageRef;
    QString drinkSubtitle;
    QString drinktDescription;
};

class DrinkOrder : public QObject
{
    Q_OBJECT
    int m_optionNumber;
    double m_drinkML;
    double m_drinkPrice;

public:
    DrinkOrder();
    DrinkOrder(const DrinkOrder& other);
    ~DrinkOrder();
    DrinkOrder& operator=(const DrinkOrder& other);

    // Getters
//    int getOption() const {return selectedDrink->optionNumber;}
//    double getPrice() const {return selectedDrink->drinkPrice;}
//    double getSize() const {return selectedDrink->drinkML;}

    int getOption() const {return m_optionNumber;}
    double getPrice() const {return m_drinkPrice;}
    double getSize() const {return m_drinkML;}


public slots:
    void changeOption(int optNumber);
    void setPrice(double price);
    void setSize(double size);

signals:
    void optionChange(int newOpt);
    void priceChange(double newPrice);
    void sizeChange(double newSize);

private:
    DrinkSelection *selectedDrink;
};

#endif // DRINKORDER_H
