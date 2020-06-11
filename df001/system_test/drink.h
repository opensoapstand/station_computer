//***************************************
//
// drink.h
// class to access gpio infomation 
//
// created: 10-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#define STILL 0
#define COMPRESS 1

class drink
{
    public:
        drink();
        drink(bool type, int* drink, int* water, int *air, int *flow, int *fowardP, int *reversP);

        int getDrinkPinAddress();
        int getDrinkPin_pin();

        int getWaterPinAddress();
        int getWaterPin_pin();

        int getAirPinAddress();
        int getAirPin_pin();

    private:
        bool pump;

        int* drinkPin;
        int* waterPin;
        int* airPin;
        int* flowPin;

        int* pumpFwdPin;
        int* pumpRevPin;
};