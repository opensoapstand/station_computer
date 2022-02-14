//***************************************
//
// messageMediator.cpp
// Messaging IPC Controller and Model
// Implementation:
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

#include "messageMediator.h"
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/socket.h>
#include <string>
#include <unistd.h>

#define DELAY_USEC 1

extern messageMediator df_messaging;

bool messageMediator::m_fExitThreads = false;
bool messageMediator::m_bCommandStringReceived = false;
string messageMediator::m_receiveStringBuffer;
string messageMediator::m_processCommand;
int messageMediator::m_RequestedProductIndexInt;
int messageMediator::m_nSolenoid;
char messageMediator::m_requestedAction;
char messageMediator::m_requestedVolume;
double messageMediator::m_nVolumeTarget;

// CTOR
messageMediator::messageMediator()
{
   debugOutput::sendMessage("------messageMediator------", INFO);
   // TODO: Initialize with Pointer reference to socket...
   // new_sock = new ServerSocket();
   m_fExitThreads = false;
   // m_pKBThread = -1;
}

// DTOR
messageMediator::~messageMediator()
{
   debugOutput::sendMessage("~messageMediator", INFO);

   //terminate the threads
   m_fExitThreads = true;
}

// Sends a message string to QT through a socket
// IPC on Local network
DF_ERROR messageMediator::sendMessage(string msg)
{
   DF_ERROR dfError = OK;

   try
   {
      ClientSocket client_socket("localhost", 1235);
      std::string reply;
      try
      {
         client_socket << msg;
         client_socket >> reply;
      }
      catch (SocketException &)
      {
         // TODO: Should catch no message error...
      }
      //std::cout << "We received this response from the server:\n\"" << reply << "\"\n";
      ;
   }
   catch (SocketException &e)
   {
      //  std::cout << "Connection Exception was caught:" << e.description() << "\n";
   }
   dfError = ERROR_PTHREADS_IPTHREAD_NAK;

   return dfError;
}

// Sends a progress of dispensing to QT through a socket
// TODO: Need to grab information from flow sensor...and update on GUI...
DF_ERROR messageMediator::sendProgress(int percentComplete)
{
   DF_ERROR dfError = OK;

   return dfError;
}

// Sends an ACK to QT through a socket
// TODO: Refactor to work with sendMessage...Code duplication here
DF_ERROR messageMediator::sendQtACK(string AckOrNak)
{
   DF_ERROR dfError = OK;
   try
   {
      ClientSocket client_socket("localhost", 1235);
      std::string reply;
      try
      {
         client_socket << AckOrNak;
         client_socket >> reply;
      }
      catch (SocketException &)
      {
      }

      // std::cout << "We received this response from the server:\n\"" << reply << "\"\n";
      ;
   }
   catch (SocketException &e)
   {
      // std::cout << "Exception was caught:" << e.description() << "\n";
   }

   dfError = ERROR_PTHREADS_IPTHREAD_NAK;
   return dfError;
}

// Initialize thread to listen to console for input and ipThread to listen
// for Socket Data.
DF_ERROR messageMediator::createThreads(pthread_t &kbThread, pthread_t &ipThread)
{
   debugOutput::sendMessage("messageMediator::createThreads", INFO);
   DF_ERROR df_ret = OK;
   int rc = 0;
   pthread_attr_t attr;
   pthread_attr_init(&attr);

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(&kbThread, &attr, doKBThread, NULL);

   if (rc)
   {
      debugOutput::sendMessage("failed to create KB Thread", INFO);
      df_ret = ERROR_PTHREADS_KBTHREAD;
   }

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(&ipThread, &attr, doIPThread, NULL);

   if (rc)
   {
      debugOutput::sendMessage("failed to create IP Thread", INFO);
      df_ret = ERROR_PTHREADS_IPTHREAD;
   }

   return df_ret;
}

// Parse a character command and build a command string.
DF_ERROR messageMediator::updateCmdString(char key)
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   string incommingCharMsg = "Incomming CHAR: ";
   incommingCharMsg += key;
   debugOutput::sendMessage(incommingCharMsg, INFO);
   incommingCharMsg.clear();

   if (';' != key)
   {
      // build up command as long as no ;
      m_processCommand.push_back(key);
   }
   else
   {
      // ; is the command finished char
      m_receiveStringBuffer.clear();
      m_bCommandStringReceived = true;

      debugOutput::sendMessage("Provided command (unparsed) : " + m_processCommand, INFO);
   }

   return df_ret;
}

