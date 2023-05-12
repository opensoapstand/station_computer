// #include "df_util.h" // lode added for settings
// #include "dbmanager.h"
// #include "page_dispenser.h"
// #include "page_dispenser.h"

// // Ctor
// product::product()
// {
//     selectedProduct = new productSelection;
//     // overruledPrice = INVALID_PRICE;
//     m_discount_percentage_fraction = 0.0;
//     m_promoCode = "";
// }

// // Ctor Object Copy
// product::product(const product &other) : QObject(nullptr)
// {
//     selectedProduct = new productSelection(*other.selectedProduct);
//     dispenser myDispenser;

//     slot = myDispenser.getSlot();
// }

// // Dtor
// product::~product()
// {
//     delete selectedProduct;
// }

// // Object Reassignment
// product &product::operator=(const product &other)
// {
//     *selectedProduct = *other.selectedProduct;
//     return *this;
// }

// void product::loadFromDb(int slot)
// {
//     qDebug() << "Open db: db load product properties";
//     DbManager db(DB_PATH);
//     int slot = myDispenser.getSlot();
//     m_product_id = db.getProductID(slot);
//     soapstand_product_serial = db.getProductDrinkfillSerial(slot);
//     size_unit = getUnits(slot);
//     payment = getPaymentMethod(slot):
//     // volume_full;
//     // volume_remaining;
//     // volume_dispensed_since_restock;
//     // douvolume_dispensed_toal;
//     // is_enabled_small;
    
// ;    db.closeDB();
// }

// void product::setSlot(int slot)
// {
//     // first slot is 1
//     this->slot = slot;
// }

// void product::loadProductPropertiesFromProductsFile()
// {
//     getProductPropertiesFromProductsFile(m_product_id, &m_name_ui, &m_product_type, &m_description_ui, &m_features_ui, &m_ingredients_ui);
// }

// char product::getSelectedSizeAsChar()
// {
//     // ! = invalid.
//     // t  test to fsm, but should become c for custom. we're so ready for it.
//     // t

//     return df_util::sizeIndexToChar(selectedSize);
// }

// void product::getCustomDiscountDetails(bool *large_volume_discount_is_enabled, double *min_volume_for_discount, double *discount_price_per_liter)
// {
//     qDebug() << "Open db: get bulk volume discount details ";

//     DbManager db(DB_PATH);
//     db.getCustomDiscountProperties(getSelectedSlot(), large_volume_discount_is_enabled, min_volume_for_discount, discount_price_per_liter);
//     db.closeDB();
// }

// int product::getSelectedSize()
// {
//     // e.g. SIZE_SMALL_INDEX
//     return selectedSize;
// }

// void product::setSelectedSize(int sizeIndex)
// {
//     // index!!!!  e.g. 0=small
//     // overruledPrice = INVALID_PRICE;
//     // m_discount_percentage_fraction = 0.0;
//     qDebug() << "Set size index: " << sizeIndex;
//     selectedSize = sizeIndex;
// }

// void product::setLoadedProductBiggestEnabledSizeIndex()
// {
//     setSelectedSize(getLoadedProductBiggestEnabledSizeIndex());
// }

// int product::getLoadedProductBiggestEnabledSizeIndex()
// {

//     // cascade to largest size. Custom volume is seen as superior.
//     int maxSize = SIZE_SMALL_INDEX;
//     if (getLoadedProductSizeEnabled(SIZE_MEDIUM_INDEX))
//     {
//         maxSize = SIZE_MEDIUM_INDEX;
//     }
//     if (getLoadedProductSizeEnabled(SIZE_LARGE_INDEX))
//     {
//         maxSize = SIZE_LARGE_INDEX;
//     }
//     if (getLoadedProductSizeEnabled(SIZE_CUSTOM_INDEX))
//     {
//         maxSize = SIZE_CUSTOM_INDEX;
//     }
//     return maxSize;
// }

// bool product::getLoadedProductSizeEnabled(int size)
// {
//     // caution!:  provide size index (0=small, ...)
//     return m_sizeIndexIsEnabled[size];
// }

// int product::getSelectedSlot()
// {
//     return m_selectedSlot;
// }

// void product::setSelectedSlot(int slot)
// {

//     if (slot >= OPTION_SLOT_INVALID && slot <= SLOT_COUNT)
//     {

