#ifndef MACHINE_H
#define MACHINE_H

#include "df_util.h"
#include "dbmanager.h"

typedef enum UserRole
{
    user,
    maintainer,
    admin
} UserRole;

typedef enum StateCoupon
{
    no_state,
    disabled,
    enabled_not_set,
    enabled_show_keyboard,
    enabled_processing_input,
    enabled_invalid_input,
    enabled_valid_active,
    enabled_not_eligible,
    network_error
} StateCoupon;

class machine : public QObject
{
    Q_OBJECT

public:
    machine();
    ~machine();
    void loadParametersFromDb();
    void setDb(DbManager *db);



    void dispenseButtonLightsAnimateState(bool animateElseOff);
    bool slotNumberValidityCheck(int slot);
    QString getStatusText(int slot);
    void setStatusText(int slot, bool isSlotEnabled, QString status);
    void loadProductPropertiesFromProductsFile(QString soapstand_product_number, QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui);

    QString getPumpId(int slot);

    QString getMachineId();
    bool getCouponsEnabled();
    bool getShowTransactionHistory();
    bool isAelenPillarElseSoapStand();

    void processRolePassword(QString password_input);
    QString getActiveRoleAsText();
    void setRole(UserRole role);
    bool isAllowedAsAdmin();
    bool isAllowedAsMaintainer();

    void createSessionId();
    void resetSessionId();
    QString getSessionId();

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
    void getPrinterStatusFromDb(bool *isOnline, bool *hasPaper);
    void getTemperatureFromController();
    // void getTemperature2FromController(); // not needed, all available tempertures are sent with one request to controller. 
    void writeTemperatureToDb(double temperature_1, double temperature_2);
    // void writeTemperature2ToDb(double temperature2);
    double getTemperature_1();
    // double getTemperature2();
    bool isTemperatureTooHigh_1();
    void fsmReceiveTemperature(double temperature_1, double temperature_2);
    // void fsmReceiveTemperature2(double temperature2);
    // void temperatureFromControllerFeedback(double m_temperature);

    StateCoupon getCouponState();
    void setCouponState(StateCoupon state);
    void initCouponState();

    void setPromoCode(QString promoCode);
    QString getPromoCode();

    void setCouponConditions(QString couponConditions);
    std::map<QString , QString > getCouponConditions();

    QString m_session_id;

    void setDiscountPercentageFraction(double percentageFraction);
    double getDiscountPercentageFraction();
    double getDiscountAmount(double price);
    double getPriceWithDiscount(double price);

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
    double m_temperature;
    double m_alert_temperature;
    double m_temperature2;
    double m_alert_temperature2;

    QString m_min_threshold_vol_ml_discount;
    QString m_max_threshold_vol_ml_discount;
    QString m_max_dollar_amount_discount;

    df_util *dfUtility;

    QJsonObject m_propertiesObject;
    // QStringList getChildNames(QObject *parent);

    void loadElementPropertiesFile();

public slots:

signals:

private:
    StateCoupon m_stateCoupon;
    double m_discount_percentage_fraction = 0.0;
    double max_discount = 0.0;
    QString m_promoCode;

    DbManager *m_db;
    UserRole active_role;


    QString m_pump_id_slots[SLOT_COUNT];
    int m_is_enabled_slots[SLOT_COUNT];
    QString m_status_text_slots[SLOT_COUNT];
};

#endif // MACHINE_H
