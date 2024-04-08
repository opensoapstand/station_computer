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

    void checkAndRepairConfigurationDb();
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
    void checkAndRepairTableInConfigDb(const std::vector<std::tuple<QString, QString, QString> >&, QString);
    // Slots table schema as tuple 
    // Datatype as first value, column name as second value, default value as third value
    std::vector<std::tuple<QString, QString, QString>> slotsTableLayout = {
                        {"QString", "dispense_pnumbers","2"},
                        {"QString", "base_pnumber", "2"},
                        {"QString",  "additive_pnumbers",""},
                        {"bool",  "is_enabled","0"},
                        {"QString",  "status_text","SLOT_STATE_DISABLED"}
                        };

    bool getAllSlotProperties(int slot,
                              QVector<int> &dispensePNumbers,
                              int &basePNumber,
                              QVector<int> &additivesPNumbers,
                              bool &is_enabled,
                              QString &status_text);
                              
    // Product table schema as tuple 
    // Datatype as first value, column name as second value, default value as third value
    std::vector<std::tuple<QString, QString, QString>> productsTableLayout = {
                        //Using random soapstand_product_serial number as 55.
                        {"QString", "soapstand_product_serial","2"},
                        {"QString", "mix_pnumbers", ""},
                        {"QString",  "mix_ratios_low",""},
                        {"QString",  "mix_ratios_default",""},
                        {"QString",  "mix_ratios_high",""},
                        {"QString",  "productId",""},
                        {"QString",  "name",""},
                        {"QString",  "currency","CAD"},
                        {"QString",  "name_receipt",""},
                        {"int",  "concentrate_multiplier",""},
                        {"int",  "dispense_speed","127"},
                        {"double",  "threshold_flow","1"},
                        {"int",  "retraction_time","0"},
                        {"double",  "calibration_const","1.3"},
                        {"double",  "volume_per_tick","6.0"},
                        {"QString",  "last_restock",""},
                        {"double",  "volume_full","18000"},
                        {"double",  "volume_remaining","18000"},
                        {"double",  "volume_dispensed_since_restock","0"},
                        {"double",  "volume_dispensed_total","0"},
                        {"bool",  "is_enabled_small","1"},
                        {"bool",  "is_enabled_medium","0"},
                        {"bool",  "is_enabled_large","1"},
                        {"bool",  "is_enabled_custom","0"},
                        {"double",  "size_small","500"},
                        {"double",  "size_medium","750"},
                        {"double",  "size_large","1000"},
                        {"double", "size_custom_min","10"},
                        {"double",  "size_custom_max","2500"},
                        {"double",  "price_small","1"},
                        {"double",  "price_medium","2"},
                        {"double",  "price_large","3"},
                        {"double",  "price_custom","0.01"},
                        {"QString",  "plu_small",""},
                        {"QString",  "plu_medium",""},
                        {"QString",  "plu_large",""},
                        {"QString",  "plu_custom",""},
                        {"QString",  "pid_small",""},
                        {"QString",  "pid_medium",""},
                        {"QString",  "pid_large",""},
                        {"QString",  "pid_custom",""},
                        {"QString",  "flavour",""},
                        {"QString",  "image_url",""},
                        {"QString",  "type",""},
                        {"QString",  "ingredients",""},
                        {"QString",  "features",""},
                        {"QString",  "description",""},
                        {"int",  "is_enabled_custom_discount","0"},
                        {"double",  "size_custom_discount","0"},
                        {"double",  "price_custom_discount","0"},
                        {"int",  "is_enabled","0"},
                        {"int",  "is_empty_or_has_problem","0"},
                        {"QString",  "status_text","PRODUCT_STATE_DISABLED"},
                        {"bool",  "is_enabled_sample","0"},
                        {"double",  "size_sample","0"},
                        {"double",  "price_sample","0"}
                    };
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

    // Machine Table schema
    // Datatype as first value, column name as second value, default value as third value
    std::vector<std::tuple<QString, QString, QString>> machineTableLayout = {
                        {"QString", "machine_id","SS-DEV-003"},
                        {"int", "soapstand_customer_id", "0"},
                        {"QString",  "template","default"},
                        {"QString",  "controller_type",""},
                        {"QString",  "controller_id",""},
                        {"QString",  "screen_type","touchscreen"},
                        {"QString",  "screen_id",""},
                        {"QString",  "has_receipt_printer","0"},
                        {"QString",  "receipt_printer_is_online","0"},
                        {"QString",  "receipt_printer_has_paper","0"},
                        {"bool",  "has_tap_payment","0"},
                        {"QString",  "hardware_version",""},
                        {"QString",  "software_version",""},
                        {"int",  "aws_port",""},
                        {"bool",  "coupons_enabled","0"},
                        {"bool",  "has_empty_detection","0"},
                        {"int",  "enable_pump_ramping","0"},
                        {"int",  "enable_pump_reversal","0"},
                        {"int",  "dispense_buttons_count","4"},
                        {"QString",  "maintenance_pwd","soap"},
                        {"int",  "show_transactions","0"},
                        {"QString",  "help_text_html",""},
                        {"QString",  "idle_page_type",""},
                        {"QString",  "admin_pwd","soap"},
                        {"double",  "alert_temperature","666"},
                        {"QString",  "software_version_controller",""},
                        {"int",  "is_enabled","1"},
                        {"QString", "status_text","MACHINE_STATE_AVAILABLE"},
                        {"QString",  "payment",""},
                        {"QString",  "size_unit",""},
                        {"int",  "screen_sleep_time24h",""},
                        {"int",  "screen_wakeup_time24h",""},
                        {"int",  "buy_bottle_1",""},
                        {"int",  "buy_bottle_2",""},
                        {"QString",  "portal_base_url","https://soapstandportal.com/"},
                        {"int",  "enable_offline_payment","0"},
                        {"int",  "page_init_timeout","120"}
                    };
       
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
