//***************************************
//
// drink.cpp
// class to access gpio infomation 
//
// created: 10-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "drink.h"

drink::drink()
{
    pump = COMPRESS;
    drinkPin = nullptr;
    waterPin = nullptr;
    airPin = nullptr;
    flowPin = nullptr;

    pumpFwdPin = nullptr;
    pumpRevPin = nullptr;
}

drink::drink(bool type, int* drink, int* water, int *air, int *flow, int *fowardP, int *reversP)
{
    drinkPin = drink;
    waterPin = water;
    airPin = air;
    flowPin = flow;

    pump = type;

    if(pump == STILL){
        pumpFwdPin = nullptr;
        pumpRevPin = nullptr;
    }
    else
    {
        pumpFwdPin = fowardP;
        pumpRevPin = reversP;
    }
}

int drink::getDrinkPinAddress()
{
    return *drinkPin/100;
}

int drink::getDrinkPin_pin()
{
    return *drinkPin-getDrinkPinAddress()*100;
}

int drink::getWaterPinAddress()
{
    return *waterPin/100;
}

int drink::getWaterPin_pin()
{
    return *waterPin-getWaterPinAddress()*100;
}

int drink::getAirPinAddress()
{
    return *airPin/100;
}

int drink::getAirPin_pin()
{
    return *airPin-getAirPinAddress()*100;
}