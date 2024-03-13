#ifndef DFUICOMMTHREAD_H
#define DFUICOMMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QStringList>


#include "df_util.h"

class page_dispenser;

class DfUiCommThread : public QThread
{
    Q_OBJECT
public:
    explicit DfUiCommThread(qintptr ID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void transactionEndSignal(void);
    void resetTimerSignal(void);
    void targetHitSignal(void);
    void noFlowAbortSignal(void);
    void updateVolumeSignal(double dispensed);
    // void updateFinalVolumeDispensedSignal(double dispensed);
    void finalTransactionMessageSignal(QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed, QString volumeDispensedMixProduct);
    void dispenseRateSignal(double flowrate);
    void dispenseStatusSignal(QString status);
    void printerStatusSignal(bool isOnline, bool hasPaper);
    void initReadySignal(void);
    void temperatureSignal(double temperature_1, double temperature_2);
    // void temperature2Signal(double temperature2);
    // void MMSignal(void);
    void dispenseButtonPressedPosEdgeSignal(void);
    void dispenseButtonPressedNegEdgeSignal(void);
    void messageHandlerFinishedSignal(void);

public slots:
    QByteArray readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    char * pEnd;
    size_t sz;
};

#endif // DFUICOMMTHREAD_H
