
#include "../objects/debugOutput.h"
#include "../components/pcbEN134.h"


int main (int argc, char *argv[])
{

        pcbEN134 *pcb;
//     dsed8344 *the_8344;
//     unsigned char pump_number;
//     bool button_state;
//     unsigned int count;
//     unsigned char pwm_value;
    
     pcb = new pcbEN134 ();

//     button_state = the_8344->getDispenseButtonState();
//     printf ("Button is %s\n", (true ? "down" : "up"));

    pcb->setup();

//     pwm_value = 100;
    
//     the_8344->setPumpPWM (pwm_value);
//     the_8344->setPumpDirection (true);
//     the_8344->setButtonPower(false);

//     for (pump_number=1; pump_number<=4; pump_number++)
//     {
//         printf ("Testing pump %d.\n", pump_number);

//         the_8344->startPump(pump_number);
//         for (count=0; count<255; count++)
//         {
// //            printf("here");
//             printf ("Pump tach: %u\n", the_8344->getPumpTach ());

//             if (the_8344->getDispenseButtonState() != button_state)
//             {
//                 button_state = !button_state;
//                 printf ("Button is %s\n", (button_state ? "down" : "up"));
//             }
//             usleep (10000);

//             pwm_value += 1;
//             the_8344->setPumpPWM (pwm_value);
//             printf ("PWM: %d, ", the_8344->getPumpPWM());

//         }


//     }
//     the_8344->stopPump();
}  // End of main()
