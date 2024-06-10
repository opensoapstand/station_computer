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

    void setPNumber(int pnumber);
    int getPNumber();

    int getFirstMixPNumberOrPNumberAsBasePNumber();
    QVector<int> getMixPNumbers();
    QVector<int> getMixPNumbersAlwaysIncludingBase();
    QString getMixPNumbersAsCsv();
    QString getMixRatiosAsCsv(QVector<double> ratios);
    QVector<double> getMixRatiosLow();
    QVector<double> getMixRatiosDefault();
    QVector<double> getMixRatiosHigh();

    bool loadProductPropertiesFromProductsFile();
    void getProductProperties(QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui);

    bool loadProductPropertiesFromDb();
    bool loadProductProperties();

    QString convertPNumberToPNotation(int pnumber);
    int convertPStringToPInt(QString pnumberNotation);

    double inputTextToMlConvertUnits(QString inputValueAsText);

    QString getAwsProductIdSuffix();
    QString getAwsProductId();
    QString getFullVolumeCorrectUnits(bool addUnits);

    bool resetVolumeRemainingUserInput(QString volumeRemainingAsUserText);
    bool resetVolumeRemaining(double volume_as_ml);
    void setVolumeRemaining(double volume_as_ml);
    bool restock();

    QString getVolumeRemainingCorrectUnits(bool addUnits);
    QString getTotalDispensedCorrectUnits();
    QString getVolumeDispensedSinceRestockCorrectUnits();

    double getVolumeDispensedMl();
    void setVolumeDispensedMl(double volumeMl);
    void resetVolumeDispensed();

    void setFullVolumeCorrectUnits(QString inputFullValue);

    QString getPlu(int sizeIndex);
    void setPlu(int sizeIndex, QString plu);
    bool is_valid_size_selected();

    QString getProductName();
    QString getProductType();
    QString getPNumberAsPString();

    QString getProductDescription();
    QString getProductIngredients();
    QString getProductFeatures();
    QString getLastRestockDate();

    void setSelectedSize(int sizeIndex);
    int getSelectedSize();
    char getSelectedSizeAsChar();

    bool toggleSizeEnabled(int size);
    bool getSizeEnabled(int size);
    bool setSizeEnabled(int size, bool enabled);
    int getBiggestEnabledSizeIndex();
    void setBiggestEnabledSizeIndex();

    QString getProductPicturePath();

    double getPriceOfSelectedBottle();
    double getVolumeOfSelectedBottle();
    double getVolumeOfSelectedSize();
    double getVolumeBySize(int size);

    double getRestockVolume();

    void configureVolumeToSizeForSlot(QString volumeInput, int size);
    QString getVolumePerTickAsStringForSlot();
    double getVolumePerTickForSlot();
    void setVolumePerTickForSlot(QString volumePerTickInput);

    void setSizeUnit(QString units);
    QString getSizeUnit();
    // QString getSizeAsVolume(QString units);

    QString getSizeAsVolumeWithCorrectUnits(bool round, bool addUnits);
    QString getSizeAsVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits);

    double getBasePrice(int sizeIndex);
    void setPrice(int size, double price);

    double getPriceCustom();
    double getBasePriceSelectedSize();
    void getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter);

    int getDispenseSpeedPercentage();
    void setDispenseSpeedPercentage(int percentage);

    double getVolumeRemaining();

    bool getIsProductEnabled();
    void setIsProductEnabled(bool isEnabled);

    bool getIsProductEmptyOrHasProblem();
    void setIsProductEmptyOrHasProblem(bool isEmptyOrHasProblem);
    void setEmptyDetectionEnabledPointer(bool *enabled);
    bool getEmptyDetectionEnabled();

    bool* mp_emptyContainerDetectionEnabled;

    // QString getProductStatusText();
    // void setProductStatusText(QString statusText);

    // void setProductState();
    ProductState getProductState();
    QString getProductStateAsString();

    bool isCustomMix();
    // void adjustAdditivesRatioModifier(int index, double additiveModifier);
    QVector<double> getCustomMixRatios();
    double getCustomMixRatios(int index);
    void resetCustomMixRatioParameters();
    void setCustomMixRatios(int index, QString plusOrMinus);
    QString getVolumeDispensedMixedProduct();
    void setVolumeDispensedMixedProduct(QString volumeDispensedMixProduct);

private:
    int m_PNumber;
    int m_basePNumber;
    bool m_isBaseProduct; // 100%, no additives.
    QVector<int> m_mixPNumbers;
    QVector<double> m_mixRatiosLow;
    QVector<double> m_mixRatiosDefault;
    QVector<double> m_mixRatiosHigh;
    QVector<double> m_additivesCustomMixRatioModifiers;
    QVector<double> m_customMixRatios;

    int m_selected_size;
    double DispensedVolumeMl;

    QString m_size_unit;
    // QString m_productId;
    QString m_soapstand_product_serial;
    QString m_ingredients_ui;
    QString m_product_type;

    bool m_is_enabled;
    bool m_is_empty_or_has_problem;
    // QString m_status_text;
    ProductState m_product_state;

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
    double m_threshold_flow_maximum;
    double m_volume_per_tick;
    QString m_last_restock;
    double m_volume_full;
    double m_volume_remaining;
    double m_volume_dispensed_since_restock;
    double m_volume_dispensed_total;
    double m_size_custom_discount;
    double m_price_custom_discount;
    int m_is_enabled_custom_discount;

    // QString m_paymentOptions_deprecated;  // placeholders
    // QString m_currency_deprecated; // placeholders

    bool m_sizeIndexIsEnabled[SIZES_COUNT]; // size indeces.
    QString m_sizeIndexPLUs[SIZES_COUNT];
    QString m_sizeIndexPIDs[SIZES_COUNT];
    double m_sizeIndexPrices[SIZES_COUNT];
    double m_sizeIndexVolumes[SIZES_COUNT];
    QString volumeDispensedMixProduct;
};

#endif // PNUMBERPRODUCT_H
