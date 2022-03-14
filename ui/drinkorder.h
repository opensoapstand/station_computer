#ifndef DRINKORDER_H
#define DRINKORDER_H

#include "df_util.h"

// TODO: Remove and direct link to dftypes.h and debug value headers in FSM
typedef enum DF_QT_OPTIONS {
   // Drink Slots
   OPTION_SLOT = 0,
   OPTION_SLOT_1,
   OPTION_SLOT_2,
   OPTION_SLOT_3,
   OPTION_SLOT_4,
   OPTION_SLOT_5,
   OPTION_SLOT_6,
   OPTION_SLOT_7,
   OPTION_SLOT_8,
   OPTION_SLOT_9
   ,

   // Drink Sizes
   DRINK_SIZE_OPTIONS = 15,
   SMALL_DRINK,
//   MEDIUM_DRINK,
   LARGE_DRINK
//    ,
//    DRINK1,
//    DRINK2,
//    DRINK3,
//    DRINK4
} DF_QT_OPTION_PICKED;

// Values for Selected drink.
struct DrinkSelection{
    int optionNumber;
    double drinkML;
    double drinkPrice;
};

// Values for labels; TODO: Can get from seeded DB
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

    // HACK: Fixed volume reference; Need to figure out best storage location...
    constexpr static double EMPTY_SIZE_ML = 0.00;
//    constexpr static double SMALL_SIZE_ML = 355.00;
//    constexpr static double LARGE_SIZE_ML = 473.00;

    double DRINK1_SIZE_ML_L;
    double DRINK2_SIZE_ML_L;
    double DRINK3_SIZE_ML_L;
    double DRINK4_SIZE_ML_L;

    double DRINK1_SIZE_ML_S;
    double DRINK2_SIZE_ML_S;
    double DRINK3_SIZE_ML_S;
    double DRINK4_SIZE_ML_S;

    double DRINK1_PRICE_LARGE;
    double DRINK2_PRICE_LARGE;
    double DRINK3_PRICE_LARGE;
    double DRINK4_PRICE_LARGE;

    double DRINK1_price_small;
    double DRINK2_price_small;
    double DRINK3_price_small;
    double DRINK4_price_small;

    //constexpr static double price_small_TEST = 3.00;
    //constexpr static double PRICE_LARGE_TEST = 4.00;

    // Setters and Getters
    void setOrderSize(DF_QT_OPTIONS sizeOption);
    void setOrderSlot(DF_QT_OPTIONS optionSlot);

    int getOrderSlot() const {return m_optionNumber;}
    double getOrderPrice() const {return m_drinkPrice;}
    double getSize() const {return m_drinkML;}
    DF_QT_OPTIONS getSizeOption();

public slots:
    void changeOption(int optNumber);
    void setPrice(double price);
    void setSize(double size);

signals:
    void orderSlotChange(int newOpt);
    void priceChange(double newPrice);
    void sizeChange(double newSize);

private:
    DrinkSelection *selectedDrink;
    DF_QT_OPTIONS sizeOptionSelected;
};

#endif // DRINKORDER_H
