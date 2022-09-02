#ifndef DFUICOMMTHREAD_H
#define DFUICOMMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

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
    void printerStatusSignal(bool isOnline, bool hasPaper);
    void initReadySignal(void);
    void MMSignal(void);
    void dispenseButtonPressedSignal(void);

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
