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

#ifndef PACKETREADSORT_H
#define PACKETREADSORT_H

#include"packetfromux410.h"
#include"packetfromecr.h"

#include <QDebug>

class packetReadSort{
public:
    packetReadSort();
    ~packetReadSort();
    void sortPacket(packetResponse readPacket);

private:
   packetResponse packetToSort;
};

#endif // PACKETREADSORT_H
