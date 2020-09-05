#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "dfuiserver.h"

// TODO: Refactor to fit with dfuicommthread

using namespace std;

typedef enum FSM_COMM {
    SEND_EMPTY = 0,
    SEND_DRINK,
    SEND_CLEAN,
    SEND_ERROR
} FSM_COMM;

class df_util
{

public:
    df_util();
    void send_to_FSM(QString * msg);
    void initialize_local_db();
    bool open_local_db();
    bool close_local_db();
    bool getVendorDetails();
    QString get_local_db_max_transaction();

protected:
    QSqlDatabase db;

private:
    QString local_db_path = "/home/df-admin/Project/drinkfill/db/sqlite/";
    QString local_db_name = "drinkfill-sqlite.db";

    QString remote_psql_db_path;
    QString remote_psql_db_name;
};

#endif // DF_UTIL_H
