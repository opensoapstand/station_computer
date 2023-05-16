#include "drinkorder.h"
#include "page_idle.h"
#include "df_util.h" // lode added for settings

// Ctor
DrinkOrder::DrinkOrder()
{
    selectedDrink = new DrinkSelection;
    // overruledPrice = INVALID_PRICE;
    m_discount_percentage_fraction = 0.0;
    m_promoCode = "";
}

// Ctor Object Copy
DrinkOrder::DrinkOrder(const DrinkOrder &other) : QObject(nullptr)
{
    selectedDrink = new DrinkSelection(*other.selectedDrink);
}

// Dtor
DrinkOrder::~DrinkOrder()
{
    delete selectedDrink;
}

// Object Reassignment
DrinkOrder &DrinkOrder::operator=(const DrinkOrder &other)
{
    *selectedDrink = *other.selectedDrink;
    return *this;
}

char DrinkOrder::getSelectedSizeAsChar()
{
    // ! = invalid.
    // t  test to fsm, but should become c for custom. we're so ready for it.
    // t

    return df_util::sizeIndexToChar(selectedSize);
}

void DrinkOrder::getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter)
{
    qDebug() << "Open db: get bulk volume discount details ";

    DbManager db(DB_PATH);
    db.getCustomDiscountProperties(getSelectedSlot(), large_volume_discount_is_enabled, min_volume_for_discount, discount_price_per_liter);
    db.closeDB();
}
// void DrinkOrder::getCustomPriceDetails(QString *unitsInvoice, double *selectedPriceP, double *discountP, double *selectedPriceCorrectedP)
// {

//     double selectedPrice = getSelectedPrice();
//     double discount = getDiscount();
//     double selectedPriceCorrected = getSelectedPriceCorrected();

//     QString unitsInvoiceBBB = getUnitsForSelectedSlot();

//     if (unitsInvoiceBBB == "ml")
//     {
//         unitsInvoiceBBB = "L";
//         selectedPrice = selectedPrice * 1000;
//         selectedPriceCorrected = selectedPriceCorrected * 1000;
//         discount = discount * 1000;
//     }
//     else if (unitsInvoiceBBB == "g")
//     {
//         if (getVolume(SIZE_CUSTOM_INDEX) == VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G)
//         {
//             unitsInvoiceBBB = "100g";
//             selectedPrice = selectedPrice * 100;
//             selectedPriceCorrected = selectedPriceCorrected * 100;
//             discount = discount * 100;
//         }
//         else
//         {
//             unitsInvoiceBBB = "kg";
//             selectedPrice = selectedPrice * 1000;
//             selectedPriceCorrected = selectedPriceCorrected * 1000;
//             discount = discount * 1000;
//         }
//     }
//     else if (unitsInvoiceBBB == "oz")
//     {
//         unitsInvoiceBBB = "oz";
//         selectedPrice = selectedPrice * OZ_TO_ML;
//         selectedPriceCorrected = selectedPriceCorrected * OZ_TO_ML;
//         discount = discount * OZ_TO_ML;
//     }
//     unitsInvoice = &unitsInvoiceBBB;

//     selectedPriceP = &selectedPrice;
//     discountP = &discount;
//     selectedPriceCorrectedP = &selectedPriceCorrected;
// }

int DrinkOrder::getSelectedSize()
{
    // e.g. SIZE_SMALL_INDEX
    return selectedSize;
}

void DrinkOrder::setSelectedSize(int sizeIndex)
{
    // index!!!!  e.g. 0=small
    // overruledPrice = INVALID_PRICE;
    // m_discount_percentage_fraction = 0.0;
    qDebug() << "Set size index: " << sizeIndex;
    selectedSize = sizeIndex;
}

void DrinkOrder::setLoadedProductBiggestEnabledSizeIndex()
{
    setSelectedSize(getLoadedProductBiggestEnabledSizeIndex());
}

int DrinkOrder::getLoadedProductBiggestEnabledSizeIndex()
{

    // cascade to largest size. Custom volume is seen as superior.
    int maxSize = SIZE_SMALL_INDEX;
    if (getLoadedProductSizeEnabled(SIZE_MEDIUM_INDEX))
    {
        maxSize = SIZE_MEDIUM_INDEX;
    }
    if (getLoadedProductSizeEnabled(SIZE_LARGE_INDEX))
    {
        maxSize = SIZE_LARGE_INDEX;
    }
    if (getLoadedProductSizeEnabled(SIZE_CUSTOM_INDEX))
    {
        maxSize = SIZE_CUSTOM_INDEX;
    }
    return maxSize;
}

bool DrinkOrder::getLoadedProductSizeEnabled(int size)
{
    // caution!:  provide size index (0=small, ...)
    return m_sizeIndexIsEnabled[size];
}

