#include <iostream>
#include <QApplication>
#include <QDebug>
#include <vector>
#include <string>
#include "packetfromux410.h"
#include "packetfromecr.h"
#include "moneriscommunication.h"
#include "enums.h"
#include "transactionPackets.h"

#include "simulator.h"

#include "packetreadsort.h"

using namespace std;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    simulator sim;
    sim.show();

//    transactionPacket logon;
//    string pedID = "U4383144";
//    vector<uint8_t> packetToSend = logon.ppPosSetConfigPkt(CONFIG_ID::CON_PED_ID, pedID);
//    cout<< logon.getSendPacket();


//    transactionPacket logon;

//    vector<uint8_t> packetToSend = logon.logonPacket();
//    vector<uint8_t> packetRecievedTest;

//    monerisCommunication com;
//    com.init();

//    com.sendPacket(packetToSend, uint(packetToSend.size()));


//    packetRecievedTest = com.readForAck();

//    packetFromUX410 test;
//    test.packetReadFromUX(packetRecievedTest);
//    packetRecievedTest.clear();

//    if (test.getAckOrNak() == communicationPacketField::ACK)
//    {
//        while(packetRecievedTest[0] != 0x02){
//            packetRecievedTest = com.readPacket();
//        }

//       test.packetReadFromUX(packetRecievedTest);
//    }
//    packetResponse currentPacket = test.getPacket();


//    cout << logon.getSendPacket();
//    cout << test;

//    string priceString;
//    cout<< "Enter Price: ";
//    getline(cin, priceString);
//    packetToSend.clear();
//    packetRecievedTest.clear();
//    packetToSend = logon.purchasePacket(priceString);

//    cout << logon.getSendPacket();
//    com.sendPacket(packetToSend, uint(packetToSend.size()));

//    packetRecievedTest = com.readForAck();
//    test.packetReadFromUX(packetRecievedTest);
//    packetRecievedTest.clear();

//    if (test.getAckOrNak() == communicationPacketField::ACK)
//    {
//        while(packetRecievedTest[0] != 0x02){
//            packetRecievedTest = com.readPacket();
//        }

//       test.packetReadFromUX(packetRecievedTest);
//    }
//    //logon.cancelPurchasePacket();

//    currentPacket = test.getPacket();
//    cout << test;


//    vector<uint8_t> packetApproveTest;
//    uint8_t packetRead[] = {0x06, 0x02, 0x00, 21, 0x02, 0x0D, 0x00, 0x00, 0x06, 0x50, 0x30, 0x31, 0x1C, 0x30, 0x31, 0x30, 0x30, 0x1C, 0x33, 0x32, 0x41, 0x1C, 0x30, 0x32, 0x31, 0x03, 0x09};
//    for(uint i = 0; i < sizeof (packetRead); i++)
//        packetApproveTest.push_back(packetRead[i]);

//    packetFromUX410 approveTest;

//    packetReadSort approvePacket;
//    approvePacket.sortPacket(approveTest.getPacket());

    return a.exec();
}

