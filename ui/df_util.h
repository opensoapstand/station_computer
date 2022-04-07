#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>
#include "drinkorder.h"
#include "dfuicommthread.h"
#include "dfuiserver.h"

// TODO: Refactor to fit with dfuicommthread
// #define USE_OLD_DATABASE
//#define START_FSM_FROM_UI //enabled by default (start controller from ui)

#define GENERIC_PRODUCT_SELECT
#define ENABLE_COUPON   // Petros stations have no coupon

#define OPTION_SLOT_INVALID 0
#define SLOT_COUNT 4  // number of products

#define SIZES_COUNT 6

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
// #define DB_PATH "/release/db/sqlite/drinkfill-sqlite.db"
//#define DB_PATH "/home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db"
 
#define TRANSACTION_DISPENSE_END_OFFINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_dispense_end_%1.txt"
#define TRANSACTIONS_RESTOCK_OFFINE_PATH "/home/df-admin/production/logging/transactions/failed_curl_transaction_restock_%1.txt"
//  QString log_file_base_path = "/home/df-admin/production/logging/ui/ui_%1.txt"; // https://stackoverflow.com/questions/4784155/how-to-format-a-qstring
//     QString log_file_path = QString(log_file_base_path).arg(time_stamp_date);

#define DB_PATH_CLICKS "/release/db/sqlite/clicks.db"
#define DB_PATH_TEMPERATURE "/release/db/sqlite/temperature.db"

#define ML_TO_OZ 0.033814


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
#define SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS 3000
#define QR_PROCESSED_PERIODICAL_CHECK_SECONDS 5


#define PAGE_INIT_BACKGROUND_PATH           "/home/df-admin/production/references/templates/nada/background_init.png"
#define PAGE_IDLE_BACKGROUND_PATH           "/home/df-admin/production/references/templates/nada/background_welcome.png"
#define PAGE_SELECT_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/background_product_select.png"
#define PAGE_HELP_BACKGROUND_PATH           "/home/df-admin/production/references/templates/nada/background_help.png"
#define PAGE_PRODUCT_BACKGROUND_PATH        "/home/df-admin/production/references/templates/nada/background_product.png"
#define PAGE_DISPENSE_BACKGROUND_PATH       "/home/df-admin/production/references/templates/nada/background_dispense_instructions.png"
#define PAGE_THANK_YOU_BACKGROUND_PATH      "/home/df-admin/production/references/templates/nada/background_thank_you.png"
#define PAGE_QR_PAY_BACKGROUND_PATH         "/home/df-admin/production/references/templates/nada/background_pay_qr.png"

#define PAGE_WIFI_ERROR_BACKGROUND_PATH     "/home/df-admin/production/references/oops.png"
// #define PAGE_IDLE_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/1_welcome_panel.png"
// #define PAGE_SELECT_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/2_soap_page_1.png"


// #define PAGE_SELECT_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/2_drink_page_1.png"
// #define PAGE_HELP_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/3_help_page.png"

// #define PAGE_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/4_pay_select_page_s_3-1.png"
// #define PAGE_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/4_pay_select_page_s_1.png"
// #define PAGE_DISPENSE_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/5_dispense_page_before.png"

// #define PAGE_THANK_YOU_BACKGROUND_PATH "/home/df-admin/production/references/templates/nada/7_thank_you_page.png"



#define PRODUCT_PICTURES_ROOT_PATH "/home/df-admin/production/references/products/%1.jpeg"

// #define PAGE_PRODUCT_1_L_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_l_1.png"
// #define PAGE_PRODUCT_2_L_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_l_2.png"
// #define PAGE_PRODUCT_3_L_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_l_3.png"
// #define PAGE_PRODUCT_4_L_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_l_4.png"
// #define PAGE_PRODUCT_1_S_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_s_1.png"
// #define PAGE_PRODUCT_2_S_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_s_2.png"
// #define PAGE_PRODUCT_3_S_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_s_3.png"
// #define PAGE_PRODUCT_4_S_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_s_4.png"

#define PRODUCT_1_IMAGE_PATH "/home/df-admin/production/references/product1.png"
#define PRODUCT_2_IMAGE_PATH "/home/df-admin/production/references/product2.png"
#define PRODUCT_3_IMAGE_PATH "/home/df-admin/production/references/product3.png"
#define PRODUCT_4_IMAGE_PATH "/home/df-admin/production/references/product4.png"

#define BOTTLE_FILL_FOR_ANIMATION_IMAGE_PATH "/home/df-admin/production/references/bottle_fill_for_animation.png"
#define SOLD_OUT_IMAGE_PATH "/home/df-admin/production/references/soldOut.png"
#define KEYBOARD_IMAGE_PATH "/home/df-admin/production/references/soapstand-keyboard.png"
#define FULL_TRANSPARENT_IMAGE_PATH "/home/df-admin/production/references/background.png"

// #define PAGE_PRODUCT_BACKGROUND_PATH "/ghome/df-admin/production/references/page_product_generic.png"

#define ICON_TYPE_CONCENTRATE_PATH "/home/df-admin/production/references/Soapstand_UI-concentrate-icon.png"
#define ICON_TYPE_ALL_PURPOSE_PATH "/home/df-admin/production/references/Soapstand-UI-icon-all-purposs.png"
#define ICON_TYPE_DISH_PATH "/home/df-admin/production/references/Soapstand-UI-icon-dish.png"
#define ICON_TYPE_HAND_PATH "/home/df-admin/production/references/Soapstand-UI-icon-hand-soap.png"
#define ICON_TYPE_LAUNDRY_PATH "/home/df-admin/production/references/Soapstand-UI-icon-laundry.png"
                         
#define COMPANY_LOGO_PATH "/home/df-admin/production/references/logos/%1_logo_white.png"
//#define COMPANY_LOGO_PATH "/home/df-admin/drinkfill/ui/references/logos/"
// #define PAGE_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/4_pay_select_page_l_1.png"
// #define PAGE_SELECT_PRODUCT_BACKGROUND_PATH "/home/df-admin/production/references/page_select_product_generic.png" // todo

#define SEND_DISPENSE_START "d"
#define SEND_DISPENSE_STOP "f"

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
