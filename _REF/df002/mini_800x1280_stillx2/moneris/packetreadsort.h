#ifndef PACKETREADSORT_H
#define PACKETREADSORT_H

#include"packetfromux410.h"
#include"packetfromecr.h"

class packetReadSort{
public:
    packetReadSort();
    ~packetReadSort();
    void sortPacket(packetResponse readPacket);

private:
   packetResponse packetToSort;
};

#endif // PACKETREADSORT_H
