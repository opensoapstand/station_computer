

#include "df_util.h" // lode added for settings
#include "dbmanager.h"
#include "machine.h"

machine::machine()
{
    setRole(UserRole::user);
    setCouponState(no_state);
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

void machine::setCouponState(StateCoupon state)
{
    m_stateCoupon = state;
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
    return price * (1 - m_discount_percentage_fraction);
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

void machine::printerStatus(bool *isOnline, bool *hasPaper)
{
    qDebug() << "DB call: Check printer status. ";
    m_db->printerStatus(isOnline, hasPaper);
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
        m_status_text_slots);

    qDebug() << "Machine ID as loaded from db: " << getMachineId();
    qDebug() << "Template folder from db : " << getTemplateFolder();
}

int machine::getDispensersCount()
{
    return m_dispense_buttons_count;
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