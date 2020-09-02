#include "simulator.h"
#include "ui_simulator.h"

#include <iostream>
#include <QTimer>
using namespace std;
static string Mid = "0030410835171"; //0030128909430 0030410835171
//static string pedID = "U4393371";//Unit 8"U4383002"23383002; //U4383017 //U4383144 //U4383005
static string pedID = "U4383002"; //U4383017 //U4383144 //U4383005
static string URL = "ipg1.moneris.com:443";
static string Name =  "DRINKFILL LTD";
static string Address = "2608 GRANVILLE ST #330\n   VANCOUVER  BC  V6H 3H8";
simulator::simulator(QWidget *parent) :
    QWidget(parent),

    ui(new Ui::simulator)
{
    ui->setupUi(this);
    timerEnabled = false;
    purchaseEnable = false;
    ui->textEdit->setPlaceholderText("Price $");
    readtimer = new QTimer(this);
    connect (readtimer, SIGNAL(timeout()), this, SLOT(readtimer_loop()));
    com.init();
}

simulator::~simulator()
{
    delete ui;
}

void simulator::on_logonButton_clicked()
{
    pktToSend = pkt.logonPacket();
    purchaseEnable = false;
}

void simulator::on_BalanceButton_clicked()
{
    pktToSend = pkt.batchBalancePkt();
    purchaseEnable = false;
}

void simulator::on_CloseButton_clicked()
{
    pktToSend = pkt.batchClosePkt();
    purchaseEnable = false;
}

void simulator::on_purchaseButton_clicked()
{
    //QString temp = ui->lineEdit->text();
    QString temp = ui->textEdit->toPlainText();
    std::string  inputPrice = temp.toUtf8().constData();
    pktToSend = pkt.purchasePacket(inputPrice);
    purchaseEnable = true;
}

void simulator::on_cancelButton_clicked()
{
    timerEnabled = true;
    purchaseEnable = false;
    pktToSend = pkt.purchaseCancelPacket();
}

void simulator::on_sendButton_clicked()
{
    std::cout << com.sendPacket(pktToSend, uint(pktToSend.size()));

    std::cout<<pkt.getSendPacket();

    //read back what is responded

    packetRecievedTest = com.readForAck();

    ux410.packetReadFromUX(packetRecievedTest);
    packetRecievedTest.clear();

    if (ux410.getAckOrNak() == communicationPacketField::ACK)
    {
        timerEnabled = true;        
        readtimer->start(10);
    }


}

void simulator::readtimer_loop()
{

    if(packetRecievedTest[0] != 0x02){
        packetRecievedTest = com.readPacket();
        cout << "packetReceivedTest: " << to_string(packetRecievedTest[0]) << endl;
        cout << "restart timer" << endl;
        readtimer->start(10);
    }
    else {
        cout << "Post Tap" << endl;
        cout<<pkt.getSendPacket();
        ux410.packetReadFromUX(packetRecievedTest);
        if (purchaseEnable == true){//once purchase successed create a receipt and store into database
            dataPacket.transactionID(ux410.getPacket().data);
            dataPacket.makeReceipt();
        }
        std::cout << ux410;
        timerEnabled = false;

    }


    if (timerEnabled == false){
        readtimer->stop();
        purchaseEnable = false;
    }
}

