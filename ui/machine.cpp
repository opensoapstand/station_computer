

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

void machine::loadParametersFromDb()
{
    qDebug() << "Load machine parametersssss";
    DbManager db(DB_PATH);
    db.getAllMachineProperties(
        &m_machine_id,
        &m_Soapstand_customer_id,
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
        m_status_text_slots
    );
     db.closeDB();
}

QString machine::getMachineId()
{
    return m_machine_id;
}