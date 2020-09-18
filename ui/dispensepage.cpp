//***************************************
//
// dispensepage.cpp
// GUI class to dispense drink.
// Coordinates User input to payment class
// then communcates results to FSM IP_thread
//
// Allows navigation to paymentpage and
// thankyou page
//
// created: 16-07-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "dispensepage.h"
#include "ui_dispensepage.h"

#include "idle.h"

#include "thankyoupage.h"

// CTOR
dispensePage::dispensePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dispensePage)
  , tcpSocket(new QTcpSocket(this))
{
    is_sending_to_FSM = false;
    ui->setupUi(this);
    QPixmap background(":/light/6_dispense_page.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    /* Networking */
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

//    m_fsmMsg = df_util::FSM_COMM::SEND_EMPTY;

    connect(tcpSocket, &QIODevice::readyRead, this, &dispensePage::send_to_FSM);

    //    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
    //    this, SLOT(&dispensePage::displayError(QAbstractSocket::SocketError)));
    //    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError), SLOT()));
    //    connect(tcpSocket, &QAbstractSocket::error, this, &dispensePage::displayError);
}

/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::setPage(payPage *pagePayment, thankYouPage* pageThankYou, idle* pageIdle)
{
    this->thanksPage = pageThankYou;
    this->paymentPage = pagePayment;
    this->idlePage = pageIdle;
}

// DTOR
dispensePage::~dispensePage()
{
    delete ui;
}

void dispensePage::showEvent(QShowEvent *event)
{
    if(is_sending_to_FSM) {
        return;
    }
    is_sending_to_FSM = true;
    QWidget::showEvent(event);
    m_fsmMsg = SEND_DRINK;
    send_to_FSM();
    is_sending_to_FSM = false;
    ui->finish_Button->setEnabled(true);
}


/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::on_finish_Button_clicked()
{
    qDebug() << "finish button clicked" << endl;
    // TODO: Link to FSM for Dispense
    is_sending_to_FSM = true;
    m_fsmMsg = SEND_CLEAN;

    // Send a Cleanse and TODO: helps FSM onExit...
    send_to_FSM();

//    while(is_sending_to_FSM) {
//        qDebug() << "CLEAN MODE" << endl;
//    }
//    is_sending_to_FSM = false;
//    tcpSocket->disconnectFromHost();
    this->hide();
    thanksPage->showFullScreen();
}


/*
 *
 */
void dispensePage::send_to_FSM()
{
    qDebug() << "Sending to FSM" << endl;
    // TODO: Local socket to FSM
    ui->finish_Button->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost(host,port);

    QString msg;
    msg = QString::number(this->idlePage->userDrinkOrder->getOption());

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
    out.setVersion(QDataStream::Qt_4_1);
    tcpSocket->write(block);
    tcpSocket->close();
}

void dispensePage::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("DF Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("DF Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("DF Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    ui->finish_Button->setEnabled(true);
}
