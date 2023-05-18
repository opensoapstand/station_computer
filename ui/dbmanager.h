#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <includefiles.h>

class DbManager
{
public:
    DbManager(const QString &path);
    ~DbManager();
    void closeDB();
    bool isDatabaseLocked(const QSqlDatabase &db);

    bool addPageClick(const QString &page);
    QString getProductName(int slot);
    double getProductPrice(int slot, char size);
    // double getProductTargetVolume(int slot);
    double getProductVolumePerTick(int slot);
    // bool remainingVolumeIsBiggerThanLargestFixedSize(int slot);

    bool getRecentTransactions(QString values[][5], int count, int* count_retreived);
    bool hasReceiptPrinter();
    void printerStatus(bool* isOnline, bool* hasPaper );
    bool showTransactions();
    QString getProductReceiptName(int slot);
    QString getPaymentMethod(int slot);
    double getProductVolume(int slot, char ml);
    QString getUnits(int slot);
    QString getMaintenanceAdminPassword();
    QString getCustomerId();
    QString getHelpPageHtmlText();
    QString getSizeSmall(int slot);
    QString getSizeMedium(int slot);
    QString getSizeLarge(int slot);
    QString getPriceSmall(int slot);
    QString getPriceMedium(int slot);
    QString getPriceLarge(int slot);


    QString getIdlePageType();

    double getFullProduct(int slot);
    int getTotalTransactions();
    int getNumberOfProducts();
    double getVolumeRemaining(int slot);
    double getTotalDispensed(int slot);
    double getVolumeDispensedSinceRestock(int slot);
    QString getLastRefillTime(int slot);
    bool restockProduct(int slot);
    bool sellout(int slot);
    bool unsellout(int slot);
    int getLastTransactionIdFromDb();

    double getTemperature();
    int getPWM(int slot);
    int getDispenseButtonCount();
    double getBuffer(int slot);
    QString getPLU(int slot, char size);
    QString getMachineID();
    QString getProductID(int slot);
    int getSlotEnabled(int slot);
    bool setVolumeRemaining(int slot, double volumeMl);
    QString getStatusText(int slot);
    // bool setStatusText(int slot, QString text);
    bool updateSlotAvailability(int slot, int isEnabled, QString status_text);

    uint32_t getNumberOfRows(QString table);
    bool initialize(const QString &path);

    bool updatePaymentsDb(QString date, QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr);
    // bool updatePriceSmall(int slot, double new_price);
    // bool updatePriceLarge(int slot, double new_price);
    bool updatePrice(int slot, int size, double new_price);
    bool getCouponsEnabled();
    bool getEmptyContainerDetectionEnabled();
    bool setEmptyContainerDetectionEnabled(int isEnabled);
    bool getPumpRampingEnabled();
    bool setPumpRampingEnabled(int isEnabled);
    void getCustomDiscountProperties(int slot, bool *isEnabled, double *volumeDiscount, double *pricePerLiterDiscount);

    // bool updateTargetVolume_s(int slot, double new_volume);
    // bool updateTargetVolume_l(int slot, double new_volume);
    bool updateTargetVolume(int slot, int size, double new_volume);
    bool updateVolumePerTick(int slot, double new_volume_per_tick);
    bool updateFullVolume(int slot, double new_full_volume);
    bool updatePWM(int slot, int new_pwm);
    bool updateBuffer(int slot, double new_buffer);
    bool updatePluSmall(int slot, QString new_plu);
    bool updatePluLarge(int slot, QString new_plu);
    QString getProductType(int slot);
    QString getProductDrinkfillSerial(int slot);
    // void getProductProperties(int slot, QString*name, QString *description, QString *features,  QString *ingredients);
    void getProductProperties(int slot, QString *product_id, bool *isSizeEnabled);
    QString getTemplateName();
    void updateTapToQR();
private:
    void emailEmpty(int slot);
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
