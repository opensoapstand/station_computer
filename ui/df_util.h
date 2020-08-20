#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
#include "drinkorder.h"


using namespace std;

class df_util
{
public:
    df_util();
    void send_to_FSM(QString * msg);
    void open_database();
    QString db_get_max_transaction();

    QSqlDatabase db;

};

#endif // DF_UTIL_H
