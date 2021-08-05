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
    QString getProductReceiptName(int slot);
    QString getPaymentMethod(int slot);
    double getProductVolume(int slot, char ml);
    bool refill(int slot);
    bool sellout(int slot);
    bool unsellout(int slot);
    double getFullProduct(int slot);
    int getTotalTransactions();
    int getNumberOfProducts();
    double getRemaining(int slot);
    double getTotalDispensed(int slot);
    QString getLastRefill(int slot);
    double getTemperature();
    bool updatePaymentsDb(QString date, QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr);

private:
    void emailEmpty(int slot);
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
