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
    qDebug() << "Sending to FSM" << endl;
    // TODO: Local socket to FSM
//    ui->finish_Button->setEnabled(false);
    // Message Validation
//    QRegExp re("\\d*");
//    if(re.exactMatch(sendMsg)) {
//        int checkOption = sendMsg.toInt();
//        if(checkOption >= 0 && checkOption <= 8) {
//            msg.append(sendMsg);
//        }
//    }

    tcpSocket->abort();

    tcpSocket->connectToHost(host,port);

    if(tcpSocket->waitForConnected(3000)){
          tcpSocket->flush();
//        tcpSocket->write("HELLO");
//        tcpSocket->waitForBytesWritten(1000);
//        tcpSocket->waitForReadyRead(3000);
//        qDebug() << "Reading: " << tcpSocket->bytesAvailable();
    } else {
        qDebug() << "Failed Connection" << endl;
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

    default:
        msg.append("e");
        msg.append(";");
        break;
    }

    qDebug() << msg << endl;

    QByteArray block;
    block.append(msg);
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    qDebug() << "Writable Socket? " << tcpSocket->isWritable();

    tcpSocket->write(block);
    tcpSocket->flush();
    tcpSocket->close();
}

void df_util::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
//        QMessageBox::information(this, tr("DF Client"),
//                                 tr("The host was not found. Please check the "
//                                    "host name and port settings."));
        qDebug() << "DF Client: The host was not found. Please check the host name and port settings." << endl;
        break;
    case QAbstractSocket::ConnectionRefusedError:
//        QMessageBox::information(this, tr("DF Client"),
//                                 tr("The connection was refused by the peer. "
//                                    "Make sure the fortune server is running, "
//                                    "and check that the host name and port "
//                                    "settings are correct."));
        qDebug() << "DF Client: The connection was refused by the peer. Make sure the fortune server is running, and check that the host name and port settings are correct." << endl;
        break;
    default:
//        QMessageBox::information(this, tr("DF Client"),
//                                 tr("The following error occurred: %1.")
//                                 .arg(tcpSocket->errorString()));
        qDebug() << "DF Client: Error..." << endl;
    }

//    ui->finish_Button->setEnabled(true);
}


void df_util::initialize_local_db() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(local_db_path + "drinkfill-sqlite.db");

}

bool df_util::open_local_db()
{
    initialize_local_db();
    if(!db.open())
    {
        qDebug() << "Can't Connect to DB!";
    }
    else
    {
        qDebug() << "Connected Successfully to" + db.databaseName();
    }
    return db.open();
}


bool df_util::close_local_db()
{
   if(db.open()) {
        db.close();
   } else {
        qDebug() << "No Database open!" << endl;
   }

   return db.open();
}

bool df_util::getVendorDetails()
{
    bool isResultAvailable = open_local_db();

    if(isResultAvailable)
    {
        QSqlQuery query;
        query.prepare("SELECT name, full_address "
                      "FROM vendor WHERE vendor_id = 1;");
        if(!(isResultAvailable = query.exec()))
        {
            qDebug() << "Can't Execute Query !";
        }
        else
        {
            qDebug() << "Query Executed Successfully !";
            while(query.next())
            {
                qDebug() << "Vendor Name : " << query.value(0).toString();
                qDebug() << "Vendor Full Address: " << query.value(1).toString();
            }
        }
    }
    close_local_db();
    return isResultAvailable;
}

QString df_util::get_local_db_max_transaction()
{
    QString result = "Db Error";

    QString path = "/home/df-admin/Project/drinkfill/sqlite/";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "drinkfill-sqlite.db");

    if(!db.open())
    {
        qDebug() << "Can't Connect to DB !";
    }
    else
    {
        qDebug() << "Connected Successfully to" + db.databaseName();
        QSqlQuery query;
        query.prepare("SELECT MAX(inventory_id) FROM inventory;");
        if(!query.exec())
        {
            qDebug() << "Can't Execute Query !";
        }
        else
        {
            qDebug() << "Query Executed Successfully !";
            result = query.value(0).toString();
        }
    }

    return result;
}
