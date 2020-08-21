#include "df_util.h"
#include "includefiles.h"

df_util::df_util()
{

}

// TODO: Decouple send_to_FSM from Dispenser to here
void df_util::send_to_FSM(QString * msg)
{
}

void df_util::initialize_local_db() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(local_db_path + "drinkfill-sqlite.db");

}

bool df_util::open_local_db()
{
    initialize_local_db();
    if(!db.open())
    {
        qDebug() << "Can't Connect to DB!";
    }
    else
    {
        qDebug() << "Connected Successfully to" + db.databaseName();
    }
    return db.open();
}


bool df_util::close_local_db()
{
   if(db.open()) {
        db.close();
   } else {
        qDebug() << "No Database open!" << endl;
   }

   return db.open();
}

bool df_util::getVendorDetails()
{
    bool isResultAvailble;

    if(isResultAvailble = open_local_db())
    {
        QSqlQuery query;
        query.prepare("SELECT name, full_address "
                      "FROM vendor WHERE vendor_id = 1;");
        if(!(isResultAvailble = query.exec()))
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
    close_local_db();
    return isResultAvailble;
}

QString df_util::get_local_db_max_transaction()
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
