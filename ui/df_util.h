#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "dfuiserver.h"

// TODO: Refactor to fit with dfuicommthread
#define USE_OLD_DATABASE
//#define START_FSM_FROM_UI //enabled by default (start controller from ui)

#ifndef START_FSM_FROM_UI
//#define WAIT_FOR_CONTROLLER_READY // will wait for response of controller before continuing.
#endif

#define DB_PATH "/home/df-admin/drinkfill/production/db/drinkfill-sqlite.db"
// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
//#define DB_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db"

#define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
#define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

#define ML_TO_OZ 0.033814

using namespace std;

typedef enum FSM_COMM {
    SEND_EMPTY = 0,
    SEND_DISPENSE_START,
    SEND_DISPENSE_STOP, // used to be SEND_CLEAN
    SEND_PWM,
    SEND_ERROR,
} FSM_COMM;

class df_util : public QWidget
{
    Q_OBJECT
public:
    explicit df_util(QWidget *parent = nullptr);
    
    // static long getTimeStamp();
    // static string format_string(long time_stamp,string fmt,int cutBack=0);

    void initialize_local_db();
    bool open_local_db();
    bool close_local_db();
    bool getVendorDetails();
    QString get_local_db_max_transaction();

//    void setIsSendingFSM(bool isSendingFSM){m_IsSendingFSM = isSendingFSM;};
//    bool getIsSendingFSM(){return m_IsSendingFSM;};

    bool m_IsSendingFSM;
    FSM_COMM m_fsmMsg; // Sets type of message/command

    QString msg;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;


protected:
    // FSM communication

    // TODO: move host and port to XML
    const char* host = "localhost";
    int port = 1234;

    QSqlDatabase db;

public slots:
    void send_to_FSM();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    //QString local_db_path = "/home/df-admin/Project/drinkfill/db/sqlite/";
    QString local_db_name = "drinkfill-sqlite.db";

    QString remote_psql_db_path;
    QString remote_psql_db_name;
};

#endif // DF_UTIL_H
