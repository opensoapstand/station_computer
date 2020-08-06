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

namespace Ui {
class dispensePage;
}

class dispensePage : public QWidget
{
    Q_OBJECT

public:
    explicit dispensePage(QWidget *parent = nullptr);
    void setPage(payPage* pagePayment, thankYouPage* pageThankYou);
    ~dispensePage();

private slots:
    void on_finish_Button_clicked();
    void send_to_FSM();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    Ui::dispensePage *ui;

    payPage* paymentPage;
    thankYouPage* thanksPage;

//    QComboBox *host = "localhost";
//    QLineEdit *port = "1234";
    const char* host = "localhost";
    int port = 1234;
    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
};

#endif // DISPENSEPAGE_H
