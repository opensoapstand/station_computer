//***************************************
//
// drink.cpp
// Drink (Model) class definition:
//
// Owns current beverage use/transaction.
// Holds and coordinates values from database
// to dispense classes.
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef _DRINK__H_
#define _DRINK__H_

#include "../dftypes.h"
#include <iostream>
#include <string>
#include <unistd.h>

#include <sqlite3.h> 

using namespace std;

class drink
{
public:
	drink();
	drink(int slot);
	~drink();

	//getter
	int getDrinkOption(){return m_nSlot;}
	bool getIsStillDrink();
	int getVolumeRemaining();

	// DB Updates
	void recordSale(int volume);
	void refill(int volume);

private:
	// TODO: Determine more data to modify per transaction...
	int m_nSlot;
	string m_name;
	int m_nVolume;

	bool m_isStillDrink;

	void setSlot(int slot);
	void setDrinkName(string drinkName);
	void setIsStillDrink(bool isStillDrink);

	//last filled as date
	//best before as date
};

#endif