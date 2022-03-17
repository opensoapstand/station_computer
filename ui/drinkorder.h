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

// typedef enum DF_QT_SIZES {
//    INVALID_DRINK=0,
//    SIZE_SMALL_INDEX,
//    MEDIUM_DRINK,
//    SIZE_LARGE_INDEX,
//    CUSTOM_DRINK,
//    TEST_DRINK,
//    MAX_INVALID_DRINK
// //    ,
// //    DRINK1,
// //    DRINK2,
// //    DRINK3,
// //    DRINK4
// } DF_QT_SIZE_PICKED;

// Values for Selected drink.
struct DrinkSelection
{
    int optionNumber;
    double drinkML;
    double drinkPrice;
};

// Values for labels; TODO: Can get from seeded DB
struct QTProductFacing
{
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

public:
    DrinkOrder();
    DrinkOrder(const DrinkOrder &other);
    ~DrinkOrder();
    DrinkOrder &operator=(const DrinkOrder &other);

    // HACK: Fixed volume reference; Need to figure out best storage location...
    constexpr static double EMPTY_SIZE_ML = 0.00;

    // Setters and Getters
    void setSelectedSlot(int optionSlot);
    int getSelectedSlot();

    void setSelectedSize(int sizeOption);
    int getSelectedSize();

    bool isSelectedOrderValid();
    
    QString getSelectedProductName();

    double getSelectedVolume();
    double getVolume(int size);
    QString getSelectedSizeToVolumeWithCorrectUnits();
    QString getSizeToVolumeWithCorrectUnitsForSelectedSlot(int size);

    double getPrice(int sizeIndex);
    double getSelectedPrice();

    char getSelectedSizeAsChar();
    QString getSelectedPaymentMethod();
    

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
    int selectedSize;
    int m_selectedSlot;
    double overruledPrice;
};

#endif // DRINKORDER_H
