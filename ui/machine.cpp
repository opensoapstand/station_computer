

#include "df_util.h" // lode added for settings
#include "dbmanager.h"
#include "machine.h"
#include "product.h"
#include <QtWidgets>
#include <map>
#include <fstream>

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

void machine::initMachine()
{
    loadParametersFromDb();
    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {

        m_products[slot_index].setSlot(slot_index + 1);
        m_products[slot_index].setMachine(this);
        m_products[slot_index].setDb(m_db);
    }
    loadDynamicContent();
}

void machine::loadDynamicContent()
{
    // load global machine data
    loadParametersFromDb();

    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        m_products[slot_index].loadProductProperties();
    }

    loadTextsFromTemplateCsv();                                // dynamic content (text by template)
    loadTextsFromDefaultHardwareCsv();                         // dynamic styling (css by template)
    loadTextsFromDefaultCsv();                                 // dynamic styling (css by template)
    loadElementDynamicPropertiesFromTemplate();                // dynamic elements (position, visibility)
    loadElementDynamicPropertiesFromDefaultHardwareTemplate(); // dynamic elements (position, visibility)
    loadElementDynamicPropertiesFromDefaultTemplate();         // dynamic elements (position, visibility)
}

DbManager *machine::getDb()
{
    return m_db;
}

void machine::setDb(DbManager *db)
{
    m_db = db;
}

product *machine::getProduct(int slot)
{
    if (slot == 0)
    {

        qDebug() << "ERROR: slot numbers start from 1!!!";
    }
    return &m_products[slot - 1];
}

