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
#include <time.h>

#include <sqlite3.h> 

using namespace std;

class drink
{
public:
	drink();
	drink(int slot);
        drink(int slot, string name, double nDispenseVolume, double nTargetVolume_l, double nTargetVolume_s, double calibration_const, double price_l, double price_s, bool isStillDrink, double nVolumePerTick);
	~drink();

	//getter
	int getDrinkOption(){return m_nSlot;} // For IPC
	bool getIsStillDrink(); // For pump check
        int getVolumeDispensed(){return m_nVolumeDispensed;}
        int getVolumeDispensedPreviously();
        double getTargetVolume(char size);
        double getPrice(char size);

        //void setTargetVolume(double nVolumeTarget){m_nVolumeTarget = nVolumeTarget;};

	// Interrupt Helpers
        DF_ERROR startDispense(int nVolumeToDispense, double nPrice);
        DF_ERROR stopDispense();

        DF_ERROR initDispense();

	int getVolumeSinceLastPoll();
	bool isDispenseComplete();
	bool registerFlowSensorTick();

	// int getTickCount(){return m_nTickCount;};
	// void resetTickCount(){m_nTickCount = 0;};

	// DB Updates
	void recordSale(int volume);
	void refill(int volume);

	void drinkInfo();
	void drinkVolumeInfo();

        double m_nVolumeDispensedPreviously;
        double m_nVolumeTarget;
        double m_nVolumeTarget_l;  //how much to dispense
        double m_nVolumeTarget_s;  //how much to dispense
        double m_nVolumeDispensed; //how much has been dispensed in this sale
        double m_price;
        double m_price_l;
        double m_price_s;
        string m_name;
        double m_buttonPressDuration;

        time_t rawtime;
        struct tm * timeinfo;

        char m_nStartTime[50];

private:
	// TODO: Determine more data to modify per transaction...
	int m_nSlot;


	bool isDispenseFinished;
        double m_nVolumeDispensedSinceLastPoll;
	double m_calibration_const;
	double m_nVolumePerTick;


	double m_nTickCount;
	bool valueChange;


	bool m_isStillDrink;

	void setSlot(int slot);
	void setDrinkName(string drinkName);
	void setIsStillDrink(bool isStillDrink);

	//last filled as date
	//best before as date
};

#endif
