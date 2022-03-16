#ifndef DRINKORDER_H
#define DRINKORDER_H

#include "df_util.h"

#define INVALID_PRICE -666

// TODO: Remove and direct link to dftypes.h and debug value headers in FSM
// typedef enum DF_QT_SLOTS {
//    // Drink Slots
//    OPTION_SLOT_INVALID = 0,
//    OPTION_SLOT_1,
//    OPTION_SLOT_2,
//    OPTION_SLOT_3,
//    OPTION_SLOT_4,
//    OPTION_SLOT_5,
//    OPTION_SLOT_6,
//    OPTION_SLOT_7,
//    OPTION_SLOT_8,
//    OPTION_SLOT_9,
//    OPTION_MAX_INVALID
// } DF_QT_OPTION_PICKED;

typedef enum DF_QT_SIZES {
   INVALID_DRINK=0,
   SMALL_DRINK,
   MEDIUM_DRINK,
   LARGE_DRINK,
   CUSTOM_DRINK,
   TEST_DRINK,
   MAX_INVALID_DRINK
//    ,
//    DRINK1,
//    DRINK2,
//    DRINK3,
//    DRINK4
} DF_QT_SIZE_PICKED;


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
    int m_selectedSlot; // 
    double m_drinkML;
    // double m_drinkPrice;

public:
    DrinkOrder();
    DrinkOrder(const DrinkOrder& other);
    ~DrinkOrder();
    DrinkOrder& operator=(const DrinkOrder& other);

    // HACK: Fixed volume reference; Need to figure out best storage location...
    constexpr static double EMPTY_SIZE_ML = 0.00;
//    constexpr static double SMALL_SIZE_ML = 355.00;
//    constexpr static double LARGE_SIZE_ML = 473.00;

    // double DRINK1_SIZE_ML_L;
    // double DRINK2_SIZE_ML_L;
    // double DRINK3_SIZE_ML_L;
    // double DRINK4_SIZE_ML_L;

    // double DRINK1_SIZE_ML_S;
    // double DRINK2_SIZE_ML_S;
    // double DRINK3_SIZE_ML_S;
    // double DRINK4_SIZE_ML_S;

    // double DRINK1_PRICE_LARGE;
    // double DRINK2_PRICE_LARGE;
    // double DRINK3_PRICE_LARGE;
    // double DRINK4_PRICE_LARGE;

    // double DRINK1_price_small;
    // double DRINK2_price_small;
    // double DRINK3_price_small;
    // double DRINK4_price_small;

    //constexpr static double price_small_TEST = 3.00;
    //constexpr static double PRICE_LARGE_TEST = 4.00;

    // Setters and Getters
    void setSelectedSlot(int optionSlot);
    int getSelectedSlot();

    // int getSelectedSlot() const {return m_selectedSlot;}

    void setSelectedSize(DF_QT_SIZES sizeOption);
    // double getSelectedProductPrice() const {return m_drinkPrice;}
    double getSelectedVolume();
    // double getSelectedVolume() const {return m_drinkML;}
    double getSelectedProductPrice();
    DF_QT_SIZES getSelectedSizeOption();
    char getSelectedSizeAsChar();
    bool isSelectedOrderValid();

public slots:

    // void setSelectedSlot(int optNumber);
    void setSelectedOverrulePrice(double price);
    // void setSize(double size);

signals:
    void orderSlotChange(int newOpt);
    void priceChange(double newPrice);
    void sizeChange(double newSize);

private:
    DrinkSelection *selectedDrink;
    //DF_QT_SLOTS sizeOptionSelected;
    DF_QT_SIZES sizeOptionSelected;

    double overruledPrice;
};

#endif // DRINKORDER_H
