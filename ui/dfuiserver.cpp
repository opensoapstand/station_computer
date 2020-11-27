#include "dfuiserver.h"
#include "dfuicommthread.h"

DfUiServer::DfUiServer(QObject *parent) :
    QTcpServer(parent)
{
}

void DfUiServer::startServer()
{
    int port = 1235;

    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

// This function is called by QTcpServer when a new connection is available.
void DfUiServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    DfUiCommThread *thread = new DfUiCommThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later

    if (thread->isDispense == true){
        qDebug() << "setting isdispense true" << endl;
        isDispense = true;
    }
    if (thread->isDispense == false){
        qDebug() << "setting isdispense FLASE" << endl;
        isDispense = false;
    }

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
