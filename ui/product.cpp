#include "df_util.h" // lode added for settings
#include "dbmanager.h"
// #include "page_dispenser.h"
#include "page_idle.h"
#include "machine.h"

// Ctor
product::product()
{
    // overruledPrice = INVALID_PRICE;
    m_discount_percentage_fraction = 0.0;
    m_promoCode = "";
}

// Ctor Object Copy
product::product(const product &other) : QObject(nullptr)
{
    // slot = getSlot();
}

// Dtor
product::~product()
{
}

void product::setMachine(machine *machine)
{
    thisMachine = machine;
}

void product::loadProductProperties()
{
    loadProductPropertiesFromDb();
    qDebug() << "done loading from db";
    loadProductPropertiesFromProductsFile();
    qDebug() << "done loading from csv";
}

void product::loadProductPropertiesFromDb()
{
    qDebug() << "Open db: db load product properties";
    DbManager db(DB_PATH);

    // db.getProductProperties(getSlot(), &m_aws_product_id, m_sizeIndexIsEnabled);
    db.getAllProductProperties(getSlot(), &m_aws_product_id,
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

    // m_slot_enabled = db.getSlotEnabled(getSlot());
    // m_empty_container_detection_enabled = db.getEmptyContainerDetectionEnabled();
    db.closeDB();
}

void product::loadProductPropertiesFromProductsFile()
{
    QFile file(PRODUCT_DETAILS_TSV_PATH);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR: Opening product details file. Expect unexpected behaviour now! ";
        return;
    }

    QTextStream in(&file);
    qDebug() << "Load csv file with product properties";
    while (!in.atEnd())
    {
        QString line = in.readLine();

        QStringList fields = line.split("\t");
        int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], m_soapstand_product_serial, Qt::CaseSensitive);
        if (compareResult == 0)
        {
            // qDebug() << "compare result is 0";
            m_name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
            m_product_type = fields[CSV_PRODUCT_COL_TYPE];
            m_description_ui = fields[CSV_PRODUCT_COL_DESCRIPTION_UI];
            m_features_ui = fields[CSV_PRODUCT_COL_FEATURES_UI];
            m_ingredients_ui = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];
            break;
        }
    }
    file.close();
}

char product::getSizeAsChar()
{
    // ! = invalid.
    // t  test to fsm, but should become c for custom. we're so ready for it.
    // t
    return df_util::sizeIndexToChar(m_selected_size);
}

int product::getBiggestEnabledSizeIndex()
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

bool product::getSlotEnabled()
{
    // return m_slot_enabled;
    return thisMachine->getSlotEnabled(getSlot());
}

QString product::setStatusText(QString status)
{

    thisMachine->setStatusText(getSlot(), getSlotEnabled(), status);
    // DbManager db(DB_PATH);

    //    SLOT_STATE_AVAILABLE

    // bool success = db.updateSlotAvailability(getSlot(), getSlotEnabled(), status);
    // db.updateTableProductsWithText(getSlot(),"")
    // db.closeDB();
}

QString product::getStatusText()
{
    // return m_slot_enabled;
    return thisMachine->getStatusText(getSlot());
}

bool product::getSizeEnabled(int size)
{
    // caution!:  provide size index (0=small, ...)
    qDebug() << "Size enabled? for: " << size << "enabled? : " << m_sizeIndexIsEnabled[size];
    return m_sizeIndexIsEnabled[size];
}

int product::getSlot()
{
    return m_dispenser_slot;
}

void product::setSlot(int slot)
{

    if (slot >= OPTION_SLOT_INVALID && slot <= SLOT_COUNT)
    {

        m_dispenser_slot = slot;
    }
    else
    {
        qInfo() << "OUT OF OPTION RANGE!" << slot;
    }
}

int product::getSize()
{
    // e.g. SIZE_SMALL_INDEX
    return m_selected_size;
}

void product::setSize(int sizeIndex)
{
    qDebug() << "Set size index: " << sizeIndex;
    m_selected_size = sizeIndex;
}

void product::setBiggestEnabledSizeIndex()
{
    setSize(getBiggestEnabledSizeIndex());
}

// SLOTS Section
bool product::is_valid_size_selected()
{
    if (!(m_dispenser_slot >= OPTION_SLOT_INVALID && m_dispenser_slot <= SLOT_COUNT))
    {
        qInfo() << "ERROR: no slot set. " << m_dispenser_slot;
        return false;
    }
    if (!(m_selected_size >= 0 && m_selected_size <= SIZES_COUNT))
    {
        qInfo() << "ERROR: no size set. " << m_selected_size;
        return false;
    }
    return true;
}

