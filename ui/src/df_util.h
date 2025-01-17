#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
#include "dfuicommthread.h"
#include "dfuiserver.h"

#include <QVector>


// TODO: Refactor to fit with dfuicommthread
#define UI_VERSION "3.10"

#define GET_MIX_INFO_FROM_DB_INSTEAD_OF_TSV // normally, all pnumber info comes from the tsv file. However, for quick testing and changing, the db is easier. 

#define OPTION_SLOT_INVALID 0
#define SELECT_PRODUCT_PAGE_SLOT_COUNT_MAX 4
#define MAINTENANCE_PAGE_SLOT_COUNT_MAX 4

#define SIZES_COUNT 7
#define MINIMUM_DISPENSE_VOLUME_ML 10.0
#define SIZE_INVALID_INDEX 0
#define SIZE_SMALL_INDEX 1
#define SIZE_MEDIUM_INDEX 2
#define SIZE_LARGE_INDEX 3
#define SIZE_CUSTOM_INDEX 4
#define SIZE_TEST_INDEX 5
#define SIZE_SAMPLE_INDEX 6
#define ADDITIVES_RATIO_INCREMENT 10


#define ADDITIVES_PER_SLOT_COUNT_MAX 5
#define BASE_LINE_COUNT_MAX 5 // maximum amount of base lines
#define MAX_SLOT_COUNT BASE_LINE_COUNT_MAX // number of slots
#define DISPENSE_PRODUCTS_PER_BASE_LINE_MAX 6   // drinks per base line  (not dynamic, redo ui elements in qt creator when changing... )
#define MENU_PRODUCT_SELECTION_OPTIONS_MAX BASE_LINE_COUNT_MAX * DISPENSE_PRODUCTS_PER_BASE_LINE_MAX // the offered selection of product to the user
#define DUMMY_PNUMBER 2
#define DEFAULT_PAGE_INIT_TIMEOUT_SECONDS 120

#define HIGHEST_PNUMBER_COUNT 1000 // WARNING: this is not the amount of pnumber loaded in the machine, but the amount of pnumbers existing. in this array, even if we only have 10 pnumbers loaded, P-88 will reside at index 88

#define CONFIG_DB_PATH "/home/df-admin/production/db/configuration.db"
#define USAGE_DB_PATH "/home/df-admin/production/db/usage.db"

// #define INTERRUPT_DRIVE_FLOW_SENSOR_TICKS  // enable for global interrupt per flow sensor tick. This is the old style. New style is polling of the pin state over i2c.
#define PRODUCT_DETAILS_TSV_PATH "/home/df-admin/production/references/products/product_details.tsv" // https://docs.google.com/spreadsheets/d/17WR2gRyPIDIlGKBy1YKFAqN-Hyw_3VOJ6JCmfcAtjVk/edit#gid=169583479 download as .tsv file
#define UI_TEXTS_CSV_NAME "ui_texts.csv"
#define UI_ELEMENT_PROPERTIES_NAME "ui_element_properties.txt"
#define CSV_PRODUCT_COL_ID 0
#define CSV_PRODUCT_COL_NAME 1
#define CSV_PRODUCT_COL_TYPE 2
#define CSV_PRODUCT_COL_MIX_PNUMBERS 3
#define CSV_PRODUCT_COL_MIX_RATIOS_LOW 4
#define CSV_PRODUCT_COL_MIX_RATIOS_DEFAULT 5
#define CSV_PRODUCT_COL_MIX_RATIOS_HIGH 6
#define CSV_PRODUCT_COL_SUPPLIER 7
#define CSV_PRODUCT_COL_BRAND 8
#define CSV_PRODUCT_COL_LOCATION 9
#define CSV_PRODUCT_COL_INGREDIENTS 10
#define CSV_PRODUCT_COL_PRODUCT_NAME_SHORT 11
#define CSV_PRODUCT_COL_PRODUCT_TITLE 12
#define CSV_PRODUCT_COL_PRODUCT_TAG_LINE 13
#define CSV_PRODUCT_COL_PRODUCT_NAME_RECEIPT 14
#define CSV_PRODUCT_COL_NAME_UI 15
#define CSV_PRODUCT_COL_DESCRIPTION_UI 16
#define CSV_PRODUCT_COL_FEATURES_UI 17
#define CSV_PRODUCT_COL_INGREDIENTS_UI 18
#define CSV_PRODUCT_COL_CALIBRATION 19
#define CSV_PRODUCT_COL_NOTES 20
#define CSV_PRODUCT_COL_URL 21

