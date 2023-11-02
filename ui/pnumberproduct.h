#ifndef PNUMBERPRODUCT_H
#define PNUMBERPRODUCT_H

#include "df_util.h"
#include "dbmanager.h"

class pnumberproduct
{
public:
    pnumberproduct();

    void setDb(DbManager *db);
    DbManager *m_db;

    void setPNumber(int PNumber);
    int getPNumber();

    int getFirstMixPNumberOrPNumberAsBasePNumber();
    QVector<int> getMixPNumbers();

    void loadProductPropertiesFromProductsFile();
    void getProductProperties(QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui);

    void loadProductPropertiesFromDb(); // productt
    void loadProductProperties();       // productt

    QString convertPNumberToPNotation(int pnumber);
    int convertPNotationToPNumber(QString PNumberNotation);

    double inputTextToMlConvertUnits(QString inputValueAsText);

    QString getAwsProductId(); // this is a combo of dispenser AND product.

    QString getFullVolumeCorrectUnits(bool addUnits);

    void setVolumeRemainingUserInput(QString volumeRemainingAsUserText); // productt
    void setVolumeRemaining(double volume_as_ml);                        // productt
    bool restock();                                                      // productt

    QString getVolumeRemainingCorrectUnits(bool addUnits); // productt
    QString getTotalDispensedCorrectUnits();               // productt
    QString getVolumeDispensedSinceRestockCorrectUnits();  // productt

    double getVolumeDispensedMl();              // productt
    void setVolumeDispensedMl(double volumeMl); // productt
    void resetVolumeDispensed();                // productt

    void setFullVolumeCorrectUnits(QString inputFullValue); // productt

    QString getPlu(int sizeIndex);           // productt
    void setPlu(int sizeIndex, QString plu); // productt
    bool is_valid_size_selected();           // productt

    QString getProductName();            // productt
    QString getProductType();            // productt
    QString getProductDrinkfillSerial(); // productt


    QString getProductDescription();   // productt
    QString getProductIngredients();   // productt
    QString getProductFeatures();      // productt
    QString getLastRestockDate();      // productt

    void setSelectedSize(int sizeIndex); // productt
    int getSelectedSize();               // productt
    char getSelectedSizeAsChar();        // productt

    bool toggleSizeEnabled(int size);            // productt
    bool getSizeEnabled(int size);               // productt
    bool setSizeEnabled(int size, bool enabled); // productt
    int getBiggestEnabledSizeIndex();            // productt
    void setBiggestEnabledSizeIndex();           // productt

    QString getProductPicturePath();

    double getVolumeOfSelectedSize(); // productt
    double getVolumeBySize(int size); // productt

    double getRestockVolume(); // productt

    void configureVolumeToSizeForSlot(QString volumeInput, int size); // productt
    QString getVolumePerTickAsStringForSlot();                        // productt
    double getVolumePerTickForSlot();                                 // productt
    void setVolumePerTickForSlot(QString volumePerTickInput);         // productt

    QString getUnitsForSlot();              // productt
    QString getSizeAsVolume(QString units); // productt

    QString getSizeAsVolumeWithCorrectUnits(bool round, bool addUnits);                // productt
    QString getSizeAsVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits); // productt

    double getBasePrice(int sizeIndex);                                                                                                       // productt
    void setPrice(int size, double price);                                                                                                    // productt
                                                                                                       // productt
    double getPriceCustom();                                                                                                                  // productt
    double getBasePriceSelectedSize();                                                                                                                    // productt
    void getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter); // productt

    int getDispenseSpeedPercentage();                // productt
    void setDispenseSpeedPercentage(int percentage); // productt

    QString getPaymentMethod();                   // ---> machine?! productt
    void setPaymentMethod(QString paymentMethod); //  ---> machine? or productt

    double getVolumeRemaining();


private:
    int m_PNumber;
    int m_basePNumber;
    bool m_isBaseProduct; // 100%, no additives.
    QVector<int> m_mixPNumbers;
    QVector<double> m_mixRatios;

    int m_selected_size;
    double DispensedVolumeMl;

    QString m_size_unit;
    // QString m_productId;
    QString m_soapstand_product_serial;
    QString m_ingredients_ui;
    QString m_product_type;

    QString m_name;
    QString m_name_ui;
    QString m_features_ui;
    QString m_description_ui;
    QString m_aws_product_id;
    // bool m_empty_container_detection_enabled;
    // bool m_slot_enabled;
    QString m_lastRestockDate;
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
    double m_size_custom_discount;
    double m_price_custom_discount;
    int m_is_enabled_custom_discount;

    QString m_payment_deprecated;  // placeholders
    QString m_currency_deprecated; // placeholders

    bool m_sizeIndexIsEnabled[SIZES_COUNT]; // size indeces.
    QString m_sizeIndexPLUs[SIZES_COUNT];
    QString m_sizeIndexPIDs[SIZES_COUNT];
    double m_sizeIndexPrices[SIZES_COUNT];
    double m_sizeIndexVolumes[SIZES_COUNT];
};

#endif // PNUMBERPRODUCT_H