int DrinkOrder::getSelectedSlot()
{
    return m_selectedSlot;
}

void DrinkOrder::setSelectedSlot(int slot)
{

    if (slot >= OPTION_SLOT_INVALID && slot <= SLOT_COUNT)
    {

        if (slot != getSelectedSlot())
        {
            // overruledPrice = INVALID_PRICE;
            // m_discount_percentage_fraction = 0.0;
            m_selectedSlot = slot;
            emit orderSlotChange(slot);
        }
    }
    else
    {
        qInfo() << "OUT OF OPTION RANGE!" << slot;
    }
}

// SLOTS Section

bool DrinkOrder::isSelectedOrderValid()
{
    if (!(m_selectedSlot >= OPTION_SLOT_INVALID && m_selectedSlot <= SLOT_COUNT))
    {
        qInfo() << "ERROR: no slot set. " << m_selectedSlot;
        return false;
    }
    if (!(selectedSize >= 0 && selectedSize <= SIZES_COUNT))
    {
        qInfo() << "ERROR: no size set. " << m_selectedSlot;
        return false;
    }

    return true;
}

double DrinkOrder::getDiscount()
{
    // qDebug() << "--------=========" << QString::number(getSelectedPriceCorrected());

    // the discount is the original price minus the discounted price
    return getPrice(getSelectedSize()) - getSelectedPriceCorrected();
}

double DrinkOrder::getDiscountPercentageFraction()
{
    return m_discount_percentage_fraction;
}

QString DrinkOrder::getPromoCode()
{
    return m_promoCode;
}

void DrinkOrder::setDiscountPercentageFraction(double percentageFraction)
{
    // ratio = percentage / 100;
    qDebug() << "Set discount percentage fraction: " << QString::number(percentageFraction, 'f', 3);
    m_discount_percentage_fraction = percentageFraction;

    // if (isSelectedOrderValid())
    // {
    //     qInfo() << "Set overrruled price.";

    //     if (price != overruledPrice)
    //     {
    //         overruledPrice = price;
    //         emit priceChange(overruledPrice);
    //     }
    // }
    // else
    // {
    //     qInfo() << "ERROR: no overruled price set. ";
    // }
}

void DrinkOrder::setPromoCode(QString promoCode)
{
    // ratio = percentage / 100;
    qDebug() << "Set Promo Code: " << promoCode;
    m_promoCode = promoCode;
}

void DrinkOrder::setPriceSelected(int size, double price)
{
    qDebug() << "Open db: set product price";
    DbManager db(DB_PATH);
    db.updatePrice(getSelectedSlot(), size, price);
    db.closeDB();
}

double DrinkOrder::getPrice(int sizeIndex)
{
    // always from database
    qDebug() << "Open db: get product price";
    DbManager db(DB_PATH);
    double price;
    price = db.getProductPrice(getSelectedSlot(), df_util::sizeIndexToChar(sizeIndex));
    qDebug() << "productsize: " << df_util::sizeIndexToChar(sizeIndex) << " price: " << price;
    db.closeDB();
    return price;
}

double DrinkOrder::getSelectedPrice()
{
    return getPrice(getSelectedSize());
}

double DrinkOrder::getSelectedPriceCorrected()
{
    // slot and size needs to be set.
    double price;
    if (isSelectedOrderValid())
    {
        price = getPrice(getSelectedSize()) * (1.0 - m_discount_percentage_fraction);
    }
    else
    {

        qInfo() << "ERROR: no product set";
        price = 66.6;
    }
    return price;
}

void DrinkOrder::resetSelectedVolumeDispensed()
{
    this->selectedDispensedVolumeMl = 0;
}

double DrinkOrder::getSelectedVolumeDispensedMl()
{
    // volume for this dispense
    return this->selectedDispensedVolumeMl;
}

void DrinkOrder::setSelectedVolumeDispensedMl(double volumeMl)
{
    // volume for this dispense
    this->selectedDispensedVolumeMl = volumeMl;
}

double DrinkOrder::getSelectedPriceCustom()
{
    // slot and size needs to be set.
    double price;
    if (isSelectedOrderValid())
    {
        price = getPrice(getSelectedSize()) * (1.0 - m_discount_percentage_fraction) * getSelectedVolume();
    }
    else
    {

        qInfo() << "ERROR: no product set";
        price = 66.6;
    }
    return price;
}

double DrinkOrder::getVolume(int size)
{
    double volume;
    qInfo() << "Open db: get volume";
    DbManager db(DB_PATH);
    volume = db.getProductVolume(getSelectedSlot(), df_util::sizeIndexToChar(size));
    db.closeDB();
    return volume;
}

