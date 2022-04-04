#include "drinkorder.h"
#include "page_idle.h"
#include "df_util.h" // lode added for settings

// Ctor
DrinkOrder::DrinkOrder()
{
    selectedDrink = new DrinkSelection;
    overruledPrice = INVALID_PRICE;
    // qDebug() << "ahoyy2" ;
    // DbManager db(DB_PATH);

    // DRINK1_SIZE_ML_L = db.getProductVolume(1, 'l');
    // DRINK2_SIZE_ML_L = db.getProductVolume(2, 'l');
    // DRINK3_SIZE_ML_L = db.getProductVolume(3, 'l');
    // DRINK4_SIZE_ML_L = db.getProductVolume(4, 'l');

    // DRINK1_SIZE_ML_S = db.getProductVolume(1, 's');
    // DRINK2_SIZE_ML_S = db.getProductVolume(2, 's');
    // DRINK3_SIZE_ML_S = db.getProductVolume(3, 's');
    // DRINK4_SIZE_ML_S = db.getProductVolume(4, 's');

    // DRINK1_PRICE_LARGE = db.getProductPrice(1, 'l');
    // DRINK2_PRICE_LARGE = db.getProductPrice(2, 'l');
    // DRINK3_PRICE_LARGE = db.getProductPrice(3, 'l');
    // DRINK4_PRICE_LARGE = db.getProductPrice(4, 'l');

    // DRINK1_price_small = db.getProductPrice(1, 's');
    // DRINK2_price_small = db.getProductPrice(2, 's');
    // DRINK3_price_small = db.getProductPrice(3, 's');
    // DRINK4_price_small = db.getProductPrice(4, 's');

    // db.closeDB();
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

int DrinkOrder::getSelectedSize()
{
    // e.g. SIZE_SMALL_INDEX
    return selectedSize;
}

void DrinkOrder::setSelectedSize(int sizeOption)
{
    overruledPrice = INVALID_PRICE;
    selectedSize = sizeOption;
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
            overruledPrice = INVALID_PRICE;
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
        qInfo() << "ERROR: no slot set. " << m_selectedSlot;
        return false;
    }

    return true;
}

void DrinkOrder::setSelectedOverrulePrice(double price)
{

    if (isSelectedOrderValid())
    {
        qInfo() << "Set overrruled price.";

        if (price != overruledPrice)
        {
            overruledPrice = price;
            emit priceChange(overruledPrice);
        }
    }
    else
    {
        qInfo() << "ERROR: no overruled price set. ";
    }
}

void DrinkOrder::setPriceSelected(int size, double price)
{
    qDebug() << "db... set product price";
    DbManager db(DB_PATH);
    db.updatePrice(getSelectedSlot(), size, price);
    db.closeDB();
}

double DrinkOrder::getPrice(int sizeIndex)
{
    qDebug() << "db... get product price";
    DbManager db(DB_PATH);
    double price;
    price = db.getProductPrice(getSelectedSlot(), df_util::sizeIndexToChar(sizeIndex));
    qDebug() << "productsize: " << df_util::sizeIndexToChar(sizeIndex) << " price: " << price;
    db.closeDB();
    return price;
}

