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
    void updateVolumeSlot(double dispensed);
    void targetHitSlot();
    void initReadySlot();

signals:
    void pleaseReset();
    void updateVolume(double dispensed);
    void targetHit();
    void initReady();

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // DFUISERVER_H