double DrinkOrder::getSelectedVolume()
{
    double volume;
    if (isSelectedOrderValid())
    {
        volume = getVolume(getSelectedSize());
    }
    else
    {
        qInfo() << "ERROR: No product set";
        volume = 66.6;
    }
    return volume;
}

QString DrinkOrder::getUnitsForSelectedSlot()
{

    qDebug() << "Open db:  units for label.";
    QString units;
    DbManager db(DB_PATH);
    units = db.getUnits(getSelectedSlot());
    // qDebug() << "Units: " << units;
    db.closeDB();
    return units;
}

QString DrinkOrder::getVolumePerTickAsStringForSelectedSlot()
{
    double vol_per_tick = getVolumePerTickForSelectedSlot();
    QString units = getUnitsForSelectedSlot();

    return df_util::getConvertedStringVolumeFromMl(vol_per_tick, units, false, true);
}

double DrinkOrder::getVolumePerTickForSelectedSlot()
{
    // ticks = db.getProductVolumePerTick(product_slot___);
    qInfo() << "Open db: get vol per tick";
    DbManager db(DB_PATH);
    double ml_per_tick = db.getProductVolumePerTick(getSelectedSlot());

    db.closeDB();
    return ml_per_tick;
}

void DrinkOrder::setVolumePerTickForSelectedSlot(QString volumePerTickInput)
{
    // ticks = db.getProductVolumePerTick(product_slot___);
    double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
    qInfo() << "Open db: set vol per tick";
    DbManager db(DB_PATH);
    db.updateVolumePerTick(getSelectedSlot(), ml_per_tick);

    db.closeDB();
}

void DrinkOrder::setSizeToVolumeForSelectedSlot(QString volumeInput, int size)
{
    double volume = inputTextToMlConvertUnits(volumeInput);
    qInfo() << "Open db: size to volume";
    DbManager db(DB_PATH);
    db.updateTargetVolume(getSelectedSlot(), size, volume);
    db.closeDB();
}

QString DrinkOrder::getVolumeRemainingCorrectUnits()
{
    qInfo() << "Open db: volume dispensed since last restock";
    DbManager db(DB_PATH);
    double volume = db.getVolumeRemaining(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

    return volume_as_string;
}

QString DrinkOrder::getVolumeDispensedSinceRestockCorrectUnits()
{
    qInfo() << "Open db:  volume dispensed since last restock";
    DbManager db(DB_PATH);
    double volume = db.getVolumeDispensedSinceRestock(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

    return volume_as_string;
}

QString DrinkOrder::getTotalDispensedCorrectUnits()
{
    qInfo() << "Open db:  volume dispensed";
    DbManager db(DB_PATH);
    double volume = db.getTotalDispensed(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

    return volume_as_string;
}

QString DrinkOrder::getSizeToVolumeWithCorrectUnitsForSelectedSlot(int size, bool roundValue, bool addUnits)
{
    QString volume_as_string;
    double v;
    double volume_oz;
    QString units;

    v = getVolume(size);
    units = getUnitsForSelectedSlot();
    volume_as_string = df_util::getConvertedStringVolumeFromMl(v, units, roundValue, addUnits);
    return volume_as_string;
}

double DrinkOrder::inputTextToMlConvertUnits(QString inputValueAsText)
{
    if (getUnitsForSelectedSlot() == "oz")
    {
        return df_util::convertOzToMl(inputValueAsText.toDouble());
    }
    else
    {
        return inputValueAsText.toDouble();
    }
}

QString DrinkOrder::getProductDrinkfillSerial(int slot)
{
    qDebug() << "Open db: get product id";
    DbManager db(DB_PATH);
    QString serial = db.getProductDrinkfillSerial(slot);
    db.closeDB();
    return serial;
}

void DrinkOrder::loadSelectedProductProperties()
{
    loadProductPropertiesFromDb(getSelectedSlot());
    loadSelectedProductPropertiesFromProductsFile();
}

void DrinkOrder::loadProductPropertiesFromDb(int slot)
{
    qDebug() << "Open db: db load product properties";
    DbManager db(DB_PATH);

    db.getProductProperties(slot, &m_product_id, m_sizeIndexIsEnabled);
    db.closeDB();
}

bool DrinkOrder::isProductVolumeInContainer(int slot)
{
    // QString product_id;
    // bool sizeIndexIsEnabled[SIZES_COUNT]; // size indeces.
     DbManager db(DB_PATH);
    // db.getProductProperties(slot, &product_id, sizeIndexIsEnabled);    
    
    if (db.getEmptyContainerDetectionEnabled())
    {
        return db.getVolumeRemaining(slot) > CONTAINER_EMPTY_THRESHOLD_ML;
    }
    else
    {
        return db.getVolumeRemaining(slot) > db.getProductVolume(slot, 'l'); // ----> TODO VERY BUGGY (only instance found of using char volume as a magic number)
    }
    db.closeDB();
}

void DrinkOrder::loadSelectedProductPropertiesFromProductsFile()
{
    getProductPropertiesFromProductsFile(m_product_id, &m_name_ui, &m_product_type, &m_description_ui, &m_features_ui, &m_ingredients_ui);
}

void DrinkOrder::getProductPropertiesFromProductsFile(QString product_id, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui)
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

        int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], product_id, Qt::CaseSensitive);
        if (compareResult == 0)
        {
            *name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
            *product_type = fields[CSV_PRODUCT_COL_TYPE];
            *description_ui = fields[CSV_PRODUCT_COL_DESCRIPTION_UI];
            *features_ui = fields[CSV_PRODUCT_COL_FEATURES_UI];
            *ingredients_ui = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];
            break;
        }
    }
    file.close();
}

