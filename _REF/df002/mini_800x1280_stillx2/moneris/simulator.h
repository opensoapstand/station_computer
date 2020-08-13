#ifndef simulator_H
#define simulator_H

#include <QWidget>
#include <vector>
#include <QString>
#include <string>

#include "packetfromecr.h"
#include "packetfromux410.h"
#include "moneriscommunication.h"
#include "transactionPackets.h"
#include "transactioninfo.h"
#include <QTimer>


namespace Ui {
class simulator;
}

class simulator : public QWidget
{
    Q_OBJECT

public:
    explicit simulator(QWidget *parent = nullptr);
    ~simulator();

private slots:
    void on_logonButton_clicked();

    void on_BalanceButton_clicked();

    void on_CloseButton_clicked();

    void on_purchaseButton_clicked();

    void on_cancelButton_clicked();

    void on_sendButton_clicked();

    void readtimer_loop();

    void on_configButton_clicked();


    void on_nameButton_clicked();

    void on_addressButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::simulator *ui;
    monerisCommunication com;
    bool timerEnabled;

    packetFromECR ecr;
    packetFromUX410 ux410;

    transactionPacket pkt;
    QTimer *readtimer;
    std::vector<uint8_t> pktToSend;
    std::vector<uint8_t> packetRecievedTest;

    transactionInfo dataPacket;
    bool purchaseEnable;
    int monerisConfig = 0;
};

#endif // simulator_H
