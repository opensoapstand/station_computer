
#include "../objects/debugOutput.h"
#include "../components/pcb.h"
#include "../components/odysseyx86gpio.h"
#include <chrono>

enum Dispense_state
{

    dispense_setup,
    dispense_init,
    dispense_idle,
    dispense_activate_solenoid,
    dispense_pump_delay,
    dispense_start_pumping,
    dispense_pumping,
    dispense_end_pumping,
    dispense_end_backtrack_delay,
    dispense_end_solenoid_delay,
    dispense_auto_end_of_cycle_delay
};
Dispense_state dispense_state;

#define PUMP_START_DELAY_MILLIS 50
#define PUMP_BACKTRACK_TIME_MILLIS 50
#define SOLENOID_STOP_DELAY_MILLIS 50

#define AUTO_DISPENSE_ENABLED false
#define AUTO_DISPENSE_SLOT 2
#define AUTO_DISPENSE_DELAY_BETWEEN_CYCLES_MS 3000
#define AUTO_DISPENSE_CYCLE_LENGTH_MS 3000

// void pwm_test()
// {
//     pcb *connected_pcb;
//     connected_pcb = new pcb();
//     bool edge = true;
//     connected_pcb->setup();
//     while (true)
//     {

//         using namespace std::chrono;
//         uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

//         uint64_t pulse_2s_period = now_epoch_millis % 2000;

//         if (pulse_2s_period > 1000)
//         {
//             if (edge)
//             {
//                 debugOutput::sendMessage("255", MSG_INFO);
//                 edge = false;
//                 connected_pcb->setPumpPWM(255);
//             }
//         }
//         else
//         {
//             if (!edge)
//             {
//                 debugOutput::sendMessage("0", MSG_INFO);
//                 edge = true;
//                 connected_pcb->setPumpPWM(0);
//             }
//         }
//         connected_pcb->pcb_refresh();
//     }
// }

