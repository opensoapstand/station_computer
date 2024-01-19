//***************************************
//
// stateManualPrinter.cpp
// idle state class
//
// After stateInit: Waits for a dispense
// command from IPC QT Socket
//
// created: 01-2022
// by:Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "stateManualPrinter.h"
#define STRING_STATE_MANUAL_PRINTER "Manual Printer"

#define MAX_BUF 64

// Default CTOR
stateManualPrinter::stateManualPrinter()
{
   // printerr = new Adafruit_Thermal();
}

// CTOR Linked to IP Thread Socket Listener
stateManualPrinter::stateManualPrinter(messageMediator *message)
{
   m_pMessaging = message;
}

// DTOR
stateManualPrinter::~stateManualPrinter()
{
   // delete stuff
   debugOutput::sendMessage("stateManualPrinter: ~stateManualPrinter", MSG_INFO);
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
   debugOutput::sendMessage("Enter Printer commmands state.", MSG_INFO);

   printerr = g_machine.receipt_printer;

   printerr->connectToPrinter();
   b_isContinuouslyChecking = false;
   // productDispensers = g_productDispensers;

   if (!g_machine.getPcb24VPowerSwitchStatus())
   {
      g_machine.pcb24VPowerSwitch(true); // printers take their power from the 24V converted to 5V (because of the high current)
      usleep(1200000);                   // wait for printer to come online.
      printerr->resetPollCount();
   }

   return e_ret;
}

DF_ERROR stateManualPrinter::onAction()
{
   DF_ERROR e_ret = OK;

   // not a new command, reuse when coming from idle.
   if (m_pMessaging->getAction() == ACTION_UI_COMMAND_PRINTER_SEND_STATUS)
   {
      debugOutput::sendMessage("Multi command. Send printer status and exit.", MSG_INFO);
      sendPrinterStatus();
      m_state_requested = STATE_IDLE;
   }
   else if (ACTION_UI_COMMAND_PRINT_TRANSACTION == m_pMessaging->getAction())
   {
      // ACTION_TRANSACTION_ID
      int id = m_pMessaging->getCommandValue();
      debugOutput::sendMessage("Print transaction id : " + to_string(id), MSG_INFO);
      printTransaction(id);
      m_state_requested = STATE_IDLE;
   }
   else if (ACTION_UI_COMMAND_TEST_PRINT == m_pMessaging->getAction())
   {
      // ACTION_TRANSACTION_ID
      int id = m_pMessaging->getCommandValue();
      debugOutput::sendMessage("Print test print. ", MSG_INFO);
       printTest();
      m_state_requested = STATE_IDLE;
   }


   // Check if Command String is ready
   if (m_pMessaging->isCommandStringReadyToBeParsed())
   {
      DF_ERROR ret_msg;
      ret_msg = m_pMessaging->parseCommandString();

      if (m_pMessaging->getAction() == ACTION_RESET)
      {
         m_pMessaging->sendMessageOverIP("Init Ready", true); // send to UI
         m_state_requested = STATE_IDLE;
      }
      else if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit printer status test", MSG_INFO);

         m_state_requested = STATE_IDLE;
      }
      else if ('1' == m_pMessaging->getAction())
      {
         // DO NOT CHANGE THE COMMAND NUMBER. This is sent from the UI for checking the printer status.
         debugOutput::sendMessage("Printer status requested by UI", MSG_INFO);
         // sendPrinterStatus(); // first call after startup returns always online
         // displayPrinterStatus();
         // displayPrinterStatus();
         sendPrinterStatus();
         m_state_requested = STATE_IDLE; // return after finished.
      }

      else if ('7' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Do test print", MSG_INFO);
         printTest();
      }
      // If ACTION_DISPENSE is received, enter Dispense state, else, stay in Idle state
      else if ('2' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Toggle Continuous Printer display status", MSG_INFO);
         b_isContinuouslyChecking = !b_isContinuouslyChecking;
      }

      else if ('3' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Get Printer display status", MSG_INFO);
         displayPrinterStatus();
      }
      else if ('4' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Is Printer reachable?", MSG_INFO);
         displayPrinterReachable();
      }
      else if (ACTION_PRINT_TRANSACTION == m_pMessaging->getAction())
      {
         // ACTION_TRANSACTION_ID
         int id = m_pMessaging->getCommandValue();
         debugOutput::sendMessage("Print id : " + to_string(id), MSG_INFO);
         printTransaction(id);
      }
      else if ('5' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Print transaction?", MSG_INFO);
         printTransaction(959);
      }
      else if ('6' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Test send OK to UI", MSG_INFO);
         m_pMessaging->sendMessageOverIP("OK", true); // send to UI
      }
      else
      {
         debugOutput::sendMessage("---Receipt printer menu---"
                                  "Available printer test commands: \n"
                                  " 0: Exit printer menu \n"
                                  " 1: emulated auto check request from UI \n"
                                  " 2: Printer status toggle continuous mode\n"
                                  " 3: Printer status \n"
                                  " 4: Check printer connected\n"
                                  " 5: Print transaction 959\n"
                                  " 6: Test send OK to UI\n"
                                  " 7: Test print\n"
                                  " h: Display this help menu",
                                  MSG_INFO);
      }
   }

   if (b_isContinuouslyChecking)
   {
      displayPrinterStatus();

      usleep(500000);
   }
   e_ret = OK;

   return e_ret;
}

