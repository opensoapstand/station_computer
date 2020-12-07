#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <includefiles.h>

class DbManager
{
public:
    DbManager(const QString& path);
    bool addPageClick(const QString& page);

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
