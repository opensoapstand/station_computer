//***************************************
//
// transactioninfo.h
// reciept handling
//
// created: 03-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef TRANSACTIONINFO_H
#define TRANSACTIONINFO_H

#include <vector>
#include "enums.h"
#include <map>
//#include "paymentdatabase.h"
//#include "database_logger.h"

#include <string>
#include <stdlib.h>
#include <fstream>

const int MAX_FIELD = 45;

using namespace std;

class transactionInfo
{
public:
    transactionInfo();
    void transactionID(std::vector<uint8_t> data);
    int idValue(uint8_t nibOne, uint8_t nibTwo);
    std::string dataField(std::vector<uint8_t> data);
    std::string getTransactionInfo(int id);
//    void makeReceipt(database_logger logData);
    void makeReceipt(string terminalID, string merchantName, string merchantAddress);
    std::pair<std::string, std::string> convertDateTime(const std::string& date_time);
    std::map<std::string, std::string> getTapPaymentObject(string terminalID, string merchantName, string merchantAddress);

private:
    std::string ptr[MAX_FIELD];
    //paymentDatabase logData;
};

#endif // TRANSACTIONINFO_H