// Parse/verify and incomming processing string
// then break down into a command string.
DF_ERROR messageMediator::updateCmdString()
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;
   debugOutput::sendMessage("Process string..." + m_receiveStringBuffer, INFO);

   if (!m_processCommand.empty())
   {
      debugOutput::sendMessage("Flush old command..." + m_processCommand, INFO);
      m_processCommand.clear();
      debugOutput::sendMessage(m_processCommand, INFO);
   }

   for (int i = 0; i < m_receiveStringBuffer.size(); i++)
   {
      df_ret = updateCmdString(m_receiveStringBuffer[i]);
   }

   return df_ret;
}

// Loop for threaded listening for console input
void *messageMediator::doKBThread(void *pThreadArgs)
{
   debugOutput::sendMessage("Start up infinite keyboard input listener (doKBThread).", INFO);
   DF_ERROR df_ret = OK;

   // while (!m_fExitThreads) // lode: todo not relevant?! (contains infinite while loop)
   // {
   // make sure buffer is clear (seems overkill)
   // if (!m_receiveStringBuffer.empty())
   // {
   //    updateCmdString();
   // }

   char key;
   // BLOCKING!!
   while (0 < scanf(" %c", &key))
   {
      updateCmdString(key);
   }

   // char key;
   // // BLOCKING!!
   // while (0 < scanf(" %c", &key))
   // {
   //    updateCmdString(key);
   // }

   // usleep(DELAY_USEC);
   // }

   // df_ret;
   // return 0;
}

// Loop for threaded listening for Socket input
void *messageMediator::doIPThread(void *pThreadArgs)
{
   debugOutput::sendMessage("doIPThread", INFO);
   DF_ERROR df_ret = OK;

   try
   {
      // Create the socket
      ServerSocket fsm_comm_server(1234);

      while (!m_fExitThreads)
      {
         ServerSocket new_sock;
         fsm_comm_server.accept(new_sock);

         try
         {
            while (true)
            {
               std::string data;
               // *fsm_comm_socket >> data;
               // *fsm_comm_socket << "";
               new_sock >> data;

               // AckOrNakResult = "FSM ACK";
               sendQtACK("ACK");
               // cout << data << endl;
               m_receiveStringBuffer = data;
               updateCmdString();
               // new_sock << data;
            }

            new_sock << "Hi Back";
         }
         catch (SocketException &sock)
         {
            //  std::cout << "Socket Transfer Exception was caught:" << sock.description() << "\nExiting.\n";
            // AckOrNakResult = "FSM NAK";
            // sendQtACK(AckOrNakResult);
         }
      }
   }
   catch (SocketException &e)
   {
      // std::cout << "Socket Creation Exception was caught:" << e.description() << "\nExiting.\n";
   }

   return 0;
}

/* ------Getter, Setters and Utilities------ */

string messageMediator::getProcessString()
{
   return m_receiveStringBuffer;
}

void messageMediator::clearProcessString()
{
   m_receiveStringBuffer.clear();
   m_bCommandStringReceived = false;
}

string messageMediator::getCommandString()
{
   return m_processCommand;
}

void messageMediator::clearCommandString()
{
   m_processCommand.clear();
   m_bCommandStringReceived = false;
}

DF_ERROR messageMediator::parseCommandString()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   string sCommand = getCommandString();
   char first_char = sCommand[0];

   if (
       first_char == '1' ||
       first_char == '2' ||
       first_char == '3' ||
       first_char == '4')
   {
      // check for dispensing command
      e_ret = parseDispenseCommand(sCommand);
   }
   else if (
       // other commands
       first_char == ACTION_TEST_PRINTER ||
       first_char == ACTION_QUIT_TEST ||
       first_char == ACTION_PRINTER_CHECK_STATUS ||
       first_char == ACTION_PRINTER_CHECK_STATUS_TOGGLE_CONTINUOUSLY ||
       first_char == ACTION_PRINTER_PRINT_TEST ||
       first_char == ACTION_HELP ||
       first_char == ACTION_PRINTER_REACHABLE)
   {

      m_requestedAction = first_char;
   }
   else
   {
      // invalid commands
      m_requestedAction = ACTION_NO_ACTION;
      debugOutput::sendMessage("Command received is not valid.", INFO);
   }

   this->clearCommandString();
   return e_ret;
}

