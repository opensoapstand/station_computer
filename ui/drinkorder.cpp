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

// SLOTS Section



bool DrinkOrder::isSelectedOrderValid()
{
    if (!(m_selectedSlot >= OPTION_SLOT_INVALID && m_selectedSlot <= SLOT_COUNT))
    {
        qInfo() << "ERROR: no slot set. " << m_selectedSlot;
        return false;
    }
    if (!(sizeOptionSelected >= INVALID_DRINK && sizeOptionSelected <= MAX_INVALID_DRINK))
    {
        qInfo() << "ERROR: no slot set. " << m_selectedSlot;
        return false;
    }

    return true;
}


void DrinkOrder::setSelectedOverrulePrice(double price)
{

    if (isSelectedOrderValid ())
    {

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
double DrinkOrder::getSelectedProductPrice()
{
    // slot and size needs to be set.
    if (isSelectedOrderValid())
    {
        if (overruledPrice != 0){
            return overruledPrice;
        }else{
            qInfo() << "db....pricess.....";
            DbManager db(DB_PATH);
            db.getProductPrice(getSelectedSlot(), getSelectedSizeAsChar());
            db.closeDB();
        }
    }
    else
    {
       
        qInfo() << "ERROR: no product set";
        return 66.6;
    }
}


double DrinkOrder::getSelectedVolume()
{
    if (isSelectedOrderValid())
    {
        qInfo() << "db.... vol seijsf";
        DbManager db(DB_PATH);
        db.getProductVolume(getSelectedSlot(), getSelectedSizeAsChar());
        db.closeDB();
    }
    else
    {
        
        qInfo() << "ERROR: No product set";
        return 66.6;
    }
}

DF_QT_SIZES DrinkOrder::getSelectedSizeOption()
{
    return sizeOptionSelected;
}

char DrinkOrder::getSelectedSizeAsChar()
{
    char size_to_char [MAX_INVALID_DRINK] = {'!', 's', 'm', 'l', 't', 't'};
    // ! = invalid.
    // t  test to fsm, but should become c for custom. we're so ready for it.
    // t

    return size_to_char[sizeOptionSelected];
}

// void DrinkOrder::setSize(double size)
// {
//     if(size != getSelectedVolume()) {
//         m_drinkML = size;
//         emit sizeChange(size);
//     }
// }

// Enum search for drink sizes
void DrinkOrder::setSelectedSize(DF_QT_SIZES sizeOption)
{
    overruledPrice = INVALID_PRICE;
    sizeOptionSelected = sizeOption;

    //     switch (m_selectedSlot)
    //     {
    //     case(1):
    //         switch (sizeOption)
    //         {
    //         case(SMALL_DRINK):
    //             //qDebug() << "Drink 1 : Small" << endl;
    //             setSize(DRINK1_SIZE_ML_S);
    //             setSelectedOverrulePrice(DRINK1_price_small);
    //             break;
    //         case(LARGE_DRINK):
    //             //qDebug() << "Drink 1 : Large" << endl;
    //             setSize(DRINK1_SIZE_ML_L);
    //             setSelectedOverrulePrice(DRINK1_PRICE_LARGE);
    //             break;
    //         default:
    //             break;
    //        }
    //         break;
    //     case(2):
    //         switch (sizeOption){
    //         case(SMALL_DRINK):
    // //            qDebug() << "Drink 2 : Small" << endl;
    //             setSize(DRINK2_SIZE_ML_S);
    //             setSelectedOverrulePrice(DRINK2_price_small);
    //             break;
    //         case(LARGE_DRINK):
    // //            qDebug() << "Drink 2 : Large" << endl;
    //             setSize(DRINK2_SIZE_ML_L);
    //             setSelectedOverrulePrice(DRINK2_PRICE_LARGE);
    //             break;
    //         default:
    //             break;
    //         }
    //         break;
    //     case(3):
    //         switch (sizeOption){
    //         case(SMALL_DRINK):
    // //            qDebug() << "Drink 3 : Small" << endl;
    //             setSize(DRINK3_SIZE_ML_S);
    //             setSelectedOverrulePrice(DRINK3_price_small);
    //             break;
    //         case(LARGE_DRINK):
    // //            qDebug() << "Drink 3 : Large" << endl;
    //             setSize(DRINK3_SIZE_ML_L);
    //             setSelectedOverrulePrice(DRINK3_PRICE_LARGE);
    //             break;
    //         default:
    //             break;
    //        }
    //         break;
    //     case(4):
    //         switch (sizeOption){
    //         case(SMALL_DRINK):
    // //            qDebug() << "Drink 4 : Small" << endl;
    //             setSize(DRINK4_SIZE_ML_S);
    //             setSelectedOverrulePrice(DRINK4_price_small);
    //             break;
    //         case(LARGE_DRINK):
    // //            qDebug() << "Drink 4 : Large" << endl;
    //             setSize(DRINK4_SIZE_ML_L);
    //             setSelectedOverrulePrice(DRINK4_PRICE_LARGE);
    //             break;
    //         default:
    //             break;
    //         }
    //         break;
    //     default:
    //         setSize(DRINK1_SIZE_ML_L);
    //         setSelectedOverrulePrice(DRINK1_PRICE_LARGE);
    //         break;
    //     }
}

// void DrinkOrder::setSelectedSlot(int optNumber)
// {

//     if(optNumber != getSelectedSlot()) {
//         m_selectedSlot = optNumber;
//         emit orderSlotChange(optNumber);
//     }
// }

int DrinkOrder::getSelectedSlot()
{
    return m_selectedSlot;
}
// Enum search for option slot
void DrinkOrder::setSelectedSlot(int slot)
{
    // qInfo() << "Current Option" << getSelectedSlot() << endl;

    if (slot >= OPTION_SLOT_INVALID && slot <= SLOT_COUNT)
    {
        
        if (slot != getSelectedSlot())
        {
            overruledPrice = INVALID_PRICE;
            m_selectedSlot = slot;
            emit orderSlotChange(slot);
        }
        //         switch (slot)
        //         {
        //         case(OPTION_SLOT_1):
        //             setSelectedSlot(OPTION_SLOT_1);
        //             break;
        //         case(OPTION_SLOT_2):
        //             setSelectedSlot(OPTION_SLOT_2);
        //             break;
        //         case(OPTION_SLOT_3):
        //             setSelectedSlot(OPTION_SLOT_3);
        //             break;
        //         case(OPTION_SLOT_4):
        //             setSelectedSlot(OPTION_SLOT_4);
        //             break;
        // //        case(OPTION_SLOT_5):
        // //            setSelectedSlot(OPTION_SLOT_5);
        // //            break;
        // //        case(OPTION_SLOT_6):
        // //            setSelectedSlot(OPTION_SLOT_6);
        // //            break;
        // //        case(OPTION_SLOT_7):
        // //            setSelectedSlot(OPTION_SLOT_7);
        // //            break;
        // //        case(OPTION_SLOT_8):
        // //            setSelectedSlot(OPTION_SLOT_8);
        // //            break;
        // //        case(OPTION_SLOT_9):
        // //            setSelectedSlot(OPTION_SLOT_9);
        //             break;
        //         default:
        //             //setSize(OPTION_SLOT);
        //              qInfo() << "BAD OPTION" << slot << endl;
        //             break;
        //         }
    }
    else
    {
        qInfo() << "OUT OF OPTION RANGE!" << slot;
    }
}
