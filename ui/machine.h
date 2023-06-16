#ifndef MACHINE_H
#define MACHINE_H

#include "df_util.h"
// #include "page_idle.h"
#include "dbmanager.h"

typedef enum UserRole
{
    user,
    maintainer,
    admin
} UserRole;

class machine : public QObject
{
    Q_OBJECT

public:
    machine();
    ~machine();
    void loadParametersFromDb();
    void setDb(DbManager *db);

    bool slotNumberValidityCheck(int slot);
    QString getStatusText(int slot);
    void setStatusText(int slot, bool isSlotEnabled, QString status);
    void loadProductPropertiesFromProductsFile(QString soapstand_product_number, QString* name, QString* name_ui, QString* product_type, QString* description_ui, QString* features_ui, QString* ingredients_ui);

    QString getPumpId(int slot);

    QString getMachineId();
    bool getCouponsEnabled();
    bool getShowTransactionHistory();

    void processRolePassword(QString password_input);
    QString getActiveRoleAsText();
    void setRole(UserRole role);
    bool isAllowedAsAdmin();
    bool isAllowedAsMaintainer();

    QString getCustomerId();

    QString getTemplateFolder();
    QString getTemplatePathFromName(QString fileName);
    QString getDefaultTemplatePathFromName(QString fileName);
    bool getEmptyContainerDetectionEnabled();
    void setEmptyContainerDetectionEnabled(bool isEnabled);
    void setPumpRampingEnabled(bool isEnabled);
    bool getPumpRampingEnabled();
    QString getHelpPageHtmlText();

    bool getSlotEnabled(int slot);
    void setSlotEnabled(int slot, bool isEnabled);
    QString getIdlePageType();

    int getDispensersCount();

    bool hasReceiptPrinter();
    void printerStatus(bool *isOnline, bool *hasPaper);

public slots:

signals:

private:
    DbManager *m_db;
    UserRole active_role;
    QString m_machine_id;
    QString m_soapstand_customer_id;
    QString m_template;
    QString m_location;
    QString m_controller_type;
    QString m_controller_id;
    QString m_screen_type;
    QString m_screen_id;
    int m_has_receipt_printer;
    int m_receipt_printer_is_online;
    int m_receipt_printer_has_paper;
    int m_has_tap_payment;
    QString m_hardware_version;
    QString m_software_version;
    int m_aws_port;
    int m_coupons_enabled;
    int m_has_empty_detection;
    int m_enable_pump_ramping;
    int m_enable_pump_reversal;
    int m_dispense_buttons_count;
    QString m_maintenance_pwd;
    int m_show_transactions;
    QString m_help_text_html;
    QString m_idle_page_type;
    QString m_admin_pwd;

    QString m_pump_id_slots[SLOT_COUNT];
    int m_is_enabled_slots[SLOT_COUNT];
    QString m_status_text_slots[SLOT_COUNT];
};

#endif // MACHINE_H
