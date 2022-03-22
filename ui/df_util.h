#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "dfuiserver.h"

// TODO: Refactor to fit with dfuicommthread
// #define USE_OLD_DATABASE
//#define START_FSM_FROM_UI //enabled by default (start controller from ui)

// #define GENERIC_PRODUCT_SELECT

#define OPTION_SLOT_INVALID 0
#define SLOT_COUNT 4

#define SIZES_COUNT 6

#define SIZE_INVALID_INDEX 0
#define SIZE_SMALL_INDEX 1
#define SIZE_MEDIUM_INDEX 2
#define SIZE_LARGE_INDEX 3
#define SIZE_CUSTOM_INDEX 4
#define SIZE_TEST_INDEX 5

#ifndef START_FSM_FROM_UI
//#define WAIT_FOR_CONTROLLER_READY // will wait for response of controller before continuing.
#endif

#ifdef USE_OLD_DATABASE
#define DB_PATH "/home/df-admin/production/db/drinkfill-sqlite.db"
#else
#define DB_PATH "/home/df-admin/production/db/drinkfill-sqlite_newlayout.db"
#endif
// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
//#define DB_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db"

#define ENABLE_COUPON   // Petros stations have no coupon

#define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
#define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

#define ML_TO_OZ 0.033814

#define SLOT_COUNT 4
using namespace std;

// typedef enum FSM_COMM {
//     SEND_EMPTY = 0,
//     SEND_DISPENSE_START,
//     SEND_DISPENSE_STOP, // used to be SEND_CLEAN
//     SEND_PWM,
//     SEND_ERROR,
// } FSM_COMM;

#define SEND_DISPENSE_START "d"
#define SEND_DISPENSE_STOP "f"

class df_util : public QWidget
{
    Q_OBJECT
public:
    explicit df_util(QWidget *parent = nullptr);

    static double convertMlToOz(double vol_ml);
    static double convertOzToMl(double vol_oz);
    static char sizeIndexToChar(int size_index);
    static QString getConvertedStringVolumeFromMl(double volumeMilliLiter, QString units, bool roundNumber);
    

    // static long getTimeStamp();
    // static string format_string(long time_stamp,string fmt,int cutBack=0);

    // void initialize_local_db();
    // bool open_local_db();
    // bool close_local_db();
    // bool getVendorDetails();
    // QString get_local_db_max_transaction();
    void set_message_to_send_to_FSM(QString msg);

    //    void setIsSendingFSM(bool isSendingFSM){m_IsSendingFSM = isSendingFSM;};
    //    bool getIsSendingFSM(){return m_IsSendingFSM;};

    bool m_IsSendingFSM;
    // FSM_COMM m_fsmMsg; // Sets type of message/command

    QString send_msg;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;

    // void send_to_FSM();

protected:
    // FSM communication

    // TODO: move host and port to XML
    const char *host = "localhost";
    int port = 1234;

    QSqlDatabase db;

public slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void send_to_FSM();

private:
    // QString remote_psql_db_path;
    // QString remote_psql_db_name;
};

#endif // DF_UTIL_H
