#include "pnumberproduct.h"

#include "df_util.h" // lode added for settings
#include "dbmanager.h"

pnumberproduct::pnumberproduct()
{

}

void pnumberproduct::setDb(DbManager *db)
{
    m_db = db;
}

void pnumberproduct::loadProductProperties()
{
    qDebug() << "Load pnumberproduct properties from db and csv";
    loadProductPropertiesFromDb();
    thisMachine->loadProductPropertiesFromProductsFile(getProductDrinkfillSerial(),
                                                       &m_name,
                                                       &m_name_ui,
                                                       &m_product_type,
                                                       &m_description_ui,
                                                       &m_features_ui,
                                                       &m_ingredients_ui);
}

void pnumberproduct::setPNumber(int PNumber){
    m_PNumber = PNumber;

}

int pnumberproduct::getPNumber(){
    return m_PNumber;
}

int pnumberproduct::getBasePNumber(){
    return m_basePNumber;
}

QVector<int> pnumberproduct::getAdditivesPNumbers(){
    return m_additivesPNumbers;
}
    
    
void pnumberproduct::loadProductPropertiesFromDb()
{
    qDebug() << "Open db: db load pnumberproduct properties for pnumberproduct in slot: " << getPNumber();
    m_db->getAllProductProperties(getPNumber(), &m_aws_product_id,
                                  &m_soapstand_product_serial,
                                  &m_additivesPNumbers,
                                  &m_additivesRatios,
                                  &m_size_unit,
                                  &m_currency_dummy_deprecated,
                                  &m_payment_deprecated,
                                  &m_name_receipt,
                                  &m_concentrate_multiplier,
                                  &m_dispense_speed,
                                  &m_threshold_flow,
                                  &m_retraction_time,
                                  &m_calibration_const,
                                  &m_volume_per_tick,
                                  &m_lastRestockDate,
                                  &m_volume_full,
                                  &m_volume_remaining,
                                  &m_volume_dispensed_since_restock,
                                  &m_volume_dispensed_total,
                                  &m_is_enabled_custom_discount,
                                  &m_size_custom_discount,
                                  &m_price_custom_discount, 
                                  m_sizeIndexIsEnabled, m_sizeIndexPrices, m_sizeIndexVolumes, m_sizeIndexPLUs, m_sizeIndexPIDs);
}

///////////////////////////////////////////// SIZE 
/////////////////////////////////////////////
/////////////////////////////////////////////

bool pnumberproduct::toggleSizeEnabled(int size)
{
    setSizeEnabled(size, !(getSizeEnabled(size)));
}

bool pnumberproduct::setSizeEnabled(int size, bool enabled)
{
    QString sizeIndexToText[6] = {"Invalid", "small", "medium", "large", "custom", "test"};
    // m_sizeIndexIsEnabled[size] = enabled;
    QString column_name = QString("is_enabled_%1").arg(sizeIndexToText[size]);
    m_db->updateTableProductsWithInt(getPNumber(), column_name, enabled);
}

bool pnumberproduct::getSizeEnabled(int size)
{
    // caution!:  provide size index (0=small, ...)
    qDebug() << "Size enabled? for: " << size << "enabled? : " << m_sizeIndexIsEnabled[size];
    return m_sizeIndexIsEnabled[size];
}
char pnumberproduct::getSelectedSizeAsChar()
{
    // ! = invalid.
    // t  test to fsm, but should become c for custom. we're so ready for it.
    // t
    return df_util::sizeIndexToChar(m_selected_size);
}

int pnumberproduct::getBiggestEnabledSizeIndex()
{

    // cascade to largest size. Custom volume is seen as superior.
    int maxSize = SIZE_SMALL_INDEX;
    if (getSizeEnabled(SIZE_MEDIUM_INDEX))
    {
        maxSize = SIZE_MEDIUM_INDEX;
    }
    if (getSizeEnabled(SIZE_LARGE_INDEX))
    {
        maxSize = SIZE_LARGE_INDEX;
    }
    if (getSizeEnabled(SIZE_CUSTOM_INDEX))
    {
        maxSize = SIZE_CUSTOM_INDEX;
    }
    return maxSize;
}


int pnumberproduct::getSelectedSize()
{
    // e.g. SIZE_SMALL_INDEX
    return m_selected_size;
}

void pnumberproduct::setSelectedSize(int sizeIndex)
{
    qDebug() << "Set size index: " << sizeIndex;
    m_selected_size = sizeIndex;
}

void pnumberproduct::setBiggestEnabledSizeIndex()
{
    setSelectedSize(getBiggestEnabledSizeIndex());
}

