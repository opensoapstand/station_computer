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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef _MESSAGEMEDIATOR__H_
#define _MESSAGEMEDIATOR__H_

#include "../dftypes.h"
#include "debugOutput.h"
#include <pthread.h>

#include "dispenser.h"
#include "machine.h"

#include <stdio.h>
#include <cstring>

#include "../../library/socket/ServerSocket.h"
#include "../../library/socket/SocketException.h"
#include "../../library/socket/ClientSocket.h"

// Forward declaration  to avoid circular dependencies problem
class machine;

class messageMediator
{
public:
   messageMediator();
   ~messageMediator();

   DF_ERROR createThreads(pthread_t &kbThread, pthread_t &ipThread);

   DF_ERROR sendMessageOverIP(string msg, bool isLoggingMessage);
   void setMachine(machine *machine);
   string getProcessString();
   DF_ERROR parseCommandString();
   DF_ERROR parseSingleCommandString();
   DF_ERROR parseAndApplyDispenseCommand(string sCommand);
   void setDispenseCommandToDummy();

   void clearProcessString();
   string getCommandString();
   bool isCommandStringReadyToBeParsed() { return m_bCommandStringReceived; }
   void clearCommandString();

   // dispense command
   char getAction() { return m_requestedAction; }


   void resetAction();
   //int getRequestedSlot() { return m_requested_slot; }
   // char getRequestedSize() { return m_requestedSize; }
   int getCommandValue() { return m_commandValue; }
   // void setRequestedSize(char size);
   void sendTemperatureData();
   double getRequestedPrice()
   {
      debugOutput::sendMessage("getRequestedPrice price" + to_string(m_requestedDiscountPrice), MSG_INFO);
      return m_requestedDiscountPrice;
   }

   string getCouponCode()
   {
      return m_promoCode;
   }

   // static ServerSocket *fsm_comm_socket;
   // bool m_handlingRequest;
   // bool isBusySendingMessage();

   DF_ERROR setSendingBehaviour(bool enableElseDisableSending);

   void getPrinterStatus(bool *r_isOnline, bool *r_hasPaper);
   void sendPrinterStatus();

private:
   bool m_enable_sending = true;
   int messageIP;
   static bool m_fExitThreads;

   static int percentComplete;
   static string AckOrNakResult;

   static string m_receiveStringBuffer;
   static string m_processCommand;
   static bool m_bCommandStringReceived;

   // static int m_requested_slot;
   static int m_nSolenoid;
   static char m_requestedAction;
   // static double m_nVolumeTarget;
   // static char m_requestedSize;
   double m_requestedDispenseVolumeAsChar;
   static int m_commandValue;
   static double m_requestedDiscountPrice;
   static string m_promoCode;

   static DF_ERROR sendProgress(int percentComplete);
   static DF_ERROR sendQtACK(string AckOrNak);

   static DF_ERROR updateCmdString(char key);
   static DF_ERROR updateCmdString();
   static void *doKBThread(void *pThreadArgs);
   static void *doIPThread(void *pThreadArgs);
   machine *m_machine;
   Adafruit_Thermal* printerr;

};

#endif
