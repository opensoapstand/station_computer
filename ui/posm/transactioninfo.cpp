//***************************************
//
// transactioninfo.cpp
// reciept handling
//
// created: 03-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "transactioninfo.h"

#include <iostream>
#include <algorithm>    // std::find_if
#include <stdio.h>

//#include "database_logger.h"

#include "../paypage.h"
//extern std::string merchantName;
//extern std::string merchantAddress;
//extern std::string terminalID;

//using namespace std;

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
            //qDebug() <<i<<" position recorded"<<std::endl;
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

        //cout << " idx: " << idx << endl;
        std::vector<uint8_t> subVector(first, last);

        std::string temp = dataField(subVector);

        if (idx <= MAX_FIELD)
            ptr[idx] = temp;
    }
}

int transactionInfo::idValue(uint8_t nibOne, uint8_t nibTwo)
{
    std::string transactionID = std::string(1,char(nibOne)) + std::string(1,char(nibTwo));

    //cout << "transid: " <<transactionID;

    int transNum = 0;
    if(std::all_of(transactionID.begin(), transactionID.end(), ::isdigit));
      transNum = stoi(transactionID);

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

//void transactionInfo::makeReceipt(database_logger logData)
void transactionInfo::makeReceipt(string terminalID, string merchantName, string merchantAddress)
{

    std::string cardType =  getTransactionInfo(TXN_FIDs::CARD_NAME);
    std::string cardName;
    std::string accountType = getTransactionInfo(TXN_FIDs::ACCOUNT_TYPE);
    std::string language = getTransactionInfo(TXN_FIDs::CUSTOMER_LANG);
    std::string lastFourChar = getTransactionInfo(TXN_FIDs::PAN);
    std::string purchaseDate = getTransactionInfo(TXN_FIDs::DATE_TIME);
    std::string filenameC =purchaseDate + "-" + lastFourChar.substr(lastFourChar.size()-4)+"-custo"+".txt";
    std::string filenameM =purchaseDate + "-" + lastFourChar.substr(lastFourChar.size()-4)+"-merch"+".txt";
    std::string purchaseAmount= getTransactionInfo(TXN_FIDs::AMOUNT);
    if(purchaseAmount.size() >= 3)
        purchaseAmount.insert(purchaseAmount.size()-2, ".");
    else
        purchaseAmount.insert(purchaseAmount.size()-1, ".");
    std::string txnType;
    std::string ref = terminalID + getTransactionInfo(TXN_FIDs::SEQ_NUM) +  getTransactionInfo(TXN_FIDs::TXN_TRACE_NR) + " " + getTransactionInfo(TXN_FIDs::MOP);

    if (getTransactionInfo(TXN_FIDs::SPDH_TXN_TYPE)=="00"){
        txnType = "PURCHASE";
    }

    purchaseDate.insert(2, "/");
    purchaseDate.insert(5, "/");
    purchaseDate.insert(8, " ");
    purchaseDate.insert(11, ":");
    purchaseDate.insert(14, ":");
    std::ofstream customerReceipt;
    std::ofstream merchantReceipt;

    customerReceipt.open("/release/receipts/"+filenameC);
    merchantReceipt.open("/release/receipts/"+filenameM);
    if (accountType == "01"){
        customerReceipt <<"TRANSACTION RECORD\n";
        merchantReceipt <<"TRANSACTION RECORD\n";

        accountType = "";
        cardName = cardType;
        cardType = "FLASH DEFAULT";
    }  else
        accountType = "NO SIGNATURE TRANSACTION \n\n";
    customerReceipt <<"   " << merchantName<<endl;
    customerReceipt <<"   " << merchantAddress << "\n\n";
    customerReceipt << "TYPE: " << txnType << endl;
    customerReceipt <<"                 -----------\n";
    customerReceipt<< "ACCT: "<< cardType<<"       $    " << purchaseAmount <<endl;
    customerReceipt <<"                 -----------\n\n";
    customerReceipt<<"CARD NUMBER:      "<< lastFourChar<<endl;
    customerReceipt<<"DATE/TIME:      "<< purchaseDate.substr(0,8) << " "<< purchaseDate.substr(9)<<endl;
    customerReceipt<<"REFERENCE #:      "<<ref<<endl;
    customerReceipt<<"AUTHORIZATION #:      "<<getTransactionInfo(TXN_FIDs::APPROVAL_CODE)<<endl<<endl;

    merchantReceipt <<"   " << merchantName<<endl;
    merchantReceipt <<"   " << merchantAddress << "\n\n";
    merchantReceipt << "TYPE: " << txnType <<endl;
    merchantReceipt <<"                 -----------\n";
    merchantReceipt<< "ACCT: "<< cardType<<"       $    " << purchaseAmount <<endl;
    merchantReceipt <<"                 -----------\n\n";
    merchantReceipt<<"CARD NUMBER:      "<< lastFourChar<<endl;
    merchantReceipt<<"DATE/TIME:      "<< purchaseDate.substr(0,8) << " "<< purchaseDate.substr(9)<<endl;
    merchantReceipt<<"REFERENCE #:      "<<ref<<endl;
    merchantReceipt<<"AUTHORIZATION #:      "<<getTransactionInfo(TXN_FIDs::APPROVAL_CODE)<<endl<<endl;
    //    if (getTransactionInfo(TXN_FIDs::MOP)=="H"){
    if (getTransactionInfo(TXN_FIDs::CHIP_LABEL)!= ""){
        customerReceipt<<getTransactionInfo(TXN_FIDs::CHIP_LABEL)<<endl;
        merchantReceipt<<getTransactionInfo(TXN_FIDs::CHIP_LABEL)<<endl;

    }
    //    }
    if (getTransactionInfo(TXN_FIDs::AID)!= ""){
        customerReceipt<<getTransactionInfo(TXN_FIDs::AID)<<endl;
        merchantReceipt<<getTransactionInfo(TXN_FIDs::AID)<<endl;

    }
    if (getTransactionInfo(TXN_FIDs::TVR_AFTER_ARPC)!= "" && cardType !="VISA"){
        if (getTransactionInfo(TXN_FIDs::TVR_AFTER_ARPC) == "0000000000"){
            customerReceipt<<"TVR:  ";
            merchantReceipt<<"TVR:  ";

        }
        customerReceipt<<getTransactionInfo(TXN_FIDs::TVR_AFTER_ARPC)<<endl;
        merchantReceipt<<getTransactionInfo(TXN_FIDs::TVR_AFTER_ARPC)<<endl;

    }
    if (getTransactionInfo(TXN_FIDs::TXN_FINAL_DISP) == "A"){
        if (getTransactionInfo(TXN_FIDs::FINAL_TSI)!= "" && getTransactionInfo(TXN_FIDs::FINAL_TSI)!="0000"){
            customerReceipt<<getTransactionInfo(TXN_FIDs::FINAL_TSI)<<endl;
            merchantReceipt<<getTransactionInfo(TXN_FIDs::FINAL_TSI)<<endl;

        }
        customerReceipt<<endl<< getTransactionInfo(TXN_FIDs::ISO_CODE)<<"/"<<getTransactionInfo(TXN_FIDs::HOST_CODE);
        merchantReceipt<<endl<< getTransactionInfo(TXN_FIDs::ISO_CODE)<<"/"<<getTransactionInfo(TXN_FIDs::HOST_CODE);

        if (language == "01"){
            customerReceipt<<"  APPROVED - THANK YOU"<<endl;
            merchantReceipt<<"  APPROVED - THANK YOU"<<endl;

        }
        else if(language == "02"){
            customerReceipt<< "  APPROVEE - MERCI"<<endl;
            merchantReceipt<< "  APPROVEE - MERCI"<<endl;

        }
    } else {
        customerReceipt<<endl<< getTransactionInfo(TXN_FIDs::ISO_CODE)<<"/"<<getTransactionInfo(TXN_FIDs::HOST_CODE);
        merchantReceipt<<endl<< getTransactionInfo(TXN_FIDs::ISO_CODE)<<"/"<<getTransactionInfo(TXN_FIDs::HOST_CODE);

        if (language == "01"){
            customerReceipt<<"-Transaction Not Approved - THANK YOU"<<endl;
            merchantReceipt<<"-Transaction Not Approved - THANK YOU"<<endl;

        }

        else if(language == "02"){
            customerReceipt<< "  Transaction Non Approvee - MERCI"<<endl;
            merchantReceipt<< "  Transaction Non Approvee - MERCI"<<endl;

        }
    }

    customerReceipt <<endl<<accountType;
    customerReceipt <<"   ---IMPORTANT--- \nRetain This Copy For Your Records\n\n"
                   <<"   ***CUSTOMER COPY***";
    customerReceipt.close();

    customerReceipt.flush();

    merchantReceipt <<endl<<accountType;
    merchantReceipt <<"\n\n\n"
                   <<"   ***MERCHANT COPY***";
    merchantReceipt.close();

    merchantReceipt.flush();


//    logData.reconnectDatabase();
//    logData.paymentLog(purchaseDate, getTransactionInfo(TXN_FIDs::CARD_NAME), txnType, purchaseAmount,
//                       lastFourChar, ref, getTransactionInfo(TXN_FIDs::MOP), getTransactionInfo(TXN_FIDs::APPROVAL_CODE),
//                       getTransactionInfo(TXN_FIDs::ISO_CODE));
//    for (int i = 0; i < MAX_FIELD; i++){
//        ptr[i].clear();
//    }

}
