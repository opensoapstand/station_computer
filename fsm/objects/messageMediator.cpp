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
// created: 12-06-2020
// by: Denis Londry & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
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
bool messageMediator::m_bCommandReady = false;
string messageMediator::m_processString;
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
   else if (';' == key)
   {
      // command creation finished, execute command
      debugOutput::sendMessage("Flushing processing string: " + m_processString, INFO);
      m_processString.clear();
      m_bCommandReady = true;

      debugOutput::sendMessage("Command String Ready: " + m_processCommand, INFO);
      debugOutput::sendMessage("Leftover process string: " + m_processString, INFO);
   }
   //lode: will not come here?!
   // else
   // {
   //    debugOutput::sendMessage("Command String Status: " + m_processCommand, INFO);
   //    m_bCommandReady = false;
   // }

   return df_ret;
}

// Parse/verify and incomming processing string
// then break down into a command string.
DF_ERROR messageMediator::updateCmdString()
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;
   debugOutput::sendMessage("Process string..." + m_processString, INFO);

   if (!m_processCommand.empty())
   {
      debugOutput::sendMessage("Flush old command..." + m_processCommand, INFO);
      m_processCommand.clear();
      debugOutput::sendMessage(m_processCommand, INFO);
   }

   for (int i = 0; i < m_processString.size(); i++)
   {
      df_ret = updateCmdString(m_processString[i]);
   }

   return df_ret;
}

// Loop for threaded listening for console input
void *messageMediator::doKBThread(void *pThreadArgs)
{
   debugOutput::sendMessage("doKBThread", INFO);
   DF_ERROR df_ret = OK;

   //m_fExitThreads = false;

   while (!m_fExitThreads)
   {
      if (m_processString.empty()) // still processing process string
      {
         char key;
         while (0 < scanf(" %c", &key))
         {
            updateCmdString(key);
         }
      }
      else
      {
         updateCmdString();
      }
      usleep(DELAY_USEC);
   }

   // df_ret;
   return 0;
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
               m_processString = data;
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
   return m_processString;
}

void messageMediator::clearProcessString()
{
   m_processString.clear();
   m_bCommandReady = false;
}

string messageMediator::getCommandString()
{
   return m_processCommand;
}

void messageMediator::clearCommandString()
{
   m_processCommand.clear();
   debugOutput::sendMessage("Should be emptied: clear process command" + m_processCommand, INFO);
   m_bCommandReady = false;
}

DF_ERROR messageMediator::parseCommandString()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;
   // debugOutput::sendMessage("parseCommandString", INFO);
   char temp[10];
   string commandString = getCommandString();

   char productChar;
   char actionChar;
   char volumeChar;

   // FIXME: Need a better string parser...
   for (std::string::size_type i = 0; i < commandString.size(); ++i)
   {
      if (isdigit(commandString[0]))
      {
         productChar = commandString[0];

      }
      if ((commandString[i] == DISPENSE_END_CHAR) || (commandString[i] == DRINK_CHAR) || commandString[i] == PWM_CHAR)
      {
         actionChar = commandString[i];
      }

      if (commandString[i] == SMALL_DRINK_CHAR || commandString[i] == LARGE_DRINK_CHAR || commandString[i] == TEST_CHAR)
      {
          volumeChar = (commandString[i]);
      }

   }

   // TODO: Can seperate this into char parsing switch statment and further into function.
   // pos = -1;
   // strcpy(&productChar, &temp[0]);

   if (isdigit(productChar)) //first character should be string
   {
      // debugOutput::sendMessage("Set Option", INFO);

      int check = productChar - '0';
      // pos = atoi(&productChar) - 1;
      // FIXME: MAGIC NUMBER reference...
      if (9 < check || 0 > check)
      {
         // debugOutput::sendMessage("Irrelevant input", ERROR);
         e_ret = ERROR_NETW_NO_OPTION; //require valid cassettes
      }
      else
      {
         m_RequestedProductIndexInt = check;
         //cout << m_RequestedProductIndexInt << endl;
         e_ret = OK;
      }
   }
   else
   {
      // Error Handling
      debugOutput::sendMessage("Irrelevant input, first char should be a digit: " + productChar , INFO);
      this->clearProcessString();    //make sure to clear the processed string for new input
      e_ret = ERROR_NETW_NO_COMMAND; //require valid cassettes
   }

   // Check for Char then int pairing values
   // actionChar;
   // strcpy(&actionChar, &temp[1]);

   if (!isalpha(actionChar)) //for second char not an alphabet
   {
      debugOutput::sendMessage("Irrelevant input .. ", INFO);
      e_ret = ERROR_NETW_NO_POSITION;
   }
   else
   {
      // TODO: Parse and save a reference for command string

      switch (actionChar)
      {
      case AIR_CHAR:
         debugOutput::sendMessage("Air Solenoid", INFO);
         // m_nSolenoid = AIR;
         break;

      case WATER_CHAR:
         debugOutput::sendMessage("Water Solenoid", INFO);
         // m_nSolenoid = WATER;
         break;

      case DRINK_CHAR:
         debugOutput::sendMessage("Drink CHAR", INFO);
         // m_nSolenoid = DRINK;
         m_requestedAction = DRINK_CHAR;
         break;

      case CLEAN_CHAR:
         break;

      case PWM_CHAR:
          debugOutput::sendMessage("PWM CHAR", INFO);
          m_requestedAction = PWM_CHAR;
          break;

      case DISPENSE_END_CHAR:
         debugOutput::sendMessage("Dispense END CHAR", INFO);
         m_requestedAction = DISPENSE_END_CHAR;
         break;

      default:
         break;
      }
   }

   if (!isalpha(volumeChar)) //for second char not an alphabet
   {
//      debugOutput::sendMessage("Irrelevant input", INFO);
      e_ret = ERROR_NETW_NO_POSITION;
//       m_requestedVolume = TEST_CHAR;
   }
   else
   {
       switch (volumeChar)
       {
       case SMALL_DRINK_CHAR:
           debugOutput::sendMessage("Small Size", INFO);
           m_requestedVolume = SMALL_DRINK_CHAR;
           break;

       case LARGE_DRINK_CHAR:
           debugOutput::sendMessage("Large Size", INFO);
           m_requestedVolume = LARGE_DRINK_CHAR;
           break;

       case TEST_CHAR:
           debugOutput::sendMessage("Test Size", INFO);
           m_requestedVolume = TEST_CHAR;

       default:
           break;
       }
   }

 // 
   this->clearCommandString(); // lode added
   //m_bCommandReady = true; // lode deletedf
   return e_ret;
}
