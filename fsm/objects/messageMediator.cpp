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
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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
#include "../../library/printer/Adafruit_Thermal.h"

#define DELAY_USEC 1

extern messageMediator df_messaging;

bool messageMediator::m_fExitThreads = false;
bool messageMediator::m_bCommandStringReceived = false;
string messageMediator::m_receiveStringBuffer;
string messageMediator::m_processCommand;
int messageMediator::m_requested_slot;
int messageMediator::m_nSolenoid;
char messageMediator::m_requestedAction;
int messageMediator::m_commandValue;
char messageMediator::m_requestedSize;
double messageMediator::m_nVolumeTarget;
double messageMediator::m_requestedDiscountPrice;
string messageMediator::m_promoCode;

// CTOR
messageMediator::messageMediator() : m_machine(nullptr)
{

   debugOutput::sendMessage("Init messageMediator...", MSG_INFO);
   // TODO: Initialize with Pointer reference to socket...
   // new_sock = new ServerSocket();
   m_fExitThreads = false;
   // m_machine = nullptr; // already implied from the "member initialization list" (: m_machine(nullptr))
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

DF_ERROR messageMediator::setSendingBehaviour(bool enableElseDisableSending)
{
   m_enable_sending = enableElseDisableSending;

   DF_ERROR dfError = OK;
   return dfError;
}

DF_ERROR messageMediator::sendMessageOverIP(string msg, bool isLoggingMessage)
{
   DF_ERROR dfError = OK;
   if (!m_enable_sending)
   {
      if (isLoggingMessage)
      {
         debugOutput::sendMessage("Standalone mode. Will not send msg:  " + msg, MSG_INFO);
      }
      return dfError;
   }

   if (isLoggingMessage)
   {
      debugOutput::sendMessage("Send msg to UI (don't wait for reply): " + msg, MSG_INFO);
   }

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

      // usleep(10000);
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
            // usleep(10000);

            // client_socket >> reply;
            // debugOutput::sendMessage("REPLY from UI" + reply, MSG_INFO);

            done = true;
            // if (reply.compare("accepted") == 0){

            //    debugOutput::sendMessage("Message sent and accepted by UI", MSG_INFO);
            // }else{
            //    debugOutput::sendMessage("waiting for reply", MSG_INFO);
            // }
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
         debugOutput::sendMessage("UI server not reachable (UI not running? or Still processing previous message?).  " + e.description() + " Attempts left: " + to_string(attempts), MSG_ERROR);
         usleep(100000);
         attempts--;
      }
   }

   if (attempts <= 0)
   {
      debugOutput::sendMessage("Give up on communication. Amount of attempts exhausted. ", MSG_INFO);
   }

   return dfError;
}

void messageMediator::setMachine(machine *machine)
{
   if (machine == nullptr)
   {

      debugOutput::sendMessage("ASSERT ERROR: Nullpointer as argument. ", MSG_ERROR);
   }
   else
   {

      if (m_machine == nullptr)
      {
         // debugOutput::sendMessage("normal TO BE NULLPTR AT START ", MSG_INFO);
      }
      else
      {
         debugOutput::sendMessage("ASSERT ERROR: m_machine Not nullptr at init. Reinit??", MSG_WARNING);
      }
      this->m_machine = machine;
   }
}

void messageMediator::sendPrinterStatus()
{
   debugOutput::sendMessage("af;ldoeofoeflodleodloelodeolode.", MSG_INFO);
   if (!m_machine->getPcb24VPowerSwitchStatus())
   {
      m_machine->pcb24VPowerSwitch(true); // printers take their power from the 24V converted to 5V (because of the high current)
      // usleep(1200000);                   // wait for printer to come online.
      printerr->resetPollCount();
   }

   bool isOnline;
   bool hasPaper;
   debugOutput::sendMessage("af;eja;fuaeijfaesiofj.", MSG_INFO);
   getPrinterStatus(&isOnline, &hasPaper);

   string statusString;
   if (isOnline)
   {
      if (hasPaper)
      {
         statusString = "printerstatus11";
      }
      else
      {
         statusString = "printerstatus10";
      }
   }
   else
   {
      statusString = "printerstatus00";
   }

   sendMessageOverIP(statusString, true); // send to UI // if commented out: Let's communicate by setting the db fields only
   m_machine->pcb24VPowerSwitch(false);
}

