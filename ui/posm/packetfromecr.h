//***************************************
//
// packetfromecr.h
// payment device construct packet
//
// created: 02-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef PACKETFROMECR_H
#define PACKETFROMECR_H

#include <bitset>
#include <vector>

#include "enums.h"

struct packetSend{
    uint8_t     STX = communicationPacketField::STX;
    uint8_t     dataLen[2];
    uint8_t     parentID = communicationPacketField::parent_ID;
    uint8_t     classID;
    uint8_t     APIID[2];
    uint8_t     ETX = communicationPacketField::ETX;
    uint8_t     LRC;
    std::vector<uint8_t>     data; // the flexible array memeber must be last
};

class packetFromECR
{
public:
    packetFromECR();
    void setPacketDataLen();
    void setPacketDataLenPur();
    void setPacketClassID(uint8_t id);
    void setPacketApiID(uint16_t id);
    void setPacketData(uint8_t *data, int packetSize);
    void setPacketLRC();
    void setPacketLRCPur();

    std::vector<uint8_t> getPacket();

    friend std::ostream & operator << (std::ostream &out, const packetFromECR &pkt);
    static uint16_t convertFrom8To16 (uint8_t firstByte, uint8_t secondByte);
    static void convertFrom16To8 (uint8_t arr[], uint16_t combinedByte);

private:
    packetSend currentPacket;
    std::vector<uint8_t> constructPartialPacket(uint len);
};

#endif // packetFromECR_H
