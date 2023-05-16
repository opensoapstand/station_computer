#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
// #include "drinkorder.h"
#include "dfuicommthread.h"
#include "dfuiserver.h"

// TODO: Refactor to fit with dfuicommthread
// #define USE_OLD_DATABASE
//#define START_FSM_FROM_UI //enabled by default (start controller from ui)

#define UI_VERSION "1.5+"

#define ENABLE_COUPON   // Petros stations have no coupon

#define OPTION_SLOT_INVALID 0
#define SLOT_COUNT 4  // number of products

#define SIZES_COUNT 6
#define MINIMUM_DISPENSE_VOLUME_ML 10.0
#define SIZE_INVALID_INDEX 0
#define SIZE_SMALL_INDEX 1
#define SIZE_MEDIUM_INDEX 2
#define SIZE_LARGE_INDEX 3
#define SIZE_CUSTOM_INDEX 4
#define SIZE_TEST_INDEX 5


#ifndef START_FSM_FROM_UI
//#define WAIT_FOR_CONTROLLER_READY // will wait for response of controller before continuing.
#endif

#ifdef USE_OLD_DATABASE
#define DB_PATH "/home/df-admin/production/db/drinkfill-sqlite.db"
#else
#define DB_PATH "/home/df-admin/production/db/drinkfill-sqlite_newlayout.db"
#endif

#define PRODUCT_DETAILS_TSV_PATH "/home/df-admin/production/references/products/product_details.tsv"  // https://docs.google.com/spreadsheets/d/17WR2gRyPIDIlGKBy1YKFAqN-Hyw_3VOJ6JCmfcAtjVk/edit#gid=169583479 download as .tsv file
#define CSV_PRODUCT_COL_ID 0
#define CSV_PRODUCT_COL_NAME 1
#define CSV_PRODUCT_COL_TYPE 2
#define CSV_PRODUCT_COL_SUPPLIER 3
#define CSV_PRODUCT_COL_BRAND 4
#define CSV_PRODUCT_COL_INGREDIENTS 5
#define CSV_PRODUCT_COL_LOCATION 6
#define CSV_PRODUCT_COL_NAME_UI 7
#define CSV_PRODUCT_COL_DESCRIPTION_UI 8
#define CSV_PRODUCT_COL_FEATURES_UI 9
#define CSV_PRODUCT_COL_INGREDIENTS_UI 10
#define CSV_PRODUCT_COL_NOTES 11

// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
//#define DB_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db"


// send all transaction to the 
#define TRANSACTION_DISPENSE_END_OFFINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_dispense_end.txt"
#define TRANSACTIONS_RESTOCK_OFFINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_restock.txt"

// #define TRANSACTIONS_RESTOCK_OFFINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_restock%1.txt"
//  QString log_file_base_path = "/home/df-admin/production/logging/ui/ui_%1.txt"; // https://stackoverflow.com/questions/4784155/how-to-format-a-qstring
//     QString log_file_path = QString(log_file_base_path).arg(time_stamp_date);

#define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
#define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

#define ML_TO_OZ 0.033814
#define OZ_TO_ML 29.5735
#define VOLUME_TO_TREAT_CUSTOM_DISPENSE_AS_PER_100G 2999.0


using namespace std;

// typedef enum FSM_COMM {
//     SEND_EMPTY = 0,
//     SEND_DISPENSE_START,
//     SEND_DISPENSE_STOP, // used to be SEND_CLEAN
//     SEND_PWM,
//     SEND_ERROR,
// } FSM_COMM;
#define QR_PAGE_TIMEOUT_SECONDS 420
#define QR_PAGE_TIMEOUT_WARNING_SECONDS 120
#define PAGE_MAINTENANCE_DISPENSER_TIMEOUT_SECONDS 600
#define PAGE_MAINTENANCE_TIMEOUT_SECONDS 60
#define PAGE_THANK_YOU_TIMEOUT_SECONDS 7
#define SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS 3000
#define QR_PROCESSED_PERIODICAL_CHECK_SECONDS 5

 

#define TEMPLATES_ROOT_PATH "/home/df-admin/production/references/templates/"
#define TEMPLATES_DEFAULT_NAME "default"
#define TRANSACTION_HISTORY_COUNT 15

#define PRODUCT_PICTURES_ROOT_PATH "/home/df-admin/production/references/products/%1.png"





#define PAGE_HELP_TEXT                                   "page_help_text.txt"

#define PAGE_HELP_CSS                                   "page_help.css"
#define PAGE_FEEDBACK_CSS                               "page_sendFeedback.css"
#define PAGE_IDLE_CSS                                   "page_idle.css"
#define PAGE_SELECT_PRODUCT_CSS                             "page_select_product.css"