#define TRANSACTION_DISPENSE_END_OFFLINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_dispense_end.txt"
#define TRANSACTIONS_RESTOCK_OFFLINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_restock.txt"

#define ML_TO_OZ 0.033814
#define OZ_TO_ML 29.5735
#define VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G 2999.0

using namespace std;


#define PAGE_INIT_READY_TIMEOUT_SECONDS 120
#define PAGE_INIT_REBOOT_TIMEOUT_SECONDS 3600
#define PAGE_IDLE_DELAY_BEFORE_ENTERING_IDLE_PRODUCTS 15
#define PAGE_IDLE_DELAY_BEFORE_ENTERING_IDLE_PRODUCTS 15
#define STATUS_BAR_REFRESH_PERIOD_SECONDS 3
#define PAGE_IDLE_POLL_TEMPERATURE_PERIOD_SECONDS 60 // 60
#define PAGE_IDLE_TEST_FOR_FROZEN_SCREEN_PERIOD_SECONDS 60
#define PAGE_IDLE_USER_ROLE_TIMEOUT_SECONDS 5
#define PAGE_BOTTLE_PAGE_TIMEOUT_SECONDS 18
#define PAGE_IDLE_PRODUCTS_MAIN_PAGE_DISPLAY_TIME_SECONDS 6
#define PAGE_IDLE_PRODUCTS_STEP_DISPLAY_TIME_SECONDS 1
#define PAGE_IDLE_REBOOT_NIGHTLY_TIMEOUT_SECONDS 1
#define PAGE_IDLE_RECEIPT_PRINTER_TIMEOUT_SECONDS 3
#define PAGE_IDLE_REBOOT_NIGHTLY_TIMER_COUNT_DOWN 300

#define PAGE_PAYMENT_TAP_SERIAL_PAGE_TIMEOUT_SECONDS 60
#define PAGE_PAYMENT_TAP_TCP_PAGE_TIMEOUT_SECONDS 300
#define QR_PAGE_TIMEOUT_SECONDS  420
#define QR_PAGE_TIMEOUT_WARNING_SECONDS  120
#define PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS 600
#define PAGE_MAINTENANCE_TIMEOUT_SECONDS 1800
#define PAGE_MAINTENANCE_GENERAL_TIMEOUT_SECONDS 3600
#define PAGE_THANK_YOU_TIMEOUT_SECONDS 7
#define SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS 3000
#define QR_PROCESSED_PERIODICAL_CHECK_SECONDS 5
#define MESSAGE_BOX_TIMEOUT_EXIT_QR_CONFIRM_SECONDS 10
#define MESSAGE_BOX_TIMEOUT_EXIT_TAP_CONFIRM_SECONDS 10
#define MESSAGE_BOX_TIMEOUT_DEFAULT_MILLIS 10000

#define CONTAINER_EMPTY_THRESHOLD_ML 2000

#define TEMPLATES_ROOT_PATH "/home/df-admin/production/references/templates/"
#define TEMPLATES_DEFAULT_NAME "default"
#define TRANSACTION_HISTORY_COUNT 15

