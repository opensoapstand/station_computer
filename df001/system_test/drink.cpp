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
    drinkPin = 0;
    waterPin = 0;
    airPin = 0;
    flowPin = 0;

    pumpFwdPin = 0;
    pumpRevPin = 0;
}

drink::drink(bool type, int drink, int water, int air, int flow, int fowardP, int reversP)
{
    drinkPin = drink;
    waterPin = water;
    airPin = air;
    flowPin = flow;

    pump = type;

    if(pump == STILL){
        pumpFwdPin = 99;
        pumpRevPin = 99;
    }
    else
    {
        pumpFwdPin = fowardP;
        pumpRevPin = reversP;
    }
}

int drink::getDrinkPinAddress()
{
    return drinkPin/100;
}

int drink::getDrinkPin_pin()
{
    return drinkPin-getDrinkPinAddress()*100;
}

int drink::getWaterPinAddress()
{
    return waterPin/100;
}

int drink::getWaterPin_pin()
{
    return waterPin-getWaterPinAddress()*100;
}

int drink::getAirPinAddress()
{
    return airPin/100;
}

int drink::getAirPin_pin()
{
    return airPin-getAirPinAddress()*100;
}

int drink::getFlowSensor_pin()
{
    return flowPin;
}