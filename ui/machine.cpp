

#include "df_util.h" // lode added for settings
#include "dbmanager.h"
#include "machine.h"

machine::machine()
{
}

// Dtor
machine::~machine()
{
}

QString machine::getTemplateFolder()
{
    QString template_name = m_template;
    if (template_name == "")
    {
        template_name = "default";
    }
    return TEMPLATES_ROOT_PATH + template_name + "/";
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
    DbManager db(DB_PATH);
    db.printerStatus(isOnline, hasPaper);
    db.closeDB();

    // This needs to be checked frequently, so caching is useless.
    // *isOnline = m_receipt_printer_is_online==1;
    // *hasPaper = m_receipt_printer_has_paper==1;
}

bool machine::hasReceiptPrinter()
{
    return (m_has_receipt_printer == 1);
}

bool machine::getPumpRampingEnabled()
{
    return m_enable_pump_ramping == 1;
}
bool machine::getEmptyContainerDetectionEnabled()
{
    return m_has_empty_detection == 1;
}
bool machine::getShowTransactionHistory()
{
    return m_show_transactions == 1;
}

QString machine::getMaintenanceAdminPassword()
{
    return m_maintenance_pwd;
}

QString machine::setStatusText(int slot, bool isSlotEnabled, QString status)
{

    QString column = QString("status_text_slot_%1").arg(slot);

    DbManager db(DB_PATH);
    db.updateTableMachineWithText(column, status);
    // bool success = db.updateSlotAvailability(slot, isSlotEnabled, status);

    db.closeDB();
}

QString machine::getStatusText(int slot)
{
    isSlotNumberValid(slot);
    return m_status_text_slots[slot - 1];
}
QString machine::getPumpId(int slot)
{

    isSlotNumberValid(slot);
    return m_pump_id_slots[slot - 1];
}

bool machine::isSlotNumberValid(int slot)
{
    bool valid = true;
    if (slot <= 0)
    {
        valid = false;
    }
    if (slot >= SLOT_COUNT)
    {
        valid = false;
    }
    if (!valid)
    {

        qDebug() << "Invalid slot! slots start from 1 and go up. e.g. 1,2,3,4. Slot provided: " << slot;
    }
    return valid;
}
bool machine::getSlotEnabled(int slot)
{
    // this should have been part of the products table. But it isn't. We access this from the product.cpp class.

    isSlotNumberValid(slot);
    qDebug() << "****************************************** " << m_is_enabled_slots[slot - 1];
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

    DbManager db(DB_PATH);
    // qDebug() << "machine id independaent" << db.getMachineID();

    db.getAllMachineProperties(
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
        m_pump_id_slots,
        m_is_enabled_slots,
        m_status_text_slots);
    db.closeDB();

    qDebug() << "varr: "
             << "m_machine_id:";
    qDebug() << "varr: " << m_machine_id;
    qDebug() << "varr: "
             << "m_machine_id cuostm er id:";
    qDebug() << "varr: " << m_soapstand_customer_id;
    qDebug() << "varr: " << m_template;
    qDebug() << "varr: " << m_location;
    qDebug() << "varr: " << m_controller_type;
    qDebug() << "varr: " << m_controller_id;
    qDebug() << "varr: " << m_screen_type;
    qDebug() << "varr: " << m_screen_id;
    qDebug() << "varr: " << m_has_receipt_printer;
    qDebug() << "varr: " << m_receipt_printer_is_online;
    qDebug() << "varr: " << m_receipt_printer_has_paper;
    qDebug() << "varr: " << m_has_tap_payment;
    qDebug() << "varr: " << m_hardware_version;
    qDebug() << "varr: " << m_software_version;
    qDebug() << "varr: " << m_aws_port;
    // qDebug() << "varr: " << m_pump_id_slot_1;
    // qDebug() << "varr: " << m_pump_id_slot_2;
    // qDebug() << "varr: " << m_pump_id_slot_3;
    // qDebug() << "varr: " << m_pump_id_slot_4;
    // qDebug() << "varr: " << m_is_enabled_slot_1;
    // qDebug() << "varr: " << m_is_enabled_slot_2;
    // qDebug() << "varr: " << m_is_enabled_slot_3;
    // qDebug() << "varr: " << m_is_enabled_slot_4;
    qDebug() << "varr: " << m_coupons_enabled;
    // qDebug() << "varr: " << m_status_text_slot_1;
    // qDebug() << "varr: " << m_status_text_slot_2;
    // qDebug() << "varr: " << m_status_text_slot_3;
    // qDebug() << "varr: " << m_status_text_slot_4;
    qDebug() << "varr: " << m_has_empty_detection;
    qDebug() << "varr: " << m_enable_pump_ramping;
    qDebug() << "varr: " << m_enable_pump_reversal;
    qDebug() << "varr: " << m_dispense_buttons_count;
    qDebug() << "varr: " << m_maintenance_pwd;
    qDebug() << "varr: " << m_show_transactions;
    qDebug() << "varr: " << m_help_text_html;
    qDebug() << "varr: " << m_idle_page_type;
    for (int i = 0; i < 4; i++)
    {
        qDebug() << "varrrr====";
        qDebug() << i;
        qDebug() << m_is_enabled_slots[i];
        qDebug() << m_pump_id_slots[i];
        qDebug() << m_status_text_slots[i];
    }
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