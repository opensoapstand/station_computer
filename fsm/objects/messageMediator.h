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


// #define ACTION_PING '1'
// #define ACTION_MANUAL_PRINTER '1'
// #define ACTION_MANUAL_PUMP '2'
// #define ACTION_MANUAL_PUMP_TEST 't'
// #define ACTION_TOGGLE_CYCLIC_PUMP_TEST 'c'
// #define ACTION_MANUAL_PUMP_ENABLE 'e'
// #define ACTION_MANUAL_PUMP_DISABLE 'd'
// #define ACTION_MANUAL_PUMP_DIRECTION_FORWARD 'f'
// #define ACTION_MANUAL_PUMP_DIRECTION_REVERSE 'r'

#define ACTION_NO_ACTION '-'
#define ACTION_DISPENSE_END 'f'
#define ACTION_DUMMY 'x'
#define ACTION_QUIT 'q'
#define ACTION_MANUAL_PUMP_PWM_SET 'i'
#define ACTION_MANUAL_PUMP_SET 's'

#define ACTION_UI_COMMAND_PRINTER_SEND_STATUS 'a'
#define ACTION_UI_COMMAND_PRINTER_MENU 'p'

// #define ACTION_PRINTER_CHECK_STATUS_TOGGLE_CONTINUOUSLY 'V'
// #define ACTION_PRINTER_CHECK_STATUS 'v'
// #define ACTION_PRINTER_PRINT_TEST 'l'
// #define ACTION_PRINTER_REACHABLE 'r'
// #define ACTION_MANUAL_PUMP_FLOW_TEST_TOGGLE 'a'
// #define ACTION_MANUAL_PUMP_CUSTOM_VOLUME_TEST_TOGGLE 'g'
#define ACTION_HELP 'h'
// #define ACTION_DEBUG '3'

#define PRODUCT_DUMMY 'z'


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
   int getRequestedSlot() { return m_RequestedProductIndexInt; }
   char getRequestedSize() { return m_requestedSize; }
   int getCommandValue() { return m_commandValue; }
   void setRequestedSize(char size);
   



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
   static char m_requestedSize;
   static int m_commandValue;

   static DF_ERROR sendProgress(int percentComplete);
   static DF_ERROR sendQtACK(string AckOrNak);

   static DF_ERROR updateCmdString(char key);
   static DF_ERROR updateCmdString();
   static void *doKBThread(void *pThreadArgs);
   static void *doIPThread(void *pThreadArgs);
};

#endif
