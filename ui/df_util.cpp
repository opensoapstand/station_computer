#include "df_util.h"
#include "includefiles.h"

df_util::df_util()
{

}

// TODO: Decouple send_to_FSM from Dispenser to here
void df_util::send_to_FSM(QString * msg)
{
}

void df_util::open_database()
{
    QString path = "/home/df-admin/Project/drinkfill/db/sqlite/";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "drinkfill-sqlite.db");

    if(!db.open())
    {
        qDebug() << "Can't Connect to DB !";
    }
    else
    {
        qDebug() << "Connected Successfully to" + db.databaseName();
        QSqlQuery query;
        query.prepare("SELECT name, full_address FROM vendor WHERE vendor_id = 1;");
        if(!query.exec())
        {
            qDebug() << "Can't Execute Query !";
        }
        else
        {
            qDebug() << "Query Executed Successfully !";
            while(query.next())
            {
                qDebug() << "Vendor Name : " << query.value(0).toString();
                qDebug() << "Vendor Full Address: " << query.value(1).toString();
            }
        }
    }

    db.close();
}

QString df_util::db_get_max_transaction()
{
    QString result = "Db Error";

    QString path = "/home/df-admin/Project/drinkfill/sqlite/";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "drinkfill-sqlite.db");

    if(!db.open())
    {
        qDebug() << "Can't Connect to DB !";
    }
    else
    {
        qDebug() << "Connected Successfully to" + db.databaseName();
        QSqlQuery query;
        query.prepare("SELECT MAX(inventory_id) FROM inventory;");
        if(!query.exec())
        {
            qDebug() << "Can't Execute Query !";
        }
        else
        {
            qDebug() << "Query Executed Successfully !";
            result = query.value(0).toString();
        }
    }

    return result;
}
