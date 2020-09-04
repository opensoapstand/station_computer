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
#include "../../library/socket/ClientSocket.h"

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

      string getCommandString();
      void clearCommandString();

      // static ServerSocket *fsm_comm_socket;
   
      //DF_ERROR doKBThread (void * pThreadArgs);

   private:
      int messageIP;
      static bool m_fExitThreads;
      // pthread_t m_pKBThread;

      static int percentComplete;
      static string AckOrNakResult;

      static string m_processString;
      static string m_processCommand;
      static bool m_commandReady;

      static DF_ERROR sendProgress(int percentComplete);
      static DF_ERROR sendQtACK(string AckOrNak);

      static DF_ERROR updateCmdString(char key);
      static DF_ERROR updateCmdString();
      static void * doKBThread(void * pThreadArgs);
      static void * doIPThread(void * pThreadArgs);
};

#endif