//         if (slot != getSelectedSlot())
//         {
//             // overruledPrice = INVALID_PRICE;
//             // m_discount_percentage_fraction = 0.0;
//             m_selectedSlot = slot;
//             emit orderSlotChange(slot);
//         }
//     }
//     else
//     {
//         qInfo() << "OUT OF OPTION RANGE!" << slot;
//     }
// }

// // SLOTS Section

// bool product::isSelectedOrderValid()
// {
//     if (!(m_selectedSlot >= OPTION_SLOT_INVALID && m_selectedSlot <= SLOT_COUNT))
//     {
//         qInfo() << "ERROR: no slot set. " << m_selectedSlot;
//         return false;
//     }
//     if (!(selectedSize >= 0 && selectedSize <= SIZES_COUNT))
//     {
//         qInfo() << "ERROR: no size set. " << m_selectedSlot;
//         return false;
//     }

//     return true;
// }

// double product::getDiscount()
// {
//     // qDebug() << "--------=========" << QString::number(getSelectedPriceCorrected());

//     // the discount is the original price minus the discounted price
//     return getPrice(getSelectedSize()) - getSelectedPriceCorrected();
// }

// double product::getDiscountPercentageFraction()
// {
//     return m_discount_percentage_fraction;
// }

// QString product::getPromoCode()
// {
//     return m_promoCode;
// }

// void product::setDiscountPercentageFraction(double percentageFraction)
// {
//     // ratio = percentage / 100;
//     qDebug() << "Set discount percentage fraction: " << QString::number(percentageFraction, 'f', 3);
//     m_discount_percentage_fraction = percentageFraction;

    
// }

// void product::setPromoCode(QString promoCode)
// {
//     // ratio = percentage / 100;
//     qDebug() << "Set Promo Code: " << promoCode;
//     m_promoCode = promoCode;
// }

// void product::setPriceSelected(int size, double price)
// {
//     qDebug() << "Open db: set product price";
//     DbManager db(DB_PATH);
//     db.updatePrice(getSelectedSlot(), size, price);
//     db.closeDB();
// }

// double product::getPrice(int sizeIndex)
// {
//     // always from database
//     qDebug() << "Open db: get product price";
//     DbManager db(DB_PATH);
//     double price;
//     price = db.getProductPrice(getSelectedSlot(), df_util::sizeIndexToChar(sizeIndex));
//     qDebug() << "productsize: " << df_util::sizeIndexToChar(sizeIndex) << " price: " << price;
//     db.closeDB();
//     return price;
// }

// double product::getSelectedPrice()
// {
//     return getPrice(getSelectedSize());
// }

// double product::getSelectedPriceCorrected()
// {
//     // slot and size needs to be set.
//     double price;
//     if (isSelectedOrderValid())
//     {
//         price = getPrice(getSelectedSize()) * (1.0 - m_discount_percentage_fraction);
//     }
//     else
//     {

//         qInfo() << "ERROR: no product set";
//         price = 66.6;
//     }
//     return price;
// }

// void product::resetSelectedVolumeDispensed()
// {
//     this->selectedDispensedVolumeMl = 0;
// }

// double product::getSelectedVolumeDispensedMl()
// {
//     // volume for this dispense
//     return this->selectedDispensedVolumeMl;
// }

// void product::setSelectedVolumeDispensedMl(double volumeMl)
// {
//     // volume for this dispense
//     this->selectedDispensedVolumeMl = volumeMl;
// }

// double product::getSelectedPriceCustom()
// {
//     // slot and size needs to be set.
//     double price;
//     if (isSelectedOrderValid())
//     {
//         price = getPrice(getSelectedSize()) * (1.0 - m_discount_percentage_fraction) * getSelectedVolume();
//     }
//     else
//     {

//         qInfo() << "ERROR: no product set";
//         price = 66.6;
//     }
//     return price;
// }

// double product::getVolume(int size)
// {
//     double volume;
//     qInfo() << "Open db: get volume";
//     DbManager db(DB_PATH);
//     volume = db.getProductVolume(getSelectedSlot(), df_util::sizeIndexToChar(size));
//     db.closeDB();
//     return volume;
// }

// double product::getSelectedVolume()
// {
//     double volume;
//     if (isSelectedOrderValid())
//     {
//         volume = getVolume(getSelectedSize());
//     }
//     else
//     {
//         qInfo() << "ERROR: No product set";
//         volume = 66.6;
//     }
//     return volume;
// }

// QString product::getUnitsForSelectedSlot()
// {

//     qDebug() << "Open db:  units for label.";
//     QString units;
//     DbManager db(DB_PATH);
//     units = db.getUnits(getSelectedSlot());
//     // qDebug() << "Units: " << units;
//     db.closeDB();
//     return units;
// }

