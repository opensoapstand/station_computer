#ifndef MACHINE_H
#define MACHINE_H

#include "df_util.h"
#include "pnumberproduct.h"
#include "dbmanager.h"
#include <vector>

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

class dispenser_slot; //  forward declaration.

typedef enum StateReboot
{
    initial_state,
    wait_for_trigger,
    triggered_wait_for_delay,
    delay_elapsed,
    user_cancelled_reboot
} StateReboot;

typedef enum ActivePaymentMethod
{
    qr,
    tap_canada,
    tap_usa,
    receipt_printer,
} ActivePaymentMethod;

class product; //  forward declaration.

class machine : public QObject
{
    Q_OBJECT

public:
    machine();
    ~machine();
    void loadMachineParameterFromDb();
    void setDb(DbManager *db);
    DbManager *getDb();
    void initMachine();

    QString getIdlePageType();

    void checkForHighTemperatureAndDisableProducts();

    bool isSessionLocked();

    QString getSizeUnit();

    void dispenseButtonLightsAnimateState(bool animateElseOff);
    bool slotNumberValidityCheck(int slot);

    QString getPumpId(int slot);

    double getPriceCorrected(int pnumber);

    QString getMachineId();
    QString getMachineLocation();
    QString getPaymentMethod();
    void setPaymentMethod(QString paymentMethod);
    bool getCouponsEnabled();
    bool getShowTransactionHistory();

    bool isDispenseAreaBelowElseBesideScreen();

    void registerUserInteraction(QWidget *page);

    void processRolePassword(QString password_input);
    QString getActiveRoleAsText();
    UserRole getRole();
    void setRole(UserRole role);
    bool isAllowedAsAdmin();
    bool isAllowedAsMaintainer();

    void resetUserState();
    void createSessionId();
    void resetSessionId();
    QString getSessionId();

    QString getClientId();

    QString getTemplateFolder();
    QString getTemplateName();
    QString getTemplatePathFromName(QString fileName);
    // QString getDefaultTemplatePathFromName(QString fileName);

    bool getEmptyContainerDetectionEnabled();
    void setEmptyContainerDetectionEnabled(bool isEnabled);
    void setPumpRampingEnabled(bool isEnabled);
    bool getPumpRampingEnabled();
    QString getHelpPageHtmlText();

    bool isSlotExisiting(int slot_index);

    bool getIsMachineEnabled();
    void setIsMachineEnabled(bool isEnabled);
    void setIsMachineEnabled(bool isEnabled, QString statusText);

    QString getMachineStatusText();
    void setMachineStatusText(QString status);

    int getSlotCount();
    void setSlots(dispenser_slot *slotss);
    bool isSlotCountBiggerThanMaxSlotCount(int slot_count);

    void setSelectedSlot(int slot);
    void setSelectedSlotFromSelectedProduct();
    void setSelectedSlot();
    int getSlotFromBasePNumber(int base_pnumber);
    dispenser_slot *getSlotByPosition(int slotPosition);
    dispenser_slot *getSlotFromOption(int productOption);

    dispenser_slot *getSelectedSlot();

    void initProductOptions();
    // void initProductOptions(const QVector<int> &pnumbersToBeSetAsOptions);
    bool getIsOptionAvailable(int productOption);
    bool isOptionExisting(int productOption);
    void setProductToMenuOption(int productOption, int pnumber);
    int clearOptions();
    int getOptionCount();

    void setSelectedProductByOption(int productOption);
    pnumberproduct *getProductFromMenuOption(int productOption);

    pnumberproduct *getProductByPNumber(int pnumber);
    pnumberproduct *getSlotBaseProduct(int slot);
    void setSelectedBottle(int pnumber);
    void resetSelectedBottle();
    pnumberproduct *getSelectedBottle();
    bool hasSelectedBottle();
    bool hasBuyBottleOption();
    void setSelectedProduct(int pnumber);
    pnumberproduct *getSelectedProduct();

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

    StateReboot getRebootState();
    void setRebootState(StateReboot state);

    ActivePaymentMethod getActivePaymentMethod();
    void setActivePaymentMethod(ActivePaymentMethod paymentMethod);

    std::vector<ActivePaymentMethod> getAllowedPaymentMethods();
    void setAllowedPaymentMethods(ActivePaymentMethod paymentMethod);

    void setProducts(product *products);