void messageMediator::getPrinterStatus(bool *r_isOnline, bool *r_hasPaper)
{
   bool isOnline = printerr->testComms(); // first call returns always "online"
   isOnline = printerr->testComms();      // first call returns always "online"

   bool hasPaper = false;

   if (isOnline)
   {
      hasPaper = printerr->hasPaper();
   }

   if (printerr->getPollCountLimitReached())
   {

      printerr->resetPollCount();
      debugOutput::sendMessage("Pollcount LIMIT REACHED. Will restart Printer ", MSG_INFO);
      m_machine->pcb24VPowerSwitch(false);
      usleep(1200000); // 2000000ok //1500000ok //1200000ok //1000000nok
      m_machine->pcb24VPowerSwitch(true);
      // usleep(2000000); //1000000
   }
   *r_isOnline = r_isOnline;
   *r_hasPaper = hasPaper;
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
      //debugOutput::sendMessage("Received message (unparsed, finished): " + m_processCommand, MSG_INFO);
   }

   return df_ret;
}

// Parse/verify an incoming processing string
// then break down into a command string.
DF_ERROR messageMediator::updateCmdString()
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;
   //debugOutput::sendMessage("Process received message: " + m_receiveStringBuffer, MSG_INFO);

   if (!m_processCommand.empty())
   {
      debugOutput::sendMessage("Flush previous message from buffer. Old: " + m_processCommand, MSG_INFO);
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
   return 0;
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
            // debugOutput::sendMessage("Server: New incoming socket accepted. ", MSG_INFO);

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

   // debugOutput::sendMessage("command length:" + to_string(sCommand.length()), MSG_INFO);

   if (sCommand.find("pcabugfix") != string::npos)
   {
      // debugOutput::sendMessage("*************************************************", MSG_INFO);
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      m_requestedAction = ACTION_REPAIR_PCA;
   }
   else if (sCommand.find("Ping") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // simple is alive command will reset to idle state
      m_requestedAction = ACTION_RESET;
   }
   else if (sCommand.find("getTemperature") != string::npos)
   {
      // debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      sendTemperatureData();
      m_requestedAction = ACTION_NO_ACTION;
   }
   else if (sCommand.find("pcbPower") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      std::string delimiter = "|";
      std::size_t found0 = sCommand.find(delimiter);
      std::size_t found1 = sCommand.find(delimiter, found0 + 1);

      std::string command_argument = sCommand.substr(found0 + 1, found1 - found0 - 1);

      bool switchPcbOnElseOff = false;
      if (command_argument == "ON")
      {
         switchPcbOnElseOff = true;
      }
      else if (command_argument == "OFF")
      {
         switchPcbOnElseOff = false;
      }

      m_machine->pcb3point3VPowerSwitch(switchPcbOnElseOff);
      m_requestedAction = ACTION_NO_ACTION;
   }

   else if (sCommand.find("DispenseButtonLights") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // simple is alive command will reset to idle state
      // e.g.   ButtonLights|ON
      std::string delimiter = "|";
      std::size_t found0 = sCommand.find(delimiter);
      std::size_t found1 = sCommand.find(delimiter, found0 + 1);

      std::string button_status = sCommand.substr(found0 + 1, found1 - found0 - 1);

      if (button_status == "ANIMATE")
      {
         m_machine->setButtonLightsBehaviour(Button_lights_behaviour::IDLE_ANIMATION_FROM_DB);
      }
      else if (button_status == "OFF")
      {
         m_machine->setButtonLightsBehaviour(Button_lights_behaviour::IDLE_OFF);
      }
      m_requestedAction = ACTION_NO_ACTION;
   }
   else if (sCommand.find("stopDispense") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      debugOutput::sendMessage("Action: Abort Dispense Request", MSG_INFO);
      m_requestedAction = ACTION_DISPENSE_END;
   }
   else if (sCommand.find("dispenseCustomMix") != string::npos)
   {
      // dipenseMix|slot|dispenseProduct|pnumberscsv|ratioscsv
      // e.g.    // dipenseMix|1|91|95|1|
      // e.g.    // dipenseMix|1|91|95,3,4|0.6,0.2,0.2|

      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      debugOutput::sendMessage("Dispense command found", MSG_INFO);
      std::string delimiter = "|";
      std::size_t found0 = sCommand.find(delimiter);
      std::size_t found1 = sCommand.find(delimiter, found0 + 1);
      std::size_t found2 = sCommand.find(delimiter, found1 + 1);
      std::size_t found3 = sCommand.find(delimiter, found2 + 1);
      std::size_t found4 = sCommand.find(delimiter, found3 + 1);

      // debugOutput::sendMessage(to_string(found0), MSG_INFO);
      // debugOutput::sendMessage(to_string(found1), MSG_INFO);
      // debugOutput::sendMessage(to_string(found2), MSG_INFO);
      // debugOutput::sendMessage(to_string(found3), MSG_INFO);
      // debugOutput::sendMessage(to_string(found4), MSG_INFO);

      std::string dispenseCommand = sCommand.substr(found0 + 1, found1 - found0 - 1);
      debugOutput::sendMessage("Dispense command: " + dispenseCommand, MSG_INFO);
      parseDispenseCommand(dispenseCommand);

      std::string pNumberDispenseProduct_str = sCommand.substr(found1 + 1, found2 - found1 - 1);
      debugOutput::sendMessage("Selected Dispense PNumber : " + pNumberDispenseProduct_str, MSG_INFO);

      int pNumberDispenseProduct = std::stoi(pNumberDispenseProduct_str);

      std::string pnumbers = sCommand.substr(found2 + 1, found3 - found2 - 1);
      debugOutput::sendMessage("Pnumbers : " + pnumbers, MSG_INFO);

      std::string ratios = sCommand.substr(found3 + 1, found4 - found3 - 1);
      debugOutput::sendMessage("Dispense Ratios : " + ratios, MSG_INFO);

      m_machine->m_productDispensers[getRequestedSlot() - 1].setSelectedProduct(pNumberDispenseProduct);
      m_machine->m_productDispensers[getRequestedSlot() - 1].setCustomMixParametersToSelectedProduct(pnumbers, ratios);
      // int pnumber = m_machine->m_productDispensers[getRequestedSlot() - 1].getCustomMixPNumberFromMixIndex(0);
      // m_machine->m_productDispensers[getRequestedSlot() - 1].setPNumberAsSingleDispenseSelectedProduct(pnumber);
   }
   else if (sCommand.find("dispensePNumber") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // dipenseMix|slot|dispenseProduct
      // e.g.    // dipenseMix|1|91|
      // e.g.    // dipenseMix|1|91|

      debugOutput::sendMessage("Dispense command found", MSG_INFO);
      std::string delimiter = "|";
      std::size_t found0 = sCommand.find(delimiter);
      std::size_t found1 = sCommand.find(delimiter, found0 + 1);
      std::size_t found2 = sCommand.find(delimiter, found1 + 1);

      // debugOutput::sendMessage(to_string(found0), MSG_INFO);
      // debugOutput::sendMessage(to_string(found1), MSG_INFO);
      // debugOutput::sendMessage(to_string(found2), MSG_INFO);

      std::string dispenseCommand = sCommand.substr(found0 + 1, found1 - found0 - 1);
      debugOutput::sendMessage("Dispense command: " + dispenseCommand, MSG_INFO);
      parseDispenseCommand(dispenseCommand);

      std::string pNumberDispenseProduct_str = sCommand.substr(found1 + 1, found2 - found1 - 1);

      int pNumberDispenseProduct = std::stoi(pNumberDispenseProduct_str);

      m_machine->m_productDispensers[getRequestedSlot() - 1].setSelectedProduct(pNumberDispenseProduct);
      debugOutput::sendMessage("Selected Dispense PNumber (for slot " + to_string(getRequestedSlot()) + " ): " + to_string(m_machine->m_productDispensers[getRequestedSlot() - 1].getSelectedPNumber()), MSG_INFO);
   }
   else if (sCommand.find("orderDetails") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // e.g.   orderDetails|2.2|super30off  // price|couponcode
      debugOutput::sendMessage("Order details received", MSG_INFO);
      std::string delimiter = "|";
      std::size_t found0 = sCommand.find(delimiter);
      std::size_t found1 = sCommand.find(delimiter, found0 + 1);
      std::size_t found2 = sCommand.find(delimiter, found1 + 1);

      debugOutput::sendMessage(to_string(found0), MSG_INFO);
      debugOutput::sendMessage(to_string(found1), MSG_INFO);
      debugOutput::sendMessage(to_string(found2), MSG_INFO);

      std::string pricestr = sCommand.substr(found0 + 1, found1 - found0 - 1);
      double price = std::stod(pricestr);
      m_requestedDiscountPrice = price;
      debugOutput::sendMessage("(Discount) price : " + to_string(m_requestedDiscountPrice), MSG_INFO);

      std::string promoCode = "";
      if (found1 != string::npos)
      {
         promoCode = sCommand.substr(found1 + 1, found2 - found1 - 1);
      }
      m_promoCode = promoCode;
      debugOutput::sendMessage("Promo code" + m_promoCode, MSG_INFO);
      m_requestedAction = ACTION_NO_ACTION;
   }
   // else if (sCommand.find("Order") != string::npos)
   // {
   //    // e.g.   Order|1sd|2.2|super30off
   //    debugOutput::sendMessage("Order command found", MSG_INFO);
   //    std::string delimiter = "|";
   //    std::size_t found0 = sCommand.find(delimiter);
   //    std::size_t found1 = sCommand.find(delimiter, found0 + 1);
   //    std::size_t found2 = sCommand.find(delimiter, found1 + 1);
   //    std::size_t found3 = sCommand.find(delimiter, found2 + 1);

   //    debugOutput::sendMessage(to_string(found0), MSG_INFO);
   //    debugOutput::sendMessage(to_string(found1), MSG_INFO);
   //    debugOutput::sendMessage(to_string(found2), MSG_INFO);
   //    debugOutput::sendMessage(to_string(found3), MSG_INFO);

   //    // if (found1 != string::npos)
   //    //    cout<< std::to_string(found1) << "\n";

   //    // std::size_t found = str.find(str2);
   //    std::string dispenseCommand = sCommand.substr(found0 + 1, found1 - found0 - 1);
   //    debugOutput::sendMessage("Dispense command: " + dispenseCommand, MSG_INFO);
   //    parseDispenseCommand(dispenseCommand);

   //    std::string pricestr = sCommand.substr(found1 + 1, found2 - found1 - 1);
   //    double price = std::stod(pricestr);
   //    m_requestedDiscountPrice = price;
   //    debugOutput::sendMessage("(Discount) price : " + to_string(m_requestedDiscountPrice), MSG_INFO);

   //    std::string promoCode = "";
   //    if (found1 != string::npos)
   //    {
   //       promoCode = sCommand.substr(found2 + 1, found3 - found2 - 1);
   //    }
   //    m_promoCode = promoCode;
   //    debugOutput::sendMessage("Promo code" + m_promoCode, MSG_INFO);

   //    m_machine->m_productDispensers[getRequestedSlot() - 1].setBasePNumberAsSingleDispenseSelectedProduct();
   // }
   else if (sCommand.length() == 3)
   //  first_char == '1' ||
   //  first_char == '2' ||
   //  first_char == '3' ||
   //  first_char == '4')

   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // length 3 command is always a dispense instruction. This is annoying, but it's grown organically like that.
      // check for dispensing command
      e_ret = parseDispenseCommand(sCommand);
   }
   else if (sCommand.find("Printer") != string::npos)
   {
      // debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      m_requestedAction = ACTION_UI_COMMAND_PRINTER_MENU;
   }
   else if (sCommand.find("getThermalprinterStatus") != string::npos)
   {
      // debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // debugOutput::sendMessage("Printer status request received.", MSG_INFO);
      // sendPrinterStatus();
      m_requestedAction = ACTION_UI_COMMAND_PRINTER_SEND_STATUS;
   }
   else if (sCommand.find("thermalprinterPrintTest") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // debugOutput::sendMessage("Printer status request received.", MSG_INFO);
      // sendPrinterStatus();
      m_requestedAction = ACTION_UI_COMMAND_TEST_PRINT;
   }
   else if (sCommand.find("thermalprinterPrintTransaction") != string::npos)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      // sendPrinterStatus();

      debugOutput::sendMessage("Print transaction command received", MSG_INFO);
      std::string delimiter = "|";
      std::size_t found0 = sCommand.find(delimiter);
      std::size_t found1 = sCommand.find(delimiter, found0 + 1);

      debugOutput::sendMessage(to_string(found0), MSG_INFO);
      debugOutput::sendMessage(to_string(found1), MSG_INFO);

      std::string id = sCommand.substr(found0 + 1, found1 - found0 - 1);
      int transaction_id = std::stoi(id);
      debugOutput::sendMessage("Transaction id: " + to_string(transaction_id), MSG_INFO);

      m_requestedAction = ACTION_UI_COMMAND_PRINT_TRANSACTION;
      m_commandValue = transaction_id;
   }
   else if (
       // other commands
       first_char == ACTION_MANUAL_PUMP_PWM_SET ||
       first_char == ACTION_MANUAL_PUMP_SET)
   {
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
      m_requestedAction = first_char;
      std::string number = sCommand.substr(1, sCommand.size());
      m_commandValue = std::stoi(number);
   }
   else if (
      debugOutput::sendMessage("Received Message: " + sCommand, MSG_INFO);
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
       //  first_char == ACTION_UI_COMMAND_PRINTER_SEND_STATUS ||
       // first_char == ACTION_UI_COMMAND_PRINTER_MENU ||
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

   else
   {
      // invalid commands
      m_requestedAction = ACTION_NO_ACTION;
      debugOutput::sendMessage("Command received is not valid.", MSG_INFO);
   }

   this->clearCommandString();
   return e_ret;
}