// SLOTS Section
bool pnumberproduct::is_valid_size_selected()
{
    // if (!(m_pnumberproduct >= OPTION_SLOT_INVALID && m_pnumberproduct <= thisMachine->getPNumberCount()))
    // {
    // qInfo() << "ERROR: no slot set. " << m_pnumberproduct;
    // return false;
    // }
    if (!(m_selected_size >= 0 && m_selected_size <= SIZES_COUNT))
    {
        qInfo() << "ERROR: no size set. " << m_selected_size;
        return false;
    }
    return true;
}

double pnumberproduct::getVolumeOfSelectedSize()
{
    double volume;
    if (is_valid_size_selected())
    {
        volume = getVolumeBySize(getSelectedSize());
    }
    else
    {
        qInfo() << "ERROR: size set";
        volume = 66.6;
    }
    return volume;
}

double pnumberproduct::getVolumeBySize(int size)
{

    return m_sizeIndexVolumes[size];
}



///////////////////////////////////////////// PRICE 
/////////////////////////////////////////////
/////////////////////////////////////////////


void pnumberproduct::setPrice(int size, double price)
{
    QString price_columns[5] = {"size_error", "price_small", "price_medium", "price_large", "price_custom"};
    QString column_name = price_columns[size];

    qDebug() << "Open db: set p roduct price";

    m_db->updateTableProductsWithDouble(getPNumber(), column_name, price, 2);
}

double pnumberproduct::getBasePrice(int sizeIndex)
{
    return m_sizeIndexPrices[sizeIndex];
}

double pnumberproduct::getBasePrice()
{
    return getBasePrice(getSelectedSize());
}

double pnumberproduct::getPriceCorrected()
{
    double price;
    if (is_valid_size_selected())
    {
        price = getBasePrice(getSelectedSize()) * (1.0 - thisMachine->getDiscountPercentageFraction());
    }
    else
    {

        qInfo() << "ERROR: no size set";
        price = 66.6;
    }
    return price;
}

double pnumberproduct::getPriceCustom()
{
    double price;
    if (is_valid_size_selected())
    {
        price = getBasePrice(getSelectedSize()) * (1.0 - m_discount_percentage_fraction) * getVolumeOfSelectedSize();
    }
    else
    {
        qInfo() << "ERROR: no size set";
        price = 66.6;
    }
    return price;
}


///////////////////////////////////////////// DISPENSE 
/////////////////////////////////////////////
/////////////////////////////////////////////



void pnumberproduct::resetVolumeDispensed()
{
    this->DispensedVolumeMl = 0;
}

double pnumberproduct::getVolumeDispensedMl()
{
    // volume for this dispense
    return this->DispensedVolumeMl;
}

void pnumberproduct::setVolumeDispensedMl(double volumeMl)
{
    // volume for this dispense
    this->DispensedVolumeMl = volumeMl;
}



double pnumberproduct::inputTextToMlConvertUnits(QString inputValueAsText)
{
    if (getUnitsForSlot() == "oz")
    {
        return df_util::convertOzToMl(inputValueAsText.toDouble());
    }
    else
    {
        return inputValueAsText.toDouble();
    }
}




///////////////////////////////////////////// GENERAL 
/////////////////////////////////////////////
/////////////////////////////////////////////




void pnumberproduct::getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter)
{
    *large_volume_discount_is_enabled = m_is_enabled_custom_discount;
    *min_volume_for_discount = m_size_custom_discount;
    *discount_price_per_liter = m_price_custom_discount;
}

QString pnumberproduct::getLastRestockDate()
{

    return m_lastRestockDate;
}

double pnumberproduct::getRestockVolume()
{
    return m_volume_full;
}

QString pnumberproduct::getUnitsForSlot()
{

    return m_size_unit;
}

QString pnumberproduct::getVolumePerTickAsStringForSlot()
{
    double vol_per_tick = getVolumePerTickForSlot();
    QString units = getUnitsForSlot();

    return df_util::getConvertedStringVolumeFromMl(vol_per_tick, units, false, true);
}

double pnumberproduct::getVolumePerTickForSlot()
{
    return m_volume_per_tick;
}

void pnumberproduct::setVolumePerTickForSlot(QString volumePerTickInput)
{
    double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
    qInfo() << "Open db: set vol per tick";
    m_db->updateTableProductsWithDouble(getPNumber(), "volume_per_tick", ml_per_tick, 2);
}

