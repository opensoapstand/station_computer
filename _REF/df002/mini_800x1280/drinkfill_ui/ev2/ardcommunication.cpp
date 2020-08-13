#include "ardcommunication.h"

#include <QDebug>

#define  ARDU_PORT "/dev/arduino"
#define  ARDU_READ "gpio read %c\r"
#define  ARDU_SET  "gpio set %c\r"
#define  ARDU_CLEAR "gpio clear %c\r"
#define  ARDU_PAUSE 5000
#define  MAX_SIZE 1024

QByteArray tempValue;
ardCommunication::ardCommunication()
{}

bool ardCommunication::init()
{
    bool bRet = false;
    {

        //arduDevice = -1;
        //QSerialPort serial;
        serial.setPortName(ARDU_PORT);

        serial.open(QIODevice::ReadWrite);
        serial.setBaudRate(QSerialPort::Baud9600);
        serial.setParity(QSerialPort::NoParity);
        serial.setDataBits(QSerialPort::Data8);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);
        if(serial.isOpen() && serial.isWritable())
        {
            qDebug() << "Serial (ard) Status: "<< "Success";
            bRet = true;

        } else {
            qDebug() << "Serial (ard) Status: " << "Fail";
            bRet = false;
            serial.close();
            serial.open(QIODevice::ReadWrite);
        }
        //serial.write("a");
        //qDebug() << serial.readAll();
    }
    return bRet;
}

void ardCommunication::writePacket(QByteArray writeChar)
{

    serial.write(writeChar);
}

bool ardCommunication::checkConnected()
{
    qDebug()<< serial.isOpen();
    serial.clear();
    return true;
}

void ardCommunication::clearVolume()
{
    serial.clear();
}

QByteArray ardCommunication::readPacket(int sizeByte)
{
    bool readB = true;
    QByteArray readByte;

    if(serial.bytesAvailable() >= sizeByte)
    {
        readByte = serial.readLine();
        readB = false;

    }

    //serial.flush();
    if (readByte.mid(0,readByte.size()-2)==""){
        readByte=tempValue;
    } else{
        tempValue = readByte;
    }

    return readByte.mid(0,readByte.size()-2);
}
