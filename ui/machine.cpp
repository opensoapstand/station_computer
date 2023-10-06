

#include "df_util.h" // lode added for settings
#include "dbmanager.h"
#include "machine.h"
#include "product.h"

machine::machine()
{
    setRole(UserRole::user);
    setCouponState(no_state);

    // IPC Networking
    dfUtility = new df_util();
}

// Dtor
machine::~machine()
{
}

void machine::setDb(DbManager *db)
{
    m_db = db;
}

StateCoupon machine::getCouponState()
{
    return m_stateCoupon;
}

void machine::setProducts(product *products)
{
    m_products = products;
}
bool machine::isDispenseAreaBelowElseBesideScreen()

{
    // check in database if hardware_version starts with AP or SS
    // get hardware_version from db_manager
    // if starts with SS return false, if starts with ap reeturn true

    if (m_hardware_version.startsWith("SS1"))
    {
        return false;
    }
    return true;
}

bool machine::isAelenPillarElseSoapStand()
{
    // check in database if hardware_version starts with AP or SS
    // get hardware_version from db_manager
    // if starts with SS return false, if starts with ap reeturn true

    if (m_hardware_version.startsWith("AP"))
    {
        return true;
    }
    else if (m_hardware_version.startsWith("SS"))
    {
        return false;
    }
    else
    {
        qDebug() << "ASSERT ERROR: Unknown Hardware version:  " << m_hardware_version;
    }
    return false; // default case
}

int machine::getSlotCount()
{
    //check hardwarenumber
    int slot_count;
    if (m_hardware_version.startsWith("AP")){
        if(m_hardware_version == "AP1"){
            slot_count = 4;
        } 
        else if (m_hardware_version == "AP1.1"){
            slot_count = 4;
        } else{
            slot_count = 8;
        }
    }
    else if (m_hardware_version.startsWith("SS"))
    {
        if(m_hardware_version == "SS1"){
            slot_count = 4;
        }
        else if (m_hardware_version == "SS2"){
            slot_count = 4;
        } else{
            slot_count = 8;
        }
    }
    cout << g_slotcount;
    return slot_count;
    // dispensers is the same as slots.

    //return m_dispense_buttons_count % 1000;
}

void machine::setCouponState(StateCoupon state)
{
    m_stateCoupon = state;
}

void machine::dispenseButtonLightsAnimateState(bool animateElseOff)
{
    if (animateElseOff)
    {
        dfUtility->send_command_to_FSM("DispenseButtonLights|ANIMATE", true);
    }
    else
    {
        dfUtility->send_command_to_FSM("DispenseButtonLights|OFF", true);
    }
}

void machine::initCouponState()
{

    if (getCouponsEnabled())
    {
        m_stateCoupon = enabled_not_set;
    }
    else
    {
        m_stateCoupon = disabled;
    }

    setDiscountPercentageFraction(0.0);
    setPromoCode("");
    m_max_dollar_amount_discount = "0.0";
}

void machine::setDiscountPercentageFraction(double percentageFraction)
{
    // ratio = percentage / 100;
    qDebug() << "Set discount percentage as a fraction. " << QString::number(percentageFraction, 'f', 3);
    m_discount_percentage_fraction = percentageFraction;
}

// void machine::setConditionalDiscount(const std::map<std::string, std::string>& dictionary){
//     qDebug() <<
// }

double machine::getDiscountPercentageFraction()
{
    return m_discount_percentage_fraction;
}

double machine::getDiscountAmount(double price)
{
    // the discount is the original price minus the discounted price
    return price - getPriceWithDiscount(price);
}

double machine::getPriceWithDiscount(double price)
{
    double discount = price * m_discount_percentage_fraction;
    qDebug() << m_max_dollar_amount_discount;
    max_discount = m_max_dollar_amount_discount.toDouble();
    double result = (max_discount == 0.0) ? discount : std::min(max_discount, discount);
    return (price - result);
}

QString machine::getPromoCode()
{
    return m_promoCode;
}

void machine::setPromoCode(QString promoCode)
{
    qDebug() << "Set Promo Code: " << promoCode;
    m_promoCode = promoCode;
}

