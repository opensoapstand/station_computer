//***************************************
//
// machine.cpp
// machine class owns the package for a dispense.
//
// Links to circuit board addresses and
// holds instructions for dispensing.
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef _MACHINE__H_
#define _MACHINE__H_

#include "../dftypes.h"
#include "../components/gpio.h"
// #include "../components/dsed8344.h"
#include "../components/fsmodysseyx86gpio.h"
#include "../components/pcb.h"
#include "product.h"
#include <sqlite3.h>

#include "../../library/printer/Adafruit_Thermal.h"

class machine
{
public:
    machine();
    void pcb24VPowerSwitch(bool enableElseDisable);
    bool getPcb24VPowerSwitchStatus();
    // void print_text(string text);
    void print_receipt(string name_receipt, string receipt_cost, string receipt_volume_formatted, string time_stamp, string char_units_formatted, string paymentMethod, string plu, string promoCode, bool sleep_until_printed);
    gpio *switch_24V;
    bool power24VEnabled = false;
    void setup();
    pcb *getPcb();

    pcb *control_pcb;
    Adafruit_Thermal *receipt_printer;
    void setButtonLightsBehaviour(Button_lights_behaviour behaviour);

    void refreshButtonLightAnimationCaterpillar();
    void refreshButtonLightAnimationPingPong();
    void refreshButtonLightAnimation();
    void resetButtonLightAnimation();
    Button_lights_behaviour m_button_lights_behaviour;
    Button_lights_behaviour m_button_lights_behaviour_memory;
    uint64_t m_lights_animation_most_recent_step_millis;
    uint8_t m_lights_animation_step;
    void syncSoftwareVersionWithDb();
    void executeSQLStatement(string sql_string);

    int getButtonAnimationProgram();
    // void loadButtonPropertiesFromDb();
    bool getMultiDispenseButtonEnabled();

    void loadGeneralProperties();
    void loadParametersFromDb();
    string getMachineId();

    static int convertPStringToPNumber(const std::string &inputString);

    bool getPumpReversalEnabled();
    bool getEmptyContainerDetectionEnabled();
    bool getPumpSlowStartStopEnabled();

private:
    bool m_isMultiButtonEnabled = false;
    int m_button_animation_program;
    sqlite3 *db;
    int rc;

    string m_machine_id;
    int m_soapstand_customer_id;
    string m_template;
    string m_location;
    string m_controller_type;
    string m_controller_id;
    string m_screen_type;
    string m_screen_id;
    bool m_has_receipt_printer;
    bool m_receipt_printer_is_online;
    bool m_receipt_printer_has_paper;
    bool m_has_tap_payment;
    string m_hardware_version;
    string m_software_version;
    int m_aws_port;
    string m_pump_id_slot_1;
    string m_pump_id_slot_2;
    string m_pump_id_slot_3;
    string m_pump_id_slot_4;
    bool m_is_enabled_slot_1;
    bool m_is_enabled_slot_2;
    bool m_is_enabled_slot_3;
    bool m_is_enabled_slot_4;
    bool m_coupons_enabled;
    string m_status_text_slot_1;
    string m_status_text_slot_2;
    string m_status_text_slot_3;
    string m_status_text_slot_4;
    bool m_has_empty_detection;
    bool m_enable_pump_ramping;
    bool m_enable_pump_reversal;
    int m_dispense_buttons_count;
    string m_maintenance_pwd;
    bool m_show_transactions;
    string m_help_text_html;
    string m_idle_page_type;
    string m_admin_pwd;
    string m_alert_temperature;
    string m_software_version_controller;
    bool m_is_enabled;
    string m_status_text;
};

#endif
