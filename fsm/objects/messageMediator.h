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

// #define AIR_CHAR 'a'
// #define WATER_CHAR 'w'
#define ACTION_DISPENSE 'd'
// #define CLEAN_CHAR 'c'
#define ACTION_DISPENSE_END 'f'
#define ACTION_DUMMY 'x'

#define REQUESTED_VOLUME_1 's'
#define REQUESTED_VOLUME_2 'l'
#define REQUESTED_VOLUME_CUSTOM 't'

#define REQUESTED_VOLUME_DUMMY '0'

#define PWM_CHAR 'P'

class messageMediator
{
   public:
      messageMediator();
      ~messageMediator();

      DF_ERROR createThreads(pthread_t &kbThread, pthread_t &ipThread);

      DF_ERROR sendMessage(string msg);

      string getProcessString();
      DF_ERROR parseCommandString();
      void clearProcessString();
      //void clearcCommand(){m_requestedAction = '0';}

      string getCommandString();
      bool isCommandReady(){return m_bCommandReady;}
      void clearCommandString();

      int getProductNumber(){return m_RequestedProductIndexInt;}
      char getAction(){return m_requestedAction;}
     
      double getnTargetVolume(){return m_nVolumeTarget;}
      char getRequestedVolume(){return m_requestedVolume;}


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
      static int m_RequestedProductIndexInt;
      static int m_nSolenoid;
      static char m_requestedAction;
      static double m_nVolumeTarget;
      static char m_requestedVolume;
   
      static DF_ERROR sendProgress(int percentComplete);
      static DF_ERROR sendQtACK(string AckOrNak);

      static DF_ERROR updateCmdString(char key);
      static DF_ERROR updateCmdString();
      static void * doKBThread(void * pThreadArgs);
      static void * doIPThread(void * pThreadArgs);
};

#endif
