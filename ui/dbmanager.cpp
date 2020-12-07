#include "dbmanager.h"

DbManager::DbManager(const QString& path)
{
   QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

   m_db = QSqlDatabase::addDatabase("QSQLITE");
   m_db.setDatabaseName(path);

   if (!m_db.open())
   {
      qDebug() << "Error: connection with database failed";
   }
   else
   {
      qDebug() << "Database: connection ok";
   }
}

bool DbManager::addPageClick(const QString& page){

    bool success = false;
    // you should check if args are ok first...
    QSqlQuery query;
    query.prepare("INSERT INTO clicks (id, time, page_info) VALUES (NULL, :time, :page);");
    query.bindValue(":time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":page", page);
    if(query.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "addPageClick error:"
                 << query.lastError();
    }

    return success;
}
