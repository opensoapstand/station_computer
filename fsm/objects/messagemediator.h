//***************************************
//
// messageMediator.h
// messaging class owns the IP interfaces
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef _MESSAGEMEDIATOR__H_
#define _MESSAGEMEDIATOR__H_

#include "..\dftypes.h"

class messageMediator
{
   public:
      messageMediator();
      ~messageMediator();

      DF_ERROR sendMessage();   //this is just a prototype needs to be fleshed out
};

#endif