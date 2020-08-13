//***************************************
//
// packetfromux410.h
// moneris payment device response packet sorting
//
// created: 03-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef PACKETFROMUX410_H
#define PACKETFROMUX410_H

#include <iostream>
#include <vector>

#include "enums.h"

struct packetResponse{
    uint8_t     STX;
    uint8_t     dataLen[2];
    uint8_t     parentID;
    uint8_t     classID;
    uint8_t     APIID[2];
    uint8_t     status; //ACK(0x06) successful execution or NAK(0x15) error code
    uint8_t     ETX;
    uint8_t     LRC;
    std::vector<uint8_t>     data; //ACK - may contain 0 or more response data bytes
                                   //NAK -  this field has only 2 bytes indicating the error code
    //                                      (review Appendix A: Error Codes for a complete list of error codes and causes)
};

class packetFromUX410
{
public:
    packetFromUX410();
    void packetReadFromUX(std::vector<uint8_t> curPacket);
    void sortPacket(std::vector<uint8_t> packetRead);
    void checkAckOrNak(uint8_t AKNK);
    uint8_t getAckOrNak();
    packetResponse getPacket();

    friend std::ostream & operator << (std::ostream &out, const packetFromUX410 &pkt);

private:
    packetResponse packetToSort;
    std::vector<uint8_t> partialPacket;
    uint8_t AckorNak;

    uint16_t convertFrom8To16(uint8_t firstByte, uint8_t secondByte);
    void constructPartialPacket(uint len);
};

#endif // PACKETFROMUX410_H
