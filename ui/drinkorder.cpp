#include "drinkorder.h"
#include "idle.h"

// Ctor
DrinkOrder::DrinkOrder()
{
    selectedDrink = new DrinkSelection;
    DbManager db(DB_PATH);

    DRINK1_SIZE_ML_L = db.getProductVolume(1, 'l');
    DRINK2_SIZE_ML_L = db.getProductVolume(2, 'l');
    DRINK3_SIZE_ML_L = db.getProductVolume(3, 'l');
    DRINK4_SIZE_ML_L = db.getProductVolume(4, 'l');

    DRINK1_SIZE_ML_S = db.getProductVolume(1, 's');
    DRINK2_SIZE_ML_S = db.getProductVolume(2, 's');
    DRINK3_SIZE_ML_S = db.getProductVolume(3, 's');
    DRINK4_SIZE_ML_S = db.getProductVolume(4, 's');

    DRINK1_PRICE_L = db.getProductPrice(1, 'l');
    DRINK2_PRICE_L = db.getProductPrice(2, 'l');
    DRINK3_PRICE_L = db.getProductPrice(3, 'l');
    DRINK4_PRICE_L = db.getProductPrice(4, 'l');

    DRINK1_PRICE_S = db.getProductPrice(1, 's');
    DRINK2_PRICE_S = db.getProductPrice(2, 's');
    DRINK3_PRICE_S = db.getProductPrice(3, 's');
    DRINK4_PRICE_S = db.getProductPrice(4, 's');

}

// Ctor Object Copy
DrinkOrder::DrinkOrder(const DrinkOrder& other ):QObject(nullptr)
{
    selectedDrink = new DrinkSelection( *other.selectedDrink );
}

// Dtor
DrinkOrder::~DrinkOrder()
{
    delete selectedDrink;
}

// Object Reassignment
DrinkOrder& DrinkOrder::operator=( const DrinkOrder& other )
{
    *selectedDrink = *other.selectedDrink;
    return *this;
}


// SLOTS Section
void DrinkOrder::changeOption(int optNumber)
{
    if(optNumber != getOption()) {
        m_optionNumber = optNumber;
        emit optionChange(optNumber);
    }
}

void DrinkOrder::setPrice(double price)
{
    if(price != getPrice()) {
        m_drinkPrice = price;
        emit priceChange(price);
    }
}

void DrinkOrder::setSize(double size)
{
    if(size != getSize()) {
        m_drinkML = size;
        emit sizeChange(size);
    }
}

// Enum search for drink sizes
void DrinkOrder::setDrinkSize(DF_QT_OPTIONS sizeOption) {
    //qInfo() << "Existing Size" << getSize() << endl;

    switch (sizeOption)
    {
//    case(SMALL_DRINK):
//        setSize(SMALL_SIZE_ML);
//        setPrice(PRICE_SMALL_TEST);
//        break;

//    case(LARGE_DRINK):
//        setSize(LARGE_SIZE_ML);
//        setPrice(PRICE_LARGE_TEST);
//        break;

    case(DRINK1):
        setSize(DRINK1_SIZE_ML_L);
        setPrice(DRINK1_PRICE_L);
        break;

    case(DRINK2):
        setSize(DRINK2_SIZE_ML_L);
        setPrice(DRINK2_PRICE_L);
        break;

    case(DRINK3):
        setSize(DRINK3_SIZE_ML_L);
        setPrice(DRINK3_PRICE_L);
        break;

    case(DRINK4):
        setSize(DRINK4_SIZE_ML_L);
        setPrice(DRINK4_PRICE_L);
        break;

    default:
        setSize(DRINK4);
        break;
    }

    //qInfo() << "Order Size" << getSize() << endl;
}

// Enum search for option slot
void DrinkOrder::setDrinkOption(DF_QT_OPTIONS sizeOption) {
   // qInfo() << "Current Option" << getOption() << endl;

    if(sizeOption >= OPTION_SLOT && sizeOption <= DRINK_SIZE_OPTIONS)
    {
        switch (sizeOption)
        {
        case(OPTION_SLOT_1):
            changeOption(OPTION_SLOT_1);
            break;
        case(OPTION_SLOT_2):
            changeOption(OPTION_SLOT_2);
            break;
        case(OPTION_SLOT_3):
            changeOption(OPTION_SLOT_3);
            break;
        case(OPTION_SLOT_4):
            changeOption(OPTION_SLOT_4);
            break;
//        case(OPTION_SLOT_5):
//            changeOption(OPTION_SLOT_5);
//            break;
//        case(OPTION_SLOT_6):
//            changeOption(OPTION_SLOT_6);
//            break;
//        case(OPTION_SLOT_7):
//            changeOption(OPTION_SLOT_7);
//            break;
//        case(OPTION_SLOT_8):
//            changeOption(OPTION_SLOT_8);
//            break;
//        case(OPTION_SLOT_9):
//            changeOption(OPTION_SLOT_9);
            break;
        default:
            setSize(OPTION_SLOT);
            break;
        }
    } else {
        qInfo() << "OUT OF OPTION RANGE!" << sizeOption << endl;
    }

    // HACK: For now asssume small sized order; Will need to validate for empty.
    setDrinkSize(DRINK4);

    //qInfo() << "Changed Option" << getOption() << endl;
}

// SIGNALS Section
