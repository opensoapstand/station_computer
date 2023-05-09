// #include "product.h"
// #include "df_util.h" // lode added for settings
// #include "dbmanager.h"

// // Ctor
// Product::Product()
// {
//     selectedProduct = new ProductProperties;
//     // overruledPrice = INVALID_PRICE;
//     m_discount_percentage_fraction = 0.0;
//     m_promoCode = "";
// }

// // Ctor Object Copy
// // Product::Product(const DrinkOrder &other) : QObject(nullptr)
// // {
// //     selectedDrink = new DrinkSelection(*other.selectedDrink);
// // }

// void Product::loadProductPropertiesFromProductsFile(QString product_id){

//     // files as .csv or better .tsv (tabs as separator is easier the escaping comma's when needed in the text)

//     QFile file(PRODUCT_DETAILS_TSV_PATH);
//     if(!file.open(QIODevice::ReadOnly)) {
//         qDebug()<< "ERROR: Opening product details file. Expect unexpected behaviour now! ";
//         return;
//     }

//     QTextStream in(&file);
//     qDebug() << "Load csv file with product properties";

//     while(!in.atEnd()) {
//         QString line = in.readLine();    

//         QStringList fields = line.split("\t");
        
//         int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], product_id, Qt::CaseSensitive);
//         if (compareResult == 0){
//             m_name = fields[CSV_PRODUCT_COL_NAME];
//             m_description = fields[CSV_PRODUCT_COL_DESCRIPTION_UI ];
//             m_features = fields[CSV_PRODUCT_COL_FEATURES_UI];
//             m_ingredients = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];
//             m_name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
//             break;
//         }
//     }

//     file.close();

// }

// // Dtor
// Product::~Product()
// {
// }

// char Product::getSelectedSizeAsChar()
// {
//     // ! = invalid.
//     // t  test to fsm, but should become c for custom. we're so ready for it.
//     // t

//     return df_util::sizeIndexToChar(selectedSize);
// }

// int Product::getSelectedSize()
// {
//     // e.g. SIZE_SMALL_INDEX
//     return selectedSize;
// }

// void Product::setSelectedSize(int sizeOption)
// {
//     // index!!!!  e.g. 0=small
//     // overruledPrice = INVALID_PRICE;
//     // m_discount_percentage_fraction = 0.0;
//     selectedSize = sizeOption;
// }

// int Product::getSelectedSlot()
// {
//     return m_selectedSlot;
// }

// void Product::setSelectedSlot(int slot)
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

// bool Product::isSelectedOrderValid()
// {
//     if (!(m_selectedSlot >= OPTION_SLOT_INVALID && m_selectedSlot <= SLOT_COUNT))
//     {
//         qInfo() << "ERROR: no slot set. " << m_selectedSlot;
//         return false;
//     }
//     if (!(selectedSize >= 0 && selectedSize <= SIZES_COUNT))
//     {
//         qInfo() << "ERROR: no slot set. " << m_selectedSlot;
//         return false;
//     }

//     return true;
// }

// double Product::getPrice(int sizeIndex)
// {
//     // always from database
//     qDebug() << "db... get product price";
//     DbManager db(DB_PATH);
//     double price;
//     price = db.getProductPrice(getSelectedSlot(), df_util::sizeIndexToChar(sizeIndex));
//     qDebug() << "productsize: " << df_util::sizeIndexToChar(sizeIndex) << " price: " << price;
//     db.closeDB();
//     return price;
// }

// double Product::getSelectedPrice(){
//     return getPrice(getSelectedSize());
// }

// double Product::getVolume(int size)
// {
//     double volume;
//     qInfo() << "db.... vol seijsf";
//     DbManager db(DB_PATH);
//     volume = db.getProductVolume(getSelectedSlot(), df_util::sizeIndexToChar(size));
//     db.closeDB();
//     return volume;
// }

// double Product::getSelectedVolume()
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

