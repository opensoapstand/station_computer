#ifndef DRINKORDER_H
#define DRINKORDER_H

#include "df_util.h"

// #define INVALID_PRICE -666

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
    void setPromoCode(QString promoCode);
    QString getSelectedProductId();
    QString getMachineId();

    QString getFullVolumeCorrectUnits(bool addUnits);

    QString getVolumeRemainingCorrectUnits();
    QString getTotalDispensedCorrectUnits();
    QString getVolumeDispensedSinceRestockCorrectUnits();

    double getSelectedVolumeDispensedMl();
    void setSelectedVolumeDispensedMl(double volumeMl);
    void resetSelectedVolumeDispensed();

    void getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter);
    void setFullVolumeCorrectUnits(QString inputFullValue);

    void setSelectedSize(int sizeIndex);
    int getSelectedSize();
    char getSelectedSizeAsChar();

    bool isSelectedOrderValid();

    QString getProductName(int slot);
    QString getProductType(int slot);
    QString getProductDrinkfillSerial(int slot);

    void loadSelectedProductPropertiesFromProductsFile();
    void getProductPropertiesFromProductsFile(QString product_id, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui);

    void loadSelectedProductProperties();
    void loadProductPropertiesFromDb(int slot);
    QString getSelectedProductName();
    QString getLoadedProductName();
    QString getLoadedProductDescription();
    QString getLoadedProductIngredients();
    QString getLoadedProductFeatures();
    bool getLoadedProductSizeEnabled(int size);
    int getLoadedProductBiggestEnabledSizeIndex();

    void setLoadedProductBiggestEnabledSizeIndex();

    QString getProductPicturePath(int slot);
    QString getSelectedProductPicturePath();

    double getSelectedVolume();
    double getVolume(int size);

    void setSizeToVolumeForSelectedSlot(QString volumeInput, int size);
    QString getVolumePerTickAsStringForSelectedSlot();
    double getVolumePerTickForSelectedSlot();
    void setVolumePerTickForSelectedSlot(QString volumePerTickInput);

    double inputTextToMlConvertUnits(QString inputValueAsText);
    QString getUnitsForSelectedSlot();
    QString getSelectedSizeToVolume(QString units);

    QString getSelectedSizeToVolumeWithCorrectUnits(bool round, bool addUnits);
    QString getSizeToVolumeWithCorrectUnitsForSelectedSlot(int size, bool roundValue, bool addUnits);

    double getPrice(int sizeIndex);
    double getDiscount();
    void setPriceSelected(int size, double price);
    double getSelectedPriceCorrected();
    double getSelectedPriceCustom();
    double getSelectedPrice();

    int getSelectedDispenseSpeedPercentage();
    void setSelectedDispenseSpeedPercentage(int percentage);

    QString getSelectedPaymentMethod();

    double getDiscountPercentageFraction();
    QString getPromoCode();
public slots:

    // void setSelectedSlot(int optNumber);
    void setDiscountPercentageFraction(double percentageFraction);
    // void setSize(double size);

signals:
    void orderSlotChange(int newOpt);
    void priceChange(double newPrice);
    void sizeChange(double newSize);

private:
    QString m_ingredients_ui;
    QString m_product_type;

    QString m_features_ui;
    QString m_name_ui;
    QString m_description_ui;
    QString m_product_id;
    bool m_sizeIndexIsEnabled[SIZES_COUNT]; // size indeces.

    DrinkSelection *selectedDrink;
    int selectedSize;
    double selectedDispensedVolumeMl;

    int m_selectedSlot;
    double overruledPrice;
    double m_discount_percentage_fraction;
    QString m_promoCode;
};

#endif // DRINKORDER_H
