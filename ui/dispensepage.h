//***************************************
//
// dispensepage.h
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

#ifndef DISPENSEPAGE_H
#define DISPENSEPAGE_H

#include "df_util.h"

class payPage;
class thankYouPage;
class idle;

namespace Ui {
class dispensePage;
}

class dispensePage : public QWidget
{
    Q_OBJECT

public:
    // **** GUI ****
    explicit dispensePage(QWidget *parent = nullptr);
    void setPage(payPage* pagePayment, thankYouPage* pageThankYou, idle* pageIdle);
    ~dispensePage();

private slots:
    // **** Navigation ****
    void on_finish_Button_clicked();

    // FSM communication
    void send_to_FSM();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    // **** GUI *****
    Ui::dispensePage *ui;
    payPage* paymentPage;
    thankYouPage* thanksPage;
    idle* idlePage;

    // FSM communication
    bool is_sending_to_FSM;
    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
    // TODO: move host and port to XML
    const char* host = "localhost";
    int port = 1234;

protected:
    void showEvent(QShowEvent *event);
};

#endif // DISPENSEPAGE_H
