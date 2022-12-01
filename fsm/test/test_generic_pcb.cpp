
#include "../objects/debugOutput.h"
#include "../components/pcb.h"
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
#define PUMP_BACKTRACK_TIME_MILLIS 100
#define SOLENOID_STOP_DELAY_MILLIS 50

#define AUTO_DISPENSE_ENABLED false
#define AUTO_DISPENSE_SLOT 4
#define AUTO_DISPENSE_DELAY_BETWEEN_CYCLES_MS 3000
#define AUTO_DISPENSE_CYCLE_LENGTH_MS 3000

void board_test()
{

    pcb *connected_pcb;
    connected_pcb = new pcb();

    connected_pcb->setup();
    connected_pcb->setPumpPWM(0);

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
        connected_pcb->refresh();

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
                if (PUMP_BACKTRACK_TIME_MILLIS != 0)
                {
                    connected_pcb->setPumpDirection(active_slot, false);
                    connected_pcb->setPumpEnable(active_slot);
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
            connected_pcb->setPumpDirection(active_slot, true);

            solenoid_stop_delay_start_epoch = now_epoch_millis;
            dispense_state = dispense_end_solenoid_delay;
        };
        break;
        case (dispense_end_solenoid_delay):
        {

            if (!AUTO_DISPENSE_ENABLED && connected_pcb->getDispenseButtonStateDebounced(active_slot))
            {
                debugOutput::sendMessage("Restart dispensing", MSG_INFO);
                // restart pump at user press.
                connected_pcb->setPumpEnable(active_slot);
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

void motor_test()
{
    pcb *connected_pcb;
    connected_pcb = new pcb();

#define SLOT 2
    connected_pcb->setup();
    connected_pcb->setPumpPWM(240);
    connected_pcb->setPumpDirection(SLOT, false);
    connected_pcb->setPumpEnable(SLOT);
    connected_pcb->setSolenoid(SLOT, true);
    connected_pcb->setSingleDispenseButtonLight(SLOT, true);
    debugOutput::sendMessage("started. press button to stop", MSG_INFO);
    while (true)
    {
        connected_pcb->refresh();
        // if (connected_pcb->getDispenseButtonEdgePositive(SLOT))
        // {
        //     connected_pcb->setPumpsDisableAll();
        //     connected_pcb->setSolenoid(SLOT, false);
        //     debugOutput::sendMessage("button pressed. finish up", MSG_INFO);
        //     connected_pcb->setSingleDispenseButtonLight(SLOT, false);
        //     return;
        // }
    }
    debugOutput::sendMessage("end", MSG_INFO);
}
int main(int argc, char *argv[])
{
    board_test();
    //motor_test();
}