#define PRODUCT_PICTURES_ROOT_PATH                      "/home/df-admin/production/references/products/%1.png"
#define CLIENT_LOGO_PATH                                "/home/df-admin/production/references/logos/%1_logo_white.png"
#define PAGE_HELP_CSS                                   "page_help.css"
#define PAGE_HOWTO_CSS                                  "page_how_to.css"
#define PAGE_FEEDBACK_CSS                               "page_sendFeedback.css"
#define STATUSBAR_CSS                                   "statusbar.css"
#define INPUT_WIDGET_CSS                                "input_widget.css"
#define KEYBOARD_CSS                                    "keyboard.css"
#define PAGE_INIT_CSS                                   "page_init.css"
#define PAGE_IDLE_CSS                                   "page_idle.css"
#define PAGE_BUY_BOTTLE_CSS                             "page_buyBottle.css"
#define PAGE_PRODUCT_MENU_CSS                           "page_product_menu.css"
#define PAGE_SELECT_PRODUCT_CSS                         "page_select_product.css"
#define PAGE_PRODUCT_CSS                                "page_product.css"
#define PAGE_PRODUCT_MIXING_CSS                         "page_product_mixing.css"
#define PAGE_DISPENSER_CSS                              "page_dispenser.css"
#define PAGE_PRODUCT_OVERVIEW_CSS                       "page_product_overview.css"
#define PAGE_PRODUCT_FREESAMPLE_CSS                     "page_product_freeSample.css"
#define PAGE_END_CSS                                    "page_end.css"
#define PAGE_QR_PAYMENT_CSS                             "page_qr_payment.css"
#define PAGE_OFFLINE_PAYMENT_CSS                        "page_offline_payment.css"
#define PAGE_ERROR_WIFI_CSS                             "page_error_wifi.css"
#define PAGE_TAP_PAYMENT_CSS                            "page_payment_tap_tcp.css"
#define PAGE_TRANSACTIONS_CSS                           "page_transactions.css"
#define PAGE_MAINTENANCE_CSS                            "page_maintenance.css"
#define PAGE_MAINTENANCE_DISPENSER_CSS                  "page_maintenance_dispenser.css"
#define PAGE_IDLE_PRODUCTS_CSS                          "page_idle_products.css"
#define PAGE_TAP_PAYMENT_SERIAL_CSS                     "page_payment_tap_serial.css"
#define PAGE_EMAIL_CSS                                  "page_email.css"

#define PAGE_IDLE_BACKGROUND_PATH                       "background_idle.png"
#define PAGE_BUY_BOTTLE_BACKGROUND_PATH                 "background_buy_bottle.png"
#define PAGE_IDLE_PRODUCTS_BACKGROUND_PATH              "background_idle_products.png"
#define PAGE_END_BACKGROUND_PATH                        "background_end.png"
#define PAGE_TRANSACTIONS_BACKGROUND_PATH               "background_transactions.png"

