
#include "../objects/debugOutput.h"
#include "../components/pcbEN134.h"

int main(int argc, char *argv[])
{

    pcbEN134 *pcb;
    //     dsed8344 *the_8344;
    //     unsigned char pump_number;
    //     bool button_state;
    //     unsigned int count;
    //     unsigned char pwm_value;

    pcb = new pcbEN134();

    //     button_state = the_8344->getDispenseButtonState();
    //     printf ("Button is %s\n", (true ? "down" : "up"));

    pcb->setup();

    // SendByte(get_PCA9534_address_from_slot(2), 0x01, 0b00100000); // Output pin values register (has no influence on input values)
    pcb->setSingleDispenseButtonLight(2, true);
    //  setSingleDispenseButtonLight(2, false);
    // Set PWM value
    // SendByte(PIC_ADDRESS, 0x00, 50);
    pcb->setPumpPWM(0);

    pcb->setPumpDirection(2, false);
    pcb->flowSensorEnable(2);
    uint8_t tmptest;
    pcb->getDispenseButtonStateDebounced(2);
    // pcb->setSolenoid(2,true);
    while (true)
    {
        usleep(100);
        pcb->dispenseButtonRefresh();
        pcb->flowSensorRefresh();
        // usleep(50000);
        for (uint8_t slot = 1; slot <= SLOT_COUNT; slot++)
        {
            // debugOutput::sendMessage("button state: " + to_string(slot) + " " + to_string(getDispenseButtonStateDebounced(slot)), MSG_INFO);
            // debugOutput::sendMessage("button state: " + to_string(slot) + " " + to_string(getDispenseButtonState(slot)), MSG_INFO);

            pcb->setPumpEnableState(slot, pcb->getDispenseButtonStateDebounced(slot));

            // if (pcb->getDispenseButtonEdge(slot))
            // {
            //     tmptest++;
            //     if (tmptest > 100)
            //     {
            //         tmptest = 0;
            //     }
            //     debugOutput::sendMessage("Pump speed percentage:" + to_string(tmptest), MSG_INFO);
            //     pcb->setPumpSpeedPercentage(tmptest);
            // }

            // pcb->setSolenoid(slot, pcb->getDispenseButtonStateDebounced(slot));
        }
        if (pcb->getDispenseButtonStateDebounced(2))
        {
            pcb->setSolenoid(2, true);
            usleep(25000);
            pcb->setSolenoid(2, false);
            usleep(25000);
        }
    };

} // End of main()