// QString product::getVolumePerTickAsStringForSelectedSlot()
// {
//     double vol_per_tick = getVolumePerTickForSelectedSlot();
//     QString units = getUnitsForSelectedSlot();

//     return df_util::getConvertedStringVolumeFromMl(vol_per_tick, units, false, true);
// }

// double product::getVolumePerTickForSelectedSlot()
// {
//     // ticks = db.getProductVolumePerTick(product_slot___);
//     qInfo() << "Open db: get vol per tick";
//     DbManager db(DB_PATH);
//     double ml_per_tick = db.getProductVolumePerTick(getSelectedSlot());

//     db.closeDB();
//     return ml_per_tick;
// }

// void product::setVolumePerTickForSelectedSlot(QString volumePerTickInput)
// {
//     // ticks = db.getProductVolumePerTick(product_slot___);
//     double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
//     qInfo() << "Open db: set vol per tick";
//     DbManager db(DB_PATH);
//     db.updateVolumePerTick(getSelectedSlot(), ml_per_tick);

//     db.closeDB();
// }

// void product::setSizeToVolumeForSelectedSlot(QString volumeInput, int size)
// {
//     double volume = inputTextToMlConvertUnits(volumeInput);
//     qInfo() << "Open db: size to volume";
//     DbManager db(DB_PATH);
//     db.updateTargetVolume(getSelectedSlot(), size, volume);
//     db.closeDB();
// }

// QString product::getVolumeRemainingCorrectUnits()
// {
//     qInfo() << "Open db: volume dispensed since last restock";
//     DbManager db(DB_PATH);
//     double volume = db.getVolumeRemaining(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

//     return volume_as_string;
// }

// QString product::getVolumeDispensedSinceRestockCorrectUnits()
// {
//     qInfo() << "Open db:  volume dispensed since last restock";
//     DbManager db(DB_PATH);
//     double volume = db.getVolumeDispensedSinceRestock(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

//     return volume_as_string;
// }

// QString product::getTotalDispensedCorrectUnits()
// {
//     qInfo() << "Open db:  volume dispensed";
//     DbManager db(DB_PATH);
//     double volume = db.getTotalDispensed(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

//     return volume_as_string;
// }

// QString product::getSizeToVolumeWithCorrectUnitsForSelectedSlot(int size, bool roundValue, bool addUnits)
// {
//     QString volume_as_string;
//     double v;
//     double volume_oz;
//     QString units;

//     v = getVolume(size);
//     units = getUnitsForSelectedSlot();
//     volume_as_string = df_util::getConvertedStringVolumeFromMl(v, units, roundValue, addUnits);
//     return volume_as_string;
// }

// double product::inputTextToMlConvertUnits(QString inputValueAsText)
// {
//     if (getUnitsForSelectedSlot() == "oz")
//     {
//         return df_util::convertOzToMl(inputValueAsText.toDouble());
//     }
//     else
//     {
//         return inputValueAsText.toDouble();
//     }
// }

// QString product::getProductDrinkfillSerial(int slot)
// {
//     qDebug() << "Open db: get product id";
//     DbManager db(DB_PATH);
//     QString serial = db.getProductDrinkfillSerial(slot);
//     db.closeDB();
//     return serial;
// }

// void product::loadSelectedProductProperties()
// {
//     loadProductPropertiesFromDb(getSelectedSlot());
//     loadProductPropertiesFromProductsFile();
// }

// void product::loadProductPropertiesFromDb(int slot)
// {
//     qDebug() << "Open db: db load product properties";
//     DbManager db(DB_PATH);

//     db.getProductProperties(slot, &m_product_id, m_sizeIndexIsEnabled);
//     db.closeDB();
// }

// void product::loadProductPropertiesFromProductsFile()
// {
//     getProductPropertiesFromProductsFile(m_product_id, &m_name_ui, &m_product_type, &m_description_ui, &m_features_ui, &m_ingredients_ui);
// }

// void product::getProductPropertiesFromProductsFile(QString product_id, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui)
// {
//     QFile file(PRODUCT_DETAILS_TSV_PATH);
//     if (!file.open(QIODevice::ReadOnly))
//     {
//         qDebug() << "ERROR: Opening product details file. Expect unexpected behaviour now! ";
//         return;
//     }

//     QTextStream in(&file);
//     qDebug() << "Load csv file with product properties";