#define PAGE_SEND_FEEDBACK_BACKGROUND_PATH              "background_sendFeedback.png"
#define CHECKBOX_UNCHECKED_PATH                         "checkbox_unchecked.png"
#define CHECKBOX_CHECKED_PATH                           "checkbox_checked.png"
#define PAGE_PRODUCT_MENU_BACKGROUND_PATH               "background_product_menu.png"
#define PAGE_SELECT_PRODUCT_BACKGROUND_PATH             "background_product_select.png"
#define PAGE_PRODUCT_BACKGROUND_PATH                    "background_product_quantity.png"
#define PAGE_ORDER_OVERVIEW_PATH                        "background_order_overview.png"
#define PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH      "background_dispense_instructions.png"
// #define PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH   "background_dispense_instructions_multispout.png"
#define PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_RIGHT    "arrow_right_white_big.png"
#define PAGE_DISPENSE_INSTRUCTIONS_SPOUT_INDICATOR_DOWN "arrow_down_white_big.png"
#define PAGE_IDLE_WARNING_ICON "warning_icon.png"
#define PAGE_DISPENSE_BACKGROUND_PATH "background_dispense.png"
#define PAGE_DISPENSE_ALERT_ICON "label_alert_icon.png"
#define PAGE_DISPENSE_VOLUME_ICON "label_volume_icon.png"
#define PAGE_DISPENSE_PRODUCT_ICON "label_product_icon.png"
#define PAGE_QR_PAY_BACKGROUND_PATH "background_qr.png"
#define PAGE_QR_WARNING_ICON "page_qr_warning_icon.png"
#define PAGE_MAINTENANCE_BACKGROUND_PATH "background_maintenance.png"
#define ERROR_MESSAGE_PATH "error_message.png"
#define PAGE_TAP_CANCEL "tapCancel.png"
#define PAGE_TAP_PAY_SUCCESS "paymentSuccess.png"
#define PAGE_TAP_PAY_FAIL "paymentFailed.png"
#define PAGE_TAP_PAY "tapNow.png"
#define PAGE_AUTHORIZE_NOW "authorizeNow.png"
#define PAGE_TAP_GENERIC "genericTap.png"
#define PAGE_INIT_BACKGROUND_IMAGE_PATH "background_init.png"
#define IMAGE_BUTTON_HELP "help_icon.png"
#define THANK_YOU_FOR_YOUR_FEEDBACK "background_feedbacksent.png"
#define PAGE_ERROR_BACKGROUND_PATH "background_error_wifi.png"
#define KEYBOARD_IMAGE_PATH "soapstand-keyboard.png"
#define UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_UPPERCASE_IMAGE_PATH "universal_lineEdit_with_caps_uppercase_keyboard.png"
#define UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_LOWERCASE_IMAGE_PATH "universal_lineEdit_with_caps_lowercase_keyboard.png"
#define UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_WITH_CANCEL_UPPERCASE_IMAGE_PATH "universal_lineEdit_with_caps_with_cancel_uppercase_keyboard.png"
#define UNIVERSAL_LINE_EDIT_KEYBOARD_WITH_CAPS_WITH_CANCEL_LOWERCASE_IMAGE_PATH "universal_lineEdit_with_caps_with_cancel_lowercase_keyboard.png"
#define UNIVERSAL_LINE_EDIT_KEYBOARD_WITHOUT_CAPS_IMAGE_PATH "universal_lineEdit_without_caps_keyboard.png"
#define UNIVERSAL_TEXT_EDIT_KEYBOARD_UPPERCASE_IMAGE_PATH "universal_textEdit_uppercase_keyboard.png"
#define UNIVERSAL_TEXT_EDIT_KEYBOARD_LOWERCASE_IMAGE_PATH "universal_textEdit_lowercase_keyboard.png"
#define COUPON_ICON_UNAVAILABLE_PATH "coupon_unavailable.png"
#define COUPON_ICON_AVAILABLE_PATH "coupon_available.png"
#define MACHINE_LOGO_PATH "machine_logo.png"
#define REBOOT_NIGHTLY_ICON_PATH "reboot_nightly_icon.png"
#define QR_MANUAL_PATH "qr_user_manual.png"
#define PAGE_DISPENSE_FILL_ANIMATION "bottle_fill_for_animation.png"

#define ICON_TYPE_CONCENTRATE_PATH "Soapstand_UI-concentrate-icon.png"
#define ICON_TYPE_ALL_PURPOSE_PATH "Soapstand-UI-icon-all-purpose.png"
#define ICON_TYPE_DISH_PATH "Soapstand-UI-icon-dish.png"
#define ICON_TYPE_HAND_PATH "Soapstand-UI-icon-hand-soap.png"
#define ICON_TYPE_LAUNDRY_PATH "Soapstand-UI-icon-laundry.png"
#define ICON_TYPE_KOMBUCHA_PATH "Soapstand-UI-icon-kombucha.png"
#define ICON_TYPE_PROBIOTIC_PATH "Soapstand-UI-icon-probiotic.png"
#define ICON_TYPE_JUICE_PATH "Soapstand-UI-icon-juice.png"
#define ICON_TYPE_TEA_PATH "Soapstand-UI-icon-tea.png"
#define ICON_TYPE_DEFAULT "Soapstand-UI-icon-default.png"

#define PAGE_HELP_BACKGROUND_GENERIC_WHITE "background_generic_white_empty.png"
#define PAGE_HELP_BACKGROUND_PATH "background_help.png"
#define PAGE_HOWTO_BACKGROUND_PATH "background_how_to.png"
#define PAGE_HOWTO_STEP1 "page_howTo_step1.png"
#define PAGE_HOWTO_STEP2 "page_howTo_step2.png"
#define PAGE_HOWTO_STEP3 "page_howTo_step3.png"
#define SEND_DISPENSE_START "d"
// #define SEND_DISPENSE_STOP "f"
#define SEND_REPAIR_PCA "pcabugfix"
#define SEND_DISPENSE_AUTOFILL "a"

