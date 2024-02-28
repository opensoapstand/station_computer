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

bool pnumberproduct::loadProductProperties()
{
    bool success = true;
    qDebug() << "Load properties from db and csv for pnumer: " << getPNumber();
    success &= loadProductPropertiesFromDb();
    success &= loadProductPropertiesFromProductsFile();
    return success;
}

QString pnumberproduct::convertPNumberToPNotation(int pnumber)
{
    return "P-" + QString::number(pnumber);
}

int pnumberproduct::convertPStringToPInt(QString pnumberNotation)
{
    // QString pnumber_as_string;
    if (pnumberNotation.startsWith("P-") || pnumberNotation.startsWith("p-"))
    {
        pnumberNotation = pnumberNotation.mid(2);
    }
    return pnumberNotation.toInt();
}

void pnumberproduct::getProductProperties(QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui)
{
    *name = m_name;
    *name_ui = m_name_ui;
    *product_type = m_product_type;
    *description_ui = m_description_ui;
    *ingredients_ui = m_ingredients_ui;
}

bool pnumberproduct::loadProductPropertiesFromProductsFile()
{
    QFile file(PRODUCT_DETAILS_TSV_PATH);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR: Opening product details file. Expect unexpected behaviour now! ";
        return false;
    }

    QTextStream in(&file);
    qDebug() << "Load csv file with product properties";
    while (!in.atEnd())
    {
        QString line = in.readLine();

        QStringList fields = line.split("\t");
        QString pnumberNotation = convertPNumberToPNotation(getPNumber());
        QString mix_pnumbers_str;
        QString mix_ratios_low_str;
        QString mix_ratios_default_str;
        QString mix_ratios_high_str;
        int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], pnumberNotation, Qt::CaseSensitive);
        if (compareResult == 0)
        {
            // qDebug() << "compare result is 0";
            m_name = fields[CSV_PRODUCT_COL_NAME];
            m_name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
            m_product_type = fields[CSV_PRODUCT_COL_TYPE];
            m_description_ui = fields[CSV_PRODUCT_COL_DESCRIPTION_UI];
            m_features_ui = fields[CSV_PRODUCT_COL_FEATURES_UI];
            m_ingredients_ui = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];

            mix_pnumbers_str = fields[CSV_PRODUCT_COL_MIX_PNUMBERS];
            mix_ratios_low_str = fields[CSV_PRODUCT_COL_MIX_RATIOS_LOW];
            mix_ratios_default_str = fields[CSV_PRODUCT_COL_MIX_RATIOS_DEFAULT];
            mix_ratios_high_str = fields[CSV_PRODUCT_COL_MIX_RATIOS_HIGH];
            
            df_util::csvQStringToQVectorInt(mix_pnumbers_str, m_mixPNumbers);
            df_util::csvQStringToQVectorDouble(mix_ratios_low_str, m_mixRatiosLow);
            df_util::csvQStringToQVectorDouble(mix_ratios_default_str, m_mixRatiosDefault);
            df_util::csvQStringToQVectorDouble(mix_ratios_high_str, m_mixRatiosHigh);
            break;
        }

    }
    file.close();
    return true;
}

