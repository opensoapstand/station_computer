//***************************************
//
// packetcommunications.h
// tcp handler
//
// created: 16-MAY-2019
// by: Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef PACKETCOMMUNICATION_H__
#define PACKETCOMMUNICATION_H__

#include <sys/socket.h>
#include "../fsm.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "../mainwindow.h"
#include "../dispenseWindow.h"
#include "../countdownwindow.h"
#include <QDebug>
#define PORT 2020


#define WRITE_PORT 4321
#define READ_PORT  1234

struct packet {
    int version;
    int id ;
    int size ;
    command_types command;
    data_types data;
    int bevToDispense;
    int volume;
    int dispensedVolume;
    datamodel::df_State stateToSet;
    datamodel::df_State Current;
    int page;
    bool doAction;
} ;

using namespace std ;

class packetcommunications {
public:
    packetcommunications();
    ~packetcommunications();

    static void * SendPackets(void * pThreadArgs);

    static void initCommunication();
    static void packetReadSet();
    static int VolumeDispensed;





private:
    static packet _packet;
    static bool _packetToSend;
};


#endif //PACKETCOMMUNICATION_H__