QString machine::getTemplateFolder()
{
    QString template_name = m_template;
    if (template_name == "")
    {
        template_name = "default";
    }
    else
    {
    }
    return TEMPLATES_ROOT_PATH + template_name + "/";
}

QString machine::getTemplateName()
{
    QString template_name = m_template;
    if (template_name == "")
    {
        template_name = "default";
    }
    return template_name;
}

void machine::loadElementPropertiesFile()
{
    QString json_path = getTemplatePathFromName(UI_ELEMENT_PROPERTIES_PATH);
    QFile file(json_path);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray jsonData = file.readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        m_propertiesObject = jsonDoc.object();

        file.close();

        // Now you have the propertiesObject containing properties for listed elements
    }

    // QList<QPushButton *> buttonList = centralWidget->findChildren<QPushButton *>();

    // foreach (QPushButton *button, buttonList) {
    //     qDebug() << "Button text:" << button->text();
    // }
}

// void machine::setExternalPropertiesToWidget(QWidget tmp){

// }

void machine::loadProductPropertiesFromProductsFile(QString soapstand_product_number, QString *name, QString *name_ui, QString *product_type, QString *description_ui, QString *features_ui, QString *ingredients_ui)
{
    QFile file(PRODUCT_DETAILS_TSV_PATH);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR: Opening product details file. Expect unexpected behaviour now! ";
        return;
    }

    QTextStream in(&file);
    qDebug() << "Load csv file with product properties";
    while (!in.atEnd())
    {
        QString line = in.readLine();

        QStringList fields = line.split("\t");
        int compareResult = QString::compare(fields[CSV_PRODUCT_COL_ID], soapstand_product_number, Qt::CaseSensitive);
        if (compareResult == 0)
        {
            // qDebug() << "compare result is 0";
            *name = fields[CSV_PRODUCT_COL_NAME];
            *name_ui = fields[CSV_PRODUCT_COL_NAME_UI];
            *product_type = fields[CSV_PRODUCT_COL_TYPE];
            *description_ui = fields[CSV_PRODUCT_COL_DESCRIPTION_UI];
            *features_ui = fields[CSV_PRODUCT_COL_FEATURES_UI];
            *ingredients_ui = fields[CSV_PRODUCT_COL_INGREDIENTS_UI];
            break;
        }
    }
    file.close();
}

QString machine::getTemplatePathFromName(QString fileName)
{
    QString image_path = getTemplateFolder() + fileName;

    if (!df_util::pathExists(image_path))
    {
        QString image_default_path = getDefaultTemplatePathFromName(fileName);
        if (!df_util::pathExists(image_default_path))
        {
            qDebug() << "File not found in template folder and not in default template folder (will use path anyways...): " + image_default_path;
        }
        image_path = image_default_path;
    }
    return image_path;
}

QString machine::getDefaultTemplatePathFromName(QString fileName)
{
    QString template_root_path = TEMPLATES_ROOT_PATH;
    return template_root_path + TEMPLATES_DEFAULT_NAME + "/" + fileName;
}

void machine::getPrinterStatusFromDb(bool *isOnline, bool *hasPaper)
{
    qDebug() << "DB call: Check printer status. ";
    m_db->getPrinterStatus(isOnline, hasPaper);
}
void machine::writeTemperatureToDb(double temperature_1, double temperature_2)
{
    // qDebug() << "DB call: Add temperature record ";
    double defaultTemperature2 = 0.0; // Default value for temperature2
    QString defaultAlert = "";        // Default alert message (can be adjusted as needed)
    m_db->addTemperature(getMachineId(), temperature_1, temperature_2, defaultAlert);
}