// QString Product::getUnitsForSelectedSlot()
// {

//     qDebug() << "db units for label.";
//     QString units;
//     DbManager db(DB_PATH);
//     units = db.getUnits(getSelectedSlot());
//     // qDebug() << "Units: " << units;
//     db.closeDB();
//     return units;
// }

// QString Product::getVolumePerTickAsStringForSelectedSlot()
// {
//     double vol_per_tick = getVolumePerTickForSelectedSlot();
//     QString units = getUnitsForSelectedSlot();

//     return df_util::getConvertedStringVolumeFromMl(vol_per_tick, units, false, true);
// }

// double Product::getVolumePerTickForSelectedSlot()
// {
//     // ticks = db.getProductVolumePerTick(product_slot___);
//     qInfo() << "db.... vol per tick";
//     DbManager db(DB_PATH);
//     double ml_per_tick = db.getProductVolumePerTick(getSelectedSlot());

//     db.closeDB();
//     return ml_per_tick;
// }

// void Product::setVolumePerTickForSelectedSlot(QString volumePerTickInput)
// {
//     // ticks = db.getProductVolumePerTick(product_slot___);
//     double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
//     qInfo() << "db.... vol per tick";
//     DbManager db(DB_PATH);
//     db.updateVolumePerTick(getSelectedSlot(), ml_per_tick);

//     db.closeDB();
// }

// void Product::setSizeToVolumeForSelectedSlot(QString volumeInput, int size)
// {
//     double volume = inputTextToMlConvertUnits(volumeInput);
//     qInfo() << "db.... volume set";
//     DbManager db(DB_PATH);
//     db.updateTargetVolume(getSelectedSlot(), size, volume);
//     db.closeDB();
// }

// QString Product::getVolumeRemainingCorrectUnits()
// {
//     qInfo() << "db.... volume dispensed since last restock";
//     DbManager db(DB_PATH);
//     double volume = db.getVolumeRemaining(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

//     return volume_as_string;
// }

// QString Product::getVolumeDispensedSinceRestockCorrectUnits()
// {
//     qInfo() << "db.... volume dispensed since last restock";
//     DbManager db(DB_PATH);
//     double volume = db.getVolumeDispensedSinceRestock(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

//     return volume_as_string;
// }

// QString Product::getTotalDispensedCorrectUnits()
// {
//     qInfo() << "db.... volume dispensed";
//     DbManager db(DB_PATH);
//     double volume = db.getTotalDispensed(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

//     return volume_as_string;
// }

// QString Product::getSizeToVolumeWithCorrectUnitsForSelectedSlot(int size, bool roundValue, bool addUnits)
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

// double Product::inputTextToMlConvertUnits(QString inputValueAsText)
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

// QString Product::getProductDrinkfillSerial(int slot)
// {
//     qDebug() << "product db for drinkfill id";
//     DbManager db(DB_PATH);
//     QString serial = db.getProductDrinkfillSerial(slot);
//     db.closeDB();
//     return serial;
// }

// void Product::loadSelectedProductProperties()
// {
//     loadProductPropertiesFromDb(getSelectedSlot());
//     loadProductPropertiesFromProductsFile(m_product_id);
// }


// void Product::loadProductPropertiesFromDb(int slot)
// {
//     qDebug() << "db load product properties";
//     DbManager db(DB_PATH);

//     bool m_isEnabledSmall;
//     bool m_isEnabledMedium;
//     bool m_isEnabledLarge;
//     bool m_isEnabledCustom;

//     // db.getProductProperties(slot, &m_name, &m_description, &m_features, &m_ingredients, m_sizeIndexIsEnabled);
    
//     db.getProductProperties(slot, &m_product_id, m_sizeIndexIsEnabled);


//     db.closeDB();
// }



// bool Product::getLoadedProductSizeEnabled(int size)
// {
//     // caution!:  provide size index (0=small, ...)
//     return m_sizeIndexIsEnabled[size];
// }

