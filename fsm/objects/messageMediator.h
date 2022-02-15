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
// created: 01-2022
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
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
#define ACTION_NO_ACTION '-'
#define ACTION_TEST_PRINTER 'p'
#define ACTION_QUIT 'q'
#define ACTION_PRINTER_CHECK_STATUS_TOGGLE_CONTINUOUSLY 'V'
#define ACTION_PRINTER_CHECK_STATUS 'v'
#define ACTION_PRINTER_PRINT_TEST 'l'
#define ACTION_PRINTER_REACHABLE 'r'
#define ACTION_HELP 'h'

#define PRODUCT_DUMMY 'z'


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
   DF_ERROR parseDispenseCommand(string sCommand);

   void clearProcessString();
   string getCommandString();
   bool isCommandStringReadyToBeParsed() { return m_bCommandStringReceived; }
   void clearCommandString();

   // dispense command 
   char getAction() { return m_requestedAction; }
   int getProductNumber() { return m_RequestedProductIndexInt; }
   char getRequestedVolume() { return m_requestedVolume; }



   // static ServerSocket *fsm_comm_socket;

private:
   int messageIP;
   static bool m_fExitThreads;

   static int percentComplete;
   static string AckOrNakResult;

   static string m_receiveStringBuffer;
   static string m_processCommand;
   static bool m_bCommandStringReceived;

   static int m_RequestedProductIndexInt;
   static int m_nSolenoid;
   static char m_requestedAction;
   static double m_nVolumeTarget;
   static char m_requestedVolume;

   static DF_ERROR sendProgress(int percentComplete);
   static DF_ERROR sendQtACK(string AckOrNak);

   static DF_ERROR updateCmdString(char key);
   static DF_ERROR updateCmdString();
   static void *doKBThread(void *pThreadArgs);
   static void *doIPThread(void *pThreadArgs);
};

#endif
