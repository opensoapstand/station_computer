
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

    button_state = the_8344->getDispenseButtonState();
    printf ("Button is %s\n", (button_state ? "down" : "up"));

    pwm_value = 100;

#if 0

    while (true)
    {
	the_8344->setPumpPWM (pwm_value);
	if (the_8344->getPumpPWM() != pwm_value)
	{
	    printf ("Read PWM value differs from set PWM value.\n");
	}
	pwm_value++;
    }

#else
    
    the_8344->setPumpPWM (pwm_value);
    the_8344->setPumpDirection (true);

    pump_number = 2;
    printf ("Testing pump %d.\n", pump_number);
    the_8344->startPump(pump_number);

//    the_8344->stopPump();
    
    while (true)
    {
        for (count=0; count<50; count++)
        {
            if (the_8344->getDispenseButtonState() != button_state)
            {
                button_state = !button_state;
                printf ("Button is %s\n", (button_state ? "down" : "up"));
            }
            usleep (100000);

            pwm_value += 5;
            the_8344->setPumpPWM (pwm_value);
            printf ("PWM: %d,  \t", the_8344->getPumpPWM());
            printf ("Pump tach: %u\n", the_8344->getPumpTach ());
        }
    }

#endif

    return 0;
    
}  // End of main()
