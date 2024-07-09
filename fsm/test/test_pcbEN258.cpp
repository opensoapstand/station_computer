
#include "../objects/debugOutput.h"
#include "../components/pcb.h"
#include "../components/gpio_odyssey.h"
#include <chrono>

void board_test()
{

    pcb *connected_pcb;
    connected_pcb = new pcb();

    connected_pcb->setup();

    bool dispenseCycleStarted = false;
    uint64_t dispense_cycle_count = 0;
    uint64_t total_flow_pulses_count = 0;

    uint64_t pump_start_delay_start_epoch;
    uint64_t solenoid_stop_delay_start_epoch;
    uint64_t pump_backtrack_start_epoch;
    uint64_t auto_dispense_start_epoch;
    uint64_t auto_dispense_stop_epoch;
    uint8_t active_slot;
}

// void motor_test()
// // void motor_test(int slot, int speedpwm)
// {
//     pcb *connected_pcb;
//     connected_pcb = new pcb();

// #define SLOT 2
//     connected_pcb->setup();
//     connected_pcb->setPumpPWM(255);
//     connected_pcb->setPumpDirection(SLOT, true);
//     // connected_pcb->setPumpDirection(SLOT, false);
//     connected_pcb->setPumpEnable(SLOT);
//     // connected_pcb->setSpoutSolenoid(SLOT, true);
//     // connected_pcb->setSingleDispenseButtonLight(SLOT, true);
//     //     debugOutput::sendMessage("started. press button to stop", MSG_INFO);
//     using namespace std::chrono;
//     uint64_t start_millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//     uint64_t now = start_millis_epoch;

//     connected_pcb->startPump(SLOT);

//     while (now < start_millis_epoch + 2000)
//     {
//         now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//         connected_pcb->pcb_refresh();
//         // if (connected_pcb->getDispenseButtonEdgePositive(SLOT))
//         // {
//         //     connected_pcb->setPumpsDisableAll();
//         //     connected_pcb->setSpoutSolenoid(SLOT, false);
//         //     debugOutput::sendMessage("button pressed. finish up", MSG_INFO);
//         //     connected_pcb->setSingleDispenseButtonLight(SLOT, false);
//         //     return;
//         // }
//     }
//     connected_pcb->stopPump(SLOT);
//     debugOutput::sendMessage("end", MSG_INFO);
// }

// void motor_test_ramp_up()
// // void motor_test(int slot, int speedpwm)
// {
//     pcb *connected_pcb;
//     connected_pcb = new pcb();

// #define SLOT 2
//     connected_pcb->setup();

//     connected_pcb->setPumpDirection(SLOT, true);
//     connected_pcb->setPumpEnable(SLOT);

//     connected_pcb->startPump(SLOT);
//     using namespace std::chrono;

//     uint64_t start_millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//     uint64_t now = start_millis_epoch;

//     // for (int i = 0;i<255;i++){
//     for (int i = 255; i > 0; i-=10)
//     {
//         connected_pcb->pcb_refresh();
//         start_millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

//         connected_pcb->setPumpPWM(i);

//         if (connected_pcb->getDispenseButtonState(SLOT))
//         {
//             while (now < start_millis_epoch + 300)
//             {
//                 now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//             }
//             connected_pcb->pcb_refresh();
//         }
//         else
//         {
//             while (!connected_pcb->getDispenseButtonEdgePositive(SLOT))
//             {
//                 connected_pcb->pcb_refresh();
//             };
//         }
//     }

//     start_millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//     now = start_millis_epoch;
//     while (now < start_millis_epoch + 2000)
//     {
//         connected_pcb->pcb_refresh();
//         now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

//         // if (connected_pcb->getDispenseButtonEdgePositive(SLOT))
//         // {
//         //     connected_pcb->setPumpsDisableAll();
//         //     connected_pcb->setSpoutSolenoid(SLOT, false);
//         //     debugOutput::sendMessage("button pressed. finish up", MSG_INFO);
//         //     connected_pcb->setSingleDispenseButtonLight(SLOT, false);
//         //     return;
//         // }
//     }
//     connected_pcb->stopPump(SLOT);
//     debugOutput::sendMessage("end", MSG_INFO);
// }

void init_test()
{
    pcb *connected_pcb;
    connected_pcb = new pcb();

    connected_pcb->setup();

    using namespace std::chrono;
    uint64_t start_millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t now = start_millis_epoch;

    while (now < start_millis_epoch + 5000)
    {
        now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        connected_pcb->pcb_refresh();
    }
    debugOutput::sendMessage("end", MSG_INFO);
}

void test_button_lights(bool onElseOff)
{
    pcb *connected_pcb;
    connected_pcb = new pcb();

    connected_pcb->setup();
    for (int slot = 1; slot <= 4; slot++)
    {
        debugOutput::sendMessage(" ----------  Slot " + to_string(slot) + " ------------", MSG_INFO);
        connected_pcb->displayMCP23017IORegisters(slot);
        debugOutput::sendMessage("Slot light " + to_string(slot) + " Setting:" + to_string(onElseOff), MSG_INFO);
        connected_pcb->setSingleDispenseButtonLight(slot, onElseOff);
        connected_pcb->displayMCP23017IORegisters(slot);
    }
    connected_pcb->pcb_refresh();
}

