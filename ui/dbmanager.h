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

    bool executeQuery(QSqlQuery* qry, QString sql);

    bool updateProductsWithText(int slot, QString column, QString value);
    bool updateProductsWithInt(int slot, QString column, int value);
    bool updateProductsWithDouble(int slot, QString column, double value, int precision);

    bool addPageClick(const QString &page);
    void emailEmpty(int slot);
    // QString getProductName(int slot);
    // double getProductPrice(int slot, char size);
    // double getProductTargetVolume(int slot);
    // double getProductVolumePerTick(int slot);
    // bool remainingVolumeIsBiggerThanLargestFixedSize(int slot);

    bool getRecentTransactions(QString values[][5], int count, int *count_retreived);
    // bool hasReceiptPrinter();
    void printerStatus(bool *isOnline, bool *hasPaper); // do not erase

    // bool showTransactions();
    // QString getProductReceiptName(int slot);

    QString getPaymentMethod(int slot); // do not erase yet (wait for ash)
    void setPaymentToQR();              // ASh to change.


    // double getProductVolume(int slot, char ml);
    // QString getUnits(int slot);
    // QString getMaintenanceAdminPassword();
    // QString getCustomerId();
    // QString getHelpPageHtmlText();
    // QString getSizeSmall(int slot);
    // QString getSizeMedium(int slot);
    // QString getSizeLarge(int slot);
    // QString getPriceSmall(int slot);
    // QString getPriceMedium(int slot);
    // QString getPriceLarge(int slot);

    // QString getIdlePageType();

    // double getFullProduct(int slot);
    // int getTotalTransactions();
    // int getNumberOfProducts();
    // double getVolumeRemaining(int slot);
    // double getTotalDispensed(int slot);
    // double getVolumeDispensedSinceRestock(int slot);
    // QString getLastRestockDate(int slot);
    // bool restockProduct(int slot);
    // bool sellout(int slot);
    // bool unsellout(int slot);
    // int getLastTransactionIdFromDb();

    // double getTemperature();
    // int getPWM(int slot);
    // int getDispenseButtonCount();
    // double getBuffer(int slot);
    // QString getPLU(int slot, char size);
    // QString getMachineID();
    // QString getAwsProductId(int slot);
    // int getSlotEnabled(int slot);
    // bool setVolumeRemaining(int slot, double volumeMl);
    // QString getStatusText(int slot);
    // bool setStatusText(int slot, QString text);
    bool updateSlotAvailability(int slot, int isEnabled, QString status_text);


    void addUserInteraction(QString action);

    uint32_t getNumberOfRows(QString table);
    // bool initialize(const QString &path);

    bool updatePaymentsDb(QString date, QString time, QString txnType, QString amount, QString cardNo, QString refNo, QString authNo, QString cardType, QString status, QString isoCode, QString hostCode, QString tvr);
    // bool updatePriceSmall(int slot, double new_price);
    // bool updatePriceLarge(int slot, double new_price);
    bool updatePrice(int slot, int size, double new_price);
    // bool getCouponsEnabled();
    // bool getEmptyContainerDetectionEnabled();
    bool setEmptyContainerDetectionEnabled(int isEnabled);
    // bool getPumpRampingEnabled();
    bool setPumpRampingEnabled(int isEnabled);
    // void getCustomDiscountProperties(int slot, bool *isEnabled, double *volumeDiscount, double *pricePerLiterDiscount);

    // bool updateTargetVolume_s(int slot, double new_volume);
    // bool updateTargetVolume_l(int slot, double new_volume);
    bool updateTargetVolume(int slot, int size, double new_volume);
    bool updateVolumePerTick(int slot, double new_volume_per_tick);
    bool updateFullVolume(int slot, double new_full_volume);
    bool updatePWM(int slot, int new_pwm);
    bool updateBuffer(int slot, double new_buffer);
    bool updatePluSmall(int slot, QString new_plu);
    bool updatePluLarge(int slot, QString new_plu);
    // QString getProductType(int slot);
    // QString getProductDrinkfillSerial(int slot);
    // void getProductProperties(int slot, QString*name, QString *description, QString *features,  QString *ingredients);
    // void getProductProperties(int slot, QString *product_id, bool *isSizeEnabled);
    // QString getTemplateName();

    void getAllProductProperties(int slot,
                                 QString *productId,
                                 QString *soapstand_product_serial,
                                 QString *size_unit,
                                 QString *currency,
                                 QString *payment,
                                 QString *name_receipt,
                                 int *concentrate_multiplier,
                                 int *dispense_speed,
                                 double *threshold_flow,
                                 int *retraction_time,
                                 double *calibration_const,
                                 double *volume_per_tick,
                                 QString *last_restock,
                                 double *volume_full,
                                 double *volume_remaining,
                                 double *volume_dispensed_since_restock,
                                 double *volume_dispensed_total,
                                 int *is_enabled_custom_discount,
                                 double *size_custom_discount,
                                 double *price_custom_discount,
                                 bool *isSizeEnabled, double *prices, double *volumes, QString *PLUs, QString *PIDs);

    void getAllMachineProperties(QString *machine_id,
                                 QString *soapstand_customer_id,
                                 QString *ttttemplate,
                                 QString *location,
                                 QString *controller_type,
                                 QString *controller_id,
                                 QString *screen_type,
                                 QString *screen_id,
                                 int *has_receipt_printer,
                                 int *receipt_printer_is_online,
                                 int *receipt_printer_has_paper,
                                 int *has_tap_payment,
                                 QString *hardware_version,
                                 QString *software_version,
                                 int *aws_port,

                                 int *coupons_enabled,
                                 int *has_empty_detection,
                                 int *enable_pump_ramping,
                                 int *enable_pump_reversal,
                                 int *dispense_buttons_count,
                                 QString *maintenance_pwd,
                                 int *show_transactions,
                                 QString *help_text_html,
                                 QString *idle_page_type,

                                 QString *pump_id_slots,
                                 int *is_enabled_slots,
                                 QString *status_text_slots);

private:
    
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
