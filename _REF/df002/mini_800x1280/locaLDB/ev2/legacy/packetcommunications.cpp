//***************************************
//
// packetcommunications.cpp
// tcp handler
//
// created: 16-MAY-2019
// by: Tenzin Sherpa
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************
#include "packetcommunications.h"

extern packet dispensedRead;
extern datamodel g_SystemModel; // @suppress("Unused variable declaration in file scope")
extern int pageNumber;
extern bool goHome;
extern int progressValue;
extern packet toSend;
packet packetcommunications::_packet;
bool packetcommunications::_packetToSend;
extern bool _packetRead;
int packetcommunications::VolumeDispensed;
bool isDispensing;
extern int prevDispensed;


packetcommunications::packetcommunications() {
    _packetToSend = false;
    _packetRead = false;
}

void packetcommunications::initCommunication() {
    _packet.version = toSend.version;
    _packet.id = toSend.id;
    _packet.size = toSend.size;
    _packet.command = toSend.command;
    _packet.data = toSend.data;
    _packet.bevToDispense = toSend.bevToDispense;
    _packet.volume = toSend.volume;
    _packet.dispensedVolume = toSend.dispensedVolume;
    _packet.stateToSet = toSend.stateToSet;
    _packet.page = toSend.page;
    _packet.doAction = toSend.doAction;
    _packetToSend = true;
}

void* packetcommunications::SendPackets(void *pthreadArgs) {
    bool sent = false;
    int sock = 0;

    struct sockaddr_in serv_addr;
    packet buffer[1024] = { 0 };
    packet reply;
    int writesize;
    int readsize;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        //exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        //exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        //exit(EXIT_FAILURE);
    }

    _packet.id = 0;
    _packet.command = command_types::set;
    _packet.data = data_types::state;
    _packet.version = 0;
    _packet.id = 0;
    _packet.size = 5;
    _packet.bevToDispense = 10;
    _packet.volume = 50;
    _packet.dispensedVolume = 0;
    _packet.Current = datamodel::df_IDLE;
    _packet.stateToSet = datamodel::df_IDLE;
    _packet.page = 0;
    _packet.doAction = false;
    _packetRead = false;
    isDispensing = false;
    pageNumber = 0;
    writesize = write(sock, &_packet, sizeof(_packet));
    sent = true;
    usleep(200);
    while (sent) {
        //if (_packetRead){*/
        if(toSend.doAction){

            initCommunication();
            toSend.doAction = false;

        }
        else{   readsize = read(sock, buffer, 1024);
            switch (buffer->command) {
            case command_types::command_dummy:


                break;


            case command_types::query:
                switch (buffer->data) {
                case data_types::data_dummy:
                    break;

                case data_types::volume:
                    break;

                case data_types::region:
                    break;

                case data_types::state:
                    break;

                case data_types::dispenser:
                    break;

                case data_types::dispensing:

                    break;

                }

                break;

            case command_types::set:
                switch (buffer->data) {
                case data_types::data_dummy:
                    break;

                case data_types::volume:
                    break;

                case data_types::region:
                    break;

                case data_types::state:
                    pageNumber = 3;

                    break;

                case data_types::dispenser:

                    break;

                case data_types::dispensing:

                    break;
                }
                break;

            case command_types::respond:
                switch (buffer->data) {
                case data_types::data_dummy:
                    break;

                case data_types::volume:
                    break;

                case data_types::region:
                    break;

                case data_types::state:
                    _packet.command = query;
                    _packet.data = state;
                    break;

                case data_types::dispenser:
                    toSend.command = query;
                    toSend.data = dispensing;
                    pageNumber = 1;
                    initCommunication();
                    break;

                case data_types::dispensing:
                    reply.dispensedVolume = buffer->dispensedVolume;
                    reply.Current = buffer->Current;
                    _packet.command = query;
                    _packet.data = dispensing;


                    if(reply.Current == datamodel::df_DISPENSE_PAUSED){
                        isDispensing = true;
                        pageNumber = 2;

                    }

                    if(reply.Current == datamodel::df_DISPENSE){
                        isDispensing = true;
                        pageNumber = 1;
                    }

                    progressValue = reply.dispensedVolume ;
                    if (reply.Current == datamodel::df_DISPENSE_ENDED || reply.Current == datamodel::df_IDLE){
                        if (isDispensing){
                            pageNumber = 3;
                            _packet.page = 3;
                            _packet.doAction = false;
                            isDispensing = false;
                        }



                    }
                    break;
                }
                break;

            }
        }
        usleep(200);
        writesize = write(sock, &_packet, sizeof(_packet));
        _packet.doAction = false;
        usleep(200);
    }


    return 0;
}

void packetcommunications::packetReadSet(){
    _packetRead = false;
}

