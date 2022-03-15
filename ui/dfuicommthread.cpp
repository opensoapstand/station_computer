#include "dfuicommthread.h"
#include "df_util.h" // lode added for settings

DfUiCommThread::DfUiCommThread(qintptr ID, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
}

void DfUiCommThread::run()
{
    // thread starts here
    //    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if (!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    //    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

QByteArray DfUiCommThread::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    //    qDebug() << socketDescriptor << " Data in: " << Data;

    if (Data == "!")
    {
        //        qDebug() << "from CLEAN";
    }

    else if (Data == "Reset Timer")
    {
        // emit resetTimerSignal();
    }

    else if (Data == "Target Hit")
    {
        // emit targetHitSignal();
    }

    else if (Data == "Init Ready")
    {
        // emit initReadySignal();
    }

    else if (Data == "MM")
    {
        // emit MMSignal();
    }
    else if (strtol(Data, &pEnd, 10))
    {
        // double volume_dispensed = stod(Data.constData(), &sz);
        // emit updateVolumeSignal(volume_dispensed);
    }
    else
    {
        qDebug() << "No matching command found." << Data << endl;
    }

    qDebug() << "Fsm recv: " << Data << endl;

    // socket->write(Data); // THIS CAUSES THE UI TO CRASH AT TIMES.... for now, we delete it. todo. send ack to controller.

    if (Data == "!")
    {
        //        qDebug() << "from CLEAN";
    }

    else if (Data == "Reset Timer")
    {
        emit resetTimerSignal();
    }

    else if (Data == "Target Hit")
    {
        emit targetHitSignal();
    }

    else if (Data == "Init Ready")
    {
        emit initReadySignal();
    }

    else if (Data == "MM")
    {
        emit MMSignal();
    }
    else if (strtol(Data, &pEnd, 10))
    {
        double volume_dispensed = stod(Data.constData(), &sz);
        emit updateVolumeSignal(volume_dispensed);
    }

    return Data;
}

void DfUiCommThread::disconnected()
{
    //    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}
