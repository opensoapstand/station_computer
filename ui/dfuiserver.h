#ifndef DFUISERVER_H
#define DFUISERVER_H

#include <QTcpServer>

class DfUiServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit DfUiServer(QObject *parent = 0);
    void startServer();
    void resetTimerSlot();

signals:
    void pleaseReset();

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // DFUISERVER_H
