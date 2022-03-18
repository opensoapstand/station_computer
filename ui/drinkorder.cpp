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

double DrinkOrder::getPrice(int sizeIndex)
{
    qDebug() << "product db for price";
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

double DrinkOrder::getVolume(int size)
{
    double volume;
    qInfo() << "db.... vol seijsf";
    DbManager db(DB_PATH);
    volume = db.getProductVolume(getSelectedSlot(), df_util::sizeIndexToChar(size));
    db.closeDB();
    return volume;
}

QString DrinkOrder::getSizeToVolumeWithCorrectUnitsForSelectedSlot(int size)
{
    QString volume_as_string;
    double v;
    double volume_oz;
    QString units;

    v = getVolume(size);
    volume_oz = ceil(v * ML_TO_OZ);

    // db.getProductVolume(getSelectedSlot(), df_util::sizeIndexToChar(size));

    qDebug() << "db units for label.";
    DbManager db(DB_PATH);
    units = db.getUnits(getSelectedSlot());
    // qDebug() << "Units: " << units;
    db.closeDB();

    if (units == "l" || units == "ml")
    {

        if (v < 1000)
        {
            volume_as_string = QString::number(v, 'f', 0) + "ml";
            // volume_as_string = "ldoefef";
        }
        else
        {
            volume_as_string = QString::number(v / 1000, 'f', 2) + "L";
        }
    }
    else if (units == "oz")
    {
        volume_as_string = QString::number(volume_oz, 'f', 0) + "oz";
    }
    else
    {
        qDebug() << "Unhandled unit system: " << units;
    }
    qDebug() << "vol: " << volume_as_string << " .. units: " << units << " vol metric: " << v << "vol oz: " << volume_oz;
    return volume_as_string;
}
QString DrinkOrder::getSelectedSizeToVolumeWithCorrectUnits()
{
    // v = db.getProductVolume(product_slot___, drinkSize);

    // ui->label_size_small->setText(QString::number(v) + "ml");

    return getSizeToVolumeWithCorrectUnitsForSelectedSlot(getSelectedSize());
}

QString DrinkOrder::getSelectedProductName()
{
    qDebug() << "product db for name";
    DbManager db(DB_PATH);
    QString product_name = db.getProductName(getSelectedSlot());
    db.closeDB();
    return product_name;
}

QString DrinkOrder::getSelectedPaymentMethod(){
    QString paymentMethod;
    qDebug() << "product pyament method";
    DbManager db(DB_PATH);
    paymentMethod = db.getPaymentMethod(getSelectedSlot());
    db.closeDB();
    return paymentMethod;

}