DF_ERROR messageMediator::parseDispenseCommand(string sCommand)
{

   DF_ERROR e_ret = ERROR_BAD_PARAMS;
   debugOutput::sendMessage("parseDispenseCommand", INFO);
   char temp[10];

   char productChar = PRODUCT_DUMMY;
   char actionChar = ACTION_DUMMY;
   char volumeChar = REQUESTED_VOLUME_DUMMY;

   // if (isdigit(sCommand[0]))
   if (isdigit(sCommand[0]))
   {
      productChar = sCommand[0];
      debugOutput::sendMessage("digit", INFO);
   }

   if (sCommand.size() > 1)
   {

      // FIXME: Need a better string parser...
      for (std::string::size_type i = 0; i < sCommand.size(); ++i)
      {
         if ((sCommand[i] == ACTION_DISPENSE_END) || (sCommand[i] == ACTION_DISPENSE) || sCommand[i] == PWM_CHAR)
         {
            actionChar = sCommand[i];
         }

         if (sCommand[i] == REQUESTED_VOLUME_1 || sCommand[i] == REQUESTED_VOLUME_2 || sCommand[i] == REQUESTED_VOLUME_CUSTOM)
         {
            volumeChar = (sCommand[i]);
         }
      }
   }

   switch (productChar)
   {
   case '1':
   {
      m_RequestedProductIndexInt = 1;
      debugOutput::sendMessage("Product 1 requested", INFO);
      e_ret = OK;
      break;
   }
   case '2':
   {
      m_RequestedProductIndexInt = 2;
      debugOutput::sendMessage("Product 2 requested", INFO);
      e_ret = OK;
      break;
   }
   case '3':
   {
      m_RequestedProductIndexInt = 3;
      debugOutput::sendMessage("Product 3 requested", INFO);
      e_ret = OK;
      break;
   }
   case '4':
   {
      m_RequestedProductIndexInt = 4;
      debugOutput::sendMessage("Product 4 requested", INFO);
      e_ret = OK;
      break;
   }
   default:
   {
      debugOutput::sendMessage("No product requested [1..4]", INFO);
      break;
   }
   }

   if (!isalpha(actionChar))
   {
      debugOutput::sendMessage("Irrelevant input .. ", INFO);
   }
   else if (actionChar == ACTION_DUMMY)
   {
      debugOutput::sendMessage("No action provided ", INFO);
   }
   else
   {
      // TODO: Parse and save a reference for command string

      switch (actionChar)
      {
      case ACTION_DISPENSE:
         debugOutput::sendMessage("Action: Dispense", INFO);
         // m_nSolenoid = PRODUCT;
         m_requestedAction = ACTION_DISPENSE;
         e_ret = OK;
         break;

      case PWM_CHAR:
         debugOutput::sendMessage("Action: PWM", INFO);
         m_requestedAction = PWM_CHAR;
         e_ret = OK;
         break;

      case ACTION_DISPENSE_END:
         debugOutput::sendMessage("Action: End Dispense", INFO);
         m_requestedAction = ACTION_DISPENSE_END;
         e_ret = OK;
         break;

      default:
         break;
      }
   }

   if (!isalpha(volumeChar))
   {
      // e_ret = ERROR_NETW_NO_POSITION;
   }
   else if (volumeChar == REQUESTED_VOLUME_DUMMY)
   {
      debugOutput::sendMessage("No Requested volume provided", INFO);
   }
   else
   {
      switch (volumeChar)
      {
      case REQUESTED_VOLUME_1:
         debugOutput::sendMessage("Requested volume 1, Small Size", INFO);
         m_requestedVolume = REQUESTED_VOLUME_1;
         e_ret = OK;
         break;

      case REQUESTED_VOLUME_2:
         debugOutput::sendMessage("Requested volume 2, Large Size", INFO);
         m_requestedVolume = REQUESTED_VOLUME_2;
         e_ret = OK;
         break;

      case REQUESTED_VOLUME_CUSTOM:
         debugOutput::sendMessage("Requested volume custom, Test Size", INFO);
         m_requestedVolume = REQUESTED_VOLUME_CUSTOM;
         e_ret = OK;
         break;

      default:
         break;
      }
   }

   return e_ret;
}

// DF_ERROR messageMediator::parseCommandString()
// {