#define PAGE_IDLE_BACKGROUND_PATH                       "background_message.png"
#define PAGE_IDLE_PRODUCTS_BACKGROUND_PATH              "background_product_select.png"
#define PAGE_THANK_YOU_BACKGROUND_PATH                  "background_message.png"
#define PAGE_TRANSACTIONS_BACKGROUND_PATH               "background_message.png"
#define PAGE_SELECT_PRODUCT_BACKGROUND_PATH             "background_product_select.png"
#define PAGE_PRODUCT_BACKGROUND_PATH                    "background_product_quantity.png"
#define PAGE_ORDER_OVERVIEW_PATH                        "background_generic_white_empty.png"
#define PAGE_DISPENSE_INSTRUCTIONS_BACKGROUND_PATH      "background_dispense_instructions.png"
#define PAGE_DISPENSE_INSTRUCTIONS_MULTISPOUT_BACKGROUND_PATH      "background_dispense_instructions_multispout.png"
#define PAGE_DISPENSE_BACKGROUND_PATH                   "background_dispense.png"
#define PAGE_QR_PAY_BACKGROUND_PATH                     "background_generic_white.png"
#define PAGE_MAINTENANCE_BACKGROUND_PATH                "background_generic_white_empty.png"
#define ERROR_MESSAGE_PATH                              "error_message.png"
#define PAGE_TAP_PAY                                    "tapNow.png"
#define PAGE_TAP_PAY_SUCCESS                            "paymentSuccess.png"
#define PAGE_TAP_PAY_FAIL                               "paymentFailed.png"
#define PAGE_AUTHORIZE_NOW                              "authorizeNow.png"
#define PAGE_TAP_GENERIC                                "genericTap.png"
#define BOTTLE_FILL_FOR_ANIMATION_IMAGE_PATH            "bottle_fill_for_animation.png"
#define PAGE_SEND_FEEDBACK_PATH                         "background_sendfeedback.png"
#define PAGE_INIT_BACKGROUND_IMAGE_PATH                 "background_message.png"
#define IMAGE_BUTTON_HELP                               "help_icon.png"
#define THANK_YOU_FOR_YOUR_FEEDBACK                     "background_feedbacksent.png"

#define KEYBOARD_IMAGE_PATH                         "/home/df-admin/production/references/soapstand-keyboard.png"
#define FULL_TRANSPARENT_IMAGE_PATH                 "/home/df-admin/production/references/background.png"

#define ICON_TYPE_CONCENTRATE_PATH                  "Soapstand_UI-concentrate-icon.png"
#define ICON_TYPE_ALL_PURPOSE_PATH                  "Soapstand-UI-icon-all-purposs.png"
#define ICON_TYPE_DISH_PATH                         "Soapstand-UI-icon-dish.png"
#define ICON_TYPE_HAND_PATH                         "Soapstand-UI-icon-hand-soap.png"
#define ICON_TYPE_LAUNDRY_PATH                      "Soapstand-UI-icon-laundry.png"
                         
#define DRINKFILL_LOGO_ANIMATED_PATH                "/home/df-admin/production/references/logos/Soapstand-logo-animated-spin.gif"
#define COMPANY_LOGO_PATH                           "/home/df-admin/production/references/logos/%1_logo_white.png"
#define DRINKFILL_LOGO_VERTICAL_PATH                 "/home/df-admin/production/references/logos/Soapstand-logo-white-vertical.png"
#define PAGE_HELP_BACKGROUND_PATH                    "background_help.png"


// #define PAGE_INIT_BACKGROUND_IMAGE_PATH "/home/df-admin/production/references/0_background_init.png"
// 

#define SEND_DISPENSE_START "d"
#define SEND_DISPENSE_STOP "f"
#define SEND_REPAIR_PCA "pcabugfix"
#define SEND_DISPENSE_AUTOFILL "a"

class df_util : public QWidget
{
    Q_OBJECT
public:
    explicit df_util(QWidget *parent = nullptr);

    static bool fileExists(QString path);
    static double convertMlToOz(double vol_ml);
    static double convertOzToMl(double vol_oz);
    static char sizeIndexToChar(int size_index);
    static QString getConvertedStringVolumeFromMl(double volumeMilliLiter, QString units, bool roundNumber, bool addUnits);
    
    void write_to_file_timestamped(QString basePath, QString data);
    void write_to_file(QString path, QString data);

    // static long getTimeStamp();
    // static string format_string(long time_stamp,string fmt,int cutBack=0);

    // void initialize_local_db();
    // bool open_local_db();
    // bool close_local_db();
    // bool getVendorDetails();
    // QString get_local_db_max_transaction();
    // void send_command_to_FSM();
    void send_command_to_FSM(QString command);
    void set_message_to_send_to_FSM(QString msg);

    //    void setIsSendingFSM(bool isSendingFSM){m_IsSendingFSM = isSendingFSM;};
    //    bool getIsSendingFSM(){return m_IsSendingFSM;};

    bool m_IsSendingFSM;
    // FSM_COMM m_fsmMsg; // Sets type of message/command

    QString send_msg;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;

    // void send_to_FSM();

protected:
    // FSM communication

    // TODO: move host and port to XML
    const char *host = "localhost";
    int port = 1234;

    QSqlDatabase db;

public slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void send_to_FSM();

private:
    // QString remote_psql_db_path;
    // QString remote_psql_db_name;
};

#endif // DF_UTIL_H
