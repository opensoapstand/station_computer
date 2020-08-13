//***************************************
//
// df_sockets.cpp
//
// this class defines the interaction sockets
//
// created: 01-04-2019
// by: Phur Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved



#include "df_sockets.h"

#include <iostream>
#include <string>
#include <string.h>
#include <chrono>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <df_sockets.h>
#include "globalvariables.h";

#include  <string>
df_sockets::df_sockets() {

}
/*
df_sockets::df_sockets(std::string temp, header * packetToWrite)
{

 writePackets(temp, packetToWrite);

}


*/


using namespace std;
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define READ_PORT 4321
#define WRITE_PORT 1234
void * df_sockets::listenToPackets(void * pthreadArgs)
{
	 cout<<"in reader thread"<<endl;
	 ssize_t valread2;
	 packet  readPacket[128] = {};

    int server_fd, new_socket, valread;
	 struct sockaddr_in address,clientAddr;
    int opt = 1;
	 int addrlen = sizeof(address);
	 int clientAddrLength = sizeof(clientAddr);


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }


    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
	 address.sin_addr.s_addr = INADDR_ANY;
	 address.sin_port = htons(READ_PORT);

    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	 if (listen(server_fd, 20) < 0)
    {        perror("listen");
        exit(EXIT_FAILURE);
    }

	 new_socket = accept(server_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&(clientAddrLength));
	 if (new_socket<0)
	 {
		 perror("accept");
		 exit(EXIT_FAILURE);
	 }


	 while(true)
	 {
		 cout<<"UI listening"<<endl;
		 valread2 = read(new_socket, readPacket, 128);
		 if (valread2 > 0) {

			// printf("data version 1 : %d", readPacket->version);
			// std::cout<<readPacket->body;
			progressData = readPacket->body;
			cout<< readPacket->body<<endl;
		 }
		 else if(valread==0)
		 {

		 }
		 usleep(10);
	}
}


void  df_sockets::writePackets (packet packetToSend) {
	 int server_fd,n;
    struct sockaddr_in address;
	 //int opt = 1;
  //  int addrlen = sizeof(address);


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
	 address.sin_addr.s_addr = INADDR_ANY;
	 address.sin_port = htons(WRITE_PORT);

    if (connect(server_fd,(struct sockaddr *) &address,sizeof(address)) < 0) {
        std::cout<<"ERROR connecting";
    }


	 packet tempHeader ;
	 tempHeader.version= packetToSend.version;
	 tempHeader.id = packetToSend.id;
	 tempHeader.body = packetToSend.body;
	 tempHeader.query = query_types::s_dispense;
	 tempHeader.stateToSet = df_State::selected;


	 n = write(server_fd, &tempHeader,1024);

    if(n<0)
    {
        std::cout<<"ERROR writting"<<endl;
    }

     else
    {
         std::cout<<"wrote "<< n <<" bytes";
       //  close(server_fd);
     }

}


