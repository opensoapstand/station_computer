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
        // qDebug() << "Server: Listening. Ready for fsm messages on port " << port << "...";
    }
}

void DfUiServer::closeServer()
{
    this->close();
    // qDebug() << "Server: Stopped listening. Will not receive new fsm messages. ";
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

// void DfUiServer::updateFinalVolumeDispensed(double dispensed)
// {
//     emit finalVolumeDispensed(dispensed);
// }

void DfUiServer::sendFinalTransactionMessage(QString status, QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed, double volume_remaining, QString volumeDispensedMixProduct){
    emit finalTransactionMessage(status, start_time,end_time,button_press_duration,button_press_count, volume_dispensed, volume_remaining, volumeDispensedMixProduct);
}

void DfUiServer::dispenseStatusSlot(QString status)
{
    emit signalDispenseStatus(status);
}
void DfUiServer::dispenseRateSlot(double flowrate)
{
    emit signalDispenseRate(flowrate);
}

void DfUiServer::targetHitSlot()
{
    emit targetHit();
}
void DfUiServer::noFlowAbortSlot()
{
    emit noFlowAbort();
}


void DfUiServer::receiveTemperatureFromFsm(double temperature_1, double temperature_2)
{
  emit temperatureSignal(temperature_1, temperature_2);
    // emit temperatureSignal(temperature_1);
    // emit temperatureSignal(temperature_2);

}
// void DfUiServer::receiveTemperature2FromFsm(double temperature2)
// {
//     emit temperature2Signal(temperature2);
// }


void DfUiServer::controllerReadySlot()
{
    emit initReady();
}
void DfUiServer::printerStatusSlot(bool isOnline, bool hasPaper)
{
    emit printerStatus(isOnline, hasPaper);
}
void DfUiServer::dispenseButtonPressedPosEdgeSlot()
{
    emit dispenseButtonPressedPosEdgeSignal();
}
void DfUiServer::dispenseButtonPressedNegEdgeSlot()
{
    emit dispenseButtonPressedNegEdgeSignal();
}
void DfUiServer::messageHandlerFinishedSlot()
{
    //qDebug() << "Message fully handled. Ready to receive new commands.";
    busyHandlingRequest = false;
    // this->resumeAccepting();

    startServer(); // task
}

// This function is called by QTcpServer when a new connection is available.
void DfUiServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    // In this setup, there are too many ways of crashing when multiple requests are handled at the same time, do not allow for it.
    if (busyHandlingRequest)
    {

        qDebug() << "WARNING: Incoming message neglected. UI still handling a previous request. Will not allow parallel requests. Wait till unblocked and try again.....";
        return;
    }
    else
    {
        // qDebug() << "Incoming request accepted.*************";
        busyHandlingRequest = true;
        // this->pauseAccepting(); // controller does not get error when trying to connect


        // this turns out to be the most solid solution. Stop the server after every command. Start up again when command finished. 
        closeServer(); // closes server. controller gets error when trying to connect
        
        // following works well, but controller needs a blocking routine to catch feedback. --> should be in different thread --> concurrency problems.
        // QTcpSocket* socket = new QTcpSocket();
        // socket->setSocketDescriptor(socketDescriptor);
        // socket->write("accepted"); 
        
    }
    // Every new connection will be run in a newly created thread
    DfUiCommThread *messageHandlerThread = new DfUiCommThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(messageHandlerThread, SIGNAL(finished()), messageHandlerThread, SLOT(deleteLater()));
    connect(messageHandlerThread, &DfUiCommThread::transactionEndSignal, this, &DfUiServer::transactionEndSlot);
    connect(messageHandlerThread, &DfUiCommThread::resetTimerSignal, this, &DfUiServer::resetTimerSlot);
    connect(messageHandlerThread, &DfUiCommThread::updateVolumeSignal, this, &DfUiServer::updateVolumeSlot);
    // connect(messageHandlerThread, &DfUiCommThread::updateFinalVolumeDispensedSignal, this, &DfUiServer::updateFinalVolumeDispensed);
    connect(messageHandlerThread, &DfUiCommThread::finalTransactionMessageSignal, this, &DfUiServer::sendFinalTransactionMessage);
    connect(messageHandlerThread, &DfUiCommThread::dispenseRateSignal, this, &DfUiServer::dispenseRateSlot);
    connect(messageHandlerThread, &DfUiCommThread::dispenseStatusSignal, this, &DfUiServer::dispenseStatusSlot);
    connect(messageHandlerThread, &DfUiCommThread::messageHandlerFinishedSignal, this, &DfUiServer::messageHandlerFinishedSlot);

    connect(messageHandlerThread, &DfUiCommThread::noFlowAbortSignal, this, &DfUiServer::noFlowAbortSlot);
    connect(messageHandlerThread, &DfUiCommThread::temperatureSignal, this, &DfUiServer::receiveTemperatureFromFsm);
    // connect(messageHandlerThread, &DfUiCommThread::temperature2Signal, this, &DfUiServer::receiveTemperature2FromFsm);

    connect(messageHandlerThread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::targetHitSlot);

    // connect(messageHandlerThread, &DfUiCommThread::targetHitSignal, this, &DfUiServer::noFlowAbortSlot);
    // connect(messageHandlerThread, &DfUiCommThread::noFlowAbortSignal, this, &DfUiServer::targetHitSlot);

    connect(messageHandlerThread, &DfUiCommThread::initReadySignal, this, &DfUiServer::controllerReadySlot);
    connect(messageHandlerThread, &DfUiCommThread::printerStatusSignal, this, &DfUiServer::printerStatusSlot);
    // connect(messageHandlerThread, &DfUiCommThread::MMSignal, this, &DfUiServer::MMSlot);
    connect(messageHandlerThread, &DfUiCommThread::dispenseButtonPressedPosEdgeSignal, this, &DfUiServer::dispenseButtonPressedPosEdgeSlot);
    connect(messageHandlerThread, &DfUiCommThread::dispenseButtonPressedNegEdgeSignal, this, &DfUiServer::dispenseButtonPressedNegEdgeSlot);

    // send message to controller that acknowledges receipt?!
   

    messageHandlerThread->start(); // reminder: this is non blocking! It starts a thread (which is executed independently) and moves on right away
    
  
}
