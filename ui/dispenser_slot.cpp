#include "df_util.h" // lode added for settings
#include "dbmanager.h"
// #include "page_dispenser.h"
#include "page_idle.h"
#include "machine.h"

// Ctor
dispenser_slot::dispenser_slot()
{
    m_discount_percentage_fraction = 0.0;
    m_promoCode = "";
}

// Ctor Object Copy
dispenser_slot::dispenser_slot(const dispenser_slot &other) : QObject(nullptr)
{
}

// Dtor
dispenser_slot::~dispenser_slot()
{
}

void dispenser_slot::setDb(DbManager *db)
{
    m_db = db;
}

void dispenser_slot::setMachine(machine *machine)
{
    thisMachine = machine;
}

void dispenser_slot::loadProductProperties()
{
    qDebug() << "Load dispenser_slot properties from db and csv";
    loadProductPropertiesFromDb();
    thisMachine->loadProductPropertiesFromProductsFile(getProductDrinkfillSerial(),
                                                       &m_name,
                                                       &m_name_ui,
                                                       &m_product_type,
                                                       &m_description_ui,
                                                       &m_features_ui,
                                                       &m_ingredients_ui);
}

void dispenser_slot::loadProductPropertiesFromDb()
{
    qDebug() << "Open db: db load dispenser_slot properties for dispenser_slot in slot: " << getSlot();
    m_db->getAllProductProperties(getSlot(), &m_aws_product_id,
                                  &m_soapstand_product_serial,
                                  &m_size_unit,
                                  &m_currency,
                                  &m_payment,
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
                                  &m_price_custom_discount, m_sizeIndexIsEnabled, m_sizeIndexPrices, m_sizeIndexVolumes, m_sizeIndexPLUs, m_sizeIndexPIDs);
}

char dispenser_slot::getSelectedSizeAsChar()
{
    // ! = invalid.
    // t  test to fsm, but should become c for custom. we're so ready for it.
    // t
    return df_util::sizeIndexToChar(m_selected_size);
}

int dispenser_slot::getBiggestEnabledSizeIndex()
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

// void dispenser_slot::setSlotEnabled(bool isEnabled)
// {
//     thisMachine->setSlotEnabled(getSlot(), isEnabled);
// }

// void dispenser_slot::setSlotEnabled(bool isEnabled, QString statusText)
// {
//     thisMachine->setSlotEnabled(getSlot(), isEnabled);
//     setStatusText(statusText);
// }

// bool dispenser_slot::getSlotEnabled()
// {
//     return thisMachine->getSlotEnabled(getSlot());
// }

// void dispenser_slot::setStatusText(QString status)
// {
//     thisMachine->setStatusText(getSlot(), getSlotEnabled(), status);
// }

// QString dispenser_slot::getStatusText()
// {
//     return thisMachine->getStatusText(getSlot());
// }

bool dispenser_slot::toggleSizeEnabled(int size)
{
    setSizeEnabled(size, !(getSizeEnabled(size)));
}

bool dispenser_slot::setSizeEnabled(int size, bool enabled)
{
    QString sizeIndexToText[6] = {"Invalid", "small", "medium", "large", "custom", "test"};
    // m_sizeIndexIsEnabled[size] = enabled;
    QString column_name = QString("is_enabled_%1").arg(sizeIndexToText[size]);
    m_db->updateTableProductsWithInt(getSlot(), column_name, enabled);
}

bool dispenser_slot::getSizeEnabled(int size)
{
    // caution!:  provide size index (0=small, ...)
    qDebug() << "Size enabled? for: " << size << "enabled? : " << m_sizeIndexIsEnabled[size];
    return m_sizeIndexIsEnabled[size];
}

int dispenser_slot::getSlot()
{
    return m_dispenser_slot;
}

void dispenser_slot::setSlot(int slot)
{
    if (slot == 0)
    {
        qDebug() << "ERROR: 0 provided.  Slots start counting from 1. Not zero.";
    }
    if (slot > MAX_SLOT_COUNT)
    {
        qDebug() << "ERROR: Slot exceeds max slot count." << slot;
    }
    m_dispenser_slot = slot;
}

