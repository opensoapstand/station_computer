#include "dbmanager.h"

DbManager::DbManager(const QString& path)
{
   QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

   m_db = QSqlDatabase::addDatabase("QPSQL");
   m_db.setHostName("localhost");
   m_db.setDatabaseName("soapstand");
   m_db.setUserName("postgres");
   m_db.setPassword("postgres");

   bool ok = m_db.open();

   if (!ok)
   {
      qDebug() << "Error: connection with database failed";
      qDebug() << m_db.lastError().text();
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
    query.prepare("INSERT INTO clicks (time, page_info) VALUES (:time, :page);");
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