//     while (!in.atEnd())
//     {
//         QString line = in.readLine();

//         QStringList fields = line.split("\t");

//         int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], product_id, Qt::CaseSensitive);
//         if (compareResult == 0)
//         {
//             *name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
//             *product_type = fields[CSV_PRODUCT_COL_TYPE];
//             *description_ui = fields[CSV_PRODUCT_COL_DESCRIPTION_UI];
//             *features_ui = fields[CSV_PRODUCT_COL_FEATURES_UI];
//             *ingredients_ui = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];
//             break;
//         }
//     }
//     file.close();
// }

// QString product::getLoadedProductIngredients()
// {
//     return m_ingredients_ui;
// }

// QString product::getLoadedProductFeatures()
// {
//     return m_features_ui;
// }
// QString product::getLoadedProductName()
// {
//     return m_name_ui;
// }
// QString product::getLoadedProductDescription()
// {
//     return m_description_ui;
// }

// QString product::getSelectedProductPicturePath()
// {

//     // return QString(PRODUCT_PICTURES_ROOT_PATH).arg(m_product_id);
//     return getProductPicturePath(getSelectedSlot());
// }
// QString product::getProductPicturePath(int slot)
// {
//     QString serial = getProductDrinkfillSerial(slot);
//     return QString(PRODUCT_PICTURES_ROOT_PATH).arg(serial);
// }

// QString product::getProductType(int slot)
// {
//     QString product_id = getProductDrinkfillSerial(slot);
//     QString name_ui;
//     QString product_type;
//     QString description_ui;
//     QString features_ui;
//     QString ingredients_ui;

//     getProductPropertiesFromProductsFile(product_id, &name_ui, &product_type, &description_ui, &features_ui, &ingredients_ui);
//     return product_type;
// }

// QString product::getProductName(int slot)
// {
//     QString product_id = getProductDrinkfillSerial(slot);

//     QString name_ui;
//     QString product_type;
//     QString description_ui;
//     QString features_ui;
//     QString ingredients_ui;

//     getProductPropertiesFromProductsFile(product_id, &name_ui, &product_type, &description_ui, &features_ui, &ingredients_ui);
//     return name_ui;
// }

// QString product::getSelectedProductName()
// {
//     return getProductName(getSelectedSlot());
// }

// QString product::getMachineId()
// {

//     qDebug() << "Open db: getMachineID";

//     DbManager db(DB_PATH);
//     QString idString = db.getMachineID();
//     db.closeDB();
//     return idString;
// }

// QString product::getSelectedProductId()
// {
//     qDebug() << "Open db: get productId ";
//     DbManager db(DB_PATH);
//     QString idString = db.getProductID(getSelectedSlot());
//     db.closeDB();
//     return idString;
// }

// QString product::getFullVolumeCorrectUnits(bool addUnits)
// {

//     qDebug() << "Open db: get full volume ";
//     DbManager db(DB_PATH);

//     double volume = db.getFullProduct(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, addUnits);

//     return volume_as_string;
// }

// void product::setFullVolumeCorrectUnits(QString inputFullValue)
// {
//     qDebug() << "Open db: for write full vol";
//     DbManager db(DB_PATH);
//     db.updateFullVolume(getSelectedSlot(), inputTextToMlConvertUnits(inputFullValue));
//     db.closeDB();
// }

// QString product::getSizeToVolumeWithCorrectUnits(bool round, bool addUnits)
// {
//     return getSizeToVolumeWithCorrectUnitsForSelectedSlot(getSelectedSize(), round, addUnits);
// }

// QString product::getPaymentMethod()
// {
//     QString paymentMethod;
//     qDebug() << "Open db: product payment method";
//     DbManager db(DB_PATH);
//     paymentMethod = db.getPaymentMethod(getSelectedSlot());
//     db.closeDB();
//     return paymentMethod;
// }

// int product::getDispenseSpeedPercentage()
// {
//     int pwm;

//     qInfo() << "Open db: pwm speed";
//     DbManager db(DB_PATH);
//     pwm = db.getPWM(getSelectedSlot());
//     db.closeDB();

//     return (int)round((double(pwm) * 100) / 255);
// }
// void product::setDispenseSpeedPercentage(int percentage)
// {
//     int pwm = (int)round((percentage * 255) / 100);

//     qInfo() << "Open db: pwm speed set";
//     DbManager db(DB_PATH);
//     db.updatePWM(getSelectedSlot(), pwm);
//     db.closeDB();
// }



