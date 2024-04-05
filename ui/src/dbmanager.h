#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <includefiles.h>
#include <functional>
#include <map>
#include <any>
#include <variant>


class DbManager
{
public:
    DbManager(const std::string &path);
    DbManager();
    ~DbManager();
    void closeDb();


    // Function to retrieve method signatures
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

    bool updateTableSlotsWithInt(int slot, QString column, int value);
    bool updateTableSlotsWithDouble(int slot, QString column, double value, int precision);
    bool updateTableSlotsWithText(int slot, QString column, QString value);

    bool addPageClick(const QString &page);
    // void emailEmpty(int slot);
    uint32_t getNumberOfRows(QString table);

    bool getRecentTransactions(QString values[][5], int count, int *count_retreived);
    void getPrinterStatus(bool *isOnline, bool *hasPaper); // do not erase

    // bool updateSlotAvailability(int slot, int isEnabled, QString status_text);

    void addTemperature(QString machine_id, double temperature_1, double temperature_2, QString alert);
    void addUserInteraction(QString session_id, QString role, QString page, QString event);
    void addUserInteraction(QString session_id, QString role, QString page, QString event, QString data);
    void alterDatabaseSchema(QString methodSignature, QString tableName);

    bool getAllSlotProperties(int slot,
                              QVector<int> &dispensePNumbers,
                              int &basePNumber,
                              QVector<int> &additivesPNumbers,
                              bool &is_enabled,
                              QString &status_text);
                              
    //Need to add all the variable names that are required in the database
    QStringList productTableColumns = {"soapstand_product_serial","mix_pnumbers","mix_ratios_low","mix_ratios_default","mix_ratios_high","productId","name","currency","name_receipt",
                                "concentrate_multiplier","dispense_speed","threshold_flow","retraction_time","calibration_const","volume_per_tick","last_restock","volume_full","volume_remaining",
                                "volume_dispensed_since_restock","volume_dispensed_total","is_enabled_small","is_enabled_medium","is_enabled_large","is_enabled_custom","size_small","size_medium",
                                "size_large","size_custom_min","size_custom_max","price_small","price_medium","price_large", "price_custom","plu_small","plu_medium","plu_large","plu_custom",
                                "pid_small","pid_medium","pid_large","pid_custom","flavour","image_url","type","ingredients","features","description","is_enabled_custom_discount",
                                "size_custom_discount","price_custom_discount","is_enabled","status_text","is_enabled_sample","size_sample","price_sample","is_empty_or_has_problem"};

    bool getAllProductProperties(int pnumber,
                                 QString *productId,
                                 QString *soapstand_product_serial,
                                //  QVector<int> &mixPNumbers,
                                //  QVector<double> &mixRatiosLow,
                                //  QVector<double> &mixRatiosDefault,
                                //  QVector<double> &mixRatiosHigh,
                                 //  QString *size_unit,
                                 QString *name_receipt,
                                 //  QString *m_currency_deprecated, //_dummy_deprecated
                                 //  QString *m_paymentOptions_deprecated,  //_deprecated,
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
                                 bool *is_enabled,
                                 bool *is_empty_or_has_problem,
                                 QString *status_text,
                                 bool *isSizeEnabled, double *prices, double *volumes, QString *PLUs, QString *PIDs);

    //Need to add all the variable names that are required in the database
    QStringList machineTableColumns = {"machine_id","soapstand_customer_id","ui_template","location","controller_type","controller_id","screen_type","screen_id",
                                    "has_receipt_printer","receipt_printer_is_online","receipt_printer_has_paper","has_tap_payment","hardware_version","software_version",
                                    "aws_port","coupons_enabled","has_empty_detection","enable_pump_ramping","enable_pump_reversal","dispense_buttons_count","maintenance_pwd",
                                    "show_transactions","help_text_html","idle_page_type","admin_pwd","pump_id_slots","is_enabled_slots","status_text_slots","alert_temperature",
                                    "software_version_controller","is_enabled","status_text","payment","size_unit","screen_sleep_time24h","screen_wakeup_time24h","buy_bottle_1", 
                                    "buy_bottle_2", "portal_base_url", "enable_offline_payment","page_init_timeout"};

    bool getAllMachineProperties(QString* machine_id,
                                QString* soapstand_customer_id,
                                QString* ui_template,
                                QString* location,
                                QString* controller_type,
                                QString* controller_id,
                                QString* screen_type,
                                QString* screen_id,
                                int* has_receipt_printer,
                                int* receipt_printer_is_online,
                                int* receipt_printer_has_paper,
                                int* has_tap_payment,
                                QString* hardware_version,
                                QString* software_version,
                                int* aws_port,
                                int* coupons_enabled,
                                bool* has_empty_detection,
                                int* enable_pump_ramping,
                                int* enable_pump_reversal,
                                int* dispense_buttons_count,
                                QString* maintenance_pwd,
                                int* show_transactions,
                                QString* help_text_html,
                                QString* idle_page_type,
                                QString* admin_pwd,
                                QString* pump_id_slots,
                                int* is_enabled_slots,
                                QString* status_text_slots,
                                double* alert_temperature,
                                QString* software_version_controller,
                                int* is_enabled,
                                QString* status_text,
                                QString* payment,
                                QString* size_unit,
                                int* screen_sleep_time24h,
                                int* screen_wakeup_time24h,
                                int* buy_bottle_1,
                                int* buy_bottle_2,
                                QString* portal_base_url,
                                int* enable_offline_payment,
                                int* page_init_timeout
                                );
    QString getHardwareVersion();
    

    

    void setPaymentTransaction(const std::map<std::string, std::string> &paymentObject);
    // QString m_dbPath2;

private:
    // QSqlDatabase m_db;
};

#endif // DBMANAGER_H
