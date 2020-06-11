//***************************************
//
// main.cpp
// class to handle input and control of the pins
//
// created: 10-06-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************
#include <iostream>
#include <string.h>

#include "gpios.h"
#include "drink.h"
#include "i2cgpios.h"

using namespace std;

#define Num_Cassettes 9

bool type = false;

bool inputNumber(drink *drinkArray[Num_Cassettes], char *inputArray);
bool inputType(drink *drinkArray, char *inputArray);

i2cGpios gpioControl = i2cGpios();

int main()
{
    drink *drinks[Num_Cassettes];

    for (int i = 0; i < 9; i++)
    {
        if (i >= 5)
        {
            type = STILL;
            drinks[i] = new drink(type, drinkP[i], waterP[i], airP[i], flowP[i], pumpFwdP[i - 5], pumpRevP[i - 5]);
        }
        else
        {
            type = COMPRESS;
            drinks[i] = new drink(type, drinkP[i], waterP[i], airP[i], flowP[i], 99, 99); //99 just indicate that it doesn't really exist for now
        }
    }

    char inputChar[4];
    do
    {
        cout << "Enter which cassette (1-9) to activate (eg. 1 = cassette 1)" << endl;
        cin >> inputChar;

    } while (inputNumber(drinks, inputChar));
    return 0;
}

bool inputNumber(drink *drinkArray[Num_Cassettes], char *inputArray)
{
    cout << "Your input is: " << inputArray << endl;
    char inputChar[4];
    if (strcmp("stop", inputArray) == 0)
    {
        return false;
    }
    else if (strcmp("1", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 1 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[1 - 1], inputChar));

        cout << "turn off all relay for cassette 1" << endl;

        return true;
    }
    else if (strcmp("2", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 2 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[2 - 1], inputChar));

        cout << "turn off all relay for cassette 2" << endl;

        return true;
    }
    else if (strcmp("3", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 3 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[3 - 1], inputChar));

        cout << "turn off all relay for cassette 3" << endl;

        return true;
    }
    else if (strcmp("4", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 4 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[4 - 1], inputChar));

        cout << "turn off all relay for cassette 4" << endl;

        return true;
    }
    else if (strcmp("5", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 5 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[5 - 1], inputChar));

        cout << "turn off all relay for cassette 5" << endl;

        return true;
    }
    else if (strcmp("6", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 6 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[6 - 1], inputChar));

        cout << "turn off all relay for cassette 6" << endl;

        return true;
    }
    else if (strcmp("7", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 7 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[7 - 1], inputChar));

        cout << "turn off all relay for cassette 7" << endl;

        return true;
    }
    else if (strcmp("8", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 8 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[8 - 1], inputChar));

        cout << "turn off all relay for cassette 8" << endl;

        return true;
    }
    else if (strcmp("9", inputArray) == 0)
    {
        do
        {
            cout << "Cassette 9 selected, activate solenoid with input (d, w, a, stop)" << endl;

            cin >> inputChar;
        } while (inputType(drinkArray[9 - 1], inputChar));

        cout << "turn off all relay for cassette 9" << endl;

        return true;
    }
    else
    {
        cout << "Invalid input, input 1-9 or stop to exit the program\n";
        return true;
    }
}

bool inputType(drink *drinkArray, char *inputArray)
{

    if (strcmp("stop", inputArray) == 0)
    {
        gpioControl.setPin_off(drinkArray->getDrinkPinAddress(), drinkArray->getDrinkPin_pin());
        gpioControl.setPin_off(drinkArray->getWaterPinAddress(), drinkArray->getWaterPin_pin());
        gpioControl.setPin_off(drinkArray->getAirPinAddress(), drinkArray->getAirPin_pin());

        return false;
    }
    else if (strcmp("d", inputArray) == 0)
    {
        cout << "Address: " << drinkArray->getDrinkPinAddress();
        cout << " Pin:" << drinkArray->getDrinkPin_pin() << endl;

        gpioControl.setPinMode_out(drinkArray->getDrinkPinAddress(), drinkArray->getDrinkPin_pin());
        gpioControl.setPin_on(drinkArray->getDrinkPinAddress(), drinkArray->getDrinkPin_pin());
        return true;
    }
    else if (strcmp("a", inputArray) == 0)
    {
        cout << "Address: " << drinkArray->getAirPinAddress();
        cout << " Pin:" << drinkArray->getAirPin_pin() << endl;

        gpioControl.setPinMode_out(drinkArray->getAirPinAddress(), drinkArray->getAirPin_pin());
        gpioControl.setPin_on(drinkArray->getAirPinAddress(), drinkArray->getAirPin_pin());

        return true;
    }
    else if (strcmp("w", inputArray) == 0)
    {
        cout << "Address: " << drinkArray->getWaterPinAddress();
        cout << " Pin:" << drinkArray->getWaterPin_pin() << endl;

        gpioControl.setPinMode_out(drinkArray->getWaterPinAddress(), drinkArray->getWaterPin_pin());
        gpioControl.setPin_on(drinkArray->getWaterPinAddress(), drinkArray->getWaterPin_pin());

        return true;
    }
    else
    {
        cout << "\nInvalid input try again with (d, w, a, stop)\n";
        return true;
    }
}