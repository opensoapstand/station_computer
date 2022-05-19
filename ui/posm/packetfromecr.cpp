//***************************************
//
// packetfromecr.cpp
// payment device construct packet
//
// created: 02-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "packetfromecr.h"
#include "lrcgenerator.h"
#include <cmath>

#include <QDebug>
#include <stdio.h>

packetFromECR::packetFromECR()
{

}

void packetFromECR::setPacketDataLen()
{
   uint16_t length = uint16_t(currentPacket.data.size()) + 2 + 1 + 1; //size of data + apiid + parent id + class id
   convertFrom16To8(currentPacket.dataLen, length);
}

void packetFromECR::setPacketClassID(uint8_t id)
{
    currentPacket.classID = id;
}

void packetFromECR::setPacketApiID(uint16_t id)
{
    convertFrom16To8(currentPacket.APIID, id);
}

void packetFromECR::setPacketData(uint8_t *data, int packetSize)
{
    currentPacket.data.clear();
    currentPacket.data.reserve(uint(packetSize));

    for(uint i = 0; i < uint(packetSize); i++){
        currentPacket.data.push_back(data[i]);
    }
}

void packetFromECR::setPacketLRC()
{
    uint16_t combineLength = convertFrom8To16(currentPacket.dataLen[0], currentPacket.dataLen[1]) + 2;
    std::vector<uint8_t> lrcPkt = constructPartialPacket(combineLength); //exclude STX, ETX

    LRCgenerator curLRC(lrcPkt, combineLength);
    currentPacket.LRC = curLRC.getLRC();
}

std::vector<uint8_t> packetFromECR::getPacket()
{
    uint16_t combineLength = convertFrom8To16(currentPacket.dataLen[0], currentPacket.dataLen[1]) + 2;
    std::vector<uint8_t> fullPkt = constructPartialPacket(combineLength); //2 bytes from the data length

    fullPkt.insert(fullPkt.begin(), currentPacket.STX);
    fullPkt.insert(fullPkt.end(), currentPacket.ETX);
    fullPkt.insert(fullPkt.end(), currentPacket.LRC);

    return fullPkt;
}

std::ostream &operator <<(std::ostream &out, const packetFromECR &pkt)
{
    out << "Packet to Send: ";
    out << std::hex << int(pkt.currentPacket.STX) << " ";
    out << std::hex << int(pkt.currentPacket.dataLen[0]) << " ";
    out << std::hex << int(pkt.currentPacket.dataLen[1]) << " ";
    out << std::hex << int(pkt.currentPacket.parentID) << " ";
    out << std::hex << int(pkt.currentPacket.classID) << " ";
    out << std::hex << int(pkt.currentPacket.APIID[0]) << " ";
    out << std::hex << int(pkt.currentPacket.APIID[1]) << " ";

    for (uint i = 0; i < pkt.currentPacket.data.size(); i++)
        out << std::hex << int(pkt.currentPacket.data[i]) << " ";

    out << std::hex << int(pkt.currentPacket.ETX) << " ";
    out << std::hex << int(pkt.currentPacket.LRC) << std::endl;

    return out;
}

uint16_t packetFromECR::convertFrom8To16(uint8_t firstByte, uint8_t secondByte)
{
    uint16_t combinedByte = 0x0000;

    combinedByte = firstByte;
    combinedByte = combinedByte << 8; //shift first byte
    combinedByte |= secondByte;

    return combinedByte;
}

void packetFromECR::convertFrom16To8(uint8_t arr[], uint16_t combinedByte)
{
    arr[0] = (combinedByte >> 8) & 0x00FF;
    arr[1] = combinedByte & 0x00FF;
}

std::vector<uint8_t> packetFromECR::constructPartialPacket(uint len)
{
    unsigned int i = 0;
    std::vector<uint8_t> pkt;
    pkt.reserve(len);
    //memset(lrcPkt, 0, decimalLength*sizeof (uint8_t));

    pkt.push_back(currentPacket.dataLen[0]); i++;
    pkt.push_back(currentPacket.dataLen[1]); i++;
    pkt.push_back(currentPacket.parentID); i++;
    pkt.push_back(currentPacket.classID);  i++;
    // if(currentPacket.APIID[0]!=0x99){

    pkt.push_back(currentPacket.APIID[0]); i++;
    pkt.push_back(currentPacket.APIID[1]); i++;
    // }

    int j = 0;
    for(; i < len; i++)
    {
        pkt.push_back(currentPacket.data[uint(j)]);
        j++;
    }

    return pkt;
}
