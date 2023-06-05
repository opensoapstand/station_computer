#include "df_util.h" // lode added for settings
#include "dbmanager.h"
#include "page_dispenser.h"
#include "page_idle.h"
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

void product::load()
{
    // load all properties at once in a db call.
    loadProductProperties();
}

void product::loadProductProperties()
{
    loadProductPropertiesFromDb();
    qDebug() << "done loading from db";
    loadProductPropertiesFromProductsFile();
    qDebug() << "done loading from csv";

    qDebug() << "testtesttest: lst restock gasetzaset:: " << m_lastRestockDate;
    qDebug() << "testtesttest: lst restock name:: " << getProductName();
}

void product::loadProductPropertiesFromDb()
{
    qDebug() << "Open db: db load product properties";
    DbManager db(DB_PATH);

    db.getProductProperties(getSlot(), &m_aws_product_id, m_sizeIndexIsEnabled);
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
            qDebug() << "compare result is 0";
            m_name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
            m_product_type = fields[CSV_PRODUCT_COL_TYPE];
            m_description_ui = fields[CSV_PRODUCT_COL_DESCRIPTION_UI];
            m_features_ui = fields[CSV_PRODUCT_COL_FEATURES_UI];
            m_ingredients_ui = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];
            break;
        }
    }
    qDebug() << "properties file read before close ";
    file.close();
    qDebug() << "properties file read done ";
}

char product::getSizeAsChar()
{
    // ! = invalid.
    // t  test to fsm, but should become c for custom. we're so ready for it.
    // t

    return df_util::sizeIndexToChar(Size);
}

int product::getSize()
{
    // e.g. SIZE_SMALL_INDEX
    return Size;
}

void product::setSize(int sizeIndex)
{
    qDebug() << "Set size index: " << sizeIndex;
    Size = sizeIndex;
}

void product::setBiggestEnabledSizeIndex()
{
    setSize(getBiggestEnabledSizeIndex());
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
    qDebug() << "Open db: get slot enabled";
    DbManager db(DB_PATH);
    bool enabled = db.getSlotEnabled(getSlot());

    db.closeDB();
    return enabled;
}

bool product::getSizeEnabled(int size)
{
    // caution!:  provide size index (0=small, ...)
    qDebug() << size;
    return m_sizeIndexIsEnabled[size];
}

int product::getSlot()
{
    return m_selectedSlot;
}

void product::setSlot(int slot)
{

    if (slot >= OPTION_SLOT_INVALID && slot <= SLOT_COUNT)
    {

        m_selectedSlot = slot;
    }
    else
    {
        qInfo() << "OUT OF OPTION RANGE!" << slot;
    }
}

// SLOTS Section

