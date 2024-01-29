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
    void updateFinalVolumeDispensed(double dispensed);
    void sendFinalTransactionMessage(QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed);
    void dispenseRateSlot(double flowrate);
    void dispenseStatusSlot(QString status);
    void printerStatusSlot(bool isOnline, bool hasPaper);
    void targetHitSlot();
    void dispenseButtonPressedPosEdgeSlot();
    void dispenseButtonPressedNegEdgeSlot();
    void noFlowAbortSlot();
    void receiveTemperatureFromFsm(double temperature_1, double temperature_2);
    // void receiveTemperatureFromFsm(double temperature_1);
    // void receiveTemperature2FromFsm(double temperature2);
    void controllerReadySlot();
    // void MMSlot();
    void messageHandlerFinishedSlot();

    bool busyHandlingRequest;

signals:
    void printerStatus(bool isOnline, bool hasPaper);
    void controllerFinishedAck();
    void pleaseReset();
    void signalUpdateVolume(double dispensed, bool isFull);
    void finalVolumeDispensed(double dispensed);
    void finalTransactionMessage(QString start_time, QString end_time, double button_press_duration, double button_press_count, double volume_dispensed);
    void signalDispenseRate(double flowrate);
    void signalDispenseStatus(QString status);
    void targetHit();
    void dispenseButtonPressedPosEdgeSignal();
    void dispenseButtonPressedNegEdgeSignal();
    void noFlowAbort();
    void temperatureSignal(double temperature_1, double temperature_2);
    // void temperature2Signal(double temperature2);
    void initReady();
    void MM();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    

};

#endif // DFUISERVER_H
