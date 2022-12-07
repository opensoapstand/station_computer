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

void DfUiServer::MMSlot()
{
    emit MM();
}

// This function is called by QTcpServer when a new connection is available.
void DfUiServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection

    // In this setup, there are too many ways of crashing when multiple requests are handled at the same time, do not allow for it.
    if (busyHandlingRequest)
    {

        qDebug() << "UI still handling a previous request. Will not allow parallel requests. Wait till unblocked and try again.....";
        return;
    }
    else
    {
        qDebug() << "Incoming request accepted.*************";
    }
    busyHandlingRequest = true;
    // Every new connection will be run in a newly created thread
    DfUiCommThread *thread = new DfUiCommThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, &DfUiCommThread::transactionEndSignal, this, &DfUiServer::transactionEndSlot);
    connect(thread, &DfUiCommThread::resetTimerSignal, this, &DfUiServer::resetTimerSlot);
    connect(thread, &DfUiCommThread::updateVolumeSignal, this, &DfUiServer::updateVolumeSlot);

    connect(thread, &DfUiCommThread::noFlowAbortSignal, this, &DfUiServer::noFlowAbortSlot);
    connect(thread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::targetHitSlot);

    // connect(thread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::noFlowAbortSlot);
    // connect(thread, &DfUiCommThread::noFlowAbortSignal, this, &DfUiServer::targetHitSlot);

    connect(thread, &DfUiCommThread::initReadySignal, this, &DfUiServer::initReadySlot);
    connect(thread, &DfUiCommThread::printerStatusSignal, this, &DfUiServer::printerStatusSlot);
    connect(thread, &DfUiCommThread::MMSignal, this, &DfUiServer::MMSlot);
    connect(thread, &DfUiCommThread::dispenseButtonPressedSignal, this, &DfUiServer::dispenseButtonPressedSlot);

    thread->start();
    busyHandlingRequest = false;
}