int dispenser_slot::getSelectedSize()
{
    // e.g. SIZE_SMALL_INDEX
    return m_selected_size;
}

void dispenser_slot::setSelectedSize(int sizeIndex)
{
    qDebug() << "Set size index: " << sizeIndex;
    m_selected_size = sizeIndex;
}

void dispenser_slot::setBiggestEnabledSizeIndex()
{
    setSelectedSize(getBiggestEnabledSizeIndex());
}

// SLOTS Section
bool dispenser_slot::is_valid_size_selected()
{
    // if (!(m_dispenser_slot >= OPTION_SLOT_INVALID && m_dispenser_slot <= thisMachine->getSlotCount()))
    // {
    qInfo() << "ERROR: no slot set. " << m_dispenser_slot;
    return false;
    // }
    if (!(m_selected_size >= 0 && m_selected_size <= SIZES_COUNT))
    {
        qInfo() << "ERROR: no size set. " << m_selected_size;
        return false;
    }
    return true;
}

QString dispenser_slot::getLastRestockDate()
{

    return m_lastRestockDate;
}

void dispenser_slot::setPrice(int size, double price)
{
    QString price_columns[5] = {"size_error", "price_small", "price_medium", "price_large", "price_custom"};
    QString column_name = price_columns[size];

    qDebug() << "Open db: set p roduct price";

    m_db->updateTableProductsWithDouble(getSlot(), column_name, price, 2);
}

double dispenser_slot::getBasePrice(int sizeIndex)
{
    return m_sizeIndexPrices[sizeIndex];
}

double dispenser_slot::getBasePrice()
{
    return getBasePrice(getSelectedSize());
}

double dispenser_slot::getPriceCorrected()
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

double dispenser_slot::getPriceCustom()
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

void dispenser_slot::resetVolumeDispensed()
{
    this->DispensedVolumeMl = 0;
}

double dispenser_slot::getVolumeDispensedMl()
{
    // volume for this dispense
    return this->DispensedVolumeMl;
}

void dispenser_slot::setVolumeDispensedMl(double volumeMl)
{
    // volume for this dispense
    this->DispensedVolumeMl = volumeMl;
}

double dispenser_slot::getRestockVolume()
{
    return m_volume_full;
}

double dispenser_slot::getVolumeBySize(int size)
{

    return m_sizeIndexVolumes[size];
}

double dispenser_slot::getVolumeOfSelectedSize()
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

QString dispenser_slot::getUnitsForSlot()
{

    return m_size_unit;
}

QString dispenser_slot::getVolumePerTickAsStringForSlot()
{
    double vol_per_tick = getVolumePerTickForSlot();
    QString units = getUnitsForSlot();

    return df_util::getConvertedStringVolumeFromMl(vol_per_tick, units, false, true);
}

double dispenser_slot::getVolumePerTickForSlot()
{
    return m_volume_per_tick;
}

void dispenser_slot::setVolumePerTickForSlot(QString volumePerTickInput)
{
    double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
    qInfo() << "Open db: set vol per tick";
    m_db->updateTableProductsWithDouble(getSlot(), "volume_per_tick", ml_per_tick, 2);
}

void dispenser_slot::configureVolumeToSizeForSlot(QString volumeInput, int size)
{
    double volume = inputTextToMlConvertUnits(volumeInput);
    QString volume_columns[5] = {"invalid_size", "size_small", "size_medium", "size_large", "size_custom_max"};
    QString column_name = volume_columns[size];

    qInfo() << "Open db: size to volume";
    m_db->updateTableProductsWithDouble(getSlot(), column_name, volume, 2);
}

void dispenser_slot::setVolumeRemainingUserInput(QString volumeRemainingAsUserText)
{
    double vol_as_ml = inputTextToMlConvertUnits(volumeRemainingAsUserText);
    setVolumeRemaining(vol_as_ml);
}