void machine::checkForHighTemperatureAndDisableProducts()
{
    if (isTemperatureTooHigh_1())
    {
        if (!temperatureWasHigh)
        {
            temperatureWasHigh = true;
            temperatureHighTime = QTime::currentTime(); // Record the time when temperature became too high
        }

        QTime currentTime = QTime::currentTime();
        int elapsedMinutes = temperatureHighTime.msecsTo(currentTime) / 60000; // Convert milliseconds to minutes 60000=60min

        if (elapsedMinutes >= 60) // 60  Check if one hour has passed
        {
            for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
            {
                qDebug() << "Temperature too high for one hour, block all slots.";
                m_products[slot_index].setSlotEnabled(true, "SLOT_STATE_DISABLED_COMING_SOON");
            }
        }
    }
    else
    {
        temperatureWasHigh = false; // Reset the flag and the time when the temperature goes back to normal
    }
}
bool machine::isTemperatureTooHigh_1()
{
    // qDebug() << "alert temperature: " << m_alert_temperature;
    // qDebug() << "current temperature: " << m_temperature;
    if (m_alert_temperature > 100.0) // if higher than 100 we consider the value bogus (sensor broken, dummy value...)
    {
        return false;
    }
    return m_temperature > m_alert_temperature;
}

void machine::fsmReceiveTemperature(double temperature_1, double temperature_2)
{
    m_temperature = temperature_1;
    m_temperature2 = temperature_2;
    // qDebug() << "Temperature received from FSM in machine: " << m_temperature;
    // qDebug() << "Temperature 2 received from FSM in machine: " << m_temperature2;

    writeTemperatureToDb(m_temperature, m_temperature2);

    if (isTemperatureTooHigh_1())
    {
        // ui->label_printer_status->setText("Temperature= " + QString::number(temperature, 'f', 2) + " is too high")
        qDebug() << "Temperature 1 too high";
    }
}

double machine::getTemperature_1()
{
    return m_temperature;
}

void machine::getTemperatureFromController()
{
    dfUtility->send_command_to_FSM("getTemperature", false);
}

bool machine::hasReceiptPrinter()
{
    return (m_has_receipt_printer == 1);
}

bool machine::getPumpRampingEnabled()
{
    return m_enable_pump_ramping == 1;
}

void machine::setPumpRampingEnabled(bool isEnabled)
{

    m_db->updateTableMachineWithInt("enable_pump_ramping", isEnabled);
}

void machine::setEmptyContainerDetectionEnabled(bool isEnabled)
{
    m_db->updateTableMachineWithInt("has_empty_detection", isEnabled);
}

bool machine::getEmptyContainerDetectionEnabled()
{
    return m_has_empty_detection == 1;
}
bool machine::getShowTransactionHistory()
{
    return m_show_transactions == 1;
}

void machine::processRolePassword(QString password_input)
{
    int compareResult_maintainer = QString::compare(password_input, m_maintenance_pwd, Qt::CaseInsensitive);
    int compareResult_admin = QString::compare(password_input, m_admin_pwd, Qt::CaseInsensitive);

    // if admin password is the same as maintainer password, make sure to set as admin!
    if (compareResult_admin == 0)
    {
        setRole(UserRole::admin);
    }
    else if (compareResult_maintainer == 0)
    {
        setRole(UserRole::maintainer);
    }
    else
    {
        setRole(UserRole::user);
        qDebug() << "Provided password not correct. Check machine tabel in database or contact soapstand.";
    }
}

QString machine::getSessionId()
{
    return m_session_id;
}

void machine::resetSessionId()
{
    m_session_id = "";
}
void machine::createSessionId()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_session_id = time;
}

QString machine::getActiveRoleAsText()
{
    switch (active_role)
    {
    case user:
        return "User";
    case maintainer:
        return "Maintainer";
    case admin:
        return "Admin";
    default:
        return "Unknown";
    }
}

bool machine::isAllowedAsAdmin()
{
    bool allowed = false;
    if (active_role == UserRole::admin)
    {
        allowed = true;
    }
    qDebug() << "Allowed as admin? " << allowed;
    return allowed;
}

bool machine::isAllowedAsMaintainer()
{
    bool allowed = false;
    if (active_role == UserRole::maintainer || active_role == UserRole::admin)
    {
        allowed = true;
    }
    return allowed;
}

void machine::setRole(UserRole role)
{
    if (active_role != role)
    {
        active_role = role;
        qDebug() << "Role set as: " << getActiveRoleAsText();
    }
}

