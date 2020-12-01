#ifndef DFUISERVER_H
#define DFUISERVER_H

#include <QTcpServer>

#include "dispensepage.h"

class dispensePage;

class DfUiServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit DfUiServer(QObject *parent = 0);
    void startServer();
    void resetTimerSlot();

signals:
    void pleaseReset();

public slots:

private:
    dispensePage* dispensingPage;

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // DFUISERVER_H