void messageMediator::sendTemperatureData()
{
   // debugOutput::sendMessage("Temperature requested", MSG_INFO);   //check if we send the temperature data
   double temperature_1 = 666.0;
   double temperature_2 = 666.0;

   string str_temperature_1;
   string str_temperature_2;

   if (m_machine->control_pcb->isTemperatureSensorMCP9808Available_1())
   {
      temperature_1 = m_machine->control_pcb->getTemperature(pcb::external_sensor_fridge);
   }

   if (m_machine->control_pcb->isTemperatureSensorMCP9808Available_2())
   {
      temperature_2 = m_machine->control_pcb->getTemperature(pcb::external_sensor_cavity);
   }
   char temp_celcius_chars[MAX_BUF];
   snprintf(temp_celcius_chars, sizeof(temp_celcius_chars), "%.2f", temperature_1);

   str_temperature_1 = temp_celcius_chars;

   snprintf(temp_celcius_chars, sizeof(temp_celcius_chars), "%.2f", temperature_2);
   str_temperature_2 = temp_celcius_chars;

   sendMessageOverIP("temperature|" + str_temperature_1 + "|" + str_temperature_2, false);
}
void messageMediator::resetAction()
{
   m_requestedAction = ACTION_NO_ACTION;
}

void messageMediator::setDispenseCommandToDummy()
{
   m_requestedAction = ACTION_DUMMY;
   m_requested_slot = PRODUCT_SLOT_DUMMY;
   m_requestedSize = SIZE_DUMMY;
}