void pnumberproduct::setPNumber(int pnumber)
{
    m_PNumber = pnumber;
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

QString pnumberproduct::getMixPNumbersAsCsv()
{
    QString csvString;
    if (m_mixPNumbers.size() > 0)
    {
        for (int i = 0; i < m_mixPNumbers.size(); ++i)
        {
            csvString += QString::number(m_mixPNumbers[i]);
            if (i < m_mixPNumbers.size() - 1) // Don't add a comma after the last element
                csvString += ",";
        }
    }
    else
    {
        csvString += QString::number(m_PNumber);
    }
    return csvString;
}

QString pnumberproduct::getMixRatiosAsCsv(QVector<double> ratios)
{
    QString csvString;
    if (ratios.size() > 0)
    {

        for (int i = 0; i < ratios.size(); ++i)
        {
            csvString += QString::number(ratios[i]);
            if (i < ratios.size() - 1) // Don't add a comma after the last element
                csvString += ",";
        }
    }
    else
    {
        csvString += "1";
    }
    return csvString;
}

QVector<int> pnumberproduct::getMixPNumbers()
{
    return m_mixPNumbers;
}
QVector<double> pnumberproduct::getMixRatiosLow()
{
    return m_mixRatiosLow;
}
QVector<double> pnumberproduct::getMixRatios()
{
    return m_mixRatiosDefault;
}
QVector<double> pnumberproduct::getMixRatiosHigh()
{
    return m_mixRatiosHigh;
}

bool pnumberproduct::loadProductPropertiesFromDb()
{
    qDebug() << "Open db: db load pnumberproduct properties for pnumberproduct for pnumber: " << getPNumber();
    bool success = m_db->getAllProductProperties(getPNumber(),
                                  &m_aws_product_id,
                                  &m_soapstand_product_serial,
                                //   m_mixPNumbers,
                                //   m_mixRatiosLow,
                                //   m_mixRatiosDefault,
                                //   m_mixRatiosHigh,
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

    int pnumberFromDb = convertPStringToPInt(m_soapstand_product_serial);

    if (getPNumber() != pnumberFromDb)
    {
        qDebug() << "ERROR: Could not load from DB: " << getPNumber() << " : " << pnumberFromDb;
        success = false;
    }
    return success;
}

bool pnumberproduct::getIsProductEnabled()
{
    return m_is_enabled;
}
void pnumberproduct::setIsProductEnabled(bool isEnabled)
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
    QString sizeIndexToText[7] = {"Invalid", "small", "medium", "large", "custom", "test", "sample"};
    // m_sizeIndexIsEnabled[size] = enabled;
    qDebug() << "Size enabled" << size;
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

double pnumberproduct::getPriceOfSelectedBottle(){
    //size: 1 for default bottle size
    return getBasePrice(1);
}

double pnumberproduct::getVolumeOfSelectedBottle()
{
    //size: 1 for default bottle size
    return getVolumeBySize(1);
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
    QString price_columns[6] = {"size_error", "price_small", "price_medium", "price_large", "price_custom", "price_sample"};
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
    if (getSizeUnit() == "oz")
    {
        return df_util::convertOzToMl(inputValueAsText.toDouble());
    }
    else
    {
        return inputValueAsText.toDouble();
    }
}

void pnumberproduct::setVolumeDispensedMixedProduct(QString volumeDispensedMixProduct)
{
    // volume for this dispense
    this->volumeDispensedMixProduct = volumeDispensedMixProduct;
}

QString pnumberproduct::getVolumeDispensedMixedProduct()
{
    // volume for this dispense
    return this->volumeDispensedMixProduct;
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

void pnumberproduct::setSizeUnit(QString unit)
{
    m_size_unit = unit;
}

QString pnumberproduct::getSizeUnit()
{

    return m_size_unit;
}

QString pnumberproduct::getVolumePerTickAsStringForSlot()
{
    double vol_per_tick = getVolumePerTickForSlot();
    QString units = getSizeUnit();

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
    QString volume_columns[6] = {"invalid_size", "size_small", "size_medium", "size_large", "size_custom_max", "size_sample"};
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
    QString units = getSizeUnit();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(getVolumeRemaining(), units, false, addUnits);

    return volume_as_string;
}

QString pnumberproduct::getVolumeDispensedSinceRestockCorrectUnits()
{
    QString units = getSizeUnit();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(m_volume_dispensed_since_restock, units, false, true);
    return volume_as_string;
}

QString pnumberproduct::getTotalDispensedCorrectUnits()
{
    QString units = getSizeUnit();
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
    units = getSizeUnit();
    // qDebug() << "unitsssssss" << units;
    volume_as_string = df_util::getConvertedStringVolumeFromMl(v, units, roundValue, addUnits);
    return volume_as_string;
}

QString pnumberproduct::getPNumberAsPString()
{
    // return m_soapstand_product_serial;
    return convertPNumberToPNotation(getPNumber());
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
    QString path = QString(PRODUCT_PICTURES_ROOT_PATH).arg(getPNumberAsPString());
    qDebug() << "Picture path: " << path;
    return path;
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
    QString units = getSizeUnit();
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

bool pnumberproduct::isCustomMix()
{
    bool isCustomMix = false;

    for (int i = 0; i < getMixRatios().size(); i++)
    {
        if (m_customMixRatios[i] != getMixRatios()[i])
        {
            // qInfo() << "i: normal mix ratio: " << getMixRatios()[i]<< "  custom: " << m_customMixRatios[i];
            isCustomMix = true;
        }
    }
    return isCustomMix;
}

QVector<double> pnumberproduct::getCustomMixRatios()
{
    return m_customMixRatios;
}

double pnumberproduct::getCustomMixRatios(int index)
{
    return m_customMixRatios[index];
}

void pnumberproduct::resetCustomMixRatioParameters(){
    m_customMixRatios.clear();
    for (int i = 0; i < getMixRatios().size(); i++)
    {
        // set default mix ratio to custom mix ratio
        m_customMixRatios.append(getMixRatios()[i]);
    }
}

void pnumberproduct::setCustomMixRatios(int index, QString plusOrMinus){
    double custom_ratios_total = 0;
    // m_customMixRatios.append(getMixRatios()[0]); // add base product ratio
    // minus
    if(plusOrMinus == "-"){
        if(m_customMixRatios[index] == getMixRatiosLow()[index]){
            // dont do anything
        }else{
            if(m_customMixRatios[index] == getMixRatios()[index]){
                m_customMixRatios[index] = getMixRatiosLow()[index];
            }else{
                m_customMixRatios[index] = getMixRatios()[index];
            }

        }
    }else{
        // plus
        if(m_customMixRatios[index] == getMixRatiosHigh()[index]){
            // dont do anything
        }else{
            if(m_customMixRatios[index] == getMixRatios()[index]){
                m_customMixRatios[index] = getMixRatiosHigh()[index];
            }else{
                m_customMixRatios[index] = getMixRatios()[index];
            }

        }
    }

    for (int i = 1; i < getMixRatios().size(); i++)
    {
        // go over all ADDITIVES (skip base)
        custom_ratios_total += m_customMixRatios[i];
    }

    if (custom_ratios_total > 1.0)
    {
        qDebug() << "ERROR: too much additives added, will reset to default.";
        resetCustomMixRatioParameters();
    }
    else
    {
        m_customMixRatios[0] = 1.0 - custom_ratios_total;
    }
}

///////// FOR PERCENTAGE /////////////
// void pnumberproduct::setCustomMixRatios()
// {
//     // two options:
//     //    1. keep rations e.g. 0.7,0.2,0.1 --> 0.1 become custom *2 ==> 0.2  ==> remaining 0.7+0.2 used to be 90%, but will now be 80%, so: modifier: 0.8/0.9  ==> new ratios = 62.22%,17.77%,20% ONLY WORKS FOR ONE CHANGE AT A TIME
//     //    or
//     //    2. used modifiers to change the additives percentages and use "the rest" to add with base product. e.g. 0.7,0.2,0.1   --> 0.1 becomes custom 0.2 ==> 0.2,0.2 for the addtives = 40% ==> base will be 60% now.    0.6,0.2,0.2 as end custom mix

//     // let's keep it simple and use method 2, always recalculate for zero.

//     double custom_ratios_total = 0;
//     m_customMixRatios.clear();
//     m_customMixRatios.append(getMixRatios()[0]); // add base product ratio
//     for (int i = 1; i < getMixRatios().size(); i++)
//     {
//         // go over all ADDITIVES (skip base)
//         // ratio_modifiers_total += m_additivesCustomMixRatioModifiers[i - 1];
//         double customRatio = m_additivesCustomMixRatioModifiers[i - 1] * getMixRatios()[i];
//         custom_ratios_total += customRatio;
//         m_customMixRatios.append(customRatio);
//     }

//     if (custom_ratios_total > 1.0)
//     {
//         qDebug() << "ERROR: too much additives added, will reset to default.";
//         resetCustomMixRatioParameters();
//     }
//     else
//     {
//         m_customMixRatios[0] = 1.0 - custom_ratios_total;
//     }
// }

// void pnumberproduct::resetCustomMixRatioParameters()
// {
//     // the mixing ratios might be altered by the user. For this we use a ratio-modifier.
//     // additives is mixPNumbers - base product.
//     m_additivesCustomMixRatioModifiers.clear();
//     for (int i = 0; i < getMixRatios().size() - 1; i++)
//     {
//         m_additivesCustomMixRatioModifiers.append(1);
//     }
//     m_customMixRatios.clear();
//     for (int i = 0; i < getMixRatios().size(); i++)
//     {

//         m_customMixRatios.append(getMixRatios()[i]); // add base product ratio
//     }
// }

// void pnumberproduct::adjustAdditivesRatioModifier(int index, double additiveModifier)
// {

//     m_additivesCustomMixRatioModifiers[index] = additiveModifier;
//     setCustomMixRatios();
// }

// QVector<double> pnumberproduct::getAdditivesRatioModifier()
// {
//     return m_additivesCustomMixRatioModifiers;
// }

// double pnumberproduct::getAdditivesRatioModifier(int index)
// {
//     return m_additivesCustomMixRatioModifiers[index];
// }