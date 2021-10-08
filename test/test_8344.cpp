
#include "../fsm/objects/debugOutput.h"
#include "../fsm/components/dsed8344.h"


int main (int argc, char *argv[])
{
    dsed8344 *the_8344;
    unsigned char pump_number;
    bool button_state;
    unsigned int count;
    unsigned char pwm_value;
    
    the_8344 = new dsed8344 ();

    button_state = the_8344->getButton();
    printf ("Button is %s\n", (button_state ? "down" : "up"));

    pwm_value = 100;
    
    the_8344->setPumpPWM (pwm_value);
    the_8344->setPumpDirection (true);
    the_8344->setButtonPower(false);

//    for (pump_number=1; pump_number<=4; pump_number++)
//    {
//        printf ("Testing pump %d.\n", pump_number);

        the_8344->startPump(1);
        for (count=0; count<255; count++)
        {
//            printf("here");
//            printf ("Pump tach: %u\n", the_8344->getPumpTach ());

//            if (the_8344->getButton() != button_state)
//            {
//                button_state = !button_state;
//                printf ("Button is %s\n", (button_state ? "down" : "up"));
//            }
            usleep (500000);

//            pwm_value += 1;
//            the_8344->setPumpPWM (pwm_value);
//            printf ("PWM: %d, ", the_8344->getPumpPWM());

        }


//    }
    the_8344->stopPump();
}  // End of main()
