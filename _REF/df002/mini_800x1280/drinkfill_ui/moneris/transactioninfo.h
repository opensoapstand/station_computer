#ifndef TRANSACTIONINFO_H
#define TRANSACTIONINFO_H

#include <vector>
#include "enums.h"
//#include "paymentdatabase.h"

#include <string>
#include <stdlib.h>
#include <fstream>

const int MAX_FIELD = 43;

class transactionInfo
{
public:
    transactionInfo();
    void transactionID(std::vector<uint8_t> data);
    int idValue(uint8_t nibOne, uint8_t nibTwo);
    std::string dataField(std::vector<uint8_t> data);
    std::string getTransactionInfo(int id);
    void makeReceipt();

private:
    std::string ptr[MAX_FIELD];
//    paymentDatabase logData;
};

#endif // TRANSACTIONINFO_H
