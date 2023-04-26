#ifndef DFUISERVER_H
#define DFUISERVER_H

#include <QTcpServer>

    
class DfUiServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit DfUiServer(QObject *parent = 0);
    void startServer();
    void closeServer();

    void resetTimerSlot();
    void transactionEndSlot();
    void updateVolumeSlot(double dispensed);
    void printerStatusSlot(bool isOnline, bool hasPaper);
    void targetHitSlot();
    void dispenseButtonPressedPosEdgeSlot();
    void dispenseButtonPressedNegEdgeSlot();
    void noFlowAbortSlot();
    void initReadySlot();
    // void MMSlot();
    void messageHandlerFinishedSlot();

    bool busyHandlingRequest;

signals:
    void printerStatus(bool isOnline, bool hasPaper);
    void controllerFinishedAck();
    void pleaseReset();
    void signalUpdateVolume(double dispensed, bool isFull);
    void targetHit();
    void dispenseButtonPressedPosEdgeSignal();
    void dispenseButtonPressedNegEdgeSignal();
    void noFlowAbort();
    void initReady();
    void MM();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    

};

#endif // DFUISERVER_H
