#include "df_util.h"
#include "includefiles.h"
#include <sys/time.h>
#include <stdio.h>

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QProcess>

static QPointer<QFile> file_out = nullptr;

void df_util::warnIfPathDoesNotExist(QString path)
{
    if (!pathExists(path))
    {
        qDebug() << "WARNING: File not found: " << path;
    }
}

QVector<int> splitQStringToQVectorInt(QString commaSeparatedNoSpaces)
{
    // data is commaSeparatedNoSpaces. e.g. "34,112,442,444" --> {34,112,442,444}
    // qDeleteAll(list);
    // list.clear();
    QVector<int> intslist;
    QStringList stringList = commaSeparatedNoSpaces.split(",");
    foreach (QString num, stringList)
    {
        intslist.append(num.toInt());
    }
    return intslist;
}

QVector<double> splitQStringToQVectorDouble(QString commaSeparatedNoSpaces)
{
    // data is commaSeparatedNoSpaces. e.g. "34,112,442,444" --> {34,112,442,444}
    // qDeleteAll(list);
    // list.clear();
    QVector<double> doubleslist;
    QStringList stringList = commaSeparatedNoSpaces.split(",");
    foreach (QString num, stringList)
    {
        doubleslist.append(num.toFloat());
    }
    return doubleslist;
}

void df_util::csvQStringToQVectorInt(const QString &csvList, QVector<int> &returnIntList)
{
    // call as csvQStringToQVectorInt(myQString, myQVector)
    returnIntList.clear();

    // if empty csv string, return empty list.
    if (csvList.isEmpty())
    {
        return ;
    }

    QStringList stringList = csvList.split(",");
    foreach (QString num, stringList)
    {
        returnIntList.append(num.toInt());
    }
}

void df_util::csvQStringToQVectorDouble(const QString &csvList, QVector<double> &returnDoubleList)
{
    // call as csvQStringToQVectorInt(myQString, myQVector)
    returnDoubleList.clear();
     // if empty csv string, return empty list.
    if (csvList.isEmpty())
    {
        return;
    }
    QStringList stringList = csvList.split(",");
    foreach (QString num, stringList)
    {
        returnDoubleList.append(num.toDouble());
    }
}

bool df_util::pathExists(QString path)
{
    // check if path exists and if yes: Is it a file and no directory?
    // https://stackoverflow.com/questions/10273816/how-to-check-whether-file-exists-in-qt-in-c
    return QFileInfo::exists(path) && QFileInfo(path).isFile();
}

QStringList df_util::getFileList(const QString &folderPath)
{
    QDir directory(folderPath);
    QStringList fileList = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

    return fileList;
}

char df_util::sizeIndexToChar(int size_index)
{
    char size_to_char[SIZES_COUNT] = {'!', 's', 'm', 'l', 'c', 't','f'}; // last c "test mode should become a t"
    return size_to_char[size_index];
}

double df_util::convertMlToOz(double vol_ml)
{
    return (double)vol_ml * (double)ML_TO_OZ * 1.0;
}

double df_util::convertOzToMl(double vol_oz)
{
    return vol_oz / ML_TO_OZ;
}

QString df_util::getConvertedStringVolumeFromMl(double volumeMilliLiter, QString units, bool roundNumber, bool addUnits)
{
    QString volume_as_string;

    double volume_oz = convertMlToOz(volumeMilliLiter);

    if (roundNumber)
    {
        volume_oz = ceil(volume_oz);
    }

    if (units == "ml")
    {

        int decimals = 2;
        if (roundNumber)
        {
            decimals = 0;
        }

        if (volumeMilliLiter < 1000 || (!roundNumber))
        {
            QString units_string = "";
            if (addUnits)
            {
                units_string = "ml";
            }
            volume_as_string = QString::number(volumeMilliLiter, 'f', decimals) + units_string;
        }
        else
        {
            QString units_string = "";
            if (addUnits)
            {
                units_string = "L";
            }
            volume_as_string = QString::number(volumeMilliLiter / 1000, 'f', 2) + units_string;
        }
    }
    else if (units == "g")
    {
        int decimals = 2;
        if (roundNumber)
        {
            decimals = 0;
        }
        if (volumeMilliLiter < 1000 || (!roundNumber))
        {
            QString units_string = "";
            if (addUnits)
            {
                units_string = "g";
            }
            volume_as_string = QString::number(volumeMilliLiter, 'f', decimals) + units_string;
        }

        else
        {
            QString units_string = "";
            if (addUnits)
            {
                units_string = "kg";
            }
            volume_as_string = QString::number(volumeMilliLiter / 1000, 'f', 2) + units_string;
        }
    }
    else if (units == "oz")
    {
        QString units_string = "";
        if (addUnits)
        {
            units_string = "oz";
        }
        int decimals = 2;
        if (roundNumber)
        {
            decimals = 0;
        }
        volume_as_string = QString::number(volume_oz, 'f', decimals) + units_string;
    }
    else
    {
        QString units_string = "";
        if (addUnits)
        {
            units_string = "oz";
        }
        qDebug() << "Unhandled unit system: " << units;
        volume_as_string = QString::number(volumeMilliLiter, 'f', 0) + units_string;
    }

    // qDebug() << "vol: " << volume_as_string << " .. units: " << units << " vol metric: " << volumeMilliLiter << "vol oz: " << volume_oz;
    qWarning() << "volume stringggggg" << volume_as_string;
    return volume_as_string;
}