void pnumberproduct::configureVolumeToSizeForSlot(QString volumeInput, int size)
{
    double volume = inputTextToMlConvertUnits(volumeInput);
    QString volume_columns[5] = {"invalid_size", "size_small", "size_medium", "size_large", "size_custom_max"};
    QString column_name = volume_columns[size];

    qInfo() << "Open db: size to volume";
    m_db->updateTableProductsWithDouble(getPNumber(), column_name, volume, 2);
}

void pnumberproduct::setVolumeRemainingUserInput(QString volumeRemainingAsUserText)
{
    double vol_as_ml = inputTextToMlConvertUnits(volumeRemainingAsUserText);
    setVolumeRemaining(vol_as_ml);
}

bool pnumberproduct::restock()
{
    qDebug() << "Open db: restock";

    bool success = true;
    success &= m_db->updateTableProductsWithDouble(getPNumber(), "volume_remaining", m_volume_full, 0);
    success &= m_db->updateTableProductsWithDouble(getPNumber(), "volume_dispensed_since_restock", 0, 0);
    success &= m_db->updateTableProductsWithText(getPNumber(), "last_restock", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return success;
}

void pnumberproduct::setVolumeRemaining(double volume_as_ml)
{
    qDebug() << "Open db: set volume remaining";
    m_db->updateTableProductsWithDouble(getPNumber(), "volume_remaining", volume_as_ml, 0);
}

QString pnumberproduct::getVolumeRemainingCorrectUnits(bool addUnits)
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_remaining, units, false, addUnits);

    return volume_as_string;
}

QString pnumberproduct::getVolumeDispensedSinceRestockCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_since_restock, units, false, true);
    return volume_as_string;
}

QString pnumberproduct::getTotalDispensedCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_total, units, false, true);
    return volume_as_string;
}

QString pnumberproduct::getSizeAsVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits)
{
    QString volume_as_string;
    double v;
    double volume_oz;
    QString units;

    v = getVolumeBySize(size);
    units = getUnitsForSlot();
    volume_as_string = df_util::getConvertedStringVolumeFromMl(v, units, roundValue, addUnits);
    return volume_as_string;
}


QString pnumberproduct::getProductDrinkfillSerial()
{
    return m_soapstand_product_serial;
}

bool pnumberproduct::isProductVolumeInContainer()
{

    bool retval = true;

    if (!thisMachine->getEmptyContainerDetectionEnabled())
    {
        retval = m_volume_remaining > CONTAINER_EMPTY_THRESHOLD_ML;
    }
    return retval;
}



QString pnumberproduct::getProductIngredients()
{
    return m_ingredients_ui;
}

QString pnumberproduct::getProductFeatures()
{
    return m_features_ui;
}
QString pnumberproduct::getProductName()
{
    return m_name_ui;
}
QString pnumberproduct::getProductDescription()
{
    return m_description_ui;
}

QString pnumberproduct::getProductType()
{

    return m_product_type;
}

QString pnumberproduct::getProductPicturePath()
{
    return QString(PRODUCT_PICTURES_ROOT_PATH).arg(m_soapstand_product_serial);
}

QString pnumberproduct::getPlu(int sizeIndex)
{
    return m_sizeIndexPLUs[sizeIndex];
}

void pnumberproduct::setPlu(int sizeIndex, QString plu)
{
    QString plu_columns[5] = {"Invalid size", "plu_small", "plu_medium", "plu_large", "plu_custom"};
    QString column_name = plu_columns[sizeIndex];

    qDebug() << "Open db: Set plu";
    bool success = m_db->updateTableProductsWithText(getPNumber(), column_name, plu);
}

QString pnumberproduct::getAwsProductId()
{
    return m_aws_product_id;
}

QString pnumberproduct::getFullVolumeCorrectUnits(bool addUnits)
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_full, units, false, addUnits);
    return volume_as_string;
}

void pnumberproduct::setFullVolumeCorrectUnits(QString inputFullValue)
{
    qDebug() << "Open db: for write full vol";
    double full_volume = inputTextToMlConvertUnits(inputFullValue);
    m_db->updateTableProductsWithDouble(getPNumber(), "volume_full", full_volume, 0);
}

QString pnumberproduct::getSizeAsVolumeWithCorrectUnits(bool round, bool addUnits)
{
    return getSizeAsVolumeWithCorrectUnits(getSelectedSize(), round, addUnits);
}

int pnumberproduct::getDispenseSpeedPercentage()
{
    return (int)round((double(m_dispense_speed) * 100) / 255);
}

void pnumberproduct::setDispenseSpeedPercentage(int percentage)
{
    int pwm = (int)round((percentage * 255) / 100);

    qInfo() << "Open db: pwm speed set";
    m_db->updateTableProductsWithInt(getPNumber(), "dispense_speed", pwm);
}
