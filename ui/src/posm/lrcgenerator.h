//***************************************
//
// lrcgenerator.h
// payment device lrc(longitudinal redundancy check)
//
// created: 01-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef LRCGENERATOR_H
#define LRCGENERATOR_H

#include <bitset>
#include <vector>

class LRCgenerator
{
public:
    LRCgenerator(std::vector<uint8_t> packet, uint packetSize);
    uint8_t getLRC();
    //uint8_t LRC_Check(std::vector<uint8_t> packet, int packetSize);
private:
    uint8_t LRC;
};

#endif // LRCGENERATOR_H
