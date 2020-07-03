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
#include <unistd.h> //usleep

messageMediator::messageMediator()
{
   debugOutput::sendMessage("messageMediator() passed\n", INFO);
}

messageMediator::~messageMediator()
{

}

DF_ERROR messageMediator::sendMessage()
{
   DF_ERROR dfError = OK;

   return dfError;
}

void* messageMediator::doKBThread (void * pThreadArgs)
{
   
   unsigned int delay_usec = 25 * 1000;
   /*g_civet.writeDebug("Starting Key Board monitor");   
   
   while (g_civet.m_fThread)
   {
      char key;
      while (0 < scanf(" %c", &key))
      {
         g_civet.updateCmdString(key);
      }
      usleep(delay_usec);   
   }   
   pthread_exit(NULL);
   */
   
}