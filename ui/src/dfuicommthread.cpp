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

    // socket->write(Data); // THIS CAUSES THE UI TO CRASH AT TIMES.... for now, we delete it. todo. send ack to controller.

    if (Data == "Transaction End")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit transactionEndSignal();
    }

    else if (Data == "Reset Timer")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit resetTimerSignal();
    }
    else if (Data == "No flow abort")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit noFlowAbortSignal();
    }
    else if (Data == "Target Hit")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit targetHitSignal();
    }

    else if (Data == "Init Ready")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit initReadySignal();
    }
    else if (Data == "OK")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        // emit initReadySignal();
        usleep(3000000);
    }
    else if (Data == "Dispense Button Pos Edge")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit dispenseButtonPressedPosEdgeSignal();
    }
    else if (Data == "Dispense Button Neg Edge")
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        emit dispenseButtonPressedNegEdgeSignal();
    }
    else if (Data.startsWith("temperatu"))
    // else if (Data.startsWith("temperature|"))
    {
        int first_delim_pos = Data.indexOf('|');
        int second_delim_pos = Data.indexOf('|', first_delim_pos + 1);

        QByteArray first_part = Data.mid(0, first_delim_pos);
        QByteArray second_part = Data.mid(first_delim_pos + 1, second_delim_pos - first_delim_pos - 1);
        QByteArray third_part = Data.mid(second_delim_pos + 1);
        double temperature_1 = second_part.toDouble();
        double temperature_2 = third_part.toDouble();

        emit temperatureSignal(temperature_1, temperature_2);
    }
    else if (Data.contains("printerstatus"))
    {
        bool isOnline = Data.at(13) == '1';
        bool hasPaper = Data.at(14) == '1';
        emit printerStatusSignal(isOnline, hasPaper);
    }
    else if (Data.contains("dispenseupdate|"))
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        int first_delim_pos = Data.indexOf('|');
        int second_delim_pos = Data.indexOf('|', first_delim_pos + 1);
        int third_delim_pos = Data.indexOf('|', second_delim_pos + 1);

        QByteArray first_part = Data.mid(0, first_delim_pos);
        QByteArray second_part = Data.mid(first_delim_pos + 1, second_delim_pos - first_delim_pos - 1);
        QByteArray third_part = Data.mid(second_delim_pos + 1, third_delim_pos - second_delim_pos - 1);
        QByteArray fourth_part = Data.mid(third_delim_pos + 1);

        double volumeDispensed = second_part.toDouble();
        double flowrate = third_part.toDouble();
        QString dispenseStatusString = QString::fromUtf8(fourth_part);

        // qDebug() << "volume: " << QString::number(volumeDispensed, 'f', 2);
        // qDebug() << "flowrate: " << QString::number(flowrate, 'f', 2);
        // qDebug() << "dispenseStatus: " << dispenseStatusString;
        emit updateVolumeSignal(volumeDispensed); // induced crash at cancel dispense.
        emit dispenseRateSignal(flowrate);
        emit dispenseStatusSignal(dispenseStatusString);
    }
    // else if (Data.contains("finalVolumeDispensed|"))
    // {
    //     qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
    //     int first_delim_pos = Data.indexOf('|');
    //     int second_delim_pos = Data.indexOf('|', first_delim_pos + 1);

    //     QByteArray first_part = Data.mid(0, first_delim_pos);
    //     QByteArray second_part = Data.mid(first_delim_pos + 1, second_delim_pos - first_delim_pos - 1);

    //     double volumeDispensed = second_part.toDouble();

    //     emit updateFinalVolumeDispensedSignal(volumeDispensed);
    // }
    else if (Data.contains("finalTransactionMessage|"))
    {
        qDebug() << "Process received message : " << Data; // not every message gets a debug acknowledgement.
        QStringList dataObjectList = QString(Data).split('|');
        QString start_time = dataObjectList[2]; 
        QString end_time = dataObjectList[4];
        double button_press_duration = dataObjectList[6].toDouble();
        double button_press_count = dataObjectList[8].toDouble();
        double volume_dispensed = dataObjectList[10].toDouble();
        QString volumeDispensedMixProduct = dataObjectList[12];
        emit finalTransactionMessageSignal(start_time, end_time,button_press_duration, button_press_count,volume_dispensed,volumeDispensedMixProduct);
    }
    else
    {
        qDebug() << "Non actionable message from fsm received: " << Data;
    }
    emit messageHandlerFinishedSignal();
    return Data;
}

void DfUiCommThread::disconnected()
{
    //    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}

