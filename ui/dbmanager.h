#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <includefiles.h>

class DbManager
{
public:
    DbManager(const QString &path);
    DbManager();
    ~DbManager();
    void closeDb();
    // void closeDb(QSqlDatabase db);
    // void setPath(QString path);
    QSqlDatabase openDb(QString dbname);

    bool isDatabaseLocked(const QSqlDatabase &db);

    bool executeQuery(QString sql);

    bool updateTableMachineWithInt(QString column, int value);
    bool updateTableMachineWithDouble(QString column, double value, int precision);
    bool updateTableMachineWithText(QString column, QString value);

    bool updateTableProductsWithText(int pnumber, QString column, QString value);
    bool updateTableProductsWithInt(int pnumber, QString column, int value);
    bool updateTableProductsWithDouble(int pnumber, QString column, double value, int precision);

    bool addPageClick(const QString &page);
    // void emailEmpty(int slot);
    uint32_t getNumberOfRows(QString table);

    bool getRecentTransactions(QString values[][5], int count, int *count_retreived);
    void getPrinterStatus(bool *isOnline, bool *hasPaper); // do not erase

    // QString getPaymentMethod(int slot); // do not erase yet (wait for ash)
    // void setPaymentToQR();              // ASh to change.

    // bool updateSlotAvailability(int slot, int isEnabled, QString status_text);

    void addTemperature(QString machine_id, double temperature_1, double temperature_2, QString alert);
    void addUserInteraction(QString session_id, QString role, QString page, QString event);
    void addUserInteraction(QString session_id, QString role, QString page, QString event, QString data);

    void getAllSlotProperties(int slot,
                              QVector<int> *dispensePNumbers,
                              int *basePNumber,
                              QVector<int> *additivesPNumbers,
                              bool *is_enabled,
                              QString *status_text);

    void getAllProductProperties(int pnumber,
                                 QString *productId,
                                 QString *soapstand_product_serial,
                                 QVector<int> &mixPNumbers,
                                 QVector<double> &mixRatios,
                                 QString *size_unit,
                                 QString *name_receipt,
                                 QString *m_currency_deprecated, //_dummy_deprecated
                                 QString *m_payment_deprecated,  //_deprecated,
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
                                 QString *admin_pwd,

                                 QString *pump_id_slots,
                                 int *is_enabled_slots,
                                 QString *status_text_slots,
                                 double *alert_temperature);
         QString getHardwareVersion();

    void setPaymentTransaction(const std::map<std::string, std::string>& paymentObject);
    // QString m_dbPath2;

private:
    // QSqlDatabase m_db;
};

#endif // DBMANAGER_H
