//***************************************
//
// page_init.cpp
// GUI class while machine is initializing.
//
// Display Fullscreen DF branded Wallpaper
// Listen for User interaction to load
// Idle Page
//
// created: 16-06-2023
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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
}

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
    p_page_idle->registerUserInteraction(this); // replaces old "<<<<<<< Page Enter: pagename >>>>>>>>>" log entry;
    QWidget::showEvent(event);

    // load template texts
    p_page_idle->loadTextsFromTemplateCsv();
    p_page_idle->loadTextsFromDefaultCsv();

    p_page_idle->setBackgroundPictureFromTemplateToPage(this, PAGE_INIT_BACKGROUND_IMAGE_PATH);

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
    else
    {
        // while(!tapSetupStarted){

        // ui->init_label->setText("Wait for Payment signal.");
        // qDebug() << "In tap init";
        // QCoreApplication::processEvents();

        // tap_serial_initiate();}
        p_page_idle->setTemplateTextToObject(ui->label_init_message);
        _initIdleTimeoutSec = 1;
    }
}

void page_init::hideCurrentPageAndShowProvided(QWidget *pageToShow)
{
    p_page_idle->pageTransition(this, pageToShow);
    initIdleTimer->stop();
    rebootTimer->stop();
}

void page_init::initReadySlot(void)
{
    qDebug() << "Signal: init ready from fsm";
    hideCurrentPageAndShowProvided(p_page_idle);
}

void page_init::onInitTimeoutTick()
{
    if (--_initIdleTimeoutSec >= 0)
    {
        ui->label_init_message->setText(ui->label_init_message->text() + ".");
    }
    else
    {
        qDebug() << "No response from controller. Will reboot";
        initIdleTimer->stop();
        p_page_idle->setTemplateTextToObject(ui->label_fail_message);
        hideCurrentPageAndShowProvided(p_page_idle);
    }
}

void page_init::onRebootTimeoutTick()
{
    if (--_rebootTimeoutSec >= 0)
    {
        qDebug() << "init: Reboot Tick Down - " << _rebootTimeoutSec;
    }
    else
    {
        qDebug() << "Reboot Timer elapsed. (should reboot computer now)" << _rebootTimeoutSec;
        rebootTimer->stop();

        // REBOOT!
        system("./release/reboot.sh");
    }
}


bool page_init::tap_serial_initiate()
{
    paymentConnected = com.page_init();
    while (!paymentConnected)
    {
        paymentConnected = com.page_init();
    }
    sleep(35);
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
        cout << readPacket.getAckOrNak();
        if (readPacket.getAckOrNak() == communicationPacketField::ACK)
        {
            return true;
        }
    }
    return false;
}