#define PAYMENT_QR                                      "qr"
#define PAYMENT_QR_EMAILFREE                            "qr/emailfree"   // for ubc
#define PAYMENT_TAP_CANADA                              "tap_canada"
#define PAYMENT_TAP_USA                                 "tap_usa"
#define PAYMENT_TAP_CANADA_QR                           "tap_canada_qr"
#define PAYMENT_TAP_USA_QR                              "tap_usa_qr"
#define PAYMENT_RECEIPT_PRINTER_BARCODE_EAN13           "barcode_EAN-13"
#define PAYMENT_RECEIPT_PRINTER_BARCODE_EAN2            "barcode_EAN-2"
#define PAYMENT_RECEIPT_PRINTER_BARCODE                 "barcode"
#define PAYMENT_RECEIPT_PRINTER_PLU                     "plu"
#define PAYMENT_NONE                                    ""

//Portal API Endpoints

//Page QR Payment
#define PORTAL_PING                                     "api/machine_data/ping"
#define PORTAL_CREATE_ORDER                             "api/machine_data/createOrderInDb"
#define PORTAL_CHECK_ORDER_STATUS                       "api/machine_data/check_order_status"
//Page End
#define PORTAL_UPDATE_ALREADY_CREATED_ORDER             "api/machine_data/updateOrder"
#define PORTAL_SEND_NEW_COMPLETE_ORDER                  "api/machine_data/pushPrinterOrder"
#define PORTAL_SEND_NEW_COMPLETE_ORDER_DEV              "api/order/sendCompleteNewOrder"

//Feedback page
#define PORTAL_SEND_FEEDBACK                            "api/alert/sendFeedbackEmail"
//Maintenance Page
#define PORTAL_RESET_STOCK                              "api/machine_data/resetStock"
#define PORTAL_UPDATE_PRODUCT_FROM_STATION              "api/product/update_product_from_station"

#define SOAPSTAND_URL                                   "http://soapstandportal.com"

// typedef enum ProductState
// {
//    PRODUCT_STATE_AVAILABLE = 0,
//    PRODUCT_STATE_AVAILABLE_LOW_STOCK,
//    PRODUCT_STATE_EMPTY
// } ProductState;

// static const QMap<QString, ProductState> ProductStateStringMap = {
//     {"PRODUCT_STATE_AVAILABLE", PRODUCT_STATE_AVAILABLE},
//     {"PRODUCT_STATE_AVAILABLE_LOW_STOCK", PRODUCT_STATE_AVAILABLE_LOW_STOCK},
//     {"PRODUCT_STATE_PROBLEM_EMPTY", PRODUCT_STATE_EMPTY}

//     // only consider the state of the product when enabled. Otherwise, it's a mix of enabled status and product level statuses.

//     // {"PRODUCT_STATE_NOT_PRIMED", PRODUCT_STATE_NOT_PRIMED},
//     // {"PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION", PRODUCT_STATE_PROBLEM_NEEDS_ATTENTION},
//     // {"PRODUCT_STATE_DISABLED", PRODUCT_STATE_DISABLED},
//     // {"PRODUCT_STATE_INVALID", PRODUCT_STATE_INVALID}
// };

typedef enum ProductState
{
   PRODUCT_STATE_AVAILABLE = 0,
   PRODUCT_STATE_AVAILABLE_LOW_STOCK,
   PRODUCT_STATE_PROBLEM_EMPTY,
   PRODUCT_STATE_DISABLED,
   PRODUCT_STATE_INVALID
} ProductState;

static const QMap<QString, ProductState> ProductStateStringMap = {
    {"PRODUCT_STATE_AVAILABLE", PRODUCT_STATE_AVAILABLE},
    {"PRODUCT_STATE_AVAILABLE_LOW_STOCK", PRODUCT_STATE_AVAILABLE_LOW_STOCK},
    {"PRODUCT_STATE_PROBLEM_EMPTY", PRODUCT_STATE_PROBLEM_EMPTY},
    {"PRODUCT_STATE_DISABLED", PRODUCT_STATE_DISABLED},
    {"PRODUCT_STATE_INVALID", PRODUCT_STATE_INVALID}
};

