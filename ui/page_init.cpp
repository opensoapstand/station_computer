//***************************************
//
// page_init.cpp
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-04-2021
// by: Paddy Riley
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "page_init.h"
#include "page_idle.h"
#include "ui_page_init.h"

// CTOR
page_init::page_init(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::page_init)
{
    // Background Set here; Inheritance on forms places image on all elements otherwise.
    ui->setupUi(this);
   

    // IPC Networking
    dfUtility = new df_util();
    dfUtility->m_IsSendingFSM = false;

    initIdleTimer = new QTimer(this);
    initIdleTimer->setInterval(1000);
    connect(initIdleTimer, SIGNAL(timeout()), this, SLOT(onInitTimeoutTick()));

    rebootTimer = new QTimer(this);
    rebootTimer->setInterval(1000);
    connect(rebootTimer, SIGNAL(timeout()), this, SLOT(onRebootTimeoutTick()));


    // usleep(1000000);

}

/*
 * Navigation to Product item
 */
void page_init::setPage(page_idle *pageIdle)
{
    // Chained to KB Listener
    this->p_page_idle = pageIdle;
}

// DTOR
page_init::~page_init()
{
    delete ui;
}

void page_init::showEvent(QShowEvent *event)
{
    qDebug() << "<<<<<<< Page Enter: Init >>>>>>>>>";
    QWidget::showEvent(event);

    // QPixmap background(PAGE_INIT_BACKGROUND_IMAGE_PATH);
    // background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    // QPalette palette;
    // palette.setBrush(QPalette::Background, background);
    // this->setPalette(palette);
    
    
    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_INIT_BACKGROUND_IMAGE_PATH);

    //    qDebug() << "Start init Timers" << endl;
    initIdleTimer->start(1000);
#ifdef START_FSM_FROM_UI
    start_controller = true;
#else
    start_controller = false;
#endif

    if (start_controller)
    {
        system("DISPLAY=:0 xterm -hold  /release/fsm/controller &");
        _initIdleTimeoutSec = 20;
    }
    else if(!tapSetupStarted){
        ui->init_label->setText("Wait for Payment signal.");

        tap_init();
    }
    else
    {

        ui->init_label->setText("Wait for controller signal.");

#ifdef WAIT_FOR_CONTROLLER_READY
        _initIdleTimeoutSec = 20;
#else

        _initIdleTimeoutSec = 1;
#endif
    }
    
    // tap_init();

}

void page_init::initReadySlot(void)
{
    qDebug() << "Signal: init ready from fsm";
    initIdleTimer->stop();
    rebootTimer->stop();
    // qDebug() << "init to idle";
    // p_page_idle->showFullScreen();
    // this->hide();
    p_page_idle->pageTransition(this, p_page_idle);
}

void page_init::onInitTimeoutTick()
{
    if (--_initIdleTimeoutSec >= 0)
    {
        //        qDebug() << "init: Tick Down - " << _initIdleTimeoutSec << endl;
        ui->init_label->setText(ui->init_label->text() + ".");
    }
    else
    {
        //        qDebug() << "Timer Done!" << _initIdleTimeoutSec << endl;
        initIdleTimer->stop();

        ui->fail_label->setText("Init Timeout. No response from controller.");

#ifdef WAIT_FOR_CONTROLLER_READY
        // if (!start_controller){
        // }
        ui->fail_label->setText("No response from controller. Will reboot.");
        _rebootTimeoutSec = 5;
        rebootTimer->start(1000);
#else
        ui->fail_label->setText("Will start standalone mode. If controller becomes active, commands will be executed. If not, no commands will be executed.");

        initReadySlot();
#endif
    }
}

void page_init::onRebootTimeoutTick()
{
    if (--_rebootTimeoutSec >= 0)
    {
        qDebug() << "init: Reboot Tick Down - " << _rebootTimeoutSec << endl;
        ui->fail_label->setText(ui->fail_label->text() + ".");
    }
    else
    {
        qDebug() << "Reboot Timer elapsed. (should reboot computer now)" << _rebootTimeoutSec << endl;
        rebootTimer->stop();

        // REBOOT!
        system("./release/reboot.sh");
    }
}

bool page_init::tap_init()
{
    paymentConnected = com.page_init();

    while (!paymentConnected)
    {
        paymentConnected = com.page_init();
    }
    sleep(35);

    // This is super shitty - there must be a better way to find out when the green light starts flashing on the UX420 but it was 35

    cout << "_----_-----__------_-----";
   
    
    /*logon packet to send*/
    cout << "Sending Logon packet..." << endl;
    pktToSend = paymentPacket.logonPacket();
    if (sendToUX410PageInit())
    {
        cout << "Receiving Logon response" << endl;
        isInitLogin = true;
        waitForUX410PageInit();
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    cout << "-----------------------------------------------" << endl;

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Name query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_NAME);
    if (sendToUX410PageInit())
    {
        cout << "Receiving Merchant Name" << endl;
        waitForUX410PageInit();
        isInitMerchant = true;
        merchantName = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        cout << merchantName << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    cout << "-----------------------------------------------" << endl;

    /*getConfiguration packet to send*/
    cout << "Sending Merchant Address query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::MERCH_ADDR);
    if (sendToUX410PageInit())
    {
        cout << "Receiving Merchant Address" << endl;
        waitForUX410PageInit();
        isInitAddress = true;
        // merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        merchantAddress = paymentPktInfo.dataField(readPacket.getPacket().data);

        std::cout << merchantAddress << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    cout << "-----------------------------------------------" << endl;

    /*getConfiguration packet to send*/
    cout << "Sending PTID query..." << endl;
    pktToSend = paymentPacket.ppPosGetConfigPkt(CONFIG_ID::CON_TID);
    if (sendToUX410PageInit())
    {
        cout << "Receiving PTID" << endl;
        waitForUX410PageInit();
        isInitTerminalID = true;
        terminalID = paymentPktInfo.dataField(readPacket.getPacket().data).substr(2);
        std::cout << terminalID << endl;
        pktResponded.clear();
    }
    else
    {
        return false;
    }
    com.flushSerial();
    tapSetupStarted = true;
    return true;
}

bool page_init::waitForUX410PageInit()
{
    bool waitResponse = false;
    while (!waitResponse)
    {
        sleep(1);
               QCoreApplication::processEvents();
        cout << readPacket << endl;
        if (pktResponded[0] != 0x02)
        {
            pktResponded.clear();
            pktResponded = com.readPacket();
            usleep(10);
        }
        else
        {
             pktResponded = com.readPacket();
            readPacket.packetReadFromUX(pktResponded);
            std::cout << readPacket;
            com.sendAck();
            waitResponse = true;
        }
    }
    return waitResponse;
}



bool page_init::sendToUX410PageInit()
{
    int waitForAck = 0;
    while (waitForAck < 3)
    {
        cout << "Wait for ACK counter: " << waitForAck << endl;
        com.sendPacket(pktToSend, uint(pktToSend.size()));
        std::cout << "sendtoUX410PageInit Electronic Card Reader: " << paymentPacket.getSendPacket() << endl;
        sleep(1);
        // read back what is responded
        pktResponded = com.readForAck();
        readPacket.packetReadFromUX(pktResponded);
        pktResponded.clear();
        waitForAck++;

        // if(isReadyForTap) {
        cout<< readPacket.getAckOrNak();
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
    }
    return false;
}