void machine::setSelectedProduct(uint8_t slot)
{
    selectedProduct = &m_products[slot - 1];
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

QString machine::getHardwareMajorVersion()
{
    return m_hardware_version.left(3);
}

int machine::getSlotCount()
{
    // check hardwarenumber
    int slot_count;
    if (m_hardware_version.startsWith("AP"))
    {
        if (m_hardware_version.startsWith("AP1"))
        {
            slot_count = 4;
        }
        else if (m_hardware_version.startsWith("AP1.1"))
        {
            slot_count = 4;
        }
        else
        {
            slot_count = 8;
        }
    }
    else if (m_hardware_version.startsWith("SS"))
    {
        if (m_hardware_version.startsWith("SS1"))
        {
            slot_count = 4;
        }
        else if (m_hardware_version.startsWith("SS2"))
        {
            slot_count = 4;
        }
        else
        {
            qDebug() << "Warning, unrecognized Hardware version. Version in db: " << m_hardware_version;
            slot_count = 4;
        }
    }
    if (compareSlotCountToMaxSlotCount(slot_count))
    {
        qDebug() << "ERROR - Slot Count:" << slot_count << " exceeded MAX_SLOT_COUNT:" << MAX_SLOT_COUNT << "threshold";
    }
    //  qDebug() << "AMOUNT OFF SLOTTST. " << slot_count;
    
    return slot_count;
    // dispensers is the same as slots.

    // return m_dispense_buttons_count % 1000;
}

bool machine::compareSlotCountToMaxSlotCount(int slot_count)
{
    return (slot_count > MAX_SLOT_COUNT);
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

void machine::resetUserState()
{
    setRole(UserRole::user);
    resetSessionId();  // fixme! bug! if manually exited, will not reset session id.
    initCouponState(); // everything coupon is reset when idle page is reached.
}

void machine::setCouponState(StateCoupon state)
{
    m_stateCoupon = state;
}
StateCoupon machine::getCouponState()
{
    return m_stateCoupon;
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
    setCouponCode("");
    m_max_dollar_amount_discount = "0.0";
}

void machine::setRebootState(StateReboot state){
    m_stateReboot = state;
}
StateReboot machine::getRebootState(){
    return m_stateReboot;
}

void machine::setDiscountPercentageFraction(double percentageFraction)
{
    // ratio = percentage / 100;
    qDebug() << "Set discount percentage as a fraction. " << QString::number(percentageFraction, 'f', 3);
    m_discount_percentage_fraction = percentageFraction;
}

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

QString machine::getCouponCode()
{
    return m_promoCode;
}

void machine::setCouponCode(QString promoCode)
{
    qDebug() << "Set Promo Code: " << promoCode;
    m_promoCode = promoCode;
}

void machine::loadElementPropertiesFile()
{
    QString json_path = getTemplatePathFromName(UI_ELEMENT_PROPERTIES_NAME);
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

QString machine::getTemplateFolder()
{

    // check for exact template folder.
    // if it doesn't exist, check for hardware default template folder.
    // if it doesn't exist, check for default template folder.

    QString template_name = m_template;
    if (template_name.isEmpty())
    {
        QString hardware_specific_template = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion();
        if (!df_util::pathExists(hardware_specific_template))
        {

            template_name = QString(TEMPLATES_DEFAULT_NAME);
        }
        else
        {
            template_name = QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion();
        }
    }

    return QString(TEMPLATES_ROOT_PATH) + template_name + "/";
}

QString machine::getTemplatePathFromName(QString fileName)
{
    // will try to find the file in specif template, default_hardware and default in this sequence. Return path from first found

    QString filePath = getTemplateFolder() + fileName; // this checks just for the existence of the most relevant template folder

    if (!df_util::pathExists(filePath))
    {
        qDebug() << "File not found in template folder : " + filePath;
        // check if file exist in hardware specific default template
        // e.g.  // "/home/df-admin/production/references/templates/default_SS2/page_idle.css"
        filePath = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion() + "/" + fileName;

        if (!df_util::pathExists(filePath))
        {
            qDebug() << "File not found in default hardware folder : " + filePath;
            // check if file exists in default template
            filePath = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "/" + fileName;
            if (!df_util::pathExists(filePath))
            {
                qDebug() << "File not found in template folder and not in default hardware folder and not in default template folder (will use path anyways...): " + fileName;
            }
        }
    }
    return filePath;
}

// QString machine::getHardwareDefaultTemplatePathFromName(QString fileName)
// {

//     return TEMPLATES_ROOT_PATH + "default_" + getHardwareMajorVersion + "/" + fileName;
// }

// QString machine::getDefaultTemplatePathFromName(QString fileName)
// {
//     // last resort super default.
//     return TEMPLATES_ROOT_PATH + TEMPLATES_DEFAULT_NAME + "/" + fileName;
// }

// QString machine::getTemplatePathFromName(QString fileName)
// {
//     QString image_path = getTemplateFolder() + fileName;

//     if (!df_util::pathExists(image_path))
//     {
//         QString image_default_path = getDefaultTemplatePathFromName(fileName);
//         if (!df_util::pathExists(image_default_path))
//         {
//             qDebug() << "File not found in template folder and not in default template folder (will use path anyways...): " + image_default_path;
//         }
//         image_path = image_default_path;
//     }
//     return image_path;
// }

// QString machine::getDefaultTemplatePathFromName(QString fileName)
// {
//     QString template_root_path = TEMPLATES_ROOT_PATH;
//     return template_root_path + TEMPLATES_DEFAULT_NAME + "/" + fileName;
// }

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
        if (!isTemperatureTooHigh)
        {
            isTemperatureTooHigh = true;
            temperatureTooHighStartMillis = QTime::currentTime(); // Record the time when temperature became too high
        }

        QTime currentTime = QTime::currentTime();
        int elapsedMinutes = temperatureTooHighStartMillis.msecsTo(currentTime) / 60000; // Convert milliseconds to minutes 60000=60min

        if (elapsedMinutes >= 60) // 60  Check if one hour has passed
        {
            for (uint8_t slot_index = 0; slot_index < getSlotCount(); slot_index++)
            {
                qDebug() << "Temperature too high for one hour, block all slots.";
                setSlotEnabled(slot_index + 1, true, "SLOT_STATE_DISABLED_COMING_SOON");
            }
        }
    }
    else
    {
        isTemperatureTooHigh = false; // Reset the flag and the time when the temperature goes back to normal
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

bool machine::isSessionLocked(){
    bool sessionActive = false;
    if (getRole() != UserRole::user)
    {
        sessionActive = true;
    }
    if (getCouponState() == enabled_valid_active)
    {
        sessionActive = true;
    }
    return sessionActive;
}

void machine::resetSessionId()
{
    m_session_id = "";
}

void machine::createSessionId()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_session_id = time;
    qDebug() << "Create Session id: " << m_session_id;
}

void machine::setRole(UserRole role)
{
    if (active_role != role)
    {
        active_role = role;
        qDebug() << "Role set as: " << getActiveRoleAsText();
    }
}

UserRole machine::getRole()
{
    return active_role;
}

QString machine::getActiveRoleAsText()
{
    switch (active_role)
    {
        // WARNING: DO NOT CHANGE TEXT, change in the templates ui_texts.csv if displaying the text needs to be different.
    case user:
        return "user";
    case maintainer:
        return "maintainer";
    case admin:
        return "admin";
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

void machine::setStatusText(int slot, QString status)
{
    QString column = QString("status_text_slot_%1").arg(slot);
    m_db->updateTableMachineWithText(column, status);
}

void machine::setSlotEnabled(int slot, bool isEnabled, QString statusText)
{
    setSlotEnabled(slot, isEnabled);
    setStatusText(slot, statusText);
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

void machine::registerUserInteraction(QWidget *page)
{
    QString page_name = page->objectName();
    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< User entered: " + page_name + " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
    QString event = "Entered Page";
    m_db->addUserInteraction(getSessionId(), getActiveRoleAsText(), page_name, event);
}

bool machine::slotNumberValidityCheck(int slot)
{
    bool valid = true;
    if (slot < 1)
    {
        valid = false;
    }
    if (slot > getSlotCount())
    {
        valid = false;
    }
    if (!valid)
    {

        qDebug() << "Invalid slot! slots start from 1 and go up to " << getSlotCount() << " e.g. 1,2,3,4. Slot provided: " << slot;
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

void machine::loadParametersFromDb()
{
    qDebug() << "DB call: Load all machine parameters";

    m_db->getAllMachineProperties(
        &m_machine_id,
        &m_client_id,
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

// for products.cpp
product *machine::getSelectedProduct()
{
    return selectedProduct;
}

QString machine::getIdlePageType()
{
    return m_idle_page_type;
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

QString machine::getClientId()
{
    QString id = m_client_id;
    if (m_client_id.at(0) != 'C')
    {
        qDebug() << "WARNING: invalid client ID. Should be with a format like C-1, C-374, ... . Provided id: " << id;
    }
    return m_client_id;
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

void machine::addClientLogoToLabel(QLabel *label)
{
    QString id = getClientId();
    QString logo_path = QString(CLIENT_LOGO_PATH).arg(id);
    addPictureToLabel(label, logo_path);
}

void machine::addPictureToButton(QPushButton *button, QString picturePath)
{
    QString p = selectedProduct->getProductPicturePath();
    if (df_util::pathExists(p))
    {
        QPixmap im(p);
        QIcon qi(im);
        button->setIcon(qi);
        button->setIconSize(QSize(271, 391));
    }
}

void machine::addPictureToLabel(QLabel *label, QString picturePath)
{
    if (df_util::pathExists(picturePath))
    {
        QPixmap picture(picturePath);

        int w = label->width();
        int h = label->height();

        // // set a scaled pixmap to a w x h window keeping its aspect ratio
        label->setPixmap(picture.scaled(w, h, Qt::KeepAspectRatio));
    }
    else
    {
        qDebug() << "Can't add picture to label: " << label->objectName() << " " << picturePath;
    }
}

void machine::addCssClassToObject(QWidget *element, QString classname, QString css_file_name)
{
    QString styleSheet = getCSS(css_file_name);
    element->setProperty("class", classname);
    element->setStyleSheet(styleSheet);
}

QString machine::getCSS(QString cssName)
{
    QString cssFilePath = getTemplatePathFromName(cssName);

    QFile cssFile(cssFilePath);
    QString styleSheet = "";
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        styleSheet = QString::fromUtf8(cssFile.readAll());
    }
    else
    {
        qDebug() << "CSS file could not be opened." << cssFilePath;
    }
    return styleSheet;
}

void machine::pageTransition(QWidget *pageToHide, QWidget *pageToShow)
{
    // page transition effects are not part of QT but of the operating system! // search for ubuntu settings program to set transition animations to "off"
    pageToShow->showFullScreen();
    pageToHide->hide();
}

void machine::setBackgroundPictureFromTemplateToPage(QWidget *p_widget, QString imageName)
{
    // on Page: if called from setPage: will not scale
    // on Page: if called from showEvent: will scale to screen

    QString image_path = imageName;
    image_path = getTemplatePathFromName(imageName);
    // qDebug()<< "background image path: " << image_path;
    setBackgroundPictureToQWidget(p_widget, image_path);
}

void machine::setBackgroundPictureToQWidget(QWidget *p_widget, QString image_path)
{
    QPixmap background(image_path);

    // background = background.scaled(p_widget->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    p_widget->setPalette(palette);
    p_widget->repaint();
    p_widget->update();
}

void machine::setTemplateTextWithIdentifierToObject(QWidget *p_element, QString identifier)
{
    QString text = getTemplateTextByElementNameAndPageAndIdentifier(p_element, identifier);
    setTextToObject(p_element, text);
}

QString machine::getTemplateTextByElementNameAndPageAndIdentifier(QWidget *p_element, QString identifier)
{
    // QString element_page_and_name = getTemplateTextByElementNameAndPage(p_element);
    QString element_page_and_name = getCombinedElementPageAndName(p_element);

    QString searchString = element_page_and_name + "->" + identifier;
    return getTemplateText(searchString);
}

void machine::setTemplateTextToObject(QWidget *p_element)
{
    QString searchString = getTemplateTextByElementNameAndPage(p_element);
    setTextToObject(p_element, searchString);
}

QString machine::getCombinedElementPageAndName(QWidget *p_element)
{
    QString elementName = p_element->objectName();
    QWidget *parentWidget = p_element->parentWidget();

    if (!parentWidget)
    {
        qDebug() << "No parent for the provided widget!! " << elementName;
    }

    QString pageName = parentWidget->objectName();

    return pageName + "->" + elementName;
}

QString machine::getTemplateTextByElementNameAndPage(QWidget *p_element)
{
    QString pageName_elementName_combination = getCombinedElementPageAndName(p_element);
    return getTemplateText(pageName_elementName_combination);
}

void machine::setTextToObject(QWidget *p_element, QString text)
{
    if (QLabel *label = qobject_cast<QLabel *>(p_element))
    {
        label->setText(text);
    }
    else if (QPushButton *button = qobject_cast<QPushButton *>(p_element))
    {
        button->setText(text);
    }
    else if (QMessageBox *msgBox = qobject_cast<QMessageBox *>(p_element))
    {
        msgBox->setText(text);
    }
    else
    {
        // Handle other types of elements if needed
    }
}

// get a text that is not linked to an elements on a specific page by its identifier
QString machine::getTemplateTextByPage(QWidget *page, QString identifier)
{
    QString pageName = page->objectName();
    QString searchString = pageName + "->" + identifier;
    return getTemplateText(searchString);
}

QString machine::getTemplateText(QString textName_to_find)
{
    std::string key = textName_to_find.toStdString();
    auto it = textNameToTextMap_template.find(QString::fromStdString(key));
    QString retval;
    if (it != textNameToTextMap_template.end())
    {
        // std::cout << "Word found! Sentence: " << it->second ;
        retval = it->second;
    }
    else
    {
        it = textNameToTextMap_default_hardware.find(QString::fromStdString(key));
        if (it != textNameToTextMap_default_hardware.end())
        {
            retval = it->second;
        }
        else
        {
            it = textNameToTextMap_default.find(QString::fromStdString(key));
            if (it != textNameToTextMap_default.end())
            {
                retval = it->second;
            }
            else
            {
                qDebug() << "No template text value found for: " + textName_to_find;
                retval = textName_to_find;
            }
        }
    }

    // \n values in the csv file get automatically escaped. We need to deescape them.
    retval.replace("\\n", "\n");

    return retval;
}

void machine::applyDynamicPropertiesFromTemplateToWidgetChildren(QWidget *widget)
{
    // Template engine can set properties of a widget (like position, visibility,...) from a text file

    // in reality, send a page widget as argument. All the childeren will be checked. (i.e. buttons, labels,...)
    QList<QObject *> allChildren = widget->findChildren<QObject *>();
    foreach (QObject *child, allChildren)
    {
        QWidget *widget = qobject_cast<QWidget *>(child);
        if (widget)
        {
            // not all child element are widgets.

            QString combinedName = getCombinedElementPageAndName(widget);
            // qDebug() << combinedName;
            applyPropertiesToQWidget(widget);
        }
    }
}

void machine::applyPropertiesToQWidget(QWidget *widget)
{

    // example of line in text file with properties:
    // machine->label_client_logo,{"x":570, "y":1580, "width":351, "height":211,"isVisibleAtLoad":true}

    QString combinedName = getCombinedElementPageAndName(widget);

    auto it = elementDynamicPropertiesMap_template.find(combinedName);
    QString jsonString;
    bool valid = true;

    if (it != elementDynamicPropertiesMap_template.end())
    {
        qDebug() << "element " << combinedName << "found in template. json string: " << it->second;
        jsonString = it->second;
    }
    else
    {

        it = elementDynamicPropertiesMap_default_hardware.find(combinedName);
        if (it != elementDynamicPropertiesMap_default_hardware.end())
        {
            qDebug() << "element " << combinedName << "found in default hardware. json string: " << it->second;
            jsonString = it->second;
        }
        else
        {
            it = elementDynamicPropertiesMap_default.find(combinedName);
            if (it != elementDynamicPropertiesMap_default.end())
            {
                qDebug() << "element " << combinedName << "found in default. json string: " << it->second;
                jsonString = it->second;
            }
            else
            {
                // qDebug() << "No template text value found for: " + combinedName;
                valid = false;
            }
        }
    }

    if (!valid)
    {
        return;
    }

    QJsonObject jsonObject = df_util::parseJsonString(jsonString);

    int x = widget->x();
    int y = widget->y();
    int width = widget->width();
    int height = widget->height();

    if (jsonObject.contains("x"))
    {
        QJsonValue xValue = jsonObject.value("x");
        x = xValue.toInt();
        // qDebug() << "x found  " << x ;
    }
    if (jsonObject.contains("y"))
    {
        QJsonValue val = jsonObject.value("y");
        y = val.toInt();
        // qDebug() << "y found  " << y ;
    }
    if (jsonObject.contains("width"))
    {
        QJsonValue val = jsonObject.value("width");
        width = val.toInt();
        // qDebug() << "width found  " << width ;
    }
    if (jsonObject.contains("height"))
    {
        QJsonValue val = jsonObject.value("height");
        height = val.toInt();
        // qDebug() << "height found  " << height ;
    }
    widget->setGeometry(x, y, width, height);

    if (jsonObject.contains("isVisibleAtLoad"))
    {
        QJsonValue val = jsonObject.value("isVisibleAtLoad");
        bool isVisible = val.toBool();
        // qDebug() << "visibility found  " << isVisible ;

        widget->setVisible(isVisible);
    }
}

void machine::loadElementDynamicPropertiesFromTemplate()
{
    qDebug() << "Load dynamic properties from template file";
    QString path = QString(TEMPLATES_ROOT_PATH) + m_template + "/" + QString(UI_ELEMENT_PROPERTIES_NAME);
    loadTextsFromCsv(path, &elementDynamicPropertiesMap_template);
}
void machine::loadElementDynamicPropertiesFromDefaultHardwareTemplate()
{
    qDebug() << "Load dynamic properties from default hardware template file";
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion() + "/" + QString(UI_ELEMENT_PROPERTIES_NAME);
    loadTextsFromCsv(path, &elementDynamicPropertiesMap_default_hardware);
}
void machine::loadElementDynamicPropertiesFromDefaultTemplate()
{
    qDebug() << "Load dynamic properties from default template file";
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "/" + QString(UI_ELEMENT_PROPERTIES_NAME);
    loadTextsFromCsv(path, &elementDynamicPropertiesMap_default);
    // Print the word-sentence mapping
    // for (const auto &pair : elementDynamicPropertiesMap_default)
    // {
    //     qDebug() << pair.first << ": " << pair.second;
    // }
}

void machine::loadTextsFromTemplateCsv()
{
    qDebug() << "Load dynamic texts from template csv";
    QString path = QString(TEMPLATES_ROOT_PATH) + m_template + "/" + QString(UI_TEXTS_CSV_NAME);
    loadTextsFromCsv(path, &textNameToTextMap_template);
}

void machine::loadTextsFromDefaultHardwareCsv()
{
    qDebug() << "Load dynamic texts from default hardware csv";
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion() + "/" + QString(UI_TEXTS_CSV_NAME);
    loadTextsFromCsv(path, &textNameToTextMap_default_hardware);
}

void machine::loadTextsFromDefaultCsv()
{
    qDebug() << "Load dynamic texts from default csv";
    // QString csv_default_hardware_template_path = TEMPLATES_ROOT_PATH + TEMPLATES_DEFAULT_NAME + "_" + getHardwareMajorVersion + "/" + UI_TEXTS_CSV_NAME;
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "/" + QString(UI_TEXTS_CSV_NAME);
    loadTextsFromCsv(path, &textNameToTextMap_default);
}

void machine::loadTextsFromCsv(QString csv_path, std::map<QString, QString> *dictionary)
{
    std::ifstream file(csv_path.toStdString());
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty() && line[0] != '#') // Skip empty lines and lines starting with '#'
            {
                // qDebug() << QString::fromStdString(line);

                std::size_t delimiter_pos = line.find(','); // left of , is the element name, right is its text
                if (delimiter_pos != std::string::npos)
                {
                    std::string word = line.substr(0, delimiter_pos);
                    std::string sentence = line.substr(delimiter_pos + 1);
                    QString qword = QString::fromStdString(word);
                    QString qsentence = QString::fromStdString(sentence);
                    (*dictionary)[qword] = qsentence;
                }
            }
        }
        file.close();

        // Print the word-sentence mapping
        // for (const auto &pair : *dictionary)
        // {
        //     qDebug() << pair.first << ": " << pair.second;
        // }
    }
    else
    {
        qDebug() << "Texts file path could not be opened: " + csv_path;
    }
}

QStringList machine::getChildNames(QObject *parent)
{
    QStringList childNames;

    if (!parent)
    {
        qDebug() << "Invalid parent object.";
        return childNames;
    }

    QList<QObject *> allChildren = parent->findChildren<QObject *>();

    foreach (QObject *child, allChildren)
    {
        childNames.append(child->objectName());
    }

    return childNames;
}