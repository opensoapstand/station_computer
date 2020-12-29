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
    void updateVolumeSlot(int dispensed);
    void targetHitSlot();

signals:
    void pleaseReset();
    void updateVolume(int dispensed);
    void targetHit();

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // DFUISERVER_H
