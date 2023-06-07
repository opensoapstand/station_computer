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

#include "transactionPackets.h"
#include "enums.h"

#include <iostream>

transactionPacket::transactionPacket()
{

}

/* 3.1.1 PpPosDirectTxn (0D – 0000)
 * Description:This command will initiate online financial or administrative transactions
 * in accordance with the defined interface.
 * The Direct Connect interface supports the following transaction types:*/

std::vector<uint8_t> transactionPacket::logonPacket()
{
    uint8_t dataPacket[] = {uint8_t(TxnType::Logon)}; //ECR transaction type 'L' - logon


    //STX is 0x02                                               //02
    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
                                                                //POS direct connect API
    packet.setPacketApiID(API_ID::PpPosDirectTxn);              //00 00
    packet.setPacketData(dataPacket, sizeof(dataPacket));       //4c
    packet.setPacketDataLen();                                  //00 05
    packet.setPacketLRC();                                      //46


    return packet.getPacket();
}

std::vector<uint8_t> transactionPacket::batchBalancePkt()
{
    uint8_t dataPacket[] = {uint8_t(TxnType::BatchTotals)}; //ECR transaction type 'T' - batch balance

    //STX is 0x02                                               //02
    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosDirectTxn);              //00 00
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //54 [T]
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //00 05
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}

std::vector<uint8_t> transactionPacket::batchClosePkt()
{
    uint8_t dataPacket[] = {uint8_t(TxnType::BatchClose)}; //ECR transaction type 'B' - batch close

    //STX is 0x02                                               //02

    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosDirectTxn);              //00 00
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //42 [B]
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //00 05
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}

/*
 * 3.1.1.2 Financial Transactions
 *
 * Description: The Purchase Transaction is used to process the purchase of goods and services at the point of purchase.
 *              Successfully completed transactions cause the cardholder account to be debited and the merchant account to be credited.
 *
 * NOTE: The maximum amount is $9,999.99 as indicated by the maximum length of 6 digits for the transaction amount field.
 *
 * Input: Amount request for purchase
 * Return: High Level ACK
 * ------> • Reader is activated && Wait for Card tapped by user
*/
std::vector<uint8_t> transactionPacket::purchasePacket(std::string amount)
{
    transactionAmount(TxnType::Purchase,amount);
    uint8_t dataPacket[7] = {};
    /*copy the converted array*/
    std::copy(std::begin(convertedAmount), std::end(convertedAmount), std::begin(dataPacket));

    //STX is 0x02                                               //02
    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosDirectTxn);              //00 00

    packet.setPacketData(dataPacket, sizeof (dataPacket));      //tagID(50) + dataPacket

    //ETX is 0x03    
                                               //03
    // packet.setPacketDataLen();                                  //00 0B
    packet.setPacketDataLenPur();                                  //00 0D
    // packet.setPacketLRC();                                      //LRC
    packet.setPacketLRCPur();                                      //LRC

    return packet.getPacket();
}

/*
 * 3.1.2 PpPosCancelEvent (0D - 0001)
 *
 * Description: Cancel a transaction at any point before sending the transaction online. If this command is sent after
 *              the transaction was sent online an error code 61060 will be returned (UX410 busy).
 * Input: None
 * Return: High Level ACK
*/

std::vector<uint8_t> transactionPacket::purchaseCancelPacket()
{
    uint8_t dataPacket[] = {}; //info tag = 0x00

    //STX is 0x02                                               //02

    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(0x01);   //0D
    packet.setPacketApiID(0x0045);                              //00 01
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //
    packet.setPacketDataLen();                                  //00 04
    //ETX is 0x03                                               //03
    packet.setPacketLRC();                                      //LRC
    return packet.getPacket();
}

std::vector<uint8_t> transactionPacket::reversePurchasePacket()
{
    uint8_t dataPacket[] = {uint8_t(TxnType::ReversePurchase)}; //ECR transaction type 'R' - forced controller reversal

    //STX is 0x02                                               //02

    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosDirectTxn);              //00 00
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //42 [B]
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //00 05
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}

std::vector<uint8_t> transactionPacket::resetDevice()
{
    uint8_t dataPacket[] = {}; //ECR transaction type 'R' - forced controller reversal

    //STX is 0x02                                               //02

    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppDvc);   //01
    packet.setPacketApiID(API_ID::PpDvcReset);                  //00 92
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //42 [B]
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //00 05
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}

