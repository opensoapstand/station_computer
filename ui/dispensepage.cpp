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
    ui->setupUi(this);
    QPixmap background(":/light/6_dispense_page.jpg");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    /* Networking */
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
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


/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::on_finish_Button_clicked()
{
    // TODO: Link to FSM for Dispense
    send_to_FSM();
    tcpSocket->disconnectFromHost();
    ui->finish_Button->setEnabled(true);
    this->hide();
    thanksPage->showFullScreen();
}


/*
 *
 */
void dispensePage::send_to_FSM()
{
    // TODO: Local socket to FSM
    ui->finish_Button->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost(host,port);

    QString msg = "QT Land Hello!";

    QByteArray block;
    block.append(msg);
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_1);
    tcpSocket->write(block);
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