void stateManualPrinter::printTransaction(int transactionNumber)
{
   // gets transaction data from db.
   char *zErrMsg = 0;
   rc = sqlite3_open(USAGE_DB_PATH, &db);
   sqlite3_stmt *stmt;
   std::string sql_string;

   //-------------------------------------------------
   sql_string = ("SELECT soapstand_product_serial,price,quantity_dispensed,quantity_requested,end_time FROM transactions WHERE id=" + to_string(transactionNumber));
   sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

   int status;
   status = sqlite3_step(stmt);

   string pNumberString;
   double price;
   double quantity_dispensed;
   double quantity_requested;
   string end_time;
   if (status == SQLITE_ROW)
   {
      pNumberString = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
      price = sqlite3_column_double(stmt, 1);
      quantity_dispensed = sqlite3_column_double(stmt, 2);
      quantity_requested = sqlite3_column_double(stmt, 3);
      end_time = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)));
      ;
   }
   else if (status == SQLITE_DONE)
   {
      debugOutput::sendMessage("ERROR: transaction not found. Id: " + to_string(transactionNumber), MSG_INFO);
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return;
   }
   else
   {
      debugOutput::sendMessage("ERROR retrieving transaction Id: " + to_string(transactionNumber), MSG_INFO);
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return;
   }

   if (rc != SQLITE_OK)
   {
      debugOutput::sendMessage("ERROR: SQL transaction retrieval : (" + to_string(rc) + "):" + sql_string, MSG_INFO);
      sqlite3_free(zErrMsg);
   }
   else
   {
      debugOutput::sendMessage("SUCCESS: SQL transaction retrieval : (" + to_string(rc) + ") " + sql_string, MSG_INFO);
   }

   debugOutput::sendMessage("----------------: " + pNumberString, MSG_INFO);
   debugOutput::sendMessage("----------------: " + to_string(price), MSG_INFO);
   debugOutput::sendMessage("----------------: " + to_string(quantity_dispensed), MSG_INFO);
   debugOutput::sendMessage("----------------: " + to_string(quantity_requested), MSG_INFO);
   debugOutput::sendMessage("----------------: " + end_time, MSG_INFO);
   sqlite3_finalize(stmt);
   sqlite3_close(db);

   int pnumber = machine::convertPStringToPNumber(pNumberString);
   setup_receipt_from_pnumber_and_dispense_data(pnumber, quantity_dispensed, quantity_requested, price, end_time);
}

DF_ERROR stateManualPrinter::sendPrinterStatus()
{

   // bool isOnline = printerr->testComms();
   // bool hasPaper = printerr->hasPaper();
   bool isOnline;
   bool hasPaper;

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

   char *zErrMsg = 0;

   rc = sqlite3_open(CONFIG_DB_PATH, &db);

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
      debugOutput::sendMessage("SUCCESS: SQL2 : (" + to_string(rc) + ") " + sql21, MSG_INFO);
   }

   sqlite3_close(db);

   // // power cycling the printer. This will erase a annoying error that every 11th poll, one charater is printed.
   // if (printerr->getPollCountLimitReached())
   // {
   //    printerr->resetPollCount();

   //    debugOutput::sendMessage("Pollcount LIMIT REACHED. Will restart Printer ", MSG_INFO);
   //    g_machine.pcb24VPowerSwitch(false);
   //    usleep(1200000);
   //    g_machine.pcb24VPowerSwitch(true);
   // }

   m_pMessaging->sendMessageOverIP(statusString, true); // send to UI // if commented out: Let's communicate by setting the db fields only
}