    void setDiscountPercentageFraction(double percentageFraction);
    void resetCouponDiscount();
    double getDiscountPercentageFraction();
    double getPriceCorrectedForSelectedSize(int pnumber, bool maximumVolumeForCustom);
    double getDiscountAmount(double price);
    double getPriceWithDiscount(double price);

    QStringList getChildNames(QObject *parent);
    void loadDynamicContent();
    void loadBottle();
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
    void loadTextsFromDefaultHardwareCsv();
    void loadTextsFromDefaultCsv();
    void loadElementDynamicPropertiesFromTemplate();
    void loadElementDynamicPropertiesFromDefaultHardwareTemplate();
    void loadElementDynamicPropertiesFromDefaultTemplate();

    QString getHardwareMajorVersion();
    bool isAelenPillarElseSoapStand();

    void addPictureToLabel(QLabel *label, QString picturePath);
    void addPictureToLabelCircle(QLabel *label, QString picturePath);
    void addPictureToButton(QPushButton *button, QString picturePath);
    void addClientLogoToLabel(QLabel *label);
    void setBackgroundPictureFromTemplateToPage(QWidget *page, QString imageName);
    void setBackgroundPictureToQWidget(QWidget *page, QString imageName);
    void pageTransition(QWidget *pageToHide, QWidget *pageToShow);
    void applyPropertiesToQWidget(QWidget *widget);
    void applyDynamicPropertiesFromTemplateToWidgetChildren(QWidget *widget);

    bool isProductVolumeInContainer(int pnumber);

    // void activateKeyboard(QWidget *page);

    QString m_machine_id;
    QString m_client_id;
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
    QString m_size_unit;
    double m_temperature;
    double m_alert_temperature;
    QString m_software_version_controller;
    double m_temperature2;
    double m_alert_temperature2;
    QString m_payment;
    int m_screen_sleep_time24h;
    int m_screen_wakeup_time24h;
    int m_buy_bottle_1;
    int m_buy_bottle_2;
    QString m_portal_base_url;

    int m_is_enabled;
    QString m_status_text;

    QString m_min_threshold_vol_ml_discount;
    QString m_max_threshold_vol_ml_discount;
    QString m_max_dollar_amount_discount;

    df_util *dfUtility;

    QJsonObject m_propertiesObject;
    void loadElementPropertiesFile();

    QString m_session_id;

    void loadTextsFromCsv(QString csv_path, std::map<QString, QString> *dictionary);

    QVector<int> getAllUsedPNumbersFromSlots();
    QVector<int> getAllUniqueDispensePNumbers();
    QVector<int> getAllDispensePNumbersFromSlot(int slot);

    void resetTransactionLogging();
    void addToTransactionLogging(QString text);
    QString getTransactionLogging();
    bool hasMixing();
    QString getPortalBaseUrl();

public slots:

signals:

private:
    dispenser_slot *m_selectedSlot; // used for maintenance mode!!  , or derived from selectedProduct.
    pnumberproduct *m_selectedProduct;
    pnumberproduct *m_selectedBottle;
    QVector<int> dispenseProductsMenuOptions;
    dispenser_slot *m_slots;
    pnumberproduct m_pnumberproducts[HIGHEST_PNUMBER_COUNT];

    std::map<QString, QString> textNameToTextMap_template;
    std::map<QString, QString> textNameToTextMap_default_hardware;
    std::map<QString, QString> textNameToTextMap_default;
    std::map<QString, QString> elementDynamicPropertiesMap_template;
    std::map<QString, QString> elementDynamicPropertiesMap_default_hardware;
    std::map<QString, QString> elementDynamicPropertiesMap_default;
    QString m_templatePath;

    QTime temperatureTooHighStartMillis;
    bool isTemperatureTooHigh = false;
    StateCoupon m_stateCoupon;
    double m_discount_percentage_fraction = 0.0;

    double max_discount = 0.0;
    QString m_promoCode;
    StateReboot m_stateReboot;
    ActivePaymentMethod m_activePaymentMethod;
    std::vector<ActivePaymentMethod> allowedPaymentMethods;
    DbManager *m_db;
    UserRole active_role;

    QString m_pump_id_slots[MAX_SLOT_COUNT];
    int m_is_enabled_slots[MAX_SLOT_COUNT];
    QString m_status_text_slots[MAX_SLOT_COUNT];
    QString transactionLogging;
};

#endif // MACHINE_H