double DrinkOrder::getSelectedPrice()
{
    // slot and size needs to be set.
    double price;
    if (isSelectedOrderValid())
    {
        if (overruledPrice != INVALID_PRICE)
        {
            qInfo() << "Overruled price is set.";
            price = overruledPrice;
        }
        else
        {
            // qInfo() << "db....pricess.....";
            // DbManager db(DB_PATH);
            // price = db.getProductPrice(getSelectedSlot(), getSelectedSizeAsChar());
            // db.closeDB();
            price = getPrice(getSelectedSize());
        }
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
    qInfo() << "db.... vol seijsf";
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

    qDebug() << "db units for label.";
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
    qInfo() << "db.... vol per tick";
    DbManager db(DB_PATH);
    double ml_per_tick = db.getProductVolumePerTick(getSelectedSlot());

    db.closeDB();
    return ml_per_tick;
}

void DrinkOrder::setVolumePerTickForSelectedSlot(QString volumePerTickInput)
{
    // ticks = db.getProductVolumePerTick(product_slot___);
    double ml_per_tick = inputTextToMlConvertUnits(volumePerTickInput);
    qInfo() << "db.... vol per tick";
    DbManager db(DB_PATH);
    db.updateVolumePerTick(getSelectedSlot(), ml_per_tick);

    db.closeDB();
}

void DrinkOrder::setSizeToVolumeForSelectedSlot(QString volumeInput, int size)
{
    double volume = inputTextToMlConvertUnits(volumeInput);
    qInfo() << "db.... volume set";
    DbManager db(DB_PATH);
    db.updateTargetVolume(getSelectedSlot(), size, volume);
    db.closeDB();
}

QString DrinkOrder::getVolumeRemainingCorrectUnits()
{
    qInfo() << "db.... volume dispensed since last restock";
    DbManager db(DB_PATH);
    double volume = db.getVolumeRemaining(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

    return volume_as_string;
}

QString DrinkOrder::getVolumeDispensedSinceRestockCorrectUnits()
{
    qInfo() << "db.... volume dispensed since last restock";
    DbManager db(DB_PATH);
    double volume = db.getVolumeDispensedSinceRestock(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, true);

    return volume_as_string;
}

QString DrinkOrder::getTotalDispensedCorrectUnits()
{
    qInfo() << "db.... volume dispensed";
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

QString DrinkOrder::getProductName(int slot)
{
qDebug() << "product db for name";
    DbManager db(DB_PATH);
    QString product_name = db.getProductName(slot);
    db.closeDB();
    return product_name;

}

QString DrinkOrder::getSelectedProductName()
{
   return  getProductName(getSelectedSlot());
}


QString DrinkOrder::getMachineId(){

    qDebug() << " db... getMachineID";

    DbManager db(DB_PATH);
    QString idString = db.getMachineID();
    db.closeDB();
    return idString;
}

QString DrinkOrder::getSelectedProductId()
{
    qDebug() << "db.... get productId ";
    DbManager db(DB_PATH);
    QString idString = db.getProductID(getSelectedSlot());
    db.closeDB();
    return idString;
}

QString DrinkOrder::getFullVolumeCorrectUnits(bool addUnits)
{

    qDebug() << "db.... get full volume ";
    DbManager db(DB_PATH);

    double volume = db.getFullProduct(getSelectedSlot());
    db.closeDB();

    QString units = getUnitsForSelectedSlot();
    QString volume_as_string = df_util::getConvertedStringVolumeFromMl(volume, units, false, addUnits);

    return volume_as_string;
}

void DrinkOrder::setFullVolumeCorrectUnits(QString inputFullValue)
{
    qDebug() << "db.... for write full vol";
    DbManager db(DB_PATH);
    db.updateFullVolume(getSelectedSlot(), inputTextToMlConvertUnits(inputFullValue));
    db.closeDB();
}

QString DrinkOrder::getSelectedSizeToVolumeWithCorrectUnits(bool round, bool addUnits)
{
    // v = db.getProductVolume(product_slot___, drinkSize);

    // ui->label_size_small->setText(QString::number(v) + "ml");

    return getSizeToVolumeWithCorrectUnitsForSelectedSlot(getSelectedSize(), round, addUnits);
}


QString DrinkOrder::getSelectedPaymentMethod()
{
    QString paymentMethod;
    qDebug() << "product pyament method";
    DbManager db(DB_PATH);
    paymentMethod = db.getPaymentMethod(getSelectedSlot());
    db.closeDB();
    return paymentMethod;
}

int DrinkOrder::getSelectedDispenseSpeedPercentage()
{
    int pwm;

    qInfo() << "db.... pwm speed";
    DbManager db(DB_PATH);
    pwm = db.getPWM(getSelectedSlot());
    db.closeDB();

    return (int)round((double(pwm) * 100) / 255);
}
void DrinkOrder::setSelectedDispenseSpeedPercentage(int percentage)
{
    int pwm = (int)round((percentage * 255) / 100);

    qInfo() << "db.... pwm speed set";
    DbManager db(DB_PATH);
    db.updatePWM(getSelectedSlot(), pwm);
    db.closeDB();
}
