//***************************************
//
// messageMediator.h
// Messaging IPC Controller and Model 
// Definition:
//
// Holds reference and cordinates string 
// commands from GUI, GPIO's and 
// Database threads
//
// created: 12-06-2020
// by: Denis Londry & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef _MESSAGEMEDIATOR__H_
#define _MESSAGEMEDIATOR__H_

#include "../dftypes.h"
#include "debugOutput.h"
#include <pthread.h>

#include "dispenser.h"

#include <stdio.h>
#include <cstring>

#include "../../library/socket/ServerSocket.h"
#include "../../library/socket/SocketException.h"
#include "../../library/socket/ClientSocket.h"

#define AIR_CHAR 'a'
#define WATER_CHAR 'w'
#define DRINK_CHAR 'd'
#define CLEAN_CHAR 'c'
#define DISPENSE_END_CHAR 'f'

class messageMediator
{
   public:
      messageMediator();
      ~messageMediator();

      DF_ERROR createThreads(pthread_t &kbThread, pthread_t &ipThread);

      DF_ERROR sendMessage(string msg);

      string getProcessString();
      DF_ERROR getPositionReady();
      void clearProcessString();
      void clearcCommand(){m_cCommand = '0';}

      string getCommandString();
      bool isCommandReady(){return m_bCommandReady;}
      void clearCommandString();

      int getnOption(){return m_nOption;}
      int getnSolenoid(){return m_nSolenoid;}
      char getcCommand(){return m_cCommand;}


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
      static bool m_bCommandReady;

      //int pos;
      static int m_nOption;
      static int m_nSolenoid;
      static char m_cCommand;
   
      static DF_ERROR sendProgress(int percentComplete);
      static DF_ERROR sendQtACK(string AckOrNak);

      static DF_ERROR updateCmdString(char key);
      static DF_ERROR updateCmdString();
      static void * doKBThread(void * pThreadArgs);
      static void * doIPThread(void * pThreadArgs);
};

#endif