double product::getDiscount()
{
    // the discount is the original price minus the discounted price
    return getPrice(getSize()) - getPriceCorrected();
}

double product::getDiscountPercentageFraction()
{
    return m_discount_percentage_fraction;
}

QString product::getLastRestockDate()
{

    return m_lastRestockDate;
}

QString product::getPromoCode()
{
    return m_promoCode;
}

void product::setDiscountPercentageFraction(double percentageFraction)
{
    qDebug() << "Set discount percentage fraction: " << QString::number(percentageFraction, 'f', 3);
    m_discount_percentage_fraction = percentageFraction;
}

void product::setPromoCode(QString promoCode)
{
    // ratio = percentage / 100;
    qDebug() << "Set Promo Code: " << promoCode;
    m_promoCode = promoCode;
}

void product::setPrice(int size, double price)
{
    // db.updatePrice(getSlot(), size, price);
    QString price_columns[5] = {"size_error", "price_small", "price_medium", "price_large", "price_custom"};
    QString column_name = price_columns[size];

    qDebug() << "Open db: set product price";
    DbManager db(DB_PATH);
    db.updateTableProductsWithDouble(getSlot(), column_name, price, 2);
    db.closeDB();
}

double product::getPrice(int sizeIndex)
{
    // always from database
    return m_sizeIndexPrices[sizeIndex];
}

double product::getPrice()
{
    return getPrice(getSize());
}

double product::getPriceCorrected()
{
    // slot and size needs to be set.
    double price;
    if (is_valid_size_selected())
    {
        price = getPrice(getSize()) * (1.0 - m_discount_percentage_fraction);
    }
    else
    {

        qInfo() << "ERROR: no product set";
        price = 66.6;
    }
    return price;
}

double product::getPriceCustom()
{
    // slot and size needs to be set.
    double price;
    if (is_valid_size_selected())
    {
        price = getPrice(getSize()) * (1.0 - m_discount_percentage_fraction) * getVolumeOfSelectedSize();
    }
    else
    {

        qInfo() << "ERROR: no product set";
        price = 66.6;
    }
    return price;
}

void product::resetVolumeDispensed()
{
    this->DispensedVolumeMl = 0;
}

double product::getVolumeDispensedMl()
{
    // volume for this dispense
    return this->DispensedVolumeMl;
}

void product::setVolumeDispensedMl(double volumeMl)
{
    // volume for this dispense
    this->DispensedVolumeMl = volumeMl;
}

// bool product::restockProduct(){

// }

double product::getRestockVolume()
{
    return m_volume_full;
}
double product::getVolumeBySize(int size)
{

    return m_sizeIndexVolumes[size];
}

double product::getVolumeOfSelectedSize()
{
    double volume;
    if (is_valid_size_selected())
    {
        volume = getVolumeBySize(getSize());
    }
    else
    {
        qInfo() << "ERROR: No product set";
        volume = 66.6;
    }
    return volume;
}

QString product::getUnitsForSlot()
{

    return m_size_unit;
}

QString product::getVolumePerTickAsStringForSlot()
{
    double vol_per_tick = getVolumePerTickForSlot();
    QString units = getUnitsForSlot();

    return df_util::getConvertedStringVolumeFromMl(vol_per_tick, units, false, true);
}

double product::getVolumePerTickForSlot()
{
    // qInfo() << "Open db: get vol per tick";
    // DbManager db(DB_PATH);
    // double ml_per_tick = db.getProductVolumePerTick(getSlot());

    // db.closeDB();
    return m_volume_per_tick;
}

void product::setVolumePerTickForSlot(QString volumePerTickInput)
{
    double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
    qInfo() << "Open db: set vol per tick";
    DbManager db(DB_PATH);
    // db.updateVolumePerTick(getSlot(), ml_per_tick);
    db.updateTableProductsWithDouble(getSlot(), "volume_per_tick", ml_per_tick, 2);
    db.closeDB();
}

void product::setSizeToVolumeForSlot(QString volumeInput, int size)
{
    double volume = inputTextToMlConvertUnits(volumeInput);
    QString volume_columns[5] = {"invalid_size", "size_small", "size_medium", "size_large", "size_custom_max"};
    QString column_name = volume_columns[size];

    qInfo() << "Open db: size to volume";
    DbManager db(DB_PATH);
    db.updateTableProductsWithDouble(getSlot(), column_name, volume, 2);
    db.closeDB();
}

void product::setVolumeRemainingUserInput(QString volumeRemainingAsUserText)
{
    double vol_as_ml = inputTextToMlConvertUnits(volumeRemainingAsUserText);
    setVolumeRemaining(vol_as_ml);
}

