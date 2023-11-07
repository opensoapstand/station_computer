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
    qDebug() << "pnumber: " << getPNumber() << " Load properties from db and csv";
    loadProductPropertiesFromDb();
    loadProductPropertiesFromProductsFile();
}

QString pnumberproduct::convertPNumberToPNotation(int pnumber)
{
    return "P-" + QString::number(pnumber);
}

int pnumberproduct::convertPNotationToPNumber(QString PNumberNotation)
{
    QString pnumber_as_string = PNumberNotation.mid(2);
    return pnumber_as_string.toInt();
}

void pnumberproduct::getProductProperties(QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui)
{
    *name = m_name;
    *name_ui = m_name_ui;
    *product_type = m_product_type;
    *description_ui = m_description_ui;
    *ingredients_ui = m_ingredients_ui;
}

void pnumberproduct::loadProductPropertiesFromProductsFile()
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
        QString PNumberNotation = convertPNumberToPNotation(getPNumber());
        int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], PNumberNotation, Qt::CaseSensitive);
        if (compareResult == 0)
        {
            // qDebug() << "compare result is 0";
            m_name = fields[CSV_PRODUCT_COL_NAME];
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

void pnumberproduct::setPNumber(int PNumber)
{
    m_PNumber = PNumber;
}

int pnumberproduct::getPNumber()
{
    return m_PNumber;
}

int pnumberproduct::getFirstMixPNumberOrPNumberAsBasePNumber()
{
    // base p-number return the pnumber if no mixing set, or else returns the first mixpnumber of the list.
    if (m_mixPNumbers.size() > 0)
    {
        return m_mixPNumbers[0];
    }
    else
    {
        return m_PNumber;
    }
}

QVector<int> pnumberproduct::getMixPNumbers()
{
    return m_mixPNumbers;
}

void pnumberproduct::loadProductPropertiesFromDb()
{
    qDebug() << "Open db: db load pnumberproduct properties for pnumberproduct for pnumber: " << getPNumber();
    m_db->getAllProductProperties(getPNumber(),
                                  &m_aws_product_id,
                                  &m_soapstand_product_serial,
                                  m_mixPNumbers,
                                  m_mixRatios,
                                  &m_size_unit,
                                  &m_currency_deprecated, //_dummy_deprecated
                                  &m_payment_deprecated,  //_deprecated,
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
                                  &m_is_enabled,
                                  &m_status_text,
                                  m_sizeIndexIsEnabled, m_sizeIndexPrices, m_sizeIndexVolumes, m_sizeIndexPLUs, m_sizeIndexPIDs);
}

bool pnumberproduct::getIsEnabled()
{
    return m_is_enabled;
}
void pnumberproduct::setIsEnabled(bool isEnabled)
{
    m_is_enabled = isEnabled;
    m_db->updateTableProductsWithInt(getPNumber(), "is_enabled", isEnabled);
}

QString pnumberproduct::getStatusText()
{
    return m_status_text;
}
void pnumberproduct::setStatusText(QString statusText)
{
    m_status_text = statusText;
    m_db->updateTableProductsWithText(getPNumber(), "status_text", statusText);
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

double pnumberproduct::getBasePriceSelectedSize()
{
    return getBasePrice(getSelectedSize());
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

double pnumberproduct::getVolumeRemaining()
{
    return m_volume_remaining;
}

QString pnumberproduct::getVolumeRemainingCorrectUnits(bool addUnits)
{
    QString units = getUnitsForSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(getVolumeRemaining(), units, false, addUnits);

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
    QString pnumber = m_soapstand_product_serial;
    qDebug() << "pnumber before p nodted " << pnumber;

    // Check if serial starts with "P-"
    if (!pnumber.startsWith("P-"))
    {
        // Add "P-" prefix if it's missing
        pnumber.prepend("P-");
    }
    qDebug() << "pnumber P- notated " << pnumber;
    return QString(PRODUCT_PICTURES_ROOT_PATH).arg(pnumber);
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

void pnumberproduct::setPaymentMethod(QString paymentMethod)
{
    qDebug() << "Open db: set payment method";
    m_db->updateTableProductsWithText(getPNumber(), "payment", paymentMethod);
}

QString pnumberproduct::getPaymentMethod()
{
    return m_payment_deprecated;
}