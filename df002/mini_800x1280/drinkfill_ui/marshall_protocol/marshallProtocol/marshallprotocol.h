//***************************************
//
// marshallprotocol.h
// class for interacting with Nayax cashless payment machine
//
// created: 12-Aug-2019
// by: Jason Wang
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef MARSHALLPROTOCOL_H
#define MARSHALLPROTOCOL_H

#define UTILS_POLYNOMIAL_CCITT 0x1021 /* x^16 + x^12 + x^5 + 1 */
#define UTILS_SEED_CCITT 0

#include <cstring>
#include <vector>

using namespace std;
//baud rate: 115200 bps
//8bits
//no parity

struct packetOptions{
    bool ACKPacketRequest;
    bool packetEncrpted;
    unsigned long int retryNumber; //0 means first transmission
    bool lastPacket; //0 means last
    char* RFU;
};    //1 byte

struct packetInfo{
    char* packetLength[2];     //2 byte  ->  use to calculate the size of data
    packetOptions packetBitmap;
    char* packetID[1];      //1 byte
    char* source[1];        //1 byte
    char* sourceLSB[1];     //1 byte
    char* destination[1];   //1 byte
    char* destinationLSB[1];//1 byte
    char* functionCode[1];     //1 byte
    char* data;             //(packet length - 9) byte
    char* signaturePacket[1];  //2 byte

    size_t packetSize;
};

class marshallProtocol
{
public:
    marshallProtocol();
    int sortMP(char const* packet);

    void getPacketLength(char const& inputByte_0, char const& inputByte_1, char const& inputByte_2, char const& inputByte_3);
    void getPacketOption(const char& inputByte_4, const char& inputByte_5);
    void getPacketID(const char& inputByte_6, const char& inputByte_7);
    void getSourcePacket(const char& inputByte_8, const char& inputByte_9);
    void getDestination(const char& inputByte_10, const char& inputByte_11);

    unsigned short Utils_CalcCrcCcitt ( unsigned char *pData, unsigned int Len, unsigned short Seed );

    unsigned long int convertBinaryToDecimal(const char* num);
    const char* convertHexToBinary(char const inputByte);

private:
    packetInfo curPacket;
    vector<char> packet;
};

#endif // MARSHALLPROTOCOL_H
