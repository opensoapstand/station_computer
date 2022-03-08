#include "df_util.h"
#include "includefiles.h"
#include <sys/time.h>
#include <stdio.h>

df_util::df_util(QWidget *parent):
    QWidget(parent),
    tcpSocket(new QTcpSocket(this))
{
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    connect(tcpSocket, &QIODevice::readyRead, this, &df_util::send_to_FSM);
}

/*
 * TODO: Decouple send_to_FSM from Dispenser to here
 */

void df_util::send_to_FSM()
{
    tcpSocket->abort();

    tcpSocket->connectToHost(host,port);

    if(tcpSocket->waitForConnected(3000)){
          tcpSocket->flush();
    } else {
        qDebug() << "ERROR: Failed Connection (Restarting the computer has worked to solve the issue in the past)" << endl;
    }

    switch (m_fsmMsg) {
    case SEND_DISPENSE_START:
        msg.append("d");
        msg.append(";");
        break;

    case SEND_DISPENSE_STOP:
        msg.append("f");
        msg.append(";");
        break;

    case SEND_PWM:
        msg.append(";");

    default:
        msg.append("e");
        msg.append(";");
        break;
    }

    QByteArray block;
    qDebug()<<"send message to FSM: " <<  msg << endl;
    
    block.append(msg);
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

void df_util::initialize_local_db() {
}

bool df_util::open_local_db()
{
}

bool df_util::close_local_db()
{
}

bool df_util::getVendorDetails()
{
}

QString df_util::get_local_db_max_transaction()
{
}
