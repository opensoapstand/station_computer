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
{

//    dfUtil->is_sending_to_FSM = false;
    ui->setupUi(this);
    QPixmap background(":/light/6_dispense_page.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);

    /*hacky transparent button*/
    ui->finish_Button->setStyleSheet("QPushButton { border-image: url(:/light/background.png); }");

    /* Networking */




    //    m_fsmMsg = df_util::FSM_COMM::SEND_EMPTY;

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
    this->idlePage->dfUtility->msg = QString::number(this->idlePage->userDrinkOrder->getOption());

//    idlePage->dfUtility->send_to_FSM(msg);

//    if(idlePage->dfUtility->m_IsSendingFSM) {
//        return;
//    }
    idlePage->dfUtility->m_IsSendingFSM = true;
    QWidget::showEvent(event);
    idlePage->dfUtility->m_fsmMsg = SEND_DRINK;
    idlePage->dfUtility->send_to_FSM();
//    send_to_FSM();

    idlePage->dfUtility->m_IsSendingFSM = false;
//    is_sending_to_FSM = false;
    ui->finish_Button->setEnabled(true);
}


/*
 * Page Tracking reference to Payment page and completed payment
 */
void dispensePage::on_finish_Button_clicked()
{
    qDebug() << "finish button clicked" << endl;
    // TODO: Link to FSM for Dispense
    idlePage->dfUtility->m_IsSendingFSM = true;
//    is_sending_to_FSM = true;
    idlePage->dfUtility->m_fsmMsg = SEND_CLEAN;


    this->idlePage->dfUtility->msg = QString::number(this->idlePage->userDrinkOrder->getOption());

    // Send a Cleanse and TODO: helps FSM onExit...
    idlePage->dfUtility->send_to_FSM();

//    while(is_sending_to_FSM) {
//        qDebug() << "CLEAN MODE" << endl;
//    }
//    is_sending_to_FSM = false;
//    tcpSocket->disconnectFromHost();
    this->hide();
    thanksPage->showFullScreen();
}



