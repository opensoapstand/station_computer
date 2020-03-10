//***************************************
//
// packetfromux410.cpp
// moneris payment device response packet sorting
//
// created: 03-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "packetfromux410.h"
#include "enums.h"
#include "lrcgenerator.h"

#include <QDebug>
#include <unistd.h>

packetFromUX410::packetFromUX410()
{
    //initialized failed to read packet setting every parameters to FF
    packetToSort.STX = 0xFF;
    packetToSort.dataLen[0] = 0xFF;
    packetToSort.dataLen[1] = 0xFF;
    packetToSort.parentID = 0xFF;
    packetToSort.classID = 0xFF;
    packetToSort.APIID[0] = 0xFF;
    packetToSort.APIID[1] = 0xFF;
    packetToSort.status = 0xFF;
    packetToSort.data.push_back(0xFF);
    packetToSort.ETX = 0xFF;

}

void packetFromUX410::packetReadFromUX(std::vector<uint8_t> curPacket)
{
    if(0xFF == curPacket[0] && 1 == curPacket.size())
    {

        uint16_t combineLength = convertFrom8To16(packetToSort.dataLen[0], packetToSort.dataLen[1]) + 2;
        constructPartialPacket(combineLength);
    }
    else if(1 == curPacket.size())
    {
       checkAckOrNak(curPacket[0]); //store if the packet sent is Ack or NAk
    }
    else
    {
        sortPacket(curPacket);
        usleep(500000);
        uint16_t combineLength = convertFrom8To16(packetToSort.dataLen[0], packetToSort.dataLen[1]) + 2;
        constructPartialPacket(combineLength);

        LRCgenerator lrcCheck(partialPacket, uint(partialPacket.size()));
        //qDebug() << lrcCheck.getLRC();
    }

}

void packetFromUX410::sortPacket(std::vector<uint8_t> packetRead)
{
    uint i = 0;

//    if (communicationPacketField::ACK !=packetRead[i]) //check if ACK is not recieved
//    //if (communicationPacketField::ACK == packetRead[i]
//    {
//        while(packetRead[i] != communicationPacketField::ETX) //find the position for full packet
//        {
//            i++;    //increment until the end of the text character (ETX) is found 0x03

//            if(i > packetRead.size())
//            {
//                //return invalid
//            }
//        }
//        i += 2; //to skip LRC and to start the packet from (STX) 0x02
//    }
//    else {  //ACK is recieved
//        i++; //to start the packet from (STX) 0x02
//    }

    /*sort the packet accordingly*/
    packetToSort.STX = packetRead[i]; i++;
    packetToSort.dataLen[0] = packetRead[i]; i++;
    packetToSort.dataLen[1] = packetRead[i]; i++;
    packetToSort.parentID = packetRead[i]; i++;
    packetToSort.classID = packetRead[i]; i++;
    packetToSort.APIID[0] = packetRead[i]; i++;
    packetToSort.APIID[1] = packetRead[i]; i++;
    packetToSort.status = packetRead[i]; i++;

    packetToSort.data.clear();
    for(int j = 0; j < (convertFrom8To16(packetToSort.dataLen[0], packetToSort.dataLen[1])-5); j++)
    {
        packetToSort.data.push_back(packetRead[i]);
        i++;
    }

    packetToSort.ETX = packetRead[i]; i++;
    packetToSort.LRC = packetRead[i];

}

void packetFromUX410::checkAckOrNak(uint8_t AKNK)
{
    AckorNak = AKNK;
}

uint8_t packetFromUX410::getAckOrNak()
{
    return AckorNak;
}

packetResponse packetFromUX410::getPacket()
{
    return packetToSort;
}

std::ostream &operator <<(std::ostream &out, const packetFromUX410 &pkt)
{
    out << "Recieved Packet: ";
    out << std::hex <<  int(pkt.packetToSort.STX) << " ";
    out << std::hex << int(pkt.packetToSort.dataLen[0]) << " ";
    out << std::hex << int(pkt.packetToSort.dataLen[1]) << " ";
    out << std::hex << int(pkt.packetToSort.parentID) << " ";
    out << std::hex << int(pkt.packetToSort.classID) << " ";
    out << std::hex << int(pkt.packetToSort.APIID[0]) << " ";
    out << std::hex << int(pkt.packetToSort.APIID[1]) << " ";
    out << std::hex << int(pkt.packetToSort.status) << " ";


    for (uint i = 0; i < pkt.packetToSort.data.size(); i++)
        out << std::hex << int(pkt.packetToSort.data[i]) << " ";

    out << std::hex << int(pkt.packetToSort.ETX) << " ";
    out << std::hex << int(pkt.packetToSort.LRC) << std::endl;

    return out;
}

uint16_t packetFromUX410::convertFrom8To16(uint8_t firstByte, uint8_t secondByte)
{
    uint16_t combinedByte = 0x0000;

    combinedByte = firstByte;
    combinedByte = combinedByte << 8; //shift first byte
    combinedByte |= secondByte;

    return combinedByte;
}


void packetFromUX410::constructPartialPacket(uint len)
{
    unsigned int i = 0;
    std::vector<uint8_t> pkt;
    pkt.reserve(len);
    //memset(lrcPkt, 0, decimalLength*sizeof (uint8_t));

    pkt.push_back(packetToSort.dataLen[0]); i++;
    pkt.push_back(packetToSort.dataLen[1]); i++;
    pkt.push_back(packetToSort.parentID); i++;
    pkt.push_back(packetToSort.classID);  i++;
    pkt.push_back(packetToSort.APIID[0]); i++;
    pkt.push_back(packetToSort.APIID[1]); i++;
    pkt.push_back(packetToSort.status); i++;

    int j = 0;
    for(; i <= len; i++)
    {
        pkt.push_back(packetToSort.data[uint(j)]);
        j++;
    }

    partialPacket = pkt;
}
