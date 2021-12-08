#include "df_util.h"
#include "includefiles.h"

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
//        qDebug() << "Failed Connection" << endl;
    }

    switch (m_fsmMsg) {
    case SEND_DRINK:
        msg.append("d");
        msg.append(";");
        break;

    case SEND_CLEAN:
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
    block.append(msg);
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    tcpSocket->write(block);
    tcpSocket->flush();
    tcpSocket->close();
}

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