DF_ERROR stateManualPrinter::getPrinterStatus(bool *r_isOnline, bool *r_hasPaper)
{
   bool isOnline = printerr->testComms(); // first call returns always "online"
   isOnline = printerr->testComms();

   bool hasPaper = false;

   if (isOnline)
   {
      hasPaper = printerr->hasPaper();
   }

   if (printerr->getPollCountLimitReached())
   {

      printerr->resetPollCount();
      debugOutput::sendMessage("Pollcount LIMIT REACHED. Will restart Printer ", MSG_INFO);
      g_machine.pcb24VPowerSwitch(false);
      usleep(1200000); // 2000000ok //1500000ok //1200000ok //1000000nok
      g_machine.pcb24VPowerSwitch(true);
      // usleep(2000000); //1000000
   }
   *r_isOnline = r_isOnline;
   *r_hasPaper = hasPaper;
}

DF_ERROR stateManualPrinter::displayPrinterStatus()
{

   bool isOnline;
   bool hasPaper;
   getPrinterStatus(&isOnline, &hasPaper);

   if (isOnline)
   {

      if (hasPaper)
      {
         debugOutput::sendMessage("Printer online, has paper.", MSG_INFO);
      }
      else
      {
         debugOutput::sendMessage("Printer online, no paper.", MSG_INFO);
      }
   }
   else
   {
      debugOutput::sendMessage("Printer not online.", MSG_INFO);
   }
}

DF_ERROR stateManualPrinter::displayPrinterReachable()
{
   if (printerr->testComms())
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

   string printerstring = "Soapstand receipt printer test.";
   string plu = "978020137962";

   // Adafruit_Thermal printerr;
   printerr->printBarcode(plu.c_str(), EAN13);
   // system("echo '\n---------------------------\n' > /dev/ttyS4");

   string printer_command_string = "echo '\n---------------------------\n" + printerstring + "' > /dev/ttyS4";
   system(printer_command_string.c_str());

   // printerr->testPage();

   //  string printer_command_string = "echo '------------------------------\n-- Vancouver Active Tourism --\n--            2022-02-12    --\n------------------------------\n 1x Special morning activity \n 1x Batard Bakery experience \n 1x Guided bike tour to \n         Lighthouse park \n 1x Soapstand workplace demo \n Participants: Wafeltje + Lodey    \n   Grand total: Happy times <3 \n   Thank you, please come again!  \n\n\n\n' > /dev/ttyS4";
   //  system(printer_command_string.c_str());

   //  printerr->setBarcodeHeight(100);
    usleep(3000000);                   // wait for printer to come online.
}

// Advances to Dispense Idle
DF_ERROR stateManualPrinter::onExit()
{
   // stop continuous checking setting
   b_isContinuouslyChecking = false;

   printerr->disconnectPrinter();
   g_machine.pcb24VPowerSwitch(false);
   printerr->resetPollCount();

   DF_ERROR e_ret = OK;
   return e_ret;
}

DF_ERROR stateManualPrinter::setup_receipt_from_pnumber_and_dispense_data(int pnumber, double volume_dispensed, double volume_requested, double price, string time_stamp)
{
   std::string name_receipt = g_pnumbers[pnumber].getProductName();
   //  std::string plu = g_machine.m_productDispensers[slot-1].getSelectedProduct()->getBasePLU( SIZE_CUSTOM_CHAR  );

   char size = g_pnumbers[pnumber].getSizeCharFromTargetVolume(volume_requested);
   string plu = g_pnumbers[pnumber].getFinalPLU(size, price, g_machine.getPaymentMethod());

   std::string units =  g_machine.getSizeUnit();
   std::string paymentMethod = g_machine.getPaymentMethod();
   volume_dispensed = g_machine.convertVolumeMetricToDisplayUnits(volume_dispensed);

   char chars_cost[MAX_BUF];
   char chars_volume_formatted[MAX_BUF];

   std::string char_units_formatted = g_machine.getSizeUnit();


   snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.0f", volume_dispensed);

   std::string receipt_volume_formatted = chars_volume_formatted + char_units_formatted;

   snprintf(chars_cost, sizeof(chars_cost), "%.2f", price);
   string receipt_cost = chars_cost;

   g_machine.print_receipt(name_receipt, receipt_cost, receipt_volume_formatted, time_stamp, units, paymentMethod, plu, "", true);
}