void board_test()
{

    pcb *connected_pcb;
    connected_pcb = new pcb();

    #define IO_PIN_ENABLE_24V 410 // connector pin 36 for EN-134 pcb


    int pin = IO_PIN_ENABLE_24V;
    oddyseyx86GPIO testping(pin) ;

    testping.setPinAsInputElseOutput(false);


    oddyseyx86GPIO* io24VEnable;
    io24VEnable = new oddyseyx86GPIO(pin);

    io24VEnable->setPinAsInputElseOutput(false);
    



    connected_pcb->setup();
    connected_pcb->setPumpPWM(255); // 255 is max speed

    bool dispenseCycleStarted = false;
    uint64_t dispense_cycle_count = 0;
    uint64_t total_flow_pulses_count = 0;

    uint64_t pump_start_delay_start_epoch;
    uint64_t solenoid_stop_delay_start_epoch;
    uint64_t pump_backtrack_start_epoch;
    uint64_t auto_dispense_start_epoch;
    uint64_t auto_dispense_stop_epoch;
    uint8_t active_slot;

    dispense_state = dispense_setup;

    while (true)
    {
        using namespace std::chrono;
        uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        connected_pcb->pcb_refresh();
        // connected_pcb->independentDispensingRefresh();

        switch (dispense_state)
        {
        case (dispense_setup):
        {

            connected_pcb->setPumpsDisableAll();
            connected_pcb->flowSensorsDisableAll();
            for (uint8_t slot = 1; slot <= 4; slot++)
            {
                if (connected_pcb->isSlotAvailable(slot))
                {
                    connected_pcb->setPumpDirection(slot, true);
                    connected_pcb->setSingleDispenseButtonLight(slot, false);
                    connected_pcb->setSolenoid(slot, false);
                }
            };

            if (AUTO_DISPENSE_ENABLED)
            {
                // // only start
                // for (uint8_t slot = 1; slot <= 4; slot++)
                // {

                //     if (connected_pcb->getDispenseButtonEdgePositive(slot))
                //     {
                dispense_state = dispense_init;
                //     }
                // }
            }
            else
            {
                dispense_state = dispense_init;
            }
        }
        break;
        case (dispense_init):
        {

            if (AUTO_DISPENSE_ENABLED)
            {
                // only start auto cycling if a button is pressed once
                for (uint8_t slot = 1; slot <= 4; slot++)
                {

                    if (connected_pcb->getDispenseButtonEdgePositive(slot))
                    {
                        debugOutput::sendMessage("Will initialize auto dispensing at " + to_string(slot), MSG_INFO);
                        active_slot = slot;
                        dispense_state = dispense_idle;
                    }
                }
            }
            else
            {
                dispense_state = dispense_idle;
            }
        }
        break;
        case (dispense_idle):
        {
            bool next_step = false;

            if (AUTO_DISPENSE_ENABLED)
            {
                next_step = true;
            }
            else
            {
                for (uint8_t slot = 1; slot <= 4; slot++)
                {
                    if (connected_pcb->isSlotAvailable(slot))
                    {
                        if (connected_pcb->getDispenseButtonEdgePositive(slot))
                        {
                            next_step = true;
                            active_slot = slot;
                        }
                    }
                }
            }

            if (next_step)
            {
                dispense_state = dispense_activate_solenoid;
                total_flow_pulses_count = connected_pcb->getFlowSensorTotalPulses(active_slot);

                debugOutput::sendMessage("---- Start of dispense cycle " + to_string(dispense_cycle_count) + "---- . Total number of flow sensor pulses since program start: " + to_string(total_flow_pulses_count), MSG_INFO);
                // connected_pcb->resetFlowSensorTotalPulses(active_slot);
                dispense_cycle_count++;
            }
        };
        break;
        case (dispense_activate_solenoid):
        {
            io24VEnable->writePin(true);

            pump_start_delay_start_epoch = now_epoch_millis;
            connected_pcb->setSolenoid(active_slot, true);
            connected_pcb->flowSensorEnable(active_slot);
            connected_pcb->setSingleDispenseButtonLight(active_slot, true);
            dispense_state = dispense_pump_delay;
            debugOutput::sendMessage("Activate solenoid", MSG_INFO);
        };
        break;
        case (dispense_pump_delay):
        {
            if (AUTO_DISPENSE_ENABLED)
            {
            }
            else
            {
                if (!connected_pcb->getDispenseButtonStateDebounced(active_slot))
                {
                    debugOutput::sendMessage("Button depressed before pump start", MSG_INFO);
                    dispense_state = dispense_setup;
                }
            }

            if (now_epoch_millis > (pump_start_delay_start_epoch + PUMP_START_DELAY_MILLIS))
            {
                debugOutput::sendMessage("Delay done.", MSG_INFO);
                dispense_state = dispense_start_pumping;
            }
        };
        break;
        case (dispense_start_pumping):
        {
            debugOutput::sendMessage("Start pump.", MSG_INFO);
            connected_pcb->setPumpDirection(active_slot, true);
            // connected_pcb->setPumpSpeedPercentage(90);
            connected_pcb->setPumpEnable(active_slot);
            connected_pcb->startPump(active_slot);
            dispense_state = dispense_pumping;
            auto_dispense_start_epoch = now_epoch_millis;
        };
        break;
        case (dispense_pumping):
        {

            bool next_state = false;
            if (AUTO_DISPENSE_ENABLED)
            {
                if (now_epoch_millis > (auto_dispense_start_epoch + AUTO_DISPENSE_CYCLE_LENGTH_MS))
                {
                    debugOutput::sendMessage("Auto Dispense. End of dispense cycle.", MSG_INFO);
                    next_state = true;
                }
            }
            else
            {

                if (!connected_pcb->getDispenseButtonStateDebounced(active_slot))
                {
                    debugOutput::sendMessage("Button release. Stop dispensing.", MSG_INFO);
                    next_state = true;
                }
            }

            if (next_state)
            {
                connected_pcb->setPumpsDisableAll();
                connected_pcb->stopPump(active_slot);
                if (PUMP_BACKTRACK_TIME_MILLIS != 0)
                {
                    connected_pcb->setPumpDirection(active_slot, false);
                    connected_pcb->setPumpEnable(active_slot);

                    // connected_pcb->setPumpSpeedPercentage(0);
                    // connected_pcb->setPumpDirection(active_slot, true);

                    connected_pcb->startPump(active_slot);

                    dispense_state = dispense_end_backtrack_delay;
                    pump_backtrack_start_epoch = now_epoch_millis;
                }
                else
                {
                    dispense_state = dispense_end_pumping;
                }
            }
        };
        break;
        case (dispense_end_backtrack_delay):
        {
            if (now_epoch_millis > (pump_backtrack_start_epoch + PUMP_BACKTRACK_TIME_MILLIS))
            {
                debugOutput::sendMessage("End of backtrack delay", MSG_INFO);
                dispense_state = dispense_end_pumping;
            }
        };
        break;
        case (dispense_end_pumping):
        {
            debugOutput::sendMessage("Disable pumps. Start solenoid delay", MSG_INFO);
            connected_pcb->setPumpsDisableAll();
            connected_pcb->stopPump(active_slot);
            connected_pcb->setPumpDirection(active_slot, true);

           

            solenoid_stop_delay_start_epoch = now_epoch_millis;
            dispense_state = dispense_end_solenoid_delay;
        };
        break;
        case (dispense_end_solenoid_delay):
        {
             io24VEnable->writePin(false);

            if (!AUTO_DISPENSE_ENABLED && connected_pcb->getDispenseButtonStateDebounced(active_slot))
            {
                debugOutput::sendMessage("Restart dispensing", MSG_INFO);
                // restart pump at user press.
                connected_pcb->setPumpEnable(active_slot);
                connected_pcb->startPump(active_slot);

                dispense_state = dispense_pumping;
            }

            if (now_epoch_millis > (solenoid_stop_delay_start_epoch + SOLENOID_STOP_DELAY_MILLIS))
            {

                connected_pcb->setSolenoid(active_slot, false);
                uint64_t cycle_pulses;
                cycle_pulses = connected_pcb->getFlowSensorPulsesSinceEnabling(active_slot);

                debugOutput::sendMessage("End of dispensing. This cycle flow sensor pulses count: " + to_string(cycle_pulses), MSG_INFO);
                connected_pcb->flowSensorsDisableAll();
                connected_pcb->setPumpDirection(active_slot, true);
                connected_pcb->setSingleDispenseButtonLight(active_slot, false);
                if (AUTO_DISPENSE_ENABLED)
                {

                    dispense_state = dispense_auto_end_of_cycle_delay;
                    debugOutput::sendMessage("Wait for auto dispense restart.", MSG_INFO);
                    auto_dispense_stop_epoch = now_epoch_millis;
                }
                else
                {
                    debugOutput::sendMessage("End of solenoid delay", MSG_INFO);
                    dispense_state = dispense_idle;
                }
            }
        };
        break;
        case (dispense_auto_end_of_cycle_delay):
        {
            if (now_epoch_millis > (auto_dispense_stop_epoch + AUTO_DISPENSE_DELAY_BETWEEN_CYCLES_MS))
            {
                debugOutput::sendMessage("End of auto dispense cycle", MSG_INFO);
                dispense_state = dispense_idle;
            }
        };
        break;

        default:
        {
            debugOutput::sendMessage("Error Unknown dispense state!", MSG_ERROR);
        };
        }
    }
}