/*
 * 3.1.3 PpPosStatus (0D - 0002)
 *
 * Description: Returns the status of various functional parameters
 * Input: Tag ID for the test/infomation requested
 * Return: high level ack following by the requested data in Tag Length Value Format
*/
std::vector<uint8_t> transactionPacket::ppPosStatusCheckPkt(char type)
{
    uint8_t dataPacket[] = {uint8_t(type)}; //tag id for infomation requested
                                            //enum TxnType
    //STX is 0x02                                               //02

    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosStatus);                 //00 02
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //00 07
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}

/*
 * 3.1.4 PpPosSetConfig (0D – 0003)
 *
 * Description: Set the value of various configuration parameters
 *              (TID, SSL certificate, initial URL and port number, terminal language, etc.).
 * Input: Configuration information in TLV (Tag – Length - Value) format
 * Return: High Level ACK
*/
std::vector<uint8_t> transactionPacket::ppPosSetConfigPkt(char tag, std::string val)
{
    /*copy the converted array*/
    std::vector<uint8_t> vecData;
    vecData.reserve(val.length()+3);
    vecData.push_back(uint8_t(tag));
    vecData = storeLength(vecData, uint16_t(val.length()));

    for (uint i = 0; i < val.length(); i++) //stores the values into the array
        vecData.push_back(uint8_t(val.at(i)));

    uint8_t* dataPacket = &vecData[0]; //tag id for infomation requested

    //STX is 0x02                                               //02
    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosSetConfig);              //00 03
    packet.setPacketData(dataPacket, int(val.length()+3));      //
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //LEN
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}


std::vector<uint8_t> transactionPacket::ppPosGetConfigPkt(int tag)
{
    uint8_t dataPacket[] = {uint8_t(tag), 0x00, 0xFF}; //tag id for infomation requested
                                            //enum TxnTyp
    //STX is 0x02                                               //02
    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(communicationPacketField::ppPos);   //0D
    packet.setPacketApiID(API_ID::PpPosGetConfig);              //00 04
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //LEN
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}




std::vector<uint8_t> transactionPacket::createTestPacket(int tag)
{    
    // uint8_t dataPacket[] = {uint8_t(0x00), 0x00, 0xFF}; //tag id for infomation requested
    uint8_t dataPacket[] = {0x00, 0x02, 0x00}; //tag id for infomation requested
                                            //enum TxnTyp
    //STX is 0x02                                               //02
    //parentID is fixed 0x02                                    //02
    packet.setPacketClassID(0x0D);   //0D
    packet.setPacketApiID(API_ID::PpDummy);              //00 04
    packet.setPacketData(dataPacket, sizeof (dataPacket));      //
    //ETX is 0x03                                               //03
    packet.setPacketDataLen();                                  //LEN
    packet.setPacketLRC();                                      //LRC

    return packet.getPacket();
}


void transactionPacket::transactionAmount(char type, std::string amount)
{
    //‘P’ (0x50) <- ECR transaction type ‘P’ - purchase
    //“000100” <- Transaction amount in ASCII; $1.00, no decimal point,
    //fixed 6 digits long (i.e. $1.00 = 0x30 0x30 0x30 0x31 0x30 0x30)
    convertedAmount[0] = uint8_t(type); //transaction typ packetToSende
    int amountLen = int(amount.length());

    int i = 6; //tagID(size 1) + dataPacket(size 6)
    int idx = 1;

    while(i > 0) //filled up data feild for the amount request for purchase
    {
        if((amountLen - int(idx)) >= 0){
            if(isdigit(amount.at(uint(amountLen-idx))))//store digits into the data array
            {
                convertedAmount[i] = uint8_t(amount.at(uint(amountLen-idx)));
                i--;
            }
        }
        else{
            convertedAmount[i] = uint8_t('0'); //the empty transaction data field should be placed with 0 (ASCII)
            i--;
        }
        idx++;
    }
}

packetFromECR transactionPacket::getSendPacket()
{
    return packet;
}

std::vector<uint8_t> transactionPacket::storeLength(std::vector<uint8_t> arr, uint16_t len)
{
    arr.push_back((len >> 8) & 0x00FF);
    arr.push_back(len & 0x00FF);

    return arr;
}

