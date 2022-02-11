//***************************************
//
// stateManualMode.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 29-06-2020
// by: Jason Wang & Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateManualMode.h"

#define IDLE_STRING "Idle"

// Default CTOR
stateManualMode::stateManualMode()
{
   // printerr = new Adafruit_Thermal();
}

// CTOR Linked to IP Thread Socket Listener
stateManualMode::stateManualMode(messageMediator *message)
{
}

// DTOR
stateManualMode::~stateManualMode()
{
   // delete stuff
}

// Overload for Debugger output
string stateManualMode::toString()
{
   return IDLE_STRING;
}

// Sets a looped Idle state
DF_ERROR stateManualMode::onEntry()
{
   m_state = MANUAL_MODE;
   m_state_requested = MANUAL_MODE;
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("test printer stuff.", INFO);
    
   waitSerial = 0;
   printerr.connectToPrinter();
   return e_ret;
}

/*
* Advances State: If IP Thread detects DISPENSE
* command then advance to DISPENSE_IDLE
*/
DF_ERROR stateManualMode::onAction()
{
   DF_ERROR e_ret = OK;

   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (m_pMessaging->getAction() == ACTION_QUIT_TEST)
      {
         
         debugOutput::sendMessage("back to idle", INFO);
         m_state_requested = IDLE;
      }
   }
   
   // printerr.feed(1);
   // printerr.connectToPrinter();
  
  
   //printerr.cancelCustomCharacters();
  
  
   char tmpTest;
   waitSerial++;
   tmpTest = printerr.testComms(waitSerial);
   
   std::string c3(1,tmpTest);
   
   debugOutput::sendMessage("test byte:==" + c3 + "==", INFO);
   debugOutput::sendMessage("0.1 ms multiplier: " + std::to_string(waitSerial), INFO);

   // if (tmpTest == 0){
   //    // debugOutput::sendMessage("zero", INFO);
   // }else if (tmpTest == 0x04){
   //    debugOutput::sendMessage("four", INFO);

   // }else{

   //    std::string c3(1,tmpTest);
      
   //    debugOutput::sendMessage("test byte:" + c3, INFO);

   // }

   // if (printerr.hasPaper()){
   //    debugOutput::sendMessage("has paper", INFO);

   // }else{
   //    debugOutput::sendMessage("has NO paper----------------------------------------", INFO);
   // }
   
   // printerr.disconnectPrinter();

   usleep(50000);			
   // usleep(500000);			
   // hasPaper

   e_ret = OK;
  
   return e_ret;
}


DF_ERROR stateManualMode::printTest(){

   
   string printerstring = "lodelode";
   string plu = "978020137962";

   // Adafruit_Thermal printerr;
   printerr.printBarcode(plu.c_str(), EAN13);
   system("echo '\n---------------------------\n\n\n' > /dev/ttyS4");

   string sysstring = "echo '\n---------------------------\n\n\n" + printerstring + "' > /dev/ttyS4";
    system(sysstring.c_str());
   //  printerr->setBarcodeHeight(100);
}

// Advances to Dispense Idle
DF_ERROR stateManualMode::onExit()
{
   // printerr.connectToPrinter();
   // printTest();
   // printerr.testPage();
   //usleep(500000);
   printerr.disconnectPrinter();
   DF_ERROR e_ret = OK;
   return e_ret;
}