bool product::isOrderValid()
{
    if (!(m_selectedSlot >= OPTION_SLOT_INVALID && m_selectedSlot <= SLOT_COUNT))
    {
        qInfo() << "ERROR: no slot set. " << m_selectedSlot;
        return false;
    }
    if (!(Size >= 0 && Size <= SIZES_COUNT))
    {
        qInfo() << "ERROR: no size set. " << m_selectedSlot;
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
    // ratio = percentage / 100;
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
    qDebug() << "Open db: set product price";
    DbManager db(DB_PATH);
    db.updatePrice(getSlot(), size, price);
    db.closeDB();
}

double product::getPrice(int sizeIndex)
{
    // always from database
    qDebug() << "Open db: get product price";
    DbManager db(DB_PATH);
    double price;
    price = db.getProductPrice(getSlot(), df_util::sizeIndexToChar(sizeIndex));
    qDebug() << "productsize: " << df_util::sizeIndexToChar(sizeIndex) << " price: " << price;
    db.closeDB();
    return price;
}

double product::getPrice()
{
    return getPrice(getSize());
}

double product::getPriceCorrected()
{
    // slot and size needs to be set.
    double price;
    if (isOrderValid())
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

double product::getPriceCustom()
{
    // slot and size needs to be set.
    double price;
    if (isOrderValid())
    {
        price = getPrice(getSize()) * (1.0 - m_discount_percentage_fraction) * getVolume();
    }
    else
    {

        qInfo() << "ERROR: no product set";
        price = 66.6;
    }
    return price;
}

double product::getVolume(int size)
{
    // double volume;
    // qInfo() << "Open db: get volume";
    // DbManager db(DB_PATH);
    // volume = db.getProductVolume(getSlot(), df_util::sizeIndexToChar(size));
    // db.closeDB();
    return m_volume_full;
}

double product::getVolume()
{
    double volume;
    if (isOrderValid())
    {
        volume = getVolume(getSize());
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

    // qDebug() << "Open db:  units for label.";
    // QString units;
    // DbManager db(DB_PATH);
    // units = db.getUnits(getSlot());
    // db.closeDB();
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
    db.updateVolumePerTick(getSlot(), ml_per_tick);

    db.closeDB();
}

void product::setSizeToVolumeForSlot(QString volumeInput, int size)
{
    double volume = inputTextToMlConvertUnits(volumeInput);
    qInfo() << "Open db: size to volume";
    DbManager db(DB_PATH);
    db.updateTargetVolume(getSlot(), size, volume);
    db.closeDB();
}

QString product::getVolumeRemainingCorrectUnits()
{
    // qInfo() << "Open db: volume dispensed since last restock";
    // DbManager db(DB_PATH);
    // double volume = db.getVolumeRemaining(getSlot());
    // db.closeDB();

    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_remaining, units, false, true);

    return volume_as_string;
}

QString product::getVolumeDispensedSinceRestockCorrectUnits()
{
    // qInfo() << "Open db:  volume dispensed since last restock";
    // DbManager db(DB_PATH);
    // double volume = db.getVolumeDispensedSinceRestock(getSlot());
    // db.closeDB();

    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_since_restock, units, false, true);

    return volume_as_string;
}

QString product::getTotalDispensedCorrectUnits()
{
    // qInfo() << "Open db:  volume dispensed";
    // DbManager db(DB_PATH);
    // double volume = db.getTotalDispensed(getSlot());
    // db.closeDB();

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

    v = getVolume(size);
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
    // qDebug() << "Open db: get product id";
    // DbManager db(DB_PATH);
    // QString serial = db.getProductDrinkfillSerial(getSlot());
    // db.closeDB();
    return m_soapstand_product_serial;
}

bool product::isProductVolumeInContainer()
{
    DbManager db(DB_PATH);
    bool retval = true;
    if (!db.getEmptyContainerDetectionEnabled())
    {
        retval = db.getVolumeRemaining(getSlot()) > CONTAINER_EMPTY_THRESHOLD_ML;
    }

    db.closeDB();
    return retval;
}

void product::getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter)
{
    qDebug() << "Open db: get bulk volume discount details ";

    DbManager db(DB_PATH);
    db.getCustomDiscountProperties(getSlot(), large_volume_discount_is_enabled, min_volume_for_discount, discount_price_per_liter);
    db.closeDB();
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
    QString serial = getProductDrinkfillSerial();
    return QString(PRODUCT_PICTURES_ROOT_PATH).arg(serial);
}

QString product::getPLU(int sizeIndex)
// QString product::getPLU(char size)
{
    return m_sizeIndexPLUs[sizeIndex];
}

QString product::getMachineId()
{

    qDebug() << "Open db: getMachineID";

    DbManager db(DB_PATH);
    QString idString = db.getMachineID();
    db.closeDB();
    return idString;
}

QString product::getAwsProductId()
{
    // qDebug() << "Open db: get productId ";
    // DbManager db(DB_PATH);
    // QString idString = db.(getSlot());
    // db.closeDB();
    return m_aws_product_id;
}

QString product::getFullVolumeCorrectUnits(bool addUnits)
{

    // qDebug() << "Open db: get full volume ";
    // DbManager db(DB_PATH);

    // double volume = db.getFullProduct(getSlot());
    // db.closeDB();

    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_full, units, false, addUnits);

    return volume_as_string;
}

void product::setFullVolumeCorrectUnits(QString inputFullValue)
{
    qDebug() << "Open db: for write full vol";
    double full_volume = inputTextToMlConvertUnits(inputFullValue);
    DbManager db(DB_PATH);
    db.updateFullVolume(getSlot(), full_volume);
    db.closeDB();
}

QString product::getSizeToVolumeWithCorrectUnits(bool round, bool addUnits)
{
    return getSizeToVolumeWithCorrectUnits(getSize(), round, addUnits);
}

QString product::getPaymentMethod()
{
    return m_payment;
}

int product::getDispenseSpeedPercentage()
{
    int pwm;

    qInfo() << "Open db: pwm speed";
    DbManager db(DB_PATH);
    pwm = db.getPWM(getSlot());
    db.closeDB();

    return (int)round((double(pwm) * 100) / 255);
}
void product::setDispenseSpeedPercentage(int percentage)
{
    int pwm = (int)round((percentage * 255) / 100);

    qInfo() << "Open db: pwm speed set";
    DbManager db(DB_PATH);
    db.updatePWM(getSlot(), pwm);
    db.closeDB();
}