//    DF_ERROR e_ret = ERROR_BAD_PARAMS;
//    debugOutput::sendMessage("parseCommandString", INFO);
//    char temp[10];
//    string commandString = getCommandString();

//    char productChar = 'z';
//    char actionChar = ACTION_DUMMY;
//    char volumeChar = REQUESTED_VOLUME_DUMMY;

//    // if (isdigit(commandString[0]))
//    if (isdigit(commandString[0]))
//    {
//       productChar = commandString[0];
//       debugOutput::sendMessage("digit", INFO);
//    }

//    if (commandString.size() > 1)
//    {

//       // FIXME: Need a better string parser...
//       for (std::string::size_type i = 0; i < commandString.size(); ++i)
//       {
//          if ((commandString[i] == ACTION_DISPENSE_END) || (commandString[i] == ACTION_DISPENSE) || commandString[i] == PWM_CHAR)
//          {
//             actionChar = commandString[i];
//          }

//          if (commandString[i] == REQUESTED_VOLUME_1 || commandString[i] == REQUESTED_VOLUME_2 || commandString[i] == REQUESTED_VOLUME_CUSTOM)
//          {
//             volumeChar = (commandString[i]);
//          }
//       }
//    }

//    switch (productChar)
//    {
//    case '1':
//    {
//       m_RequestedProductIndexInt = 1;
//       debugOutput::sendMessage("Product 1 requested", INFO);
//       e_ret = OK;
//       break;
//    }
//    case '2':
//    {
//       m_RequestedProductIndexInt = 2;
//       debugOutput::sendMessage("Product 2 requested", INFO);
//       e_ret = OK;
//       break;
//    }
//    case '3':
//    {
//       m_RequestedProductIndexInt = 3;
//       debugOutput::sendMessage("Product 3 requested", INFO);
//       e_ret = OK;
//       break;
//    }
//    case '4':
//    {
//       m_RequestedProductIndexInt = 4;
//       debugOutput::sendMessage("Product 4 requested", INFO);
//       e_ret = OK;
//       break;
//    }
//    default:
//    {
//       debugOutput::sendMessage("No product requested [1..4]", INFO);
//       break;
//    }
//    }

//    if (!isalpha(actionChar))
//    {
//       debugOutput::sendMessage("Irrelevant input .. ", INFO);
//    }
//    else if (actionChar == ACTION_DUMMY)
//    {
//       debugOutput::sendMessage("No action provided ", INFO);
//    }
//    else
//    {
//       // TODO: Parse and save a reference for command string

//       switch (actionChar)
//       {
//       case ACTION_DISPENSE:
//          debugOutput::sendMessage("Action: Dispense", INFO);
//          // m_nSolenoid = PRODUCT;
//          m_requestedAction = ACTION_DISPENSE;
//          e_ret = OK;
//          break;

//       case PWM_CHAR:
//          debugOutput::sendMessage("Action: PWM", INFO);
//          m_requestedAction = PWM_CHAR;
//          e_ret = OK;
//          break;

//       case ACTION_DISPENSE_END:
//          debugOutput::sendMessage("Action: End Dispense", INFO);
//          m_requestedAction = ACTION_DISPENSE_END;
//          e_ret = OK;
//          break;

//       default:
//          break;
//       }
//    }

//    if (!isalpha(volumeChar))
//    {
//       // e_ret = ERROR_NETW_NO_POSITION;
//    }
//    else if (volumeChar == REQUESTED_VOLUME_DUMMY)
//    {
//       debugOutput::sendMessage("No Requested volume provided", INFO);
//    }
//    else
//    {
//       switch (volumeChar)
//       {
//       case REQUESTED_VOLUME_1:
//          debugOutput::sendMessage("Requested volume 1, Small Size", INFO);
//          m_requestedVolume = REQUESTED_VOLUME_1;
//          e_ret = OK;
//          break;

//       case REQUESTED_VOLUME_2:
//          debugOutput::sendMessage("Requested volume 2, Large Size", INFO);
//          m_requestedVolume = REQUESTED_VOLUME_2;
//          e_ret = OK;
//          break;

//       case REQUESTED_VOLUME_CUSTOM:
//          debugOutput::sendMessage("Requested volume custom, Test Size", INFO);
//          m_requestedVolume = REQUESTED_VOLUME_CUSTOM;
//          e_ret = OK;
//          break;

//       default:
//          break;
//       }
//    }

//    this->clearCommandString();
//    return e_ret;
// }
