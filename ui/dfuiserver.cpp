#include "dfuiserver.h"
#include "dfuicommthread.h"
#include "df_util.h" // lode added for settings

DfUiServer::DfUiServer(QObject *parent) : QTcpServer(parent)
{
}

void DfUiServer::startServer()
{
    // int port = 1235;
    int port = 8645;
    busyHandlingRequest = false;
    requestCounter = 0;

    if (!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Server: Could not start server";
    }
    else
    {
        qDebug() << "Server: Listening for fsm messages on port " << port << "...";
    }
}

void DfUiServer::resetTimerSlot()
{
    emit pleaseReset();
}
void DfUiServer::transactionEndSlot()
{
    emit controllerFinishedAck();
}

void DfUiServer::updateVolumeSlot(double dispensed)
{
    emit signalUpdateVolume(dispensed, false);
}

void DfUiServer::targetHitSlot()
{
    emit targetHit();
}
void DfUiServer::noFlowAbortSlot()
{
    emit noFlowAbort();
}

void DfUiServer::initReadySlot()
{
    emit initReady();
}
void DfUiServer::printerStatusSlot(bool isOnline, bool hasPaper)
{
    emit printerStatus(isOnline, hasPaper);
}
void DfUiServer::dispenseButtonPressedSlot()
{
    emit dispenseButtonPressedSignal();
}
void DfUiServer::messageHandlerFinishedSlot()
{
    requestCounter--;
    qDebug() << "Message fully handled. Ready to receive new commands. counter should be zero now: " << requestCounter;
}

void DfUiServer::MMSlot()
{
    emit MM();
}

// This function is called by QTcpServer when a new connection is available.
void DfUiServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    requestCounter++;
    // In this setup, there are too many ways of crashing when multiple requests are handled at the same time, do not allow for it.
    if (requestCounter > 1)
    {

        requestCounter--;
        qDebug() << "UI still handling a previous request. Will not allow parallel requests. Wait till unblocked and try again....." << requestCounter;
        return;
    }
    else
    {
        // qDebug() << "request counter: " << requestCounter;
        // qDebug() << "Incoming request accepted.*************";
    }
    busyHandlingRequest = true;
    // Every new connection will be run in a newly created thread
    DfUiCommThread *messageHandlerThread = new DfUiCommThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(messageHandlerThread, SIGNAL(finished()), messageHandlerThread, SLOT(deleteLater()));
    connect(messageHandlerThread, &DfUiCommThread::transactionEndSignal, this, &DfUiServer::transactionEndSlot);
    connect(messageHandlerThread, &DfUiCommThread::resetTimerSignal, this, &DfUiServer::resetTimerSlot);
    connect(messageHandlerThread, &DfUiCommThread::updateVolumeSignal, this, &DfUiServer::updateVolumeSlot);
    connect(messageHandlerThread, &DfUiCommThread::messageHandlerFinishedSignal, this, &DfUiServer::messageHandlerFinishedSlot);

    connect(messageHandlerThread, &DfUiCommThread::noFlowAbortSignal, this, &DfUiServer::noFlowAbortSlot);
    connect(messageHandlerThread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::targetHitSlot);

    // connect(messageHandlerThread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::noFlowAbortSlot);
    // connect(messageHandlerThread, &DfUiCommThread::noFlowAbortSignal, this, &DfUiServer::targetHitSlot);

    connect(messageHandlerThread, &DfUiCommThread::initReadySignal, this, &DfUiServer::initReadySlot);
    connect(messageHandlerThread, &DfUiCommThread::printerStatusSignal, this, &DfUiServer::printerStatusSlot);
    connect(messageHandlerThread, &DfUiCommThread::MMSignal, this, &DfUiServer::MMSlot);
    connect(messageHandlerThread, &DfUiCommThread::dispenseButtonPressedSignal, this, &DfUiServer::dispenseButtonPressedSlot);

    messageHandlerThread->start(); // reminder: this is non blocking! It start a thread (which is executed independently) and moves on right away
    busyHandlingRequest = false;

   
}
