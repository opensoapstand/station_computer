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
#include <pthread.h>

class messageMediator
{
   public:
      messageMediator();
      ~messageMediator();

      DF_ERROR createThreads();

      DF_ERROR sendMessage();
   
   private:
      int messageIP;
      bool m_fExitThreads;
      pthread_t * m_pKBThread;

      DF_ERROR updateCmdString(char key);
      static void * doKBThread(void * pThreadArgs);
};

#endif