#include "dfuiserver.h"
#include "dfuicommthread.h"
#include "df_util.h" // lode added for settings
DfUiServer::DfUiServer(QObject *parent) :
    QTcpServer(parent)
{
}

void DfUiServer::startServer()
{
    int port = 1235;

    if(!this->listen(QHostAddress::Any, port))
    {
//        qDebug() << "Could not start server";
    }
    else
    {
//        qDebug() << "Listening to port " << port << "...";
    }
}

void DfUiServer::resetTimerSlot(){
    emit pleaseReset();
}

void DfUiServer::updateVolumeSlot(double dispensed){
    emit updateVolume(dispensed);
}

void DfUiServer::targetHitSlot(){
    emit targetHit();
}

void DfUiServer::initReadySlot(){
    emit initReady();
}

void DfUiServer::MMSlot(){
    emit MM();
}

// This function is called by QTcpServer when a new connection is available.
void DfUiServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
//    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    DfUiCommThread *thread = new DfUiCommThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, &DfUiCommThread::resetTimerSignal, this, &DfUiServer::resetTimerSlot);
    connect(thread, &DfUiCommThread::updateVolumeSignal, this, &DfUiServer::updateVolumeSlot);
    connect(thread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::targetHitSlot);
    connect(thread, &DfUiCommThread::initReadySignal, this, &DfUiServer::initReadySlot);
    connect(thread, &DfUiCommThread::MMSignal, this, &DfUiServer::MMSlot);

    thread->start();
}


