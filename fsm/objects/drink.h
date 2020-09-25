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
	drink(int slot, string name, double nDispenseVolume, double nTargetVolume, double calibration_const, double price, bool isStillDrink);
	~drink();

	//getter
	int getDrinkOption(){return m_nSlot;} // For IPC
	bool getIsStillDrink(); // For pump check
	int getVolumeDispensed(){return m_nVolumeDispensed;}
	int getTargetVolume(){return m_nVolumeTarget;};

	// Interrupt Helpers
	DF_ERROR startDispense(int nVolumeToDispense);
	int getVolumeSinceLastPoll();
	bool isDispenseComplete();
	bool registerFlowSensorTick();

	// DB Updates
	void recordSale(int volume);
	void refill(int volume);

	void drinkInfo();
	void drinkVolumeInfo();


private:
	// TODO: Determine more data to modify per transaction...
	int m_nSlot;
	string m_name;

	bool isDispenseFinished;
	double m_nVolumeTarget;  //how much to dispense
	double m_nVolumeDispensed; //how much has been dispensed in this sale
	double m_nVolumeDispensedSinceLastPoll;
	double m_calibration_const;
	double m_nVolumePerTick;

	double m_price;
	bool m_isStillDrink;

	void setSlot(int slot);
	void setDrinkName(string drinkName);
	void setIsStillDrink(bool isStillDrink);

	//last filled as date
	//best before as date
};

#endif