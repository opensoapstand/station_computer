
#include "../fsm/objects/debugOutput.h"
#include "../fsm/components/dsed8344.h"


#define I2C_BUS_NAME  "/dev/i2c-4"


int main (int argc, char *argv[])
{
    dsed8344 *the_8344;
    unsigned char pump_number;
    bool button_state;
    unsigned int count;
    unsigned char pwm_value;
    
    the_8344 = new dsed8344 (I2C_BUS_NAME);

    button_state = the_8344->getButton();
    printf ("Button is %s\n", (button_state ? "down" : "up"));

    pwm_value = 100;
    
    the_8344->setPumpPWM (pwm_value);
    the_8344->setPumpDirection (true);

    for (pump_number=1; pump_number<=4; pump_number++)
    {
        printf ("Testing pump %d.\n", pump_number);

        the_8344->startPump(pump_number);
        for (count=0; count<50; count++)
        {
            printf ("Pump tach: %u\n", the_8344->getPumpTach ());

            if (the_8344->getButton() != button_state)
            {
                button_state = !button_state;
                printf ("Button is %s\n", (button_state ? "down" : "up"));
            }
            usleep (100000);

            pwm_value += 5;
            the_8344->setPumpPWM (pwm_value);
            printf ("PWM: %d", the_8344->getPumpPWM());

        }

        the_8344->stopPump();
    }

}  // End of main()
