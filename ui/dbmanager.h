#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <includefiles.h>

#define DB_PRODUCTS_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-products.db"


class DbManager
{
public:
    DbManager(const QString& path);
    bool addPageClick(const QString& page);
    QString getProductName(int slot);
    double getProductPrice(int slot);
    double getProductVolume(int slot);

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
