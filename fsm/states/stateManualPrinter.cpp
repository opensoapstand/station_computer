//***************************************
//
// stateManualPrinter.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 01-2022
// by:Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "stateManualPrinter.h"

#define STRING_STATE_MANUAL_PRINTER "Manual Printer"

// Default CTOR
stateManualPrinter::stateManualPrinter()
{
   // printerr = new Adafruit_Thermal();
}

// CTOR Linked to IP Thread Socket Listener
stateManualPrinter::stateManualPrinter(messageMediator *message)
{
}

// DTOR
stateManualPrinter::~stateManualPrinter()
{
   // delete stuff
}

// Overload for Debugger output
string stateManualPrinter::toString()
{
   return STRING_STATE_MANUAL_PRINTER;
}

// Sets a looped Idle state
DF_ERROR stateManualPrinter::onEntry()
{
   m_state_requested = STATE_MANUAL_PRINTER;
   DF_ERROR e_ret = OK;
   debugOutput::sendMessage("Test printer manually.", MSG_INFO);
   printerr.connectToPrinter();
   b_isContinuouslyChecking = false;
   return e_ret;
}

DF_ERROR stateManualPrinter::onAction()
{
   DF_ERROR e_ret = OK;

   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit printer status test", MSG_INFO);

         m_state_requested = STATE_IDLE;
      }
      if (ACTION_PRINTER_SEND_STATUS == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Printer status requested by UI", MSG_INFO);
         sendPrinterStatus();
         m_state_requested = STATE_IDLE; // return after finished.
      }

      // If ACTION_DISPENSE is received, enter Dispense state, else, stay in Idle state
      if (ACTION_PRINTER_CHECK_STATUS_TOGGLE_CONTINUOUSLY == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Toggle Continuous Printer display status", MSG_INFO);
         b_isContinuouslyChecking = !b_isContinuouslyChecking;
      }

      if (ACTION_PRINTER_CHECK_STATUS == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Get Printer display status", MSG_INFO);
         displayPrinterStatus();
      }
      if (ACTION_PRINTER_PRINT_TEST == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Do test print", MSG_INFO);
         printTest();
      }
      if (ACTION_PRINTER_REACHABLE == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Is Printer reachable?", MSG_INFO);
         displayPrinterReachable();
      }
      if (ACTION_HELP == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("help\nAvailable printer test commands: \n l: test print\n V: printer status toggle continuous mode\n v:printer status \n q:quit test mode \n r: Printer reachable", MSG_INFO);
      }
   }

   if (b_isContinuouslyChecking)
   {
      displayPrinterStatus();
      // usleep(50000);	// 50 ms	between actions
   }

   // m_state = STATE_MANUAL_PRINTER;

   // printerr.feed(1);
   // printerr.connectToPrinter();
   // printerr.cancelCustomCharacters();

   // char tmpTest;
   // waitSerial++;
   // tmpTest = printerr.testComms(waitSerial);

   // std::string c3(1,tmpTest);

   // debugOutput::sendMessage("test byte:==" + c3 + "==", MSG_INFO);
   // debugOutput::sendMessage("0.1 ms multiplier: " + std::to_string(waitSerial), MSG_INFO);

   // if (tmpTest == 0){
   //    // debugOutput::sendMessage("zero", MSG_INFO);
   // }else if (tmpTest == 0x04){
   //    debugOutput::sendMessage("four", MSG_INFO);

   // }else{

   //    std::string c3(1,tmpTest);

   //    debugOutput::sendMessage("test byte:" + c3, MSG_INFO);

   // }

   // printerr.disconnectPrinter();

   // usleep(500000);
   // hasPaper

   e_ret = OK;

   return e_ret;
}

DF_ERROR stateManualPrinter::sendPrinterStatus()
{

   bool isOnline = printerr.testComms();
   bool hasPaper = printerr.hasPaper();
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

   m_pMessaging->sendMessage(statusString);

#ifndef USE_OLD_DATABASE

   char *zErrMsg = 0;

   // FIXME: DB needs fully qualified link to find...obscure with XML loading.
   rc = sqlite3_open(DB_PATH, &db);

   std::string sql21;
   sql21 = ("UPDATE machine SET receipt_printer_is_online=" + to_string(isOnline) + ",receipt_printer_has_paper=" + to_string(hasPaper) + ";"); // omit where cause --> all rows will be updated.

   char *sql_prod21 = new char[sql21.length() + 1];
   strcpy(sql_prod21, sql21.c_str());

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql_prod21, NULL, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
      debugOutput::sendMessage("ERROR: SQL2 : (" + to_string(rc) + "):" + sql21, MSG_INFO);

      sqlite3_free(zErrMsg);
   }
   else
   {
      debugOutput::sendMessage("SUCCES: SQL2 : (" + to_string(rc) + ") " + sql21, MSG_INFO);
   }
#endif
}

DF_ERROR stateManualPrinter::displayPrinterStatus()
{
   if (printerr.hasPaper())
   {
      debugOutput::sendMessage("has paper", MSG_INFO);
   }
   else
   {
      debugOutput::sendMessage("has NO paper----------------------------------------", MSG_INFO);
   }
}
DF_ERROR stateManualPrinter::displayPrinterReachable()
{
   if (printerr.testComms())
   {
      debugOutput::sendMessage("printer reachable", MSG_INFO);
   }
   else
   {
      debugOutput::sendMessage("printer not reachable", MSG_INFO);
   }
}

DF_ERROR stateManualPrinter::printTest()
{

   string printerstring = "lodelode";
   string plu = "978020137962";

   // Adafruit_Thermal printerr;
   printerr.printBarcode(plu.c_str(), EAN13);
   system("echo '\n---------------------------\n\n\n' > /dev/ttyS4");

   string printer_command_string = "echo '\n---------------------------\n\n\n" + printerstring + "' > /dev/ttyS4";
   system(printer_command_string.c_str());

   //  string printer_command_string = "echo '------------------------------\n-- Vancouver Active Tourism --\n--            2022-02-12    --\n------------------------------\n 1x Special morning activity \n 1x Batard Bakery experience \n 1x Guided bike tour to \n         Lighthouse park \n 1x Soapstand workplace demo \n Participants: Wafeltje + Lodey    \n   Grand total: Happy times <3 \n   Thank you, please come again!  \n\n\n\n' > /dev/ttyS4";
   //  system(printer_command_string.c_str());

   //  printerr->setBarcodeHeight(100);
}

// Advances to Dispense Idle
DF_ERROR stateManualPrinter::onExit()
{
   // stop continuous checking setting
   b_isContinuouslyChecking = false;

   // printerr.connectToPrinter();
   // printTest();
   // printerr.testPage();
   // usleep(500000);
   printerr.disconnectPrinter();
   DF_ERROR e_ret = OK;
   return e_ret;
}
