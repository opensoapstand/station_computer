#include "drinkorder.h"

// Ctor
DrinkOrder::DrinkOrder()
{
    selectedDrink = new DrinkSelection;
}

// Ctor Object Copy
DrinkOrder::DrinkOrder( const DrinkOrder& other )
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

// TODO: Decouple send_to_FSM from Dispenser to here
void DrinkOrder::setDrinkSize(DF_QT_OPTIONS sizeOption) {
    qInfo() << "Existing Size" << getSize() << endl;

    switch (sizeOption)
    {
    case(SMALL_DRINK):
        setSize(SMALL_SIZE_ML);
        break;

    case(LARGE_DRINK):
        setSize(LARGE_SIZE_ML);
        break;

    default:
        setSize(EMPTY_SIZE_ML);
        break;
    }

    qInfo() << "Order Size" << getSize() << endl;
}


// SIGNALS Section
