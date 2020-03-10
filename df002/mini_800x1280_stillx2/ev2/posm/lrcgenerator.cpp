//***************************************
//
// lrcgenerator.cpp
// payment device lrc(longitudinal redundancy check)
//
// created: 01-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "lrcgenerator.h"

LRCgenerator::LRCgenerator(std::vector<uint8_t> packet, uint packetSize)
{
    uint8_t lrcResult  = packet[0]; //Initializes the first byte as LRC;

    if (packetSize<2){
        LRC = lrcResult;
    }
    else{
        bool lrcBits[8]; //XOR of two bytes
        bool packetByte1Bits[8] = {false}; // byte 1 to be stored in bits
        bool packetByte2Bits[8] = {false}; // byte 2 to be stored in bits
        uint i = 1;

        while (i < packetSize){ //LRC if packet is greater then 2
            for(int j = 0; j < 8; ++j){ // converts bytes to bits and XOR
                packetByte1Bits[j] = (packet[i] & (1<<j)) != 0;
                packetByte2Bits[j] = (lrcResult & (1<<j)) != 0;
                lrcBits[j] = packetByte1Bits[j] ^ packetByte2Bits [j];
            }

            lrcResult = 0x00; //needs to be 0x00 in order for correct conversin back to Byte

            for (int k = 0; k <8 ; ++k){ //bits back to byte for lrc
                if(lrcBits[k]){
                    lrcResult |= 1<<k;
                }
            }

            i++;
        }

        LRC = lrcResult;
    }
}


uint8_t LRCgenerator::getLRC()
{
    return LRC;
}
