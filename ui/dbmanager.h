#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <includefiles.h>

class DbManager
{
public:
    DbManager(const QString& path);
    bool addPageClick(const QString& page);
    QString getProductName(int slot);
    double getProductPrice(int slot, char ml);
    //double getProductTargetVolume(int slot);
    double getProductVolumePerTick(int slot);
    bool checkLevels(int slot);
    double getProductVolume(int slot, char ml);
    bool refill(int slot);


private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
