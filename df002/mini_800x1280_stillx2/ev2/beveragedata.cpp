//***************************************
//
// countdownWindow
// User pause window
//
// created: 29-AUG-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "beveragedata.h"
#include <QDebug>

beverageData::beverageData()
{
    bevDispense = 0;
    currentInventory = 0;
    dispenseVolume = 0;
}

beverageData::~beverageData()
{
    //delete this;
}

beverageData::beverageData(int op, int initialVolume)
{
    bevDispense = op;
    currentInventory = initialVolume;
}

int beverageData::getBeverageOption()
{
    return  bevDispense;
}

int beverageData::getInventoryVolume()
{
    return currentInventory;
}

void beverageData::setDispenseVolume(int volume)
{
   dispenseVolume = volume;
}

void beverageData::updateInventoryVolume()
{
    {
        currentInventory = currentInventory - dispenseVolume;
        qDebug() << "Current Inventory: "<< currentInventory << " Dispense Volume: " << dispenseVolume;
    }

    dispenseVolume = 0; //reset
}

void beverageData::updateBeverage(beverageData* bev)
{
    dispenseVolume = bev->dispenseVolume;
    currentInventory = bev->currentInventory;
    bevDispense = bev->bevDispense;
}
