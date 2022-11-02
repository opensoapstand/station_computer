
#include "../objects/debugOutput.h"
#include "../components/pcb.h"

int main(int argc, char *argv[])
{

    pcb *connected_pcb;
    //     dsed8344 *the_8344;
    //     unsigned char pump_number;
    //     bool button_state;
    //     unsigned int count;
    //     unsigned char pwm_value;

    connected_pcb = new pcb();

    connected_pcb->setup();
    connected_pcb->setPumpPWM(0);
    connected_pcb->setPumpEnable(2);
    while (true)
    {
        connected_pcb->refresh();
        if (connected_pcb->getDispenseButtonStateDebounced(2))
        {
            connected_pcb->setSingleDispenseButtonLight(2, true);
            connected_pcb->setSolenoid(2, true);
        }
        else
        {
            connected_pcb->setSingleDispenseButtonLight(2, false);
            connected_pcb->setSolenoid(2, false);
            // connected_pcb->setPumpsDisableAll();
        }
    }

    // connected_pcb->initialize_pcb();
}