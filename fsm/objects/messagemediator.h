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

#include "../dftypes.h"
#include "debugoutput.h"
#include <pthread.h>

class messageMediator
{
   public:
      messageMediator();
      ~messageMediator();

      DF_ERROR createThreads();

      DF_ERROR sendMessage();
   
      //DF_ERROR doKBThread (void * pThreadArgs);
      static void * doKBThread(void * pThreadArgs);

   private:
      int messageIP;
      static bool m_fExitThreads;
      pthread_t * m_pKBThread;

      static DF_ERROR updateCmdString(char key);      
};

#endif