bool dispenser_slot::restock()
{
    qDebug() << "Open db: restock";

    bool success = true;
    success &= m_db->updateTableProductsWithDouble(getSlot(), "volume_remaining", m_volume_full, 0);
    success &= m_db->updateTableProductsWithDouble(getSlot(), "volume_dispensed_since_restock", 0, 0);
    success &= m_db->updateTableProductsWithText(getSlot(), "last_restock", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return success;
}

void dispenser_slot::setVolumeRemaining(double volume_as_ml)
{
    qDebug() << "Open db: set volume remaining";
    m_db->updateTableProductsWithDouble(getSlot(), "volume_remaining", volume_as_ml, 0);
}

QString dispenser_slot::getVolumeRemainingCorrectUnits(bool addUnits)
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_remaining, units, false, addUnits);

    return volume_as_string;
}

QString dispenser_slot::getVolumeDispensedSinceRestockCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_since_restock, units, false, true);
    return volume_as_string;
}

QString dispenser_slot::getTotalDispensedCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_total, units, false, true);
    return volume_as_string;
}

QString dispenser_slot::getSizeAsVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits)
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

double dispenser_slot::inputTextToMlConvertUnits(QString inputValueAsText)
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

QString dispenser_slot::getProductDrinkfillSerial()
{
    return m_soapstand_product_serial;
}

bool dispenser_slot::isProductVolumeInContainer()
{

    bool retval = true;

    if (!thisMachine->getEmptyContainerDetectionEnabled())
    {
        retval = m_volume_remaining > CONTAINER_EMPTY_THRESHOLD_ML;
    }
    return retval;
}

void dispenser_slot::getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter)
{
    *large_volume_discount_is_enabled = m_is_enabled_custom_discount;
    *min_volume_for_discount = m_size_custom_discount;
    *discount_price_per_liter = m_price_custom_discount;
}

QString dispenser_slot::getProductIngredients()
{
    return m_ingredients_ui;
}

QString dispenser_slot::getProductFeatures()
{
    return m_features_ui;
}
QString dispenser_slot::getProductName()
{
    return m_name_ui;
}
QString dispenser_slot::getProductDescription()
{
    return m_description_ui;
}

QString dispenser_slot::getProductType()
{

    return m_product_type;
}

QString dispenser_slot::getProductPicturePath()
{
    return QString(PRODUCT_PICTURES_ROOT_PATH).arg(m_soapstand_product_serial);
}

QString dispenser_slot::getPlu(int sizeIndex)
{
    return m_sizeIndexPLUs[sizeIndex];
}

void dispenser_slot::setPlu(int sizeIndex, QString plu)
{
    QString plu_columns[5] = {"Invalid size", "plu_small", "plu_medium", "plu_large", "plu_custom"};
    QString column_name = plu_columns[sizeIndex];

    qDebug() << "Open db: Set plu";
    bool success = m_db->updateTableProductsWithText(getSlot(), column_name, plu);
}

QString dispenser_slot::getAwsProductId()
{
    return m_aws_product_id;
}

QString dispenser_slot::getFullVolumeCorrectUnits(bool addUnits)
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_full, units, false, addUnits);
    return volume_as_string;
}

void dispenser_slot::setFullVolumeCorrectUnits(QString inputFullValue)
{
    qDebug() << "Open db: for write full vol";
    double full_volume = inputTextToMlConvertUnits(inputFullValue);
    m_db->updateTableProductsWithDouble(getSlot(), "volume_full", full_volume, 0);
}

QString dispenser_slot::getSizeAsVolumeWithCorrectUnits(bool round, bool addUnits)
{
    return getSizeAsVolumeWithCorrectUnits(getSelectedSize(), round, addUnits);
}

void dispenser_slot::setPaymentMethod(QString paymentMethod)
{
    qDebug() << "Open db: set payment method";
    m_db->updateTableProductsWithText(getSlot(), "payment", paymentMethod);
}

QString dispenser_slot::getPaymentMethod()
{
    return m_payment;
}

int dispenser_slot::getDispenseSpeedPercentage()
{
    return (int)round((double(m_dispense_speed) * 100) / 255);
}

void dispenser_slot::setDispenseSpeedPercentage(int percentage)
{
    int pwm = (int)round((percentage * 255) / 100);

    qInfo() << "Open db: pwm speed set";
    m_db->updateTableProductsWithInt(getSlot(), "dispense_speed", pwm);
}
