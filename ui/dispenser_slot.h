#ifndef PRODUCT_H
#define PRODUCT_H

#include "df_util.h"
#include "machine.h"
#include "dbmanager.h"

class machine; // forward declaration.

class dispenser_slot : public QObject
{
    Q_OBJECT

public:
    dispenser_slot();
    dispenser_slot(const dispenser_slot &other);
    ~dispenser_slot();
    dispenser_slot &operator=(const dispenser_slot &other);
    void setMachine(machine *machine);
    void setDb(DbManager* db);

    // Setters and Getters
    void setSlot(int slot);
    int getSlot();

    // void setCouponCode(QString promoCode);
    QString getAwsProductId();
    // bool getSlotEnabled();
    // void setSlotEnabled(bool isEnabled);
    // void setSlotEnabled(bool isEnabled, QString statusText);
    // QString getStatusText();
    // void setStatusText(QString status);

    QString getFullVolumeCorrectUnits(bool addUnits);

    void setVolumeRemainingUserInput(QString volumeRemainingAsUserText); //productt
    void setVolumeRemaining(double volume_as_ml); //productt

    bool restock(); //productt

    QString getVolumeRemainingCorrectUnits(bool addUnits); //productt
    QString getTotalDispensedCorrectUnits(); //productt
    QString getVolumeDispensedSinceRestockCorrectUnits(); //productt

    double getVolumeDispensedMl(); //productt
    void setVolumeDispensedMl(double volumeMl); //productt
    void resetVolumeDispensed(); //productt

    void setFullVolumeCorrectUnits(QString inputFullValue); //productt
    void getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter); //productt

    QString getPlu(int sizeIndex); //productt
    void setPlu(int sizeIndex, QString plu); //productt
    bool is_valid_size_selected(); //productt

    QString getProductName(); //productt
    QString getProductType(); //productt
    QString getProductDrinkfillSerial(); //productt

    void loadProductProperties(); //productt
    void loadProductPropertiesFromDb(); //productt
    bool isProductVolumeInContainer(); //productt
    QString getProductDescription(); //productt
    QString getProductIngredients(); //productt
    QString getProductFeatures(); //productt
    QString getLastRestockDate(); //productt

    
    void setSelectedSize(int sizeIndex); //productt
    int getSelectedSize(); //productt
    char getSelectedSizeAsChar(); //productt
    
    bool toggleSizeEnabled(int size); //productt
    bool getSizeEnabled(int size); //productt
    bool setSizeEnabled(int size, bool enabled); //productt
    int getBiggestEnabledSizeIndex(); //productt
    void setBiggestEnabledSizeIndex(); //productt

    QString getProductPicturePath();

    double getVolumeOfSelectedSize(); //productt
    double getVolumeBySize(int size); //productt

    double getRestockVolume(); //productt

    void configureVolumeToSizeForSlot(QString volumeInput, int size); //productt
    QString getVolumePerTickAsStringForSlot(); //productt
    double getVolumePerTickForSlot(); //productt
    void setVolumePerTickForSlot(QString volumePerTickInput); //productt

    double inputTextToMlConvertUnits(QString inputValueAsText); // ----> df_util!

    QString getUnitsForSlot(); //productt
    QString getSizeAsVolume(QString units); //productt

    QString getSizeAsVolumeWithCorrectUnits(bool round, bool addUnits); //productt
    QString getSizeAsVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits); //productt

    double getBasePrice(int sizeIndex); //productt

    void setPrice(int size, double price); //productt
    double getPriceCorrected(); //productt
    double getPriceCustom(); //productt

    double getBasePrice(); //productt

    int getDispenseSpeedPercentage(); //productt
    void setDispenseSpeedPercentage(int percentage); //productt

    QString getPaymentMethod(); // ---> machine?! productt
    void setPaymentMethod(QString paymentMethod); //  ---> machine? or productt

public slots:

signals:
    // void orderSlotChange(int newOpt);
    // void priceChange(double newPrice);
    // void sizeChange(double newSize);

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
