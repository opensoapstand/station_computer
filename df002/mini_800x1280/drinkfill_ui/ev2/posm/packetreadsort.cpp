//***************************************
//
// packetreadsort.h
// packet sorting
//
// created: 03-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include"enums.h"
#include"packetreadsort.h"

#include <iostream>

packetReadSort::packetReadSort()
{

}

packetReadSort::~packetReadSort()
{

}

void packetReadSort::sortPacket(packetResponse readPacket)
{
    packetToSort = readPacket;
    if (packetToSort.status == communicationPacketField::NAK){

    } else if (packetToSort.status == communicationPacketField::ACK){
        if (packetToSort.classID == communicationPacketField::ppPos){
            uint8_t APIID[2] = {packetToSort.APIID[0], packetToSort.APIID[1]};
            if (packetFromECR::convertFrom8To16(APIID[0],APIID[1]) == API_ID::PpPosDirectTxn){
                if (packetToSort.data[0]== 0x50){
                    if(packetToSort.data[2] == TXN_STATUS_CODE::SUCCCESS){
                        qDebug()  << "Approved";
                    }
                    if(packetToSort.data[2] == TXN_STATUS_CODE::NOT_COMPLETED){
                        qDebug()  << "Declined";
                    }
                    if(packetToSort.data[2] == TXN_STATUS_CODE::TIMEOUT){
                        qDebug()  << "USER TIMEOUT";
                    }
                }
            }

        }
        if (packetToSort.classID == communicationPacketField::ppDvc){

        }
        if (packetToSort.classID == communicationPacketField::ppRtc){

        }
    }
}