std::string executeCommmandLineCommand(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

void enable_24V()
{
    int pin_enable_3point3V;
    int pin_enable_24V;
    int pin_enable_5V;
    int pin_global_flow_sensor;

    std::string kernelVersion = executeCommmandLineCommand("uname -r");
    int major, minor, patch;
    sscanf(kernelVersion.c_str(), "%d.%d.%d", &major, &minor, &patch);

    if (major <= 5 && minor <= 4)
    {
        debugOutput::sendMessage("Ubuntu 18.04. Update the computer to Ubuntu 22.04 when possible.", MSG_INFO);
        pin_enable_3point3V = IO_PIN_ENABLE_3point3V_BEFORE_SYSFS_DEPRECATED;
        pin_enable_5V = IO_PIN_ENABLE_5V_BEFORE_SYSFS_DEPRECATED;
        pin_enable_24V = IO_PIN_ENABLE_24V_BEFORE_SYSFS_DEPRECATED;
    }
    else if (major >= 5 && minor >= 15)
    {
        debugOutput::sendMessage("Ubuntu 22.04. --> sysfs used, which is deprecated. Transform to character based gpio interface.", MSG_INFO);
        // std::cout << "new" << std::endl;
        pin_enable_3point3V = IO_PIN_ENABLE_3point3V_AFTER_SYSFS_DEPRECATED;
        pin_enable_5V = IO_PIN_ENABLE_5V_AFTER_SYSFS_DEPRECATED;
        pin_enable_24V = IO_PIN_ENABLE_24V_AFTER_SYSFS_DEPRECATED;
    }
    else
    {
        debugOutput::sendMessage("between 5.4.0 and 5.15.0. Check functionality and choose between the pin numbers needed...... Ubuntu 22.04 functionality chosen for now.  ", MSG_ERROR);
        pin_enable_3point3V = IO_PIN_ENABLE_3point3V_AFTER_SYSFS_DEPRECATED;
        pin_enable_5V = IO_PIN_ENABLE_5V_AFTER_SYSFS_DEPRECATED;
        pin_enable_24V = IO_PIN_ENABLE_24V_AFTER_SYSFS_DEPRECATED;
    }

    gpio_odyssey *switch_24V;
    switch_24V = new gpio_odyssey(pin_enable_24V);
    switch_24V->writePin(true);
}

void motor_period_test(int slot)
{
    using namespace std::chrono;
    uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    uint64_t start_half_period_millis = now_epoch_millis;
    bool toggle_state = false;

    pcb *connected_pcb;
    connected_pcb = new pcb();

    enable_24V();

    connected_pcb->setup();
    // connected_pcb->setPumpEnable(slot);
    while (true)
    {
        if (now_epoch_millis > start_half_period_millis + 500)
        {
            toggle_state = !toggle_state;

            if (toggle_state)
            {

                connected_pcb->setSingleDispenseButtonLight(slot, true);
                connected_pcb->setPumpEnable(slot);
                connected_pcb->startPump(slot);
                connected_pcb->setSolenoidFromArray(slot, 1, true);
                connected_pcb->setSolenoidFromArray(slot, 2, true);
                connected_pcb->setSolenoidFromArray(slot, 3, true);
                connected_pcb->setSolenoidFromArray(slot, 4, true);
                connected_pcb->setSolenoidFromArray(slot, 5, true);
                connected_pcb->setSolenoidFromArray(slot, 6, true);
                connected_pcb->setSolenoidFromArray(slot, 7, true);
                connected_pcb->setSolenoidFromArray(slot, 8, true);
            }
            else
            {
                connected_pcb->setSingleDispenseButtonLight(slot, false);
                connected_pcb->stopPump(slot);
                connected_pcb->setPumpsDisableAll();
                // connected_pcb->setSolenoidFromArray(slot, 2,false);
                connected_pcb->disableAllSolenoidsOfSlot(slot);
            }

            start_half_period_millis = now_epoch_millis;
        }
        now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        //        connected_pcb->setSingleDispenseButtonLight(slot, false);
    }
}
void button_light_blink(int slot)
{
    using namespace std::chrono;
    uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    uint64_t start_half_period_millis = now_epoch_millis;
    bool toggle_state = false;

    pcb *connected_pcb;
    connected_pcb = new pcb();

    connected_pcb->setup();
    while (true)
    {
        // debugOutput::sendMessage(" ----------  Slot " + to_string(now_epoch_millis) + " ------------", MSG_INFO);
        // usleep(500000);
        if (now_epoch_millis > start_half_period_millis + 500)
        {
            // debugOutput::sendMessage(" ----------  Slot " + to_string(slot) + " ------------", MSG_INFO);

            toggle_state = !toggle_state;
            connected_pcb->setSingleDispenseButtonLight(slot, toggle_state);

            start_half_period_millis = now_epoch_millis;
        }
        now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        //        connected_pcb->setSingleDispenseButtonLight(slot, false);
    }
}

void test_button_to_light()
{
    pcb *connected_pcb;
    connected_pcb = new pcb();
    bool button_values[4];
    connected_pcb->setup();
    while (true)
    {
        for (int slot = 1; slot <= 4; slot++)
        {
            debugOutput::sendMessage(" ----------  Slot " + to_string(slot) + " ------------", MSG_INFO);
            debugOutput::sendMessage("Slot light " + to_string(slot) + " Setting:" + to_string(button_values[slot - 1]), MSG_INFO);
            connected_pcb->displayMCP23017IORegisters(slot);
        }

        for (int16_t i = 0; i < 1000; i++)
        {
            usleep(2000);
            connected_pcb->pcb_refresh();
            for (int slot = 1; slot <= 4; slot++)
            {
                button_values[slot - 1] = connected_pcb->getDispenseButtonStateDebounced(slot);
                connected_pcb->setSingleDispenseButtonLight(slot, button_values[slot - 1]);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // pwm_test();
    // board_test();
    // test_button_to_light();
    // debugOutput::sendMessage(to_string(argc), MSG_INFO);

    // button_light_blink(1);
    motor_period_test(1);

    // motor_test(argv[1], argv[2]);
    // motor_test();
    // motor_test_ramp_up();
    // init_test();
    // test_button_lights(false);
}
