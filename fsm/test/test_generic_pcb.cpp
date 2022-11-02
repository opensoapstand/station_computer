
#include "../objects/debugOutput.h"
#include "../components/pcb.h"
#include <chrono>

enum Dispense_state
{

    dispense_setup,
    dispense_idle,
    dispense_activate_solenoid,
    dispense_pump_delay,
    dispense_start_pumping,
    dispense_pumping,
    dispense_end_pumping,
    dispense_end_solenoid_delay
};
Dispense_state dispense_state;

#define PUMP_START_DELAY_MILLIS 1000
#define SOLENOID_STOP_DELAY_MILLIS 1000

int main(int argc, char *argv[])
{

    pcb *connected_pcb;
    connected_pcb = new pcb();

    connected_pcb->setup();
    connected_pcb->setPumpPWM(0);

    bool dispenseCycleStarted = false;
    uint64_t pump_start_delay_start_epoch;
    uint64_t solenoid_stop_delay_start_epoch;
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
                    connected_pcb->setSingleDispenseButtonLight(slot, false);
                    connected_pcb->setSolenoid(slot, false);
                }
            };
            dispense_state = dispense_idle;
            break;
        }
        case (dispense_idle):
        {
            for (uint8_t slot = 1; slot <= 4; slot++)
            {
                if (connected_pcb->isSlotAvailable(slot))
                {

                    if (connected_pcb->getDispenseButtonEdgePositive(slot))
                    {
                        active_slot = slot;
                        dispense_state = dispense_activate_solenoid;
                    }
                }
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
            debugOutput::sendMessage("In delay", MSG_INFO);
            if (!connected_pcb->getDispenseButtonStateDebounced(active_slot))
            {
                debugOutput::sendMessage("Button depressed before pump start", MSG_INFO);
                dispense_state = dispense_setup;
            }
            else if (now_epoch_millis > (pump_start_delay_start_epoch + PUMP_START_DELAY_MILLIS))
            {
                debugOutput::sendMessage("Delay done.", MSG_INFO);
                dispense_state = dispense_start_pumping;
            }
        };
        break;
        case (dispense_start_pumping):
        {

            connected_pcb->setPumpEnable(active_slot);
            dispense_state = dispense_pumping;
        };
        break;
        case (dispense_pumping):
        {
            if (!connected_pcb->getDispenseButtonStateDebounced(active_slot))
            {
                dispense_state = dispense_end_pumping;
            }
        };
        break;
        case (dispense_end_pumping):
        {
            connected_pcb->setPumpsDisableAll();
            solenoid_stop_delay_start_epoch = now_epoch_millis;
            dispense_state = dispense_end_solenoid_delay;
        };
        break;
        case (dispense_end_solenoid_delay):
        {
            if (connected_pcb->getDispenseButtonStateDebounced(active_slot))
            {
                // restart pump at user press.
                connected_pcb->setPumpEnable(active_slot);
                dispense_state = dispense_pumping;
            }
            else if (now_epoch_millis > (solenoid_stop_delay_start_epoch + SOLENOID_STOP_DELAY_MILLIS))
            {
                // connected_pcb->setSolenoid(active_slot, false);
                // connected_pcb->flowSensorsDisableAll();
                // connected_pcb->setSingleDispenseButtonLight(active_slot, false);
                dispense_state = dispense_setup;
            }
        };
        break;

        default:
        {
            debugOutput::sendMessage("Error Unknown dispense state!", MSG_ERROR);
        };
        }
        // connected_pcb->refresh();
        // if (connected_pcb->getDispenseButtonEdgePositive(2))
        // {
        //     connected_pcb->setSolenoid(2, true);
        //     usleep(1000000); // first release the fluidic line before pumping
        //     connected_pcb->setPumpEnable(2);
        //     connected_pcb->setSingleDispenseButtonLight(2,connected_pcb->getDispenseButtonStateDebounced(2));
        //     connected_pcb->flowSensorEnable(2);
        // }
        // if (connected_pcb->getDispenseButtonEdgeNegative(2))
        // {
        //     connected_pcb->flowSensorsDisableAll();
        //     connected_pcb->setPumpsDisableAll();
        //     usleep(1000000);// stop pumping before closing off fluidic line
        //     connected_pcb->setSingleDispenseButtonLight(2,false);
        //     connected_pcb->setSolenoid(2, false);
        // }
    }

    // connected_pcb->initialize_pcb();
}