// void setIOPin(int pinNumber, bool inputElseOutput)
// {

// #ifndef INPUT
// #define INPUT 1
// #endif

// #ifndef OUTPUT
// #define OUTPUT 0
// #endif
// #define SYSFS_GPIO_DIR "/sys/class/gpio"

//     std::string msg = "------oddyseyx86GPIO------ pin:" + std::to_string(pinNumber);
//     debugOutput::sendMessage(msg, MSG_INFO);

//     // debugOutput::sendMessage("------oddyseyx86GPIO------", MSG_INFO);
//     int fd, len;
//     char buf[MAX_BUF];
//     fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
//     if (fd < 0)
//     {
//         debugOutput::sendMessage("~oddyseyx86GPIO could not open the gpio", MSG_ERROR);
//         return;
//     }

//     len = snprintf(buf, sizeof(buf), "%d", pinNumber);
//     write(fd, buf, len);
//     close(fd);

//     /* ------------------------- TODO: THIS NEEDS TO BE DONE BETTER ------------------------ */

//     string GPIO = std::to_string(pinNumber);
//     string command("echo 'D@nkF1ll$' | sudo -S chmod a+w /sys/class/gpio/gpio");
//     command += GPIO;
//     string command_dir = command + "/direction";
//     string command_edg = command + "/edge";
//     string command_val = command + "/value";

