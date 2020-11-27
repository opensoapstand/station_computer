#ifndef DFUICOMMTHREAD_H
#define DFUICOMMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

#include "df_util.h"
#include "dispensepage.h"

class DfUiCommThread : public QThread
{
    Q_OBJECT
public:
    explicit DfUiCommThread(qintptr ID, QObject *parent = 0);
    void run();
    void timerFunc();
    bool isDispense;

signals:
    void error(QTcpSocket::SocketError socketerror);
    void resetTimerSignal();

public slots:
    QByteArray readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    class dispensePage* dispensepage;
};

#endif // DFUICOMMTHREAD_H
