//***************************************
//
// marshallprotocol.cpp
// class for interacting with Nayax cashless payment machine
//
// created: 12-Aug-2019
// by: Jason Wang
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "marshallprotocol.h"

#include <cmath>
#include <iostream>


using namespace std;

marshallProtocol::marshallProtocol()
{
}

int marshallProtocol::sortMP(char const* packet)
{
    int nRet = 1;
    curPacket.packetSize = strlen(packet);

    if ((nullptr == packet)){ //length of the packet need to be compare as well
        //sent error
        //nRet = -1;
        //return nRet;
    }
    else {

        getPacketLength(packet[0], packet[1], packet[2], packet[3]); //two byte for packet length
        //getPacketOption(packet[4], packet[5]);
        //getPacketID(packet[6], packet[7]);
        //getSourcePacket(packet[8],packet[9]);
        //getDestination(packet[10],packet[11]);
    }

    return nRet;
}

void marshallProtocol::getPacketLength(char const& inputByte_0, char const& inputByte_1, char const& inputByte_2, char const& inputByte_3)
{
    const char* temp_0 = convertHexToBinary(inputByte_0);
    const char* temp_1 = convertHexToBinary(inputByte_1);
    const char* temp_2 = convertHexToBinary(inputByte_2);
    const char* temp_3 = convertHexToBinary(inputByte_3);

    //cout << temp_2 << "\t" << temp_3 << endl;

    char buffer[16];
    strncpy(buffer, temp_0, 16);
    strncat(buffer, temp_1, 4);
    strncat(buffer, temp_2, 4);
    strncat(buffer, temp_3, 4);

    //curPacket.packetLength = buffer;
    //cout << buffer << endl;
    //cout << packetLength <<endl;
    curPacket.packetLength[0] = &packet.at(0);
    curPacket.packetLength[1] = &packet.at(1);
}

void marshallProtocol::getPacketOption(const char& inputByte_4, const char& inputByte_5)
{
    const char* temp_0 = convertHexToBinary(inputByte_4);
    const char* temp_1 = convertHexToBinary(inputByte_5);

    char buffer[8];
    strncpy(buffer, temp_0, 8);
    strncat(buffer, temp_1, 4);

    char retryBuffer[] = "00";
    retryBuffer[0] = buffer[2];
    retryBuffer[1] = buffer[3];
    //strncpy(retryBuffer, buffer[2], 2);
    //strncat(retryBuffer, buffer[3], 1);

    char RFUBuffer[4] = "000";
    RFUBuffer[0] = buffer[5];
    RFUBuffer[1] = buffer[6];
    RFUBuffer[2] = buffer[7];

    curPacket.packetBitmap.ACKPacketRequest = buffer[0];
    curPacket.packetBitmap.packetEncrpted = buffer[1];
    curPacket.packetBitmap.retryNumber = convertBinaryToDecimal(retryBuffer);
    curPacket.packetBitmap.lastPacket = buffer[4];
    curPacket.packetBitmap.RFU = RFUBuffer;

}

void marshallProtocol::getPacketID(const char &inputByte_6, const char &inputByte_7)
{
    //in case of packet is missing, Amit will reset the specific Peripheral and this will re Synchronize the communication
    //between Amit and the Peripheral
    const char* temp_0 = convertHexToBinary(inputByte_6);
    const char* temp_1 = convertHexToBinary(inputByte_7);

    char buffer[8];
    strncpy(buffer, temp_0, 8);
    strncat(buffer, temp_1, 4);
    //cout << buffer<< endl;

    //curPacket.packetID = convertBinaryToDecimal(buffer);
}

void marshallProtocol::getSourcePacket(const char &inputByte_8, const char &inputByte_9)
{
    const char* temp_0 = convertHexToBinary(inputByte_8);
    const char* temp_1 = convertHexToBinary(inputByte_9);

    char buffer[8];
    strncpy(buffer, temp_0, 8);
    strncat(buffer, temp_1, 4);

    //curPacket.source = buffer;
    //curPacket.sourceLSB = buffer[7];

    cout << curPacket.source << endl;
    cout << curPacket.sourceLSB <<endl;
}

void marshallProtocol::getDestination(const char &inputByte_10, const char &inputByte_11)
{
    const char* temp_0 = convertHexToBinary(inputByte_10);
    const char* temp_1 = convertHexToBinary(inputByte_11);

    char buffer[8];
    strncpy(buffer, temp_0, 8);
    strncat(buffer, temp_1, 4);

    //curPacket.destination = buffer;
    //curPacket.destinationLSB = buffer [7];
}

unsigned short marshallProtocol::Utils_CalcCrcCcitt(unsigned char *pData, unsigned int Len, unsigned short Seed)
{
    //took it from nayax marshall protocal document, may need to change to and understand the calculation
    register unsigned int i, j;
    unsigned short Temp, ShortC, CurrentCRC = UTILS_SEED_CCITT, Crc = Seed;
    for ( i = 0; i < Len; i++ )
    {
        ShortC = (pData[i] & 0x00FF);
        Temp = ((Crc >> 8) ^ ShortC) << 8;

        for ( j = 0; j < 8; j++ )
        {
            if ( ((CurrentCRC ^ Temp) & 0x8000) )
                CurrentCRC = ((CurrentCRC << 1) ^ UTILS_POLYNOMIAL_CCITT);
            else
                CurrentCRC <<= 1;
            Temp <<= 1;
        }

        Crc = ( (Crc << 8) ^ CurrentCRC );
    }
    return ( Crc );
}

unsigned long int marshallProtocol::convertBinaryToDecimal(const char* num)
{   //conversion of binary to decimal

    /*unsigned long int decimalNumber = 0;
    int remainder;
    int i = 0;

    while( binaryNumber != 0)
    {
        remainder = binaryNumber%10;
        binaryNumber /= 10;
        decimalNumber += remainder*pow(2,i);
        i++;
    }*/

    unsigned long int binaryNumber = strtoul(num, nullptr, 2); //convert the binaray character arrary to decimal

    return binaryNumber;
}

const char* marshallProtocol::convertHexToBinary(const char inputByte)
{   //conversion of hex to binary
    char const* binary = "0000";

    switch(inputByte){
        case '0':
            binary = "0000";
        break;

        case '1':
            binary = "0001";
        break;

        case '2':
            binary = "0010";
        break;

        case '3':
            binary = "0011";
        break;

        case '4':
            binary = "0100";
        break;

        case '5':
            binary = "0101";
        break;

        case '6':
            binary = "0110";
        break;

        case '7':
            binary = "0111";
        break;

        case '8':
            binary = "1000";
        break;

        case '9':
            binary = "1001";
        break;

        case 'A':
            binary = "1010";
        break;

        case 'B':
            binary = "1011";
        break;

        case 'C':
            binary = "1100";
        break;

        case 'D':
            binary = "1101";
        break;

        case 'E':
            binary = "1110";
        break;

        case 'F':
            binary = "1111";
        break;
    }

    return binary;
}