typedef enum DispenseBehaviour
{
   FLOW_STATE_UNAVAILABLE = 0,
   FLOW_STATE_RAMP_UP,
   FLOW_STATE_DISPENSING,
   FLOW_STATE_PUMPING_NOT_DISPENSING,
   FLOW_STATE_NOT_PUMPING_NOT_DISPENSING,
   FLOW_STATE_PRIMING_OR_EMPTY,
   FLOW_STATE_PRIME_FAIL_OR_EMPTY,
   FLOW_STATE_EMPTY
} DispenseBehaviour;

static const QMap<QString, DispenseBehaviour> DispenseBehaviourStringMap = {
    {"FLOW_STATE_UNAVAILABLE", FLOW_STATE_UNAVAILABLE},
    {"FLOW_STATE_RAMP_UP", FLOW_STATE_RAMP_UP},
    {"FLOW_STATE_DISPENSING", FLOW_STATE_DISPENSING},
    {"FLOW_STATE_PUMPING_NOT_DISPENSING", FLOW_STATE_PUMPING_NOT_DISPENSING},
    {"FLOW_STATE_NOT_PUMPING_NOT_DISPENSING", FLOW_STATE_NOT_PUMPING_NOT_DISPENSING},
    {"FLOW_STATE_PRIMING_OR_EMPTY", FLOW_STATE_PRIMING_OR_EMPTY},
    {"FLOW_STATE_PRIME_FAIL_OR_EMPTY", FLOW_STATE_PRIME_FAIL_OR_EMPTY},
    {"FLOW_STATE_EMPTY", FLOW_STATE_EMPTY}
};


typedef enum SlotState
{
   SLOT_STATE_AVAILABLE = 0,
//    SLOT_STATE_WARNING_PRIMING,
   SLOT_STATE_PROBLEM_NEEDS_ATTENTION,
   SLOT_STATE_DISABLED,
   SLOT_STATE_INVALID
} SlotState;

static const QMap<QString, SlotState> SlotStateStringMap = {
    {"SLOT_STATE_AVAILABLE", SLOT_STATE_AVAILABLE},
    // {"SLOT_STATE_WARNING_PRIMING", SLOT_STATE_WARNING_PRIMING},
    {"SLOT_STATE_PROBLEM_NEEDS_ATTENTION", SLOT_STATE_PROBLEM_NEEDS_ATTENTION},
    {"SLOT_STATE_DISABLED", SLOT_STATE_DISABLED},
    {"SLOT_STATE_INVALID", SLOT_STATE_INVALID}
};

class df_util : public QWidget
{
    Q_OBJECT
public:
    explicit df_util(QWidget *parent = nullptr);

    static QString convertSlotStatusToString(SlotState state);
    static QString convertProductStatusToString(ProductState state);
    static QString convertDispenseStatusToString(DispenseBehaviour state);

    static void warnIfPathDoesNotExist(QString path);
    static bool pathExists(QString path);
    static QStringList getFileList(const QString &folderPath);
    static double convertMlToOz(double vol_ml);
    static double convertOzToMl(double vol_oz);
    static char sizeIndexToChar(int size_index);
    static QString getConvertedStringVolumeFromMl(double volumeMilliLiter, QString units, bool roundNumber, bool addUnits);
    static void csvQStringToQVectorInt(const QString &csvList, QVector<int> &returnIntList);
    static void csvQStringToQVectorDouble(const QString &csvList, QVector<double> &returnDoubleList);

    void write_to_file_timestamped(QString basePath, QString data);
    void write_to_file(QString path, QString data);

    void send_command_to_FSM(QString command, bool isLoggingMessage);

    static QJsonObject parseJsonString(QString jsonString);
    bool m_IsSendingFSM;

    static void executeVirtualClick(int x, int y);
    // QString send_msg;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;

protected:
    const char *host = "localhost";
    int port = 1234;

    QSqlDatabase db;

public slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void send_to_FSM(QString command, bool isLoggingMessage);

private:
};

#endif // DF_UTIL_H
