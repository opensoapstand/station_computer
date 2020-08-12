#ifndef DF_SOCKETS_H
#define DF_SOCKETS_H
#include "df_packets.h"
#include "string.h"
#include <stdlib.h>
#include <iostream>

class df_sockets
{
public:
    df_sockets();
    df_sockets(std::string temp, packet * packetToWrite);
	 static void  writePackets(packet packetToSend);
	 static void * listenToPackets(void * pthreadArgs);
	// void * listenToPackets();

};

#endif // DF_SOCKETS_H