df_util::df_util(QWidget *parent) : QWidget(parent),
                                    tcpSocket(new QTcpSocket(this))
{
    // in.setDevice(tcpSocket);
    // in.setVersion(QDataStream::Qt_4_0);
    // connect(tcpSocket, &QIODevice::readyRead, this, &df_util::send_to_FSM);
}

void df_util::send_command_to_FSM(QString command, bool isLoggingMessage)
{
    m_IsSendingFSM = true;
    // this->send_msg = msg;
    send_to_FSM(command, isLoggingMessage);
    m_IsSendingFSM = false;
    // 
    QThread::msleep(50); // Sleep for 50 milliseconds
}

void df_util::executeVirtualClick(int x, int y)
{
    QString display = ":0";
    QString command = "xdotool click 1";

    // Set DISPLAY environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DISPLAY", display);

    // Run the xdotool command
    QProcess process;
    process.setProcessEnvironment(env);
    process.start(command);

    // Wait for the process to finish (optional)
    process.waitForFinished(-1); // You can specify a timeout in milliseconds if needed

    // Handle the output if necessary
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    // Handle output and error messages as needed
    // qDebug() << "Output:" << output;
    // qDebug() << "Error:" << error;

    // QString display = ":0";
    // QString command = QString("DISPLAY=%1 xdotool mousemove --sync %2 %3 click 1")
    //                      .arg(display)
    //                      .arg(x)
    //                      .arg(y);

    // // Run the xdotool command
    // QProcess::execute(command);

    // qDebug() << "Did the click.";
}

void df_util::write_to_file_timestamped(QString basePath, QString data)
{
    // provide basepath wiht %1 in it for the timestamp to be added.
    QString time_stamp = QDateTime::currentDateTime().toString("_yyyy-MM-dd-hh.mm.ss");
    QString log_file_path = QString(basePath).arg(time_stamp);
}

void df_util::write_to_file(QString path, QString data)
{
    // will append to file if exists.

    QFile file(path);
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&file);
    out << data << "\n";
    file.close();
}

void df_util::send_to_FSM(QString command, bool isLoggingMessage)
{
    tcpSocket->abort();

    tcpSocket->connectToHost(host, port);

    if (tcpSocket->waitForConnected(3000))
    {
        tcpSocket->flush();
    }
    else
    {
        qDebug() << "ERROR: Failed Connection to controller (Port ok? or Restarting the computer has worked to solve the issue in the past)" <<  Qt::endl;;
    }
    QString send_msg = command;
    send_msg.append(";");

    QByteArray block;
    if (isLoggingMessage)
    {
        qDebug() << "send message to FSM: " << send_msg;
    }

    block.append(send_msg);
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    tcpSocket->write(block);
    tcpSocket->flush();
    tcpSocket->close();
}

void df_util::displayError(QAbstractSocket::SocketError socketError)
{
}

QJsonObject df_util::parseJsonString(QString jsonString)
// QJsonObject df_util::parseJsonString(const QString &jsonString)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
    {
        qWarning() << "JSON parse error:" << jsonParseError.errorString();
        return QJsonObject();
    }

    if (jsonDoc.isObject())
    {
        return jsonDoc.object();
    }
    else
    {
        qWarning() << "Invalid JSON object format";
        return QJsonObject();
    }
}
