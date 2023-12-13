
#include "../objects/debugOutput.h"
#include "../components/pcb.h"
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
        debugOutput::sendMessage("Slot light " + to_string(slot) + " Setting:" + to_string(onElseOff), MSG_INFO);
        connected_pcb->setSingleDispenseButtonLight(slot, onElseOff);
    }
    connected_pcb->pcb_refresh();
}

int main(int argc, char *argv[])
{
    // pwm_test();
    // board_test();
    test_button_lights();
    debugOutput::sendMessage(to_string(argc), MSG_INFO);

    // motor_test(argv[1], argv[2]);
    // motor_test();
    // motor_test_ramp_up();
    // init_test();
    // test_button_lights(false);
}
