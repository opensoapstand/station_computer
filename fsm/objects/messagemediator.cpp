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
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unistd.h>

#define DELAY_USEC 1

bool messageMediator::m_fExitThreads = false;

messageMediator::messageMediator()
{
   debugOutput::sendMessage("messageMediator", INFO);

   m_fExitThreads = false;
   m_pKBThread = NULL;
}

messageMediator::~messageMediator()
{
   debugOutput::sendMessage("~messageMediator", INFO);

   //terminate the threads
   m_fExitThreads = true;

   
}

//needs params, but this will be called by states in order to send data to receivers
DF_ERROR messageMediator::sendMessage()
{
   DF_ERROR dfError = OK;

   return dfError;
}

DF_ERROR messageMediator::createThreads()
{
   debugOutput::sendMessage("messageMediator::createThreads", INFO);
   DF_ERROR df_ret = OK;
   int rc = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(m_pKBThread, &attr, &doKBThread, NULL);
   
   if (rc)
	{
		debugOutput::sendMessage("failed to create KB Thread", INFO);
		df_ret = ERROR_PTHREADS;
	}

   return df_ret;
}

DF_ERROR messageMediator::updateCmdString(char key)
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   //up to you I would have a delimiter like ";" and concat to a string. Then when the delim goes analyse or sends to the state

   return df_ret;
}

void* messageMediator::doKBThread(void * pThreadArgs)
{
   debugOutput::sendMessage("doKBThread", INFO);
   DF_ERROR df_ret = OK;

   while (!m_fExitThreads)
   {
      char key;
		while (0 < scanf(" %c", &key))
		{
			updateCmdString(key);
		}
		usleep(DELAY_USEC);
   }

   // df_ret;
}