//     system(command_dir.c_str());
//     system(command_edg.c_str());
//     system(command_val.c_str());

//     using namespace std::chrono;
//     uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

//     std::string msg = "oddyseyx86GPIO::setPinAsInputElseOutput " + std::to_string(pinNumber);
//     debugOutput::sendMessage(msg, MSG_INFO);

//     // debugOutput::sendMessage("oddyseyx86GPIO::setPinAsInputElseOutput ", MSG_INFO);
//     DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
//     int fd, len;
//     char syscode;
//     char buf[MAX_BUF];
//     // char buf2[MAX_BUF];

//     // Composes a string with the same text that would be printed if format was used on printf, but instead of being printed,
//     // the content is stored as a C string in the buffer pointed by s
//     len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", pinNumber);
//     // len2 = snprintf(buf2, sizeof(buf2), SYSFS_GPIO_DIR "/gpio%d/edge", m_nPin);

//     fd = open(buf, O_WRONLY);
//     // fd2 = open(buf2, O_WRONLY);
//     if (fd >= 0)
//     {
//         if (INPUT == inputElseOutput)
//         {
//             // cout << "direction input set" << endl;
//             write(fd, "in", 3);
//             // cout << "edge set" << endl;
//             // write(fd2, "rising", 7);
//         }
//         else
//             write(fd, "out", 4);

//         close(fd);
//         //      close(fd2);
//         df_ret = OK;
//     }
// }



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
//     // connected_pcb->setSolenoid(SLOT, true);
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
//         //     connected_pcb->setSolenoid(SLOT, false);
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
//     for (int i = 255; i > 0; i -= 10)
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
//         //     connected_pcb->setSolenoid(SLOT, false);
//         //     debugOutput::sendMessage("button pressed. finish up", MSG_INFO);
//         //     connected_pcb->setSingleDispenseButtonLight(SLOT, false);
//         //     return;
//         // }
//     }
//     connected_pcb->stopPump(SLOT);
//     debugOutput::sendMessage("end", MSG_INFO);
// }

// void init_test()
// {
//     pcb *connected_pcb;
//     connected_pcb = new pcb();

//     connected_pcb->setup();

//     using namespace std::chrono;
//     uint64_t start_millis_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//     uint64_t now = start_millis_epoch;

//     while (now < start_millis_epoch + 5000)
//     {
//         now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//         connected_pcb->pcb_refresh();
//     }
//     debugOutput::sendMessage("end", MSG_INFO);
// }

// void test_button_lights(bool onElseOff)
// {
//     pcb *connected_pcb;
//     connected_pcb = new pcb();

//     connected_pcb->setup();
//     for (int slot = 1; slot <= 4; slot++)
//     {
//         debugOutput::sendMessage("Slot light " + to_string(slot) + " Setting:" + to_string(onElseOff), MSG_INFO);
//         connected_pcb->setSingleDispenseButtonLight(slot, onElseOff);
//     }
//     connected_pcb->pcb_refresh();
// }

int main(int argc, char *argv[])
{
    // pwm_test();
    board_test();
    debugOutput::sendMessage(to_string(argc), MSG_INFO);

    // motor_test(argv[1], argv[2]);
    // motor_test();
    // motor_test_ramp_up();
    // init_test();
    // test_button_lights(false);
}