bool product::restock()
{
    qDebug() << "Open db: restock";
    DbManager db(DB_PATH);
    bool success = true;
    success &= db.updateTableProductsWithDouble(getSlot(), "volume_remaining", m_volume_full, 0);
    success &= db.updateTableProductsWithDouble(getSlot(), "volume_dispensed_since_restock", 0, 0);
    success &= db.updateTableProductsWithText(getSlot(), "last_restock", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    db.closeDB();
    return success;
}

void product::setVolumeRemaining(double volume_as_ml)
{
    // qDebug() << "Open db: set volume remaining";
    DbManager db(DB_PATH);
    // db.setVolumeRemaining(getSlot(), volume_as_ml);
    db.updateTableProductsWithDouble(getSlot(), "volume_remaining", volume_as_ml, 0);
    db.closeDB();
}

QString product::getVolumeRemainingCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_remaining, units, false, true);

    return volume_as_string;
}

QString product::getVolumeDispensedSinceRestockCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_since_restock, units, false, true);
    return volume_as_string;
}

QString product::getTotalDispensedCorrectUnits()
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_total, units, false, true);
    return volume_as_string;
}

QString product::getSizeToVolumeWithCorrectUnits(int size, bool roundValue, bool addUnits)
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

double product::inputTextToMlConvertUnits(QString inputValueAsText)
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

QString product::getProductDrinkfillSerial()
{
    return m_soapstand_product_serial;
}

bool product::isProductVolumeInContainer()
{

    bool retval = true;
    // if (!m_empty_container_detection_enabled)

    if (!thisMachine->getEmptyContainerDetectionEnabled())
    {
        retval = m_volume_remaining > CONTAINER_EMPTY_THRESHOLD_ML;
    }
    return retval;
}

void product::getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter)
{
    qDebug() << "Open db: get bulk volume discount details ";

    *large_volume_discount_is_enabled = m_is_enabled_custom_discount;
    *min_volume_for_discount = m_size_custom_discount;
    *discount_price_per_liter = m_price_custom_discount;
}

QString product::getProductIngredients()
{
    return m_ingredients_ui;
}

QString product::getProductFeatures()
{
    return m_features_ui;
}
QString product::getProductName()
{
    return m_name_ui;
}
QString product::getProductDescription()
{
    return m_description_ui;
}

QString product::getProductType()
{

    return m_product_type;
}

QString product::getProductPicturePath()
{
    return QString(PRODUCT_PICTURES_ROOT_PATH).arg(m_soapstand_product_serial);
}

QString product::getPlu(int sizeIndex)
{
    return m_sizeIndexPLUs[sizeIndex];
}

void product::setPlu(int sizeIndex, QString plu){
    QString plu_columns [5] = {"Invalid size", "plu_small","plu_medium", "plu_large", "plu_custom"};
    QString column_name = plu_columns[sizeIndex];
    DbManager db(DB_PATH);
    bool success = db.updateTableProductsWithText(getSlot(), column_name, plu);
    qDebug()<< "set Plu success?: " << success;
    db.closeDB();
}

QString product::getAwsProductId()
{
    return m_aws_product_id;
}

QString product::getFullVolumeCorrectUnits(bool addUnits)
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_full, units, false, addUnits);
    return volume_as_string;
}

void product::setFullVolumeCorrectUnits(QString inputFullValue)
{
    qDebug() << "Open db: for write full vol";
    double full_volume = inputTextToMlConvertUnits(inputFullValue);
    DbManager db(DB_PATH);
    db.updateTableProductsWithDouble(getSlot(), "volume_full", full_volume, 0);

    db.closeDB();
}

QString product::getSizeToVolumeWithCorrectUnits(bool round, bool addUnits)
{
    return getSizeToVolumeWithCorrectUnits(getSize(), round, addUnits);
}

void product::setPaymentMethod(QString paymentMethod)
{
    qDebug() << "Open db: set payment method";
    DbManager db(DB_PATH);
    db.updateTableProductsWithText(getSlot(), "payment", paymentMethod);
    db.closeDB();
}

QString product::getPaymentMethod()
{
    return m_payment;
}

int product::getDispenseSpeedPercentage()
{
    return (int)round((double(m_dispense_speed) * 100) / 255);
}

void product::setDispenseSpeedPercentage(int percentage)
{
    int pwm = (int)round((percentage * 255) / 100);

    qInfo() << "Open db: pwm speed set";
    DbManager db(DB_PATH);
    // db.updatePWM(getSlot(), pwm);
    db.updateTableProductsWithInt(getSlot(), "dispense_speed", pwm);
    db.closeDB();
}
