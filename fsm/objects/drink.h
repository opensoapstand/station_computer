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

#define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"

using namespace std;

class drink
{
public:
	drink();
	drink(int slot);
        drink(int slot, string name, double nDispenseVolume, double nTargetVolume_l, double nTargetVolume_s, double calibration_const, double price_l, double price_s, bool isStillDrink, double nVolumePerTick, string nPLU_l, string nPLU_s, string paymentMethod);
	~drink();

	//getter
	int getDrinkOption(){return m_nSlot;} // For IPC
	bool getIsStillDrink(); // For pump check
        double getVolumeDispensed(){return m_nVolumeDispensed;}
        double getVolumeDispensedPreviously();
        double getTargetVolume(char size);
        double getPrice(char size);
        string getPLU(char size);
        string getPaymentMethod(){return m_paymentMethod;}

        //void setTargetVolume(double nVolumeTarget){m_nVolumeTarget = nVolumeTarget;};

	// Interrupt Helpers
        DF_ERROR startDispense(int nVolumeToDispense, double nPrice);
        DF_ERROR stopDispense();

        DF_ERROR initDispense();

        double getVolumeSinceLastPoll();
	bool isDispenseComplete();
	bool registerFlowSensorTick();

	// int getTickCount(){return m_nTickCount;};
	// void resetTickCount(){m_nTickCount = 0;};

	// DB Updates
	void recordSale(int volume);
	void refill(int volume);

	void drinkInfo();
	void drinkVolumeInfo();

        int getPWM();

        double m_nVolumeDispensedPreviously;
        double m_nVolumeTarget;
        double m_nVolumeTarget_l;  //how much to dispense (large)
        double m_nVolumeTarget_s;  //how much to dispense (small)
        double m_nVolumeTarget_t = 10000000;  //test dispense (infinite)
        double m_nVolumeDispensed; //how much has been dispensed in this sale
        double m_price;
        double m_price_l;
        double m_price_s;
        string m_name;
        double m_buttonPressDuration;
        string m_nPLU_l;
        string m_nPLU_s;
        string m_paymentMethod;
        int m_PWM;

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
        double getVolPerTick();


        sqlite3 *db;
        int rc;

	//last filled as date
	//best before as date
};

#endif
