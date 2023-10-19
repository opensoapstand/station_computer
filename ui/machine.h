#ifndef MACHINE_H
#define MACHINE_H

#include "df_util.h"
#include "product.h"
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

class product; //  forward declaration.

class machine : public QObject
{
    Q_OBJECT

public:
    void checkForHighTemperatureAndDisableProducts();
    machine();
    ~machine();
    void loadParametersFromDb();
    void setDb(DbManager *db);
    DbManager* getDb();
    void initMachine();

    void dispenseButtonLightsAnimateState(bool animateElseOff);
    bool slotNumberValidityCheck(int slot);
    QString getStatusText(int slot);
    void setStatusText(int slot, QString status);
    void loadProductPropertiesFromProductsFile(QString soapstand_product_number, QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui);

    QString getPumpId(int slot);

    QString getMachineId();
    bool getCouponsEnabled();
    bool getShowTransactionHistory();
    bool isAelenPillarElseSoapStand();
    bool isDispenseAreaBelowElseBesideScreen();

    void registerUserInteraction(QWidget *page);

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
    QString getTemplateName();
    QString getTemplatePathFromName(QString fileName);
    QString getDefaultTemplatePathFromName(QString fileName);

    bool getEmptyContainerDetectionEnabled();
    void setEmptyContainerDetectionEnabled(bool isEnabled);
    void setPumpRampingEnabled(bool isEnabled);
    bool getPumpRampingEnabled();
    QString getHelpPageHtmlText();

    bool getSlotEnabled(int slot);
    void setSlotEnabled(int slot, bool isEnabled);
    void setSlotEnabled(int slot, bool isEnabled, QString statusText);

    int getSlotCount();
    bool compareSlotCountToMaxSlotCount(int slot_count);

    bool hasReceiptPrinter();
    void getPrinterStatusFromDb(bool *isOnline, bool *hasPaper);
    void getTemperatureFromController();
    void writeTemperatureToDb(double temperature_1, double temperature_2);
    double getTemperature_1();
    bool isTemperatureTooHigh_1();
    void fsmReceiveTemperature(double temperature_1, double temperature_2);

    StateCoupon getCouponState();
    void setCouponState(StateCoupon state);
    void initCouponState();

    void setCouponCode(QString promoCode);
    QString getCouponCode();

    void setCouponConditions(QString couponConditions);
    std::map<QString, QString> getCouponConditions();

    void setProducts(product *products);
    QString m_session_id;

    void setDiscountPercentageFraction(double percentageFraction);
    double getDiscountPercentageFraction();
    double getDiscountAmount(double price);
    double getPriceWithDiscount(double price);
    product *getProduct(int slot);
    void loadDynamicContent();
    QString getCSS(QString cssName);
    void addCssClassToObject(QWidget *element, QString classname, QString css_file_name);
    void setTemplateTextWithIdentifierToObject(QWidget *p_element, QString identifier);
    void setTemplateTextToObject(QWidget *p_element);
    void setTextToObject(QWidget *p_element, QString text);
    QString getCombinedElementPageAndName(QWidget *p_element);
    QString getTemplateTextByElementNameAndPage(QWidget *p_element);
    QString getTemplateTextByElementNameAndPageAndIdentifier(QWidget *p_element, QString identifier);
    QString getTemplateTextByPage(QWidget *page, QString identifier);
    QString getTemplateText(QString textName_to_find);
    void loadTextsFromTemplateCsv();
    void loadTextsFromDefaultCsv();
    void loadElementDynamicPropertiesFromTemplate();
    void loadElementDynamicPropertiesFromDefaultTemplate();

    product *selectedProduct;
    // QStringList getChildNames(QObject *parent);

    void addPictureToLabel(QLabel *label, QString picturePath);
    void addPictureToButton(QPushButton *button, QString picturePath);
    void addCustomerLogoToLabel(QLabel *label);
    void setBackgroundPictureFromTemplateToPage(QWidget *page, QString imageName);
    void setBackgroundPictureToQWidget(QWidget *page, QString imageName);
    void pageTransition(QWidget *pageToHide, QWidget *pageToShow);
    void applyPropertiesToQWidget(QWidget *widget);
    void applyDynamicPropertiesFromTemplateToWidgetChildren(QWidget *widget);

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
    QString getIdlePageType();

    void setSelectedProduct(uint8_t slot);
    product *getSelectedProduct();

    QStringList getChildNames(QObject *parent);
    void loadTextsFromCsv(QString csv_path, std::map<QString, QString> *dictionary);

public slots:

signals:

private:
    std::map<QString, QString> textNameToTextMap_template;
    std::map<QString, QString> textNameToTextMap_default;
    std::map<QString, QString> elementDynamicPropertiesMap_default;
    std::map<QString, QString> elementDynamicPropertiesMap_template;
    QString m_templatePath;

    product *m_products;

    QTime temperatureTooHighStartMillis;
    bool isTemperatureTooHigh = false;
    StateCoupon m_stateCoupon;
    double m_discount_percentage_fraction = 0.0;
    double max_discount = 0.0;
    QString m_promoCode;

    DbManager *m_db;
    UserRole active_role;

    QString m_pump_id_slots[MAX_SLOT_COUNT];
    int m_is_enabled_slots[MAX_SLOT_COUNT];
    QString m_status_text_slots[MAX_SLOT_COUNT];
};

#endif // MACHINE_H
