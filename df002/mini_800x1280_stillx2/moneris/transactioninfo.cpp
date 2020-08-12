#include "transactioninfo.h"

#include <iostream>
#include <algorithm>    // std::find_if

transactionInfo::transactionInfo()
{

}

void transactionInfo::transactionID(std::vector<uint8_t> data)
{
    std::vector<int> index;

    for (int i = 0; i < int(data.size()); i++)
    {
        if (data.at(i) == 0x1C)
        {
            index.push_back(i);
            //std::cout<<i<<" position recorded"<<std::endl;
        }
    }

    std::vector<uint8_t> sub;
    sub.push_back(data.at(0));
    ptr[0] = dataField(sub);

    for (int i = 0; i < int(index.size()) - 1; i++)
    {
        std::vector<uint8_t>::const_iterator first = data.begin() + index.at(i) + 3;
        std::vector<uint8_t>::const_iterator last = data.begin() + index.at(i+1);
        int idx = idValue(data.at(index.at(i)+1), data.at(index.at(i)+2));
        std::vector<uint8_t> subVector(first, last);

        std::string temp = dataField(subVector);
        ptr[idx] = temp;
    }
}

int transactionInfo::idValue(uint8_t nibOne, uint8_t nibTwo)
{
    std::string transactionID = std::string(1,char(nibOne)) + char(nibTwo);
    int transNum = stoi(transactionID);

    return transNum;
}

std::string transactionInfo::dataField(std::vector<uint8_t> data)
{
    std::string fullString;

    for (int i = 0; i < int(data.size()); i++)
    {
        fullString += char(data.at(i));
    }

    return fullString;
}

std::string transactionInfo::getTransactionInfo(int id)
{
    return ptr[id];
}

void transactionInfo::makeReceipt()
{
    std::ofstream customerReceipt;
    std::string lastFourChar = getTransactionInfo(TXN_FIDs::PAN);
    std::string purchaseDate = getTransactionInfo(TXN_FIDs::DATE_TIME);
    std::string filename = purchaseDate + "-" + lastFourChar.substr(lastFourChar.size()-4);
    std::string purchaseAmount= getTransactionInfo(TXN_FIDs::AMOUNT);
    purchaseAmount.insert(purchaseAmount.size()-2, ".");
    std::string txnType;
    std::string ref = getTransactionInfo(TXN_FIDs::SEQ_NUM) + getTransactionInfo(TXN_FIDs::CUSTOMER_LANG) + getTransactionInfo(TXN_FIDs::TXN_TRACE_NR);

    if (getTransactionInfo(TXN_FIDs::SPDH_TXN_TYPE)=="00"){
        txnType = "PURCHASE";
    }
    purchaseDate.insert(2, "/");
    purchaseDate.insert(5, "/");
    purchaseDate.insert(8, " ");
    purchaseDate.insert(11, ":");
    purchaseDate.insert(14, ":");
    customerReceipt.open(filename);

    //customerReceipt << getTransactionInfo(TXN_FIDs::)
    customerReceipt << getTransactionInfo(TXN_FIDs::CARD_NAME) << " " << txnType <<"\n";

    customerReceipt<<"AMOUNT                $"<< purchaseAmount <<"\n\nCard #:    " << lastFourChar;
    customerReceipt<<"\nDate: "<< purchaseDate.substr(0,8) << " Time: "<< purchaseDate.substr(9);
    customerReceipt<<"\nRef. #: "<<ref
                   << " " << getTransactionInfo(TXN_FIDs::MOP);

    customerReceipt<<"\nAuth. #: "<<getTransactionInfo(TXN_FIDs::APPROVAL_CODE);
    customerReceipt.close();

//    logData.paymentLog(purchaseDate, getTransactionInfo(TXN_FIDs::CARD_NAME), txnType, purchaseAmount,
//                       lastFourChar, ref, getTransactionInfo(TXN_FIDs::MOP), getTransactionInfo(TXN_FIDs::APPROVAL_CODE),
//                       getTransactionInfo(TXN_FIDs::ISO_CODE));
}
