#ifndef PRODUCT_H
#define PRODUCT_H

#include "df_util.h"
// #include "page_idle.h"



class product : public QObject
{
    Q_OBJECT

public:
    product();
    product(const product &other);
    ~product();
    product &operator=(const product &other);

    // HACK: Fixed volume reference; Need to figure out best storage location...
    constexpr static double EMPTY_SIZE_ML = 0.00;

    // Setters and Getters
    void setSlot(int slot);
    int getSlot();
    // void setSelectedSlot(int optionSlot);
    // int getSelectedSlot();
    void setPromoCode(QString promoCode);
    QString getProductId();
    QString getMachineId();
    bool getSlotEnabled();

    QString getFullVolumeCorrectUnits(bool addUnits);

    QString getVolumeRemainingCorrectUnits();
    QString getTotalDispensedCorrectUnits();
    QString getVolumeDispensedSinceRestockCorrectUnits();

    double getVolumeDispensedMl();
    void setVolumeDispensedMl(double volumeMl);
    void resetVolumeDispensed();


    void getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter);
    void setFullVolumeCorrectUnits(QString inputFullValue);

    void setSize(int sizeIndex);
    int getSize();
    char getSizeAsChar();
    QString getPLU(char size);

    bool isOrderValid();

    QString getProductName();
    QString getProductType();
    QString getProductDrinkfillSerial();

    void load();
    // void loadProductPropertiesFromProductsFile();
    // void getProductPropertiesFromProductsFile(QString product_id, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui);
    void loadProductPropertiesFromProductsFile();

    void loadProductProperties();
    void loadProductPropertiesFromDb();
    bool isProductVolumeInContainer();
    // QString getLoadedProductName();
    QString getProductDescription();
    QString getProductIngredients();
    QString getProductFeatures();
    QString getLastRestockDate();
    bool getSizeEnabled(int size);
    int getBiggestEnabledSizeIndex();

    void setBiggestEnabledSizeIndex();

    // QString getProductPicturePath(int slot);
    QString getProductPicturePath();

    double getVolume();
    double getVolume(int size);

    void setSizeToVolumeForSlot(QString volumeInput, int size);
    QString getVolumePerTickAsStringForSlot();
    double getVolumePerTickForSlot();
    void setVolumePerTickForSlot(QString volumePerTickInput);

    double inputTextToMlConvertUnits(QString inputValueAsText);
    QString getUnitsForSlot();
    QString getSizeToVolume(QString units);

    QString getSizeToVolumeWithCorrectUnits(bool round, bool addUnits);
    QString getSizeToVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits);

    double getPrice(int sizeIndex);
    double getDiscount();
    void setPrice(int size, double price);
    double getPriceCorrected();
    double getPriceCustom();
    
    double getPrice();

    int getDispenseSpeedPercentage();
    void setDispenseSpeedPercentage(int percentage);

    QString getPaymentMethod();
    // productSelect *selectedProduct; // Declare selectedProduct as a pointer to productSelect


    double getDiscountPercentageFraction();
    QString getPromoCode();
public slots:
    void setDiscountPercentageFraction(double percentageFraction);

signals:
    void orderSlotChange(int newOpt);
    void priceChange(double newPrice);
    void sizeChange(double newSize);

private:



    QString productId;
    QString soapstand_product_serial;
    int slot;
    QString size_unit;
    QString currency;
    QString payment;
    QString name_receipt;
    int concentrate_multiplier;
    int dispense_speed;
    double threshold_flow;
    int retraction_time;
    double calibration_const;
    double volume_per_tick;
    QString last_restock;
    double volume_full;
    double volume_remaining;
    double volume_dispensed_since_restock;
    double volume_dispensed_toal;

    int is_enabled_small;
    int is_enabled_medium;
    int is_enabled_large;
    int is_enabled_custom;
    double size_small;
    double size_medium;
    double size_large;
    double size_custom_min;
    double size_custom_max;
    double price_small;
    double price_medium;
    double price_large;
    double price_custom;
    QString plu_small;
    QString plu_large;
    QString plu_custom;
    int is_enabled_custom_discount;
    double size_custom_discount;
    double price_custom_discount;

    QString m_ingredients_ui;
    QString m_product_type;

    QString m_name_ui;
    QString m_features_ui;
    QString m_description_ui;
    QString m_product_id;


    bool m_sizeIndexIsEnabled[SIZES_COUNT]; // size indeces.

    int Size;
    double DispensedVolumeMl;

    int m_selectedSlot;
    double overruledPrice;
    double m_discount_percentage_fraction;
    QString m_promoCode;
};

#endif //PRODUCT_H
