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

#define MAX_BUF 64

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
   productDispensers = g_productDispensers;

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

      if ('0' == m_pMessaging->getAction() || ACTION_QUIT == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Exit printer status test", MSG_INFO);

         m_state_requested = STATE_IDLE;
      }
      else if (ACTION_UI_COMMAND_PRINTER_SEND_STATUS == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Printer status requested by UI", MSG_INFO);
         sendPrinterStatus(); // first call after startup returns always online
         sendPrinterStatus();
         m_state_requested = STATE_IDLE; // return after finished.
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
      else if ('1' == m_pMessaging->getAction())
      {
         debugOutput::sendMessage("Do test print", MSG_INFO);
         printTest();
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
      else
      {
         debugOutput::sendMessage("---Receipt printer menu---"
                                  "Available printer test commands: \n"
                                  " 0: Exit printer menu \n"
                                  " 1: Test print\n"
                                  " 2: Printer status toggle continuous mode\n"
                                  " 3: Printer status \n"
                                  " 4: Check printer connected\n"
                                  " 5: Print transaction 959\n"
                                  " h: Display this help menu",
                                  MSG_INFO);
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

void stateManualPrinter::printTransaction(int transactionNumber)
{
   // gets transaction data from db.
   char *zErrMsg = 0;
   rc = sqlite3_open(DB_PATH, &db);
   sqlite3_stmt *stmt;
   std::string sql_string;

   //-------------------------------------------------
   sql_string = ("SELECT product,price,quantity_dispensed,quantity_requested,end_time FROM transactions WHERE id=" + to_string(transactionNumber));
   sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

   int status;
   status = sqlite3_step(stmt);

   string product;
   double price;
   double quantity_dispensed;
   double quantity_requested;
   string end_time;
   if (status == SQLITE_ROW)
   {
      product = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
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
      debugOutput::sendMessage("SUCCES: SQL transaction retrieval : (" + to_string(rc) + ") " + sql_string, MSG_INFO);
   }

   debugOutput::sendMessage("----------------: " + product, MSG_INFO);
   debugOutput::sendMessage("----------------: " + to_string(price), MSG_INFO);
   debugOutput::sendMessage("----------------: " + to_string(quantity_dispensed), MSG_INFO);
   debugOutput::sendMessage("----------------: " + to_string(quantity_requested), MSG_INFO);
   debugOutput::sendMessage("----------------: " + end_time, MSG_INFO);
   sqlite3_finalize(stmt);
   sqlite3_close(db);

   //-------------------------------------------------
   rc = sqlite3_open(DB_PATH, &db);
   sql_string = ("SELECT slot FROM products WHERE name='" + product + "';");

   
   sqlite3_prepare(db, sql_string.c_str(), -1, &stmt, NULL);

   status = sqlite3_step(stmt);

   int slot;
   if (status == SQLITE_ROW)
   {
      slot = sqlite3_column_int(stmt, 0);
   }
   if (rc != SQLITE_OK)
   {
      debugOutput::sendMessage("ERROR: SQL transaction retrieval : (" + to_string(rc) + "):" + sql_string, MSG_INFO);

      sqlite3_free(zErrMsg);
   }
   else
   {
      debugOutput::sendMessage("SUCCES: SQL transaction retrieval : (" + to_string(rc) + ") " + sql_string, MSG_INFO);
   }

   debugOutput::sendMessage("slot ----------------: " + to_string(slot), MSG_INFO);
   sqlite3_finalize(stmt);
   sqlite3_close(db);
   setup_receipt_from_data_and_slot(slot, quantity_dispensed, quantity_requested, price, end_time);
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
   sqlite3_close(db);
}

DF_ERROR stateManualPrinter::displayPrinterStatus()
{
   bool isOnline = printerr.testComms(); // first call return always "online"
   isOnline = printerr.testComms();

   if (isOnline)
   {

      if (printerr.hasPaper())
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

   string printerstring = "Soapstand receipt printer test.";
   string plu = "978020137962";

   // Adafruit_Thermal printerr;
   printerr.printBarcode(plu.c_str(), EAN13);
   system("echo '\n---------------------------\n' > /dev/ttyS4");

   string printer_command_string = "echo '\n---------------------------\n" + printerstring + "' > /dev/ttyS4";
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

DF_ERROR stateManualPrinter::setup_receipt_from_data_and_slot(int slot, double volume_dispensed, double volume_requested, double price, string time_stamp)
{
   std::string name_receipt = (productDispensers[slot - 1].getProduct()->getProductName());
   //  std::string plu = productDispensers[slot-1].getProduct()->getBasePLU( SIZE_CUSTOM_CHAR  );

   char size = productDispensers[slot - 1].getProduct()->getSizeCharFromTargetVolume(volume_requested);
   string plu = productDispensers[slot - 1].getFinalPLU(size, price);

   std::string units = (productDispensers[slot - 1].getProduct()->getDisplayUnits());
   std::string paymentMethod = productDispensers[slot - 1].getProduct()->getPaymentMethod();

   char chars_cost[MAX_BUF];
   char chars_volume_formatted[MAX_BUF];

   snprintf(chars_volume_formatted, sizeof(chars_volume_formatted), "%.2f", productDispensers[slot - 1].getProduct()->getTargetVolume(size));
   string vol = (chars_volume_formatted);
   string receipt_volume_formatted = vol + "ml";
   //  string receipt_volume_formatted = to_string(chars_volume_formatted) + "ml";

   snprintf(chars_cost, sizeof(chars_cost), "%.2f", price);
   string receipt_cost = (chars_cost);

   machine tmp;
   receipt_cost = m_pMessaging->getRequestedPrice();

   tmp.print_receipt(name_receipt, receipt_cost, receipt_volume_formatted, time_stamp, units, paymentMethod, plu,"");
}

// DF_ERROR stateManualPrinter::print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string units, string paymentMethod, string plu){
//     print_text(name_receipt + "\nPrice: $" + receipt_cost + " \nQuantity: " + receipt_volume_formatted + "\nTime: " + time_stamp);

//     if (paymentMethod == "barcode" || paymentMethod == "barcode_EAN-13" || paymentMethod == "barcode_EAN-2")
//     {

//         if (plu.size() != 13 && plu.size() != 12)
//         {
//             // EAN13 codes need to be 13 digits, or else no barcode will be printed. If 12 dgits are provided, the last digit (checksum?!) is automatically generated
//             debugOutput::sendMessage("ERROR: bar code invalid (" + plu + "). EAN13, Should be 13 digits" + to_string(plu.size()), MSG_INFO);
//             print_text("\nPLU: " + plu + " (No barcode available)");
//         }
//         else
//         {
//             Adafruit_Thermal *printerr = new Adafruit_Thermal();
//             printerr->connectToPrinter();
//             printerr->setBarcodeHeight(100);
//             printerr->printBarcode(plu.c_str(), EAN13);
//             printerr->disconnectPrinter();
//         }
//     }

//     else if (paymentMethod == "plu")
//     {
//         print_text("PLU: " + plu);
//     }
//     else
//     {
//         debugOutput::sendMessage("ERROR: Not a valid payment method" + paymentMethod, MSG_INFO);
//     }
//     print_text("\n\n\n");

// }

// DF_ERROR stateManualPrinter::print_text(string text)
// {
//     string printerstring = text;
//     string printer_command_string = "echo '\n" + printerstring + "' > /dev/ttyS4";
//     system(printer_command_string.c_str());
// }