// QString Product::getLoadedProductIngredients()
// {
//     return m_ingredients;
// }

// QString Product::getLoadedProductFeatures()
// {
//     return m_features;
// }
// QString Product::getLoadedProductName()
// {
//     return m_name;
// }
// QString Product::getLoadedProductDescription()
// {
//     return m_description;
// }

// QString Product::getSelectedProductPicturePath()
// {
    
//     //return QString(PRODUCT_PICTURES_ROOT_PATH).arg(m_product_id);
//     return getProductPicturePath(getSelectedSlot());
// }
// QString Product::getProductPicturePath(int slot)
// {
//     QString serial = getProductDrinkfillSerial(slot);
//     return QString(PRODUCT_PICTURES_ROOT_PATH).arg(serial);
// }

// QString Product::getProductName(int slot)
// {
//     // qDebug() << "product db for name";
//     // DbManager db(DB_PATH);
//     // QString product_name = db.getProductName(slot);
//     // db.closeDB();
//     // return product_name;
    
//     // qDebug() << "product db for name";
//     // DbManager db(DB_PATH);
//     // QString product_name = db.getProductName(slot);
//     // db.closeDB();
//     // return product_name;
//     return m_name;
//     //QString product_id = getProductDrinkfillSerial(slot);
// }

// QString Product::getSelectedProductName()
// {
//     return getProductName(getSelectedSlot());
// }

// QString Product::getMachineId()
// {

//     qDebug() << " db... getMachineID";

//     DbManager db(DB_PATH);
//     QString idString = db.getMachineID();
//     db.closeDB();
//     return idString;
// }

// QString Product::getSelectedProductId()
// {
//     qDebug() << "db.... get productId ";
//     DbManager db(DB_PATH);
//     QString idString = db.getProductID(getSelectedSlot());
//     db.closeDB();
//     return idString;
// }

// QString Product::getFullVolumeCorrectUnits(bool addUnits)
// {

//     qDebug() << "db.... get full volume ";
//     DbManager db(DB_PATH);

//     double volume = db.getFullProduct(getSelectedSlot());
//     db.closeDB();

//     QString units = getUnitsForSelectedSlot();
//     QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, addUnits);

//     return volume_as_string;
// }

// void Product::setFullVolumeCorrectUnits(QString inputFullValue)
// {
//     qDebug() << "db.... for write full vol";
//     DbManager db(DB_PATH);
//     db.updateFullVolume(getSelectedSlot(), inputTextToMlConvertUnits(inputFullValue));
//     db.closeDB();
// }

// QString Product::getSelectedSizeToVolumeWithCorrectUnits(bool round, bool addUnits)
// {
//     // v = db.getProductVolume(product_slot___, drinkSize);

//     // ui->label_size_small->setText(QString::number(v) + "ml");

//     return getSizeToVolumeWithCorrectUnitsForSelectedSlot(getSelectedSize(), round, addUnits);
// }

// QString Product::getSelectedPaymentMethod()
// {
//     QString paymentMethod;
//     qDebug() << "product pyament method";
//     DbManager db(DB_PATH);
//     paymentMethod = db.getPaymentMethod(getSelectedSlot());
//     db.closeDB();
//     return paymentMethod;
// }

// int Product::getSelectedDispenseSpeedPercentage()
// {
//     int pwm;

//     qInfo() << "db.... pwm speed";
//     DbManager db(DB_PATH);
//     pwm = db.getPWM(getSelectedSlot());
//     db.closeDB();

//     return (int)round((double(pwm) * 100) / 255);
// }
// void Product::setSelectedDispenseSpeedPercentage(int percentage)
// {
//     int pwm = (int)round((percentage * 255) / 100);

//     qInfo() << "db.... pwm speed set";
//     DbManager db(DB_PATH);
//     db.updatePWM(getSelectedSlot(), pwm);
//     db.closeDB();
// }
