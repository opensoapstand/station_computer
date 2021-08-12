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
    int getPWM(int slot);
    double getBuffer(int slot);

    bool updatePaymentsDb(QString date, QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr);
    bool updatePrice_s(int slot, double new_price);
    bool updatePrice_l(int slot, double new_price);
    bool updateTargetVolume_s(int slot, double new_volume);
    bool updateTargetVolume_l(int slot, double new_volume);
    bool updateVolumePerTick(int slot, double new_volume_per_tick);
    bool updateFullVolume(int slot, double new_full_volume);
    bool updatePWM(int slot, int new_pwm);
    bool updateBuffer(int slot, double new_buffer);

private:
    void emailEmpty(int slot);
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