void machine::setStatusText(int slot, bool isSlotEnabled, QString status)
{
    QString column = QString("status_text_slot_%1").arg(slot);
    m_db->updateTableMachineWithText(column, status);
}

QString machine::getStatusText(int slot)
{
    slotNumberValidityCheck(slot);
    return m_status_text_slots[slot - 1];
}

QString machine::getPumpId(int slot)
{
    slotNumberValidityCheck(slot);
    return m_pump_id_slots[slot - 1];
}

bool machine::slotNumberValidityCheck(int slot)
{
    bool valid = true;
    if (slot < 1)
    {
        valid = false;
    }
    if (slot > SLOT_COUNT)
    {
        valid = false;
    }
    if (!valid)
    {

        qDebug() << "Invalid slot! slots start from 1 and go up to " << SLOT_COUNT << " e.g. 1,2,3,4. Slot provided: " << slot;
    }
    return valid;
}

void machine::setSlotEnabled(int slot, bool isEnabled)
{
    // do this through product.cpp, as this should have been a part of products table
    QString column_name = QString("is_enabled_slot_%1").arg(slot);
    m_is_enabled_slots[slot - 1] = isEnabled; // Global variable
    m_db->updateTableMachineWithInt(column_name, isEnabled);
}

bool machine::getSlotEnabled(int slot)
{
    // this should have been part of the products table. But it isn't. We access this from the product.cpp class.
    slotNumberValidityCheck(slot);
    return (m_is_enabled_slots[slot - 1] == 1);
}

QString machine::getHelpPageHtmlText()
{
    return m_help_text_html;
}

QString machine::getIdlePageType()
{
    return m_idle_page_type;
}

void machine::loadParametersFromDb()
{
    qDebug() << "DB call: Load all machine parameters";

    m_db->getAllMachineProperties(
        &m_machine_id,
        &m_soapstand_customer_id,
        &m_template,
        &m_location,
        &m_controller_type,
        &m_controller_id,
        &m_screen_type,
        &m_screen_id,
        &m_has_receipt_printer,
        &m_receipt_printer_is_online,
        &m_receipt_printer_has_paper,
        &m_has_tap_payment,
        &m_hardware_version,
        &m_software_version,
        &m_aws_port,
        &m_coupons_enabled,
        &m_has_empty_detection,
        &m_enable_pump_ramping,
        &m_enable_pump_reversal,
        &m_dispense_buttons_count,
        &m_maintenance_pwd,
        &m_show_transactions,
        &m_help_text_html,
        &m_idle_page_type,
        &m_admin_pwd,
        m_pump_id_slots,
        m_is_enabled_slots,
        m_status_text_slots,
        &m_alert_temperature);

    qDebug() << "Machine ID as loaded from db: " << getMachineId();
    qDebug() << "Template folder from db : " << getTemplateFolder();
}



bool machine::getCouponsEnabled()
{
    qDebug() << "coupons enabled>>> :: " << m_coupons_enabled;
    return m_coupons_enabled == 1;
}
QString machine::getMachineId()
{
    return m_machine_id;
}

QString machine::getCustomerId()
{
    QString id = m_soapstand_customer_id;
    if (m_soapstand_customer_id.at(0) != 'C')
    {
        qDebug() << "WARNING: invalid customer ID. Should be with a format like C-1, C-374, ... . Provided id: " << id;
    }
    return m_soapstand_customer_id;
}

void machine::setCouponConditions(QString couponConditions)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(couponConditions.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    m_min_threshold_vol_ml_discount = jsonObj["min_threshold_vol_ml"].toString();
    m_max_threshold_vol_ml_discount = jsonObj["max_threshold_vol_ml"].toString();
    m_max_dollar_amount_discount = jsonObj["max_dollar_amount_discount"].toString();
}

std::map<QString, QString> machine::getCouponConditions()
{
    std::map<QString, QString> myMap;
    myMap["m_min_threshold_vol_ml_discount"] = m_min_threshold_vol_ml_discount;
    myMap["m_max_threshold_vol_ml_discount"] = m_max_threshold_vol_ml_discount;
    myMap["m_max_dollar_amount_discount"] = m_max_dollar_amount_discount;
    return myMap;
}
