#include "df_util.h"
#include "includefiles.h"
#include <sys/time.h>
#include <stdio.h>

char df_util::sizeIndexToChar(int size_index)
{
    char size_to_char[SIZES_COUNT] = {'!', 's', 'm', 'l', 'c', 't'}; // last c "test mode should become a t"
    // char size_to_char[SIZES_COUNT] = {'!', 's', 'm', 'l', 'c', 't'}; // last c "test mode should become a t"
    return size_to_char[size_index];
}

double df_util::convertMlToOz(double vol_ml)
{
    return vol_ml * ML_TO_OZ;
}

double df_util::convertOzToMl(double vol_oz)
{
    return vol_oz / ML_TO_OZ;
}

QString df_util::getConvertedStringVolumeFromMl(double volumeMilliLiter, QString units)
{
    QString volume_as_string;
    // switch (units){
    //     case "l":
    //     {
    //         volume_as_string = QString::number(v / 1000, 'f', 2) + "L";

    //     }
    //     break;
    //     case "ml":
    //     {
    //         volume_as_string = QString::number(v, 'f', 0) + "ml";

    //     }
    //     break;
    //     case "oz":
    //     {
    //         double volume_oz = ceil(volumeMilliLiter * ML_TO_OZ);
    //         volume_as_string = QString::number(volume_oz, 'f', 0) + "oz";

    //     }
    //     break;
    //     default:
    //     {
    //         qDebug() << "Unhandled unit system: " << units;
    //         volume_as_string = QString::number(v, 'f', 0) + "ml";
    //     }
    //     // qDebug() << "vol: " << volume_as_string << " .. units: " << units << " vol metric: " << v << "vol oz: " << volume_oz;
    // };

    double volume_oz = ceil(convertMlToOz(volumeMilliLiter));

    if (units == "l" || units == "ml")
    {

        if (volumeMilliLiter < 1000)
        {
            volume_as_string = QString::number(volumeMilliLiter, 'f', 0) + "ml";
        }
        else
        {
            volume_as_string = QString::number(volumeMilliLiter / 1000, 'f', 2) + "L";
        }
    }
    else if (units == "oz")
    {
        volume_as_string = QString::number(volume_oz, 'f', 0) + "oz";
    }
    else
    {
        qDebug() << "Unhandled unit system: " << units;
        volume_as_string = QString::number(volumeMilliLiter, 'f', 0) + "ml";
    }

    qDebug() << "vol: " << volume_as_string << " .. units: " << units << " vol metric: " << volumeMilliLiter << "vol oz: " << volume_oz;
    return volume_as_string;
}

df_util::df_util(QWidget *parent) : QWidget(parent),
                                    tcpSocket(new QTcpSocket(this))
{
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    connect(tcpSocket, &QIODevice::readyRead, this, &df_util::send_to_FSM);
}

/*
 * TODO: Decouple send_to_FSM from Dispenser to here
 */

void df_util::set_message_to_send_to_FSM(QString msg)
{
    this->send_msg = msg;
}

void df_util::send_to_FSM()
{

    // }
    // void df_util::send_to_FSM()
    // {
    // QString msg = "fneinsef";
    tcpSocket->abort();

    tcpSocket->connectToHost(host, port);

    if (tcpSocket->waitForConnected(3000))
    {
        tcpSocket->flush();
    }
    else
    {
        qDebug() << "ERROR: Failed Connection (Port ok? or Restarting the computer has worked to solve the issue in the past)" << endl;
    }

    // switch (m_fsmMsg) {
    // case SEND_DISPENSE_START:
    //     msg.append("d");
    //     msg.append(";");
    //     break;

    // case SEND_DISPENSE_STOP:
    //     msg.append("f");
    //     msg.append(";");
    //     break;

    // case SEND_PWM:
    //     msg.append(";");

    // default:
    //     msg.append("e");
    //     msg.append(";");
    //     break;
    // }
    send_msg.append(";");

    QByteArray block;
    qDebug() << "send message to FSM: " << send_msg;

    block.append(send_msg);
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    tcpSocket->write(block);
    tcpSocket->flush();
    tcpSocket->close();
}

// //https://stackoverflow.com/questions/22455357/milliseconds-since-epoch-to-dateformat-c

// static long getTimeStamp() {
//     struct timeval tp;
//     gettimeofday(&tp, NULL);
//     long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
//     return ms;
// }

// static string format_string(long time_stamp,string fmt,int cutBack=0){
//     // example
//     //std::cout << CommonUtils::format_string(CommonUtils::getTimeStamp(), "%Y-%m-%dT%H:%M:%S.%ms%Z")<<endl;

//     time_t tt = time_stamp/1000;
//     int microsec = time_stamp%1000;
//     struct std::tm * ptm = std::localtime(&tt);
//     string fmtms=std::regex_replace(fmt, std::regex("%ms"), to_string(microsec));
//     std::stringstream ss;
//     ss << std::put_time(ptm, fmtms.c_str());
//     string ret = ss.str();
//     return ret.substr(0,ret.size()-cutBack);
// }

void df_util::displayError(QAbstractSocket::SocketError socketError)
{
}

// void df_util::initialize_local_db() {
// }

// bool df_util::open_local_db()
// {
// }

// bool df_util::close_local_db()
// {
// }

// bool df_util::getVendorDetails()
// {
// }

// QString df_util::get_local_db_max_transaction()
// {
// }