QString DrinkOrder::getLoadedProductIngredients()
{
    return m_ingredients_ui;
}

QString DrinkOrder::getLoadedProductFeatures()
{
    return m_features_ui;
}
QString DrinkOrder::getLoadedProductName()
{
    return m_name_ui;
}
QString DrinkOrder::getLoadedProductDescription()
{
    return m_description_ui;
}

QString DrinkOrder::getSelectedProductPicturePath()
{

    // return QString(PRODUCT_PICTURES_ROOT_PATH).arg(m_product_id);
    return getProductPicturePath(getSelectedSlot());
}
QString DrinkOrder::getProductPicturePath(int slot)
{
    QString serial = getProductDrinkfillSerial(slot);
    return QString(PRODUCT_PICTURES_ROOT_PATH).arg(serial);
}

QString DrinkOrder::getProductType(int slot)
{
    QString product_id = getProductDrinkfillSerial(slot);
    QString name_ui;
    QString product_type;
    QString description_ui;
    QString features_ui;
    QString ingredients_ui;

    getProductPropertiesFromProductsFile(product_id, &name_ui, &product_type, &description_ui, &features_ui, &ingredients_ui);
    return product_type;
}

QString DrinkOrder::getProductName(int slot)
{
    QString product_id = getProductDrinkfillSerial(slot);

    QString name_ui;
    QString product_type;
    QString description_ui;
    QString features_ui;
    QString ingredients_ui;

    getProductPropertiesFromProductsFile(product_id, &name_ui, &product_type, &description_ui, &features_ui, &ingredients_ui);
    return name_ui;
}

QString DrinkOrder::getSelectedProductName()
{
    return getProductName(getSelectedSlot());
}

QString DrinkOrder::getMachineId()
{

    qDebug() << "Open db: getMachineID";

    DbManager db(DB_PATH);
    QString idString = db.getMachineID();
    db.closeDB();
    return idString;
}

QString DrinkOrder::getSelectedProductId()
{
    qDebug() << "Open db: get productId ";
    DbManager db(DB_PATH);
    QString idString = db.getProductID(getSelectedSlot());
    db.closeDB();
    return idString;
}

QString DrinkOrder::getFullVolumeCorrectUnits(bool addUnits)
{

    qDebug() << "Open db: get full volume ";
    DbManager db(DB_PATH);

    double volume = db.getFullProduct(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, addUnits);

    return volume_as_string;
}

void DrinkOrder::setFullVolumeCorrectUnits(QString inputFullValue)
{
    qDebug() << "Open db: for write full vol";
    DbManager db(DB_PATH);
    db.updateFullVolume(getSelectedSlot(), inputTextToMlConvertUnits(inputFullValue));
    db.closeDB();
}

QString DrinkOrder::getSelectedSizeToVolumeWithCorrectUnits(bool round, bool addUnits)
{
    return getSizeToVolumeWithCorrectUnitsForSelectedSlot(getSelectedSize(), round, addUnits);
}

QString DrinkOrder::getSelectedPaymentMethod()
{
    QString paymentMethod;
    qDebug() << "Open db: product payment method";
    DbManager db(DB_PATH);
    paymentMethod = db.getPaymentMethod(getSelectedSlot());
    db.closeDB();
    return paymentMethod;
}

int DrinkOrder::getSelectedDispenseSpeedPercentage()
{
    int pwm;

    qInfo() << "Open db: pwm speed";
    DbManager db(DB_PATH);
    pwm = db.getPWM(getSelectedSlot());
    db.closeDB();

    return (int)round((double(pwm) * 100) / 255);
}
void DrinkOrder::setSelectedDispenseSpeedPercentage(int percentage)
{
    int pwm = (int)round((percentage * 255) / 100);

    qInfo() << "Open db: pwm speed set";
    DbManager db(DB_PATH);
    db.updatePWM(getSelectedSlot(), pwm);
    db.closeDB();
}