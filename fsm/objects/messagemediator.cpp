//***************************************
//
// messageMediator.cpp
// messaging class owns the IP interfaces
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "messagemediator.h"
#include <iostream>

messageMediator::messageMediator()
{
   std::cout << "I passed\n";
}

messageMediator::~messageMediator()
{

}

DF_ERROR messageMediator::sendMessage()
{
   DF_ERROR dfError = OK;

   return dfError;
}