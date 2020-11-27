#ifndef DFUICOMMTHREAD_H
#define DFUICOMMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

#include "dispensepage.h"
#include "df_util.h"

class dispensePage;

class DfUiCommThread : public QThread
{
    Q_OBJECT
public:
    explicit DfUiCommThread(qintptr ID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void resetTimerSignal();

public slots:
    QByteArray readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    dispensePage* dispensepage;
};

#endif // DFUICOMMTHREAD_H
