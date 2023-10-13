#ifndef PRODUCT_H
#define PRODUCT_H

#include "df_util.h"
#include "machine.h"
#include "dbmanager.h"

class machine; // forward declaration.

class product : public QObject
{
    Q_OBJECT

public:
    product();
    product(const product &other);
    ~product();
    product &operator=(const product &other);
    void setMachine(machine *machine);
    void setDb(DbManager* db);

    // Setters and Getters
    void setSlot(int slot);
    int getSlot();
    void setPromoCode(QString promoCode);
    QString getAwsProductId();
    // bool getSlotEnabled();
    // void setSlotEnabled(bool isEnabled);
    // void setSlotEnabled(bool isEnabled, QString statusText);
    // QString getStatusText();
    // void setStatusText(QString status);

    QString getFullVolumeCorrectUnits(bool addUnits);

    void setVolumeRemainingUserInput(QString volumeRemainingAsUserText);
    void setVolumeRemaining(double volume_as_ml);

    bool restock();

    QString getVolumeRemainingCorrectUnits(bool addUnits);
    QString getTotalDispensedCorrectUnits();
    QString getVolumeDispensedSinceRestockCorrectUnits();

    double getVolumeDispensedMl();
    void setVolumeDispensedMl(double volumeMl);
    void resetVolumeDispensed();

    void setFullVolumeCorrectUnits(QString inputFullValue);
    void getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter);

    QString getPlu(int sizeIndex);
    void setPlu(int sizeIndex, QString plu);
    bool is_valid_size_selected();

    QString getProductName();
    QString getProductType();
    QString getProductDrinkfillSerial();

    void loadProductProperties();
    void loadProductPropertiesFromDb();
    bool isProductVolumeInContainer();
    QString getProductDescription();
    QString getProductIngredients();
    QString getProductFeatures();
    QString getLastRestockDate();

    
    void setSize(int sizeIndex);
    int getSize();
    char getSizeAsChar();
    
    bool toggleSizeEnabled(int size);
    bool getSizeEnabled(int size);
    bool setSizeEnabled(int size, bool enabled);
    int getBiggestEnabledSizeIndex();
    void setBiggestEnabledSizeIndex();

    QString getProductPicturePath();

    double getVolumeOfSelectedSize();
    double getVolumeBySize(int size);

    double getRestockVolume();

    void setSizeToVolumeForSlot(QString volumeInput, int size);
    QString getVolumePerTickAsStringForSlot();
    double getVolumePerTickForSlot();
    void setVolumePerTickForSlot(QString volumePerTickInput);

    double inputTextToMlConvertUnits(QString inputValueAsText);

    QString getUnitsForSlot();
    QString getSizeToVolume(QString units);

    QString getSizeToVolumeWithCorrectUnits(bool round, bool addUnits);
    QString getSizeToVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits);

    double getBasePrice(int sizeIndex);

    void setPrice(int size, double price);
    double getPriceCorrected();
    double getPriceCustom();

    double getBasePrice();

    int getDispenseSpeedPercentage();
    void setDispenseSpeedPercentage(int percentage);

    QString getPaymentMethod();
    void setPaymentMethod(QString paymentMethod);

    // double getDiscountPercentageFraction();
    // QString getPromoCode();

public slots:
    // void setDiscountPercentageFraction(double percentageFraction);

signals:
    void orderSlotChange(int newOpt);
    void priceChange(double newPrice);
    void sizeChange(double newSize);

private:
    machine *thisMachine;
    DbManager* m_db;

    int slot;

    QString m_size_unit;
    QString m_productId;
    QString m_soapstand_product_serial;
    QString m_payment;

    QString m_ingredients_ui;
    QString m_product_type;

    QString m_name;
    QString m_name_ui;
    QString m_features_ui;
    QString m_description_ui;
    QString m_aws_product_id;
    bool m_empty_container_detection_enabled;
    bool m_slot_enabled;
    QString m_lastRestockDate;
    QString m_currency;
    QString m_name_receipt;
    int m_concentrate_multiplier;
    int m_dispense_speed;
    double m_threshold_flow;
    int m_retraction_time;
    double m_calibration_const;
    double m_volume_per_tick;
    QString m_last_restock;
    double m_volume_full;
    double m_volume_remaining;
    double m_volume_dispensed_since_restock;
    double m_volume_dispensed_total;
    int m_is_enabled_custom_discount;
    double m_size_custom_discount;
    double m_price_custom_discount;

    bool m_sizeIndexIsEnabled[SIZES_COUNT]; // size indeces.
    QString m_sizeIndexPLUs[SIZES_COUNT];
    QString m_sizeIndexPIDs[SIZES_COUNT];
    double m_sizeIndexPrices[SIZES_COUNT];
    double m_sizeIndexVolumes[SIZES_COUNT];

    int m_selected_size;
    double DispensedVolumeMl;

    int m_dispenser_slot;
    double overruledPrice;
    double m_discount_percentage_fraction;
    QString m_promoCode;
};

#endif // PRODUCT_H
