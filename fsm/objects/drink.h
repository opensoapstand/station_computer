//***************************************
//
// drink.h
// drink class owns the beverages
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
#include <string>

#using namespace std;

class drink
{
   public:
      drink();
	  drink(int slot);
      ~drink();

	  void setSlot(int slot);
	  int getVolumeRemaining();
	  void recordSale(int volume);
	  void refill(int volume);

   private:
	   int m_nSlot
	   string m_name;
	   string m_ingredients;
	   int m_nVolume;
	   
	   //last filled as date
	   //best before as date

};

#endif