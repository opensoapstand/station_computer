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
int messageMediator::m_commandValue;
char messageMediator::m_requestedSize;
double messageMediator::m_nVolumeTarget;
double messageMediator::m_requestedDiscountPrice;
string messageMediator::m_promoCode;

// CTOR
messageMediator::messageMediator()
{

   debugOutput::sendMessage("Init messageMediator...", MSG_INFO);
   // TODO: Initialize with Pointer reference to socket...
   // new_sock = new ServerSocket();
   m_fExitThreads = false;
   // m_pKBThread = -1;
   
}

// DTOR
messageMediator::~messageMediator()
{
   debugOutput::sendMessage("~messageMediator", MSG_INFO);

   // terminate the threads
   m_fExitThreads = true;
}

// Sends a message string to QT through a socket
// IPC on Local network
// FSM is client, UI is server
// bool messageMediator::isBusySendingMessage(){
//    // return this->m_handlingRequest;
//    return false;
// }

DF_ERROR messageMediator::sendMessageOverIP(string msg)
{
   DF_ERROR dfError = OK;
   debugOutput::sendMessage("Send msg to UI (don't wait for reply): " + msg, MSG_INFO);
   
   int attempts = 200;
   bool done = false;
   
   while (attempts > 0 && !done)
   {

      // if (false)
      // {
      //    debugOutput::sendMessage("Will not send to UI. Busy handling previous request. " + msg, MSG_INFO);
      //    usleep(10000);
      //    attempts--;
      // }
      // else
      // {

         

         // m_handlingRequest = true;

         //usleep(10000);
         try
         {
            // ClientSocket client_socket("localhost", 0); // with port 1235, once in a while error :'Could not bind to port", makes UI crash. //https://stackoverflow.com/questions/29866083/tcp-socket-cannot-bind-to-port-in-use
            //  ClientSocket client_socket("localhost", 1235);
            ClientSocket client_socket("localhost", 8645);

            std::string reply;
            try
            {
               client_socket << msg;
               
               // client_socket >> reply; // blocking. And we're not sending a reply from the UI anymore (it caused crashes.)
               done = true;
            }
            catch (SocketException &)
            {
               // TODO: Should catch no message error...
               debugOutput::sendMessage("Error sending to UI " + reply, MSG_ERROR);
               usleep(10000);
               attempts--;
            }
            // debugOutput::sendMessage("We received this response from the server: " + reply, MSG_INFO);
            ;
         }
         catch (SocketException &e)
         {
            //  std::cout << "Connection Exception was caught:" << e.description() << "\n";
            debugOutput::sendMessage("Error opening socket to send to UI " + e.description(), MSG_ERROR);
            usleep(100000);
            attempts--;
         }
      // }
   }

   if (attempts <= 0)
   {
      debugOutput::sendMessage("Give up on communication. Amount of attempts exhausted. ", MSG_INFO);
   }

   //dfError = ERROR_PTHREADS_IPTHREAD_NAK;
   // m_handlingRequest = false;

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
   debugOutput::sendMessage("messageMediator::createThreads", MSG_INFO);
   DF_ERROR df_ret = OK;
   int rc = 0;
   pthread_attr_t attr;
   pthread_attr_init(&attr);

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(&kbThread, &attr, doKBThread, NULL);

   if (rc)
   {
      debugOutput::sendMessage("failed to create KB Thread", MSG_INFO);
      df_ret = ERROR_PTHREADS_KBTHREAD;
   }

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(&ipThread, &attr, doIPThread, NULL);

   if (rc)
   {
      debugOutput::sendMessage("failed to create IP Thread", MSG_INFO);
      df_ret = ERROR_PTHREADS_IPTHREAD;
   }

   return df_ret;
}

// Parse a character command and build a command string.
DF_ERROR messageMediator::updateCmdString(char key)
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   // debug input char
   // string incommingCharMsg = "Incomming CHAR: ";
   // incommingCharMsg += key;
   // debugOutput::sendMessage(incommingCharMsg, MSG_INFO);
   // incommingCharMsg.clear();

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

      debugOutput::sendMessage("Provided command (unparsed) : " + m_processCommand, MSG_INFO);
   }

   return df_ret;
}

// Parse/verify and incomming processing string
// then break down into a command string.
DF_ERROR messageMediator::updateCmdString()
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;
   debugOutput::sendMessage("Process string..." + m_receiveStringBuffer, MSG_INFO);

   if (!m_processCommand.empty())
   {
      debugOutput::sendMessage("Flush old command..." + m_processCommand, MSG_INFO);
      m_processCommand.clear();
      debugOutput::sendMessage(m_processCommand, MSG_INFO);
   }

   for (int i = 0; i < m_receiveStringBuffer.size(); i++)
   {
      df_ret = updateCmdString(m_receiveStringBuffer[i]);
   }

   return df_ret;
}

// Loop for threaded listening for console input
// KB = keyboard
void *messageMediator::doKBThread(void *pThreadArgs)
{
   debugOutput::sendMessage("Start up infinite keyboard input listener (doKBThread).", MSG_INFO);
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
   debugOutput::sendMessage("doIPThread", MSG_INFO);
   DF_ERROR df_ret = OK;

   try
   {
      // Create the socket
      ServerSocket fsm_comm_server(1234);

      while (!m_fExitThreads)
      {
         {
            ServerSocket new_sock;
            fsm_comm_server.accept(new_sock);
            debugOutput::sendMessage("new sock (UIDDFIJDF)", MSG_INFO);

            try
            {
               // while (true)
               // {
               //// debugOutput::sendMessage("char received over IP", MSG_INFO);
               std::string data;
               //// *fsm_comm_socket >> data;
               //// *fsm_comm_socket << "";
               new_sock >> data;

               //// sendQtACK("ACK");  // lode commented it out was blocking?!?! todo //// AckOrNakResult = "FSM ACK";
               // //  cout << data << endl;
               m_receiveStringBuffer = data;
               updateCmdString();
               // debugOutput::sendMessage("chars received over IP: " + data, MSG_INFO);
               //// new_sock << data;
               // }

               // new_sock << "Hi Back";
            }
            catch (SocketException &sock)
            {
               debugOutput::sendMessage("Socket Transfer Exception was caught:" + sock.description(), MSG_INFO);
               //  std::cout << "Socket Transfer Exception was caught:" << sock.description() << "\nExiting.\n";
               // AckOrNakResult = "FSM NAK";
               // sendQtACK(AckOrNakResult);
            }
         }
      }
   }
   catch (SocketException &e)
   {
      debugOutput::sendMessage("Socket Creation Exception was caught:" + e.description(), MSG_INFO);
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

void messageMediator::setRequestedSize(char size)
{
   m_requestedSize = size;
}

DF_ERROR messageMediator::parseCommandString()
{
   DF_ERROR e_ret = ERROR_BAD_PARAMS;

   string sCommand = getCommandString();
   char first_char = sCommand[0];

   // m_commandValue = std::stoi( sCommand );
   debugOutput::sendMessage("command length:" + to_string(sCommand.length()), MSG_INFO);

   if (sCommand.length() == 3)
   //  first_char == '1' ||
   //  first_char == '2' ||
   //  first_char == '3' ||
   //  first_char == '4')

   {
      // check for dispensing command
      e_ret = parseDispenseCommand(sCommand);
   }
   else if (
       // other commands
       first_char == ACTION_MANUAL_PUMP_PWM_SET ||
       first_char == ACTION_MANUAL_PUMP_SET ||
       first_char == ACTION_PRINT_TRANSACTION)
   {
      m_requestedAction = first_char;
      std::string number = sCommand.substr(1, sCommand.size());
      m_commandValue = std::stoi(number);
   }
   else if (
       // single digit commands
       first_char == '0' ||
       first_char == '1' ||
       first_char == '2' ||
       first_char == '3' ||
       first_char == '4' ||
       first_char == '5' ||
       first_char == '6' ||
       first_char == '7' ||
       first_char == '8' ||
       first_char == '9' ||

       //  first_char == ACTION_MANUAL_PRINTER ||
       //  first_char == ACTION_PRINTER_CHECK_STATUS ||
       first_char == ACTION_UI_COMMAND_PRINTER_SEND_STATUS ||
       first_char == ACTION_UI_COMMAND_PRINTER_MENU ||
       //  first_char == ACTION_PRINTER_CHECK_STATUS_TOGGLE_CONTINUOUSLY ||
       //  first_char == ACTION_PRINTER_PRINT_TEST ||
       //  first_char == ACTION_HELP ||
       //  first_char == ACTION_DEBUG ||
       //  first_char == ACTION_MANUAL_PUMP_TEST ||
       //  first_char == ACTION_TOGGLE_CYCLIC_PUMP_TEST ||
       //  first_char == ACTION_MANUAL_PUMP ||
       //  first_char == ACTION_MANUAL_PUMP_ENABLE ||
       //  first_char == ACTION_MANUAL_PUMP_DISABLE ||
       //  first_char == ACTION_MANUAL_PUMP_DIRECTION_FORWARD ||
       //  first_char == ACTION_MANUAL_PUMP_DIRECTION_REVERSE ||
       //  first_char == ACTION_MANUAL_PUMP_FLOW_TEST_TOGGLE ||
       //  first_char == ACTION_MANUAL_PUMP_CUSTOM_VOLUME_TEST_TOGGLE ||
       //  first_char == ACTION_PRINTER_REACHABLE
       first_char == ACTION_QUIT)
   {
      m_requestedAction = first_char;
   }
   else if (first_char == '$')
   {
      double price = std::stod(sCommand.substr(1, sCommand.size()));
      m_requestedDiscountPrice = price;
   }
   else if (sCommand.find("Promo") != string::npos)
   {
      std::string promoCode = sCommand.substr(6, sCommand.size());
      // debugOutput::sendMessage(promoCode, MSG_INFO);
      m_promoCode = promoCode;
   }
   else
   {
      // invalid commands
      m_requestedAction = ACTION_NO_ACTION;
      debugOutput::sendMessage("Command received is not valid.", MSG_INFO);
   }

   this->clearCommandString();
   return e_ret;
}

DF_ERROR messageMediator::parseDispenseCommand(string sCommand)
{

   DF_ERROR e_ret = ERROR_BAD_PARAMS;
   debugOutput::sendMessage("parseDispenseCommand", MSG_INFO);
   char temp[10];

   char productChar = PRODUCT_DUMMY;
   char actionChar = ACTION_DUMMY;
   char volumeChar = SIZE_DUMMY;

   // if (isdigit(sCommand[0]))
   if (isdigit(sCommand[0]))
   {
      productChar = sCommand[0];
      debugOutput::sendMessage("parsed digit", MSG_INFO);
   }

   if (sCommand.size() > 1)
   {

      // FIXME: Need a better string parser...
      for (std::string::size_type i = 0; i < sCommand.size(); ++i)
      {
         if ((sCommand[i] == ACTION_DISPENSE_END) || (sCommand[i] == ACTION_DISPENSE) || (sCommand[i] == ACTION_AUTOFILL) || sCommand[i] == PWM_CHAR)
         {
            actionChar = sCommand[i];
         }

         if (sCommand[i] == SIZE_SMALL_CHAR || sCommand[i] == SIZE_MEDIUM_CHAR || sCommand[i] == SIZE_LARGE_CHAR || sCommand[i] == SIZE_CUSTOM_CHAR || sCommand[i] == SIZE_TEST_CHAR)
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
      debugOutput::sendMessage("Product 1 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '2':
   {
      m_RequestedProductIndexInt = 2;
      debugOutput::sendMessage("Product 2 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '3':
   {
      m_RequestedProductIndexInt = 3;
      debugOutput::sendMessage("Product 3 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '4':
   {
      m_RequestedProductIndexInt = 4;
      debugOutput::sendMessage("Product 4 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   default:
   {
      debugOutput::sendMessage("No product requested [1..4]", MSG_INFO);
      break;
   }
   }

   if (!isalpha(volumeChar))
   {
      // e_ret = ERROR_NETW_NO_POSITION;
   }
   else if (volumeChar == SIZE_DUMMY)
   {
      debugOutput::sendMessage("No Requested volume provided", MSG_INFO);
   }
   else
   {
      switch (volumeChar)
      {
      case SIZE_SMALL_CHAR:
         debugOutput::sendMessage("Requested volume 1, Small Size", MSG_INFO);
         m_requestedSize = SIZE_SMALL_CHAR;
         e_ret = OK;
         break;

      case SIZE_MEDIUM_CHAR:
         debugOutput::sendMessage("Requested volume 2, Medium Size", MSG_INFO);
         m_requestedSize = SIZE_MEDIUM_CHAR;
         e_ret = OK;
         break;
      case SIZE_LARGE_CHAR:
         debugOutput::sendMessage("Requested volume 3, Large Size", MSG_INFO);
         m_requestedSize = SIZE_LARGE_CHAR;
         e_ret = OK;
         break;

      case SIZE_CUSTOM_CHAR:
         debugOutput::sendMessage("Requested volume custom Size", MSG_INFO);
         m_requestedSize = SIZE_CUSTOM_CHAR;
         e_ret = OK;
         break;

      case SIZE_TEST_CHAR:
         debugOutput::sendMessage("Requested volume test", MSG_INFO);
         m_requestedSize = SIZE_TEST_CHAR;
         e_ret = OK;
         break;

      default:
         debugOutput::sendMessage("Unknown volume received.... " + to_string(volumeChar), MSG_INFO);
         break;
      }
   }

   if (!isalpha(actionChar))
   {
      debugOutput::sendMessage("Irrelevant input .. ", MSG_INFO);
   }
   else if (actionChar == ACTION_DUMMY)
   {
      debugOutput::sendMessage("No action provided ", MSG_INFO);
   }
   else
   {
      // TODO: Parse and save a reference for command string

      switch (actionChar)
      {
      case ACTION_DISPENSE:
         debugOutput::sendMessage("Action: Dispense", MSG_INFO);
         // m_nSolenoid = PRODUCT;
         m_requestedAction = ACTION_DISPENSE;
         e_ret = OK;
         break;
      case ACTION_AUTOFILL:
         debugOutput::sendMessage("Action: Auto fill", MSG_INFO);
         // m_nSolenoid = PRODUCT;
         m_requestedAction = ACTION_AUTOFILL;
         e_ret = OK;
         break;

      case PWM_CHAR:
         debugOutput::sendMessage("Action: PWM", MSG_INFO);
         m_requestedAction = PWM_CHAR;
         e_ret = OK;
         break;

      case ACTION_DISPENSE_END:
         debugOutput::sendMessage("Action: End Dispense", MSG_INFO);
         m_requestedAction = ACTION_DISPENSE_END;
         e_ret = OK;
         break;

      default:
         break;
      }
   }

   return e_ret;
}
