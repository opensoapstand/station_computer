//***************************************
//
// transactionPacket.h
// payment device packet to send
//
// created: 03-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef transactionPacket_H
#define transactionPacket_H

#include <vector>
#include <string>
#include "packetfromecr.h"

class transactionPacket
{
public:
    transactionPacket();
    /* 3.1.1.1 Administrative Transactions */
    std::vector<uint8_t> logonPacket();
    std::vector<uint8_t> batchBalancePkt();
    std::vector<uint8_t> batchClosePkt();

    /* 3.1.1.2 Financial Transactions */
    std::vector<uint8_t> purchasePacket(std::string  amount);
    std::vector<uint8_t> preAuthPacket(std::string  amount);
    
    /* 3.1.2 PpPosCancelEvent (0D – 0001)*/
    std::vector<uint8_t> purchaseCancelPacket();
    /* 3.1.1.2.5 Forced controller reversal */
    std::vector<uint8_t> reversePurchasePacket();
    /* 3.1.3 PpPosStatus (0D – 0002)*/
    std::vector<uint8_t> ppPosStatusCheckPkt(char type);
    /* 3.1.4 PpPosSetConfig (0D – 0003)*/
    std::vector<uint8_t> ppPosSetConfigPkt(char tag, std::string val); //Tag-Length-Value Format
    std::vector<uint8_t> ppPosGetConfigPkt(int tag);
    std::vector<uint8_t> rebootDevice();

    void transactionAmount(char type, std::string amount);
    packetFromECR getSendPacket();

    std::vector<uint8_t> createTestPacket(int tag);


private:
    packetFromECR packet;
    uint8_t convertedAmount[7];

    std::vector<uint8_t> storeLength(std::vector<uint8_t> arr, uint16_t len);
};

#endif // transactionPacket_H
