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
#define ACTION_AUTOFILL 'a'
// #define CLEAN_CHAR 'c'

#define ACTION_NO_ACTION '-'
#define ACTION_DISPENSE_END 'f'
#define ACTION_DUMMY 'x'
#define ACTION_QUIT 'q'
#define ACTION_MANUAL_PUMP_PWM_SET 'i'
#define ACTION_MANUAL_PUMP_SET 's'
#define ACTION_PRINT_TRANSACTION 't'

#define ACTION_UI_COMMAND_PRINTER_SEND_STATUS 'a'
#define ACTION_UI_COMMAND_PRINTER_MENU 'p'
#define ACTION_HELP 'h'

#define PRODUCT_DUMMY 'z'

#define PWM_CHAR 'P'

class messageMediator
{
public:
   messageMediator();
   ~messageMediator();

   DF_ERROR createThreads(pthread_t &kbThread, pthread_t &ipThread);

   DF_ERROR sendMessageOverIP(string msg);

   string getProcessString();
   DF_ERROR parseCommandString();
   DF_ERROR parseSingleCommandString();
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



   double getRequestedPrice() { 
      debugOutput::sendMessage("getRequestedPrice price" + to_string(m_requestedDiscountPrice), MSG_INFO);
      return m_requestedDiscountPrice;}

   string getPromoCode() { 
      return m_promoCode;}


   // static ServerSocket *fsm_comm_socket;
   // bool m_handlingRequest;
   // bool isBusySendingMessage();

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
   static double m_requestedDiscountPrice;
   static string m_promoCode;

   static DF_ERROR sendProgress(int percentComplete);
   static DF_ERROR sendQtACK(string AckOrNak);

   static DF_ERROR updateCmdString(char key);
   static DF_ERROR updateCmdString();
   static void *doKBThread(void *pThreadArgs);
   static void *doIPThread(void *pThreadArgs);
};

#endif