void simulator::on_configButton_clicked()
{
    timerEnabled = false;
    monerisConfig = 0;
    while (monerisConfig < 4)
    {
        if (monerisConfig == 0 && timerEnabled == false)
        {
            pktToSend = pkt.ppPosSetConfigPkt(CONFIG_ID::CON_MID, Mid);
            com.sendPacket(pktToSend, uint(pktToSend.size()));
            std::cout<<pkt.getSendPacket();
            packetRecievedTest = com.readForAck();
            ux410.packetReadFromUX(packetRecievedTest);
            packetRecievedTest.clear();
            sleep(1);
            if (ux410.getAckOrNak() == communicationPacketField::ACK)
            {
                packetRecievedTest = com.readPacket();
                ux410.packetReadFromUX(packetRecievedTest);
                std::cout << ux410;
                timerEnabled = false;
                monerisConfig++;
            }
        }
        else if (monerisConfig == 1 && timerEnabled == false)
        {
            pktToSend = pkt.ppPosSetConfigPkt(CONFIG_ID::CON_PED_ID, pedID);
            com.sendPacket(pktToSend, uint(pktToSend.size()));
            std::cout<<pkt.getSendPacket();
            packetRecievedTest = com.readForAck();
            ux410.packetReadFromUX(packetRecievedTest);
            packetRecievedTest.clear();
            sleep(1);
            if (ux410.getAckOrNak() == communicationPacketField::ACK)
            {
                packetRecievedTest = com.readPacket();
                ux410.packetReadFromUX(packetRecievedTest);
                std::cout << ux410;
                timerEnabled = false;
                monerisConfig++;
            }
        }
        else if (monerisConfig == 2 && timerEnabled == false)
        {
            pktToSend = pkt.ppPosSetConfigPkt(CONFIG_ID::URL1, URL);
            com.sendPacket(pktToSend, uint(pktToSend.size()));
            std::cout<<pkt.getSendPacket();
            packetRecievedTest = com.readForAck();
            ux410.packetReadFromUX(packetRecievedTest);
            packetRecievedTest.clear();
            sleep(1);
            if (ux410.getAckOrNak() == communicationPacketField::ACK)
            {
                packetRecievedTest = com.readPacket();
                ux410.packetReadFromUX(packetRecievedTest);
                std::cout << ux410;
                timerEnabled = false;
                monerisConfig++;
            }
        }
        else if (monerisConfig == 3 && timerEnabled == false)
        {
            pktToSend = pkt.ppPosSetConfigPkt(CONFIG_ID::CON_TID, pedID);
            com.sendPacket(pktToSend, uint(pktToSend.size()));
            std::cout<<pkt.getSendPacket();
            packetRecievedTest = com.readForAck();
            ux410.packetReadFromUX(packetRecievedTest);
            packetRecievedTest.clear();
            sleep(1);
            if (ux410.getAckOrNak() == communicationPacketField::ACK)
            {
                packetRecievedTest = com.readPacket();
                ux410.packetReadFromUX(packetRecievedTest);
                std::cout << ux410;
                timerEnabled = false;
                monerisConfig++;
            }
        }

        packetRecievedTest.clear();
    }
}

void simulator::on_nameButton_clicked()
{
    pktToSend = pkt.ppPosSetConfigPkt(CONFIG_ID::MERCH_NAME, Name);
    com.sendPacket(pktToSend, uint(pktToSend.size()));
    std::cout<<pkt.getSendPacket();
    packetRecievedTest = com.readForAck();
    ux410.packetReadFromUX(packetRecievedTest);
    packetRecievedTest.clear();
    sleep(1);
    if (ux410.getAckOrNak() == communicationPacketField::ACK)
    {
        packetRecievedTest = com.readPacket();
        ux410.packetReadFromUX(packetRecievedTest);
        std::cout << ux410;
        timerEnabled = false;
        monerisConfig++;
    }

    //02 00 10 02 0d 00 03 05 00 09 44 72 69 6e 6b 66 69 6c 6c 03 45
}

void simulator::on_addressButton_clicked()
{
    pktToSend = pkt.ppPosSetConfigPkt(CONFIG_ID::MERCH_ADDR, Address);
    com.sendPacket(pktToSend, uint(pktToSend.size()));
    std::cout<<pkt.getSendPacket();
    packetRecievedTest = com.readForAck();
    ux410.packetReadFromUX(packetRecievedTest);
    packetRecievedTest.clear();
    sleep(1);
    if (ux410.getAckOrNak() == communicationPacketField::ACK)
    {
        packetRecievedTest = com.readPacket();
        ux410.packetReadFromUX(packetRecievedTest);
        std::cout << ux410;
        timerEnabled = false;
    }
    /*02 00 36 02 0d 00 03 06 00 2f 33 33 30 2d 32 36 30 38 20 47 72 61 6e 76
    69 6c 6c 65 20 53 74 2e 20 0a 20 56 61 6e 63 6f 75 76 65 72 2c 20
    42 43 2e 20 56 36 48 20 33 48 38 03 48*/

}

void simulator::on_pushButton_clicked()
{
    pktToSend = pkt.ppPosStatusCheckPkt(0x03);
    com.sendPacket(pktToSend, uint(pktToSend.size()));
    std::cout<<pkt.getSendPacket();
    packetRecievedTest = com.readForAck();
    ux410.packetReadFromUX(packetRecievedTest);
    packetRecievedTest.clear();
    sleep(1);
    if (ux410.getAckOrNak() == communicationPacketField::ACK)
    {
        packetRecievedTest = com.readPacket();
        ux410.packetReadFromUX(packetRecievedTest);
        std::cout << ux410;
        timerEnabled = false;
    }
}