DF_ERROR messageMediator::parseDispenseCommand(string sCommand)
{

   DF_ERROR e_ret = ERROR_BAD_PARAMS;
   debugOutput::sendMessage("parseDispenseCommand", MSG_INFO);
   char temp[10];

   char productChar = PRODUCT_DUMMY;
   char actionChar = ACTION_DUMMY;
   char volumeChar = SIZE_DUMMY;

   m_requestedAction = ACTION_DUMMY;
   m_requested_slot = PRODUCT_SLOT_DUMMY;
   m_requestedSize = SIZE_DUMMY;

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

         if (sCommand[i] == SIZE_SMALL_CHAR || sCommand[i] == SIZE_MEDIUM_CHAR || sCommand[i] == SIZE_LARGE_CHAR || sCommand[i] == SIZE_CUSTOM_CHAR || sCommand[i] == SIZE_SAMPLE_CHAR || sCommand[i] == SIZE_TEST_CHAR)
         {
            volumeChar = (sCommand[i]);
         }
      }
   }

   switch (productChar)
   {
   case PRODUCT_DUMMY:
   {
      m_requested_slot = PRODUCT_SLOT_DUMMY;
      debugOutput::sendMessage("Invalid product char.", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '1':
   {
      m_requested_slot = 1;
      debugOutput::sendMessage("Product from slot 1 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '2':
   {
      m_requested_slot = 2;
      debugOutput::sendMessage("Product from slot 2 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '3':
   {
      m_requested_slot = 3;
      debugOutput::sendMessage("Product from slot 3 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '4':
   {
      m_requested_slot = 4;
      debugOutput::sendMessage("Product from slot 4 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '5':
   {
      m_requested_slot = 5;
      debugOutput::sendMessage("Product from slot 5 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '6':
   {
      m_requested_slot = 6;
      debugOutput::sendMessage("Product from slot 6 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '7':
   {
      m_requested_slot = 7;
      debugOutput::sendMessage("Product from slot 7 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   case '8':
   {
      m_requested_slot = 8;
      debugOutput::sendMessage("Product from slot 8 requested", MSG_INFO);
      e_ret = OK;
      break;
   }
   default:
   {
      debugOutput::sendMessage("No product requested [1..8] required", MSG_INFO);
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

      case SIZE_SAMPLE_CHAR:
         debugOutput::sendMessage("Requested volume sample", MSG_INFO);
         m_requestedSize = SIZE_SAMPLE_CHAR;
         e_ret = OK;
         break;

      default:
         debugOutput::sendMessage("Unknown volume received.... " + to_string(volumeChar), MSG_INFO);
         break;
      }
   }

   if (!isalpha(actionChar))
   {
      m_requestedAction = ACTION_DUMMY;
      debugOutput::sendMessage("Irrelevant input .. ", MSG_INFO);
   }
   else if (actionChar == ACTION_DUMMY)
   {
      m_requestedAction = ACTION_DUMMY;
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
         m_requestedAction = ACTION_DUMMY;
         break;
      }
   }

   return e_ret;
}


