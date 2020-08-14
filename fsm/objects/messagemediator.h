//***************************************
//
// messageMediator.h
// messaging class owns the IP interfaces
//
// Holds reference and cordinates string 
// commands from GUI, GPIO's and 
// Database threads
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

#include "../../library/socket/ServerSocket.h"
#include "../../library/socket/SocketException.h"

class messageMediator
{
   public:
      messageMediator();
      ~messageMediator();

      DF_ERROR createThreads(pthread_t &kbThread, pthread_t &ipThread);

      DF_ERROR sendMessage();

      string getProcessString();
      bool getStringReady();
      void clearProcessString();
   
      //DF_ERROR doKBThread (void * pThreadArgs);

   private:
      int messageIP;
      static bool m_fExitThreads;
      // pthread_t m_pKBThread;

      static string m_processString;
      static bool m_stringReady;

      static DF_ERROR updateCmdString(char key);
      static DF_ERROR updateCmdString();    
      static void * doKBThread(void * pThreadArgs);
      static void * doIPThread(void * pThreadArgs);
};

#endif