

#include "pcbEN134.h"
#include <chrono>
#include <cmath>

#define DEFAULT_I2C_BUS "/dev/i2c-1"

#define __USE_SMBUS_I2C_LIBRARY__ 1

// Constructor that works out the name of the I2C bus
pcbEN134::pcbEN134(void)
{

#ifdef __arm__

    i2c_bus_name = (char *)calloc(strlen(DEFAULT_I2C_BUS) + 1, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("pcbEN134: Unable to allocate memory.", MSG_ERROR);
        return;
    }
    strcpy(i2c_bus_name, DEFAULT_I2C_BUS);

#else

    FILE *fp;
    char path[1035];

    // Open the command for reading.
    fp = popen("/bin/ls /sys/bus/pci/devices/*/i2c_designware.1/ | /bin/grep i2c", "r");
    if (fp == NULL)
    {
        debugOutput::sendMessage("Failed to run command.\n", MSG_ERROR);
        return;
    }

    // Read the output of the command from the pipe
    if (fgets(path, sizeof(path), fp) == NULL)
    {
        debugOutput::sendMessage("Failed to get I2C bus name.\n", MSG_ERROR);
        return;
    }
    pclose(fp);

    i2c_bus_name = (char *)calloc(strlen(path) + 5, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("pcbEN134: Unable to allocate memory.", MSG_ERROR);
        return;
    }
    strcpy(i2c_bus_name, "/dev/");
    strcpy(i2c_bus_name + 5, path);
    i2c_bus_name[strlen(i2c_bus_name) - 1] = 0;

#endif

} // End of pcbEN134() constructor

// Constructor where you can specify the name of the I2C bus
pcbEN134::pcbEN134(const char *bus)
{
    // Make a copy of the bus name for later use
    i2c_bus_name = (char *)calloc(strlen(bus) + 1, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("pcbEN134: Unable to allocate memory.", MSG_ERROR);
        close(i2c_handle);
        return;
    }
    strcpy(i2c_bus_name, bus);

    setup_i2c_bus();
    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        dispenseButtonStateMemory[i] = false;
        dispenseButtonStateDebounced[i] = false;
        dispenseButtonIsDebounced[i] = true;
    }

} // End of pcbEN134() constructor

// Destructor
pcbEN134::~pcbEN134(void)
{
    free(i2c_bus_name);
    close(i2c_handle);
} // End of pcbEN134() destructor

///////////////////////////////////////////////////////////////////////////
// Public methods
///////////////////////////////////////////////////////////////////////////

void pcbEN134::setup()
{

    if (!is_initialized)
    {

        setup_i2c_bus();

        for (uint8_t i = 0; i < SLOT_COUNT; i++)
        {
            dispenseButtonStateMemory[i] = false;
            dispenseButtonStateDebounced[i] = false;
            dispenseButtonIsDebounced[i] = true;
        }
        is_initialized = true;
    }
}
unsigned char pcbEN134::getPumpPWM(void)
{

    if (pic_pwm_found)
    {
        float f_value;

        // Rescale so the PIC values match the old MAX31760 values.
        // The PIC takes PWM values in the 0-100 range.
        f_value = (float)ReadByte(PIC_ADDRESS, 0x00); // PWM value
        f_value = floor(f_value * 2.55);

        return ((unsigned char)f_value);
    }

    return 0;

} // End of getPumpRPM()

bool pcbEN134::setPumpPWM(uint8_t pwm_val)
{

    if (pic_pwm_found)
    {
        float f_value;

        // Rescale so the PIC values match the old MAX31760 values.
        // The PIC takes PWM values in the [0-100] range. (includes 100)

        // f_value = (float)pwm_val;
        // f_value = floor(f_value / 100);

        // return SendByte(PIC_ADDRESS, 0x00, (unsigned char)f_value); // PWM value
        f_value = (float)pwm_val;
        f_value = floor(f_value / 2.55);
        unsigned char speed_percentage = (unsigned char)f_value; // invert speed. pwm is inverted.
        if (speed_percentage > 100)
        {
            debugOutput::sendMessage("Speed invalid. Will set to max. Please provide argument in [0..255] interval. Provided: " + to_string(pwm_val), MSG_WARNING);
            speed_percentage = 100;
        }

        // speed_percentage = 100 - speed_percentage; // invert speed. pwm is inverted.

        // debugOutput::sendMessage("Motor speed set to (percentage): " + to_string(speed_percentage), MSG_INFO);
        return SendByte(PIC_ADDRESS, 0x00, speed_percentage); // PWM value
    }
    else
    {
        debugOutput::sendMessage("No motor speed controller found to set PWM.", MSG_WARNING);
    }

} // End of setPumpPWM()

bool pcbEN134::setPumpDirection(uint8_t slot, bool forwardElseReverse)
{
    unsigned char pwm_value;
    unsigned char reg_value;

    // Set the RPM to zero to make sure any running pumps stop.
    // Changing the direction without stopping the pump can damage the
    // pump.

    reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);

    // bit 5 = 1 = forward

    // check if direction is different from set direction
    bool read_direction_is_forward = (reg_value & 0b00100000) > 0;
    bool direction_changed = read_direction_is_forward != forwardElseReverse;

    if (direction_changed)
    {
        // debugOutput::sendMessage("dir changed.....", MSG_INFO);
        pwm_value = getPumpPWM();

        setPumpPWM(0);
        usleep(100000); // wait for pump to stop

        // Set the direction
        if (forwardElseReverse)
        {
            reg_value = reg_value | 0b00100000;
        }
        else
        {
            reg_value = reg_value & 0b11011111;
        }
        SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);

        // Restore the pump RPM value
        setPumpPWM(pwm_value);
    }
    else
    {
        if (forwardElseReverse)
        {
            debugOutput::sendMessage("Direction not changed (forward).", MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("Direction not changed (reverse).", MSG_INFO);
        }
    }

    return true;
} // End of setPumpDirection()

bool pcbEN134::setPumpEnable(unsigned char pump_number)
{
    unsigned char reg_value;

    reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);
    switch (pump_number)
    {
    case 1:
        reg_value = reg_value | 0b00000001;
        break;
    case 2:
        reg_value = reg_value | 0b00000101;
        break;
    case 3:
        reg_value = reg_value | 0b00000011;
        break;
    case 4:
        reg_value = reg_value | 0b00000111;
        break;
    default:
        return false;
    }
    SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);

    return true;
} // End setPumpEnable()

void pcbEN134::virtualButtonPressHack()
{
    // // WARNING: This overrides the physical dispense button. As such, there is no fail safe mechanism.
    // // If the program crashes while the button is pressed, it might keep on dispensing *forever*.

    // unsigned char reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03);
    // reg_value = reg_value & 0b01111111;
    // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)

    // reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);
    // reg_value = reg_value & 0b01111111; // virtual button press
    // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);
}

void pcbEN134::virtualButtonUnpressHack()
{
    // unsigned char reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03);
    // reg_value = reg_value | 0b10000000;
    // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
}

bool pcbEN134::setPumpsDisableAll()
{
    // unsigned char reg_value;

    // reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);
    // reg_value = reg_value & 0b11111000;
    // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);

    // return true;
} // End setPumpsDisableAll()

bool pcbEN134::getDispenseButtonState(uint8_t slot)
{

    // bool isPressed = ((ReadByte(PCA9534_PIN_IN_BUTTON, 0x00) & 0x80) ? false : true); // low = pressed
    // bool isPressed = (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & PCA9534_PIN_IN_BUTTON) ;
    bool val = (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & (1<<PCA9534_PIN_IN_BUTTON)) ;

    bool isPressed = !val;

    return isPressed;

} // End of getDispenseButtonState()

bool pcbEN134::getDispenseButtonEdge(uint8_t slot)
{

} // End of getDispenseButtonState()
void pcbEN134::dispenseButtonRefresh()
{
    // as this is not in a separate thread, we'll need to call it some times...
    // up edge: state wait for debouncing.
    // down edge: instant react. Because the PWM is hardware disconnected right away. We don't want jitter on that (aka disable right away when negative edge is detected).

    for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {

        using namespace std::chrono;
        uint64_t now_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        bool state = getDispenseButtonState(slot_index + 1);
        // if (state){
        //     debugOutput::sendMessage("bbbutuuotnt", MSG_INFO);
        // }

        if (state != dispenseButtonStateMemory[slot_index])
        {
            dispenseButtonDebounceStartEpoch[slot_index] = now_epoch;
            dispenseButtonIsDebounced[slot_index] = false;
            debugOutput::sendMessage("edge detected!" + std::to_string(dispenseButtonDebounceStartEpoch[slot_index]) + "state: " + std::to_string(state), MSG_INFO);
        }

        if (!dispenseButtonIsDebounced[slot_index])
        {
            if (state)
            {
                // up edge wait for stable
                if ((now_epoch > (dispenseButtonDebounceStartEpoch[slot_index] + DISPENSE_BUTTON_DEBOUNCE_MILLIS)) && !dispenseButtonIsDebounced[slot_index] && state != dispenseButtonStateDebounced[slot_index])
                {
                    dispenseButtonIsDebounced[slot_index] = true;
                    //debugOutput::sendMessage("commit edge to state" + std::to_string(millis_since_epoch - dispenseButtonDebounceStartEpoch), MSG_INFO);
                    debugOutput::sendMessage("debounced" + to_string(state), MSG_INFO);
                    dispenseButtonStateDebounced[slot_index] = state;
                }
            }
            else
            {
                // down edge do not wait for stable
                dispenseButtonIsDebounced[slot_index] = true;
                dispenseButtonStateDebounced[slot_index] = state;
            }
        }

        dispenseButtonStateMemory[slot_index] = state;
    }
}

bool pcbEN134::getDispenseButtonStateDebounced(uint8_t slot)
{

    return dispenseButtonStateDebounced[slot - 1];
}

void pcbEN134::setPCA9534Output(uint8_t slot, int posIndex, bool onElseOff)
{
    // slot starts at 1!
    unsigned char reg_value;

    reg_value = ReadByte(get_PCA9534_address_from_slot(slot), 0x01);

    if (onElseOff)
    {
        reg_value = reg_value | (1UL << posIndex);
    }
    else
    {
        reg_value = reg_value & ~(1UL << posIndex);
    }
    debugOutput::sendMessage("value to be sent: " + to_string(reg_value) + " to address: " + to_string(get_PCA9534_address_from_slot(slot)), MSG_ERROR);

    SendByte(get_PCA9534_address_from_slot(slot), 0x01, reg_value);
}

void pcbEN134::setSingleDispenseButtonLight(uint8_t slot, bool poweron)
{
    if (poweron)
    {
        setPCA9534Output(slot, PCA9534_PIN_OUT_BUTTON_LED_LOW_IS_ON, false);
    }
    else
    {
        setPCA9534Output(slot, PCA9534_PIN_OUT_BUTTON_LED_LOW_IS_ON, true);
    }
    // unsigned char reg_value;

    // reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);
    // if (poweron)
    // {
    //     reg_value = reg_value | 0b01000000;
    // }
    // else
    // {
    //     reg_value = reg_value & 0b10111111;
    // }
    // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);

} // End of setSingleDispenseButtonLight()

///////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////
bool pcbEN134::SendByte(unsigned char address, unsigned char reg, unsigned char byte)
{

#ifdef __USE_SMBUS_I2C_LIBRARY__
    set_i2c_address(address);
    i2c_smbus_write_byte_data(i2c_handle, reg, byte);
#else
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
    unsigned char buffer[2];

    buffer[0] = reg;
    buffer[1] = byte;

    messages[0].addr = address;
    messages[0].flags = 0;
    messages[0].len = 2;
    messages[0].buf = (char *)buffer;

    packets.msgs = messages;
    packets.nmsgs = 1;
    if (ioctl(i2c_handle, I2C_RDWR, &packets) < 0)
    {
        debugOutput::sendMessage(strerror(errno), MSG_ERROR);
        return false;
    }
#endif

    return true;
} // End of SendByte()

///////////////////////////////////////////////////////////////////////////
unsigned char pcbEN134::ReadByte(unsigned char address, unsigned char reg)
{
    unsigned char buffer[2];

#ifdef __USE_SMBUS_I2C_LIBRARY__
    set_i2c_address(address);
    buffer[1] = i2c_smbus_read_byte_data(i2c_handle, reg);
#else
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    buffer[0] = reg;

    messages[0].addr = address;
    messages[0].flags = 0;
    messages[0].len = 1;
    messages[0].buf = (char *)buffer;

    messages[1].addr = address;
    messages[1].flags = I2C_M_RD;
    messages[1].len = 1;
    messages[1].buf = (char *)&(buffer[1]);

    packets.msgs = messages;
    packets.nmsgs = 2;
    if (ioctl(i2c_handle, I2C_RDWR, &packets) < 0)
    {
        debugOutput::sendMessage(strerror(errno), MSG_ERROR);
        return false;
    }
#endif
    return buffer[1];
} // End of ReadByte()

///////////////////////////////////////////////////////////////////////////
bool pcbEN134::set_i2c_address(unsigned char address)
{
    if (ioctl(i2c_handle, I2C_SLAVE, address) < 0)
    {
        if (errno == EBUSY)
        {
            debugOutput::sendMessage("I2C bus busy.  Is something else running?", MSG_INFO);
        }
        else
        {
            debugOutput::sendMessage("Could not set I2C address to " + to_string(address), MSG_ERROR);
            return false;
        }
    }

    return true;
} // End of set_i2c_address()

///////////////////////////////////////////////////////////////////////////
void pcbEN134::setup_i2c_bus(void)
{
    // Get a handle to the bus device if we haven't already
    if (i2c_handle < 0)
    {
        i2c_handle = open(i2c_bus_name, O_RDWR);
        if (i2c_handle < 0)
        {
            std::string message("pcbEN134: Error opening");
            message.append(i2c_bus_name);
            debugOutput::sendMessage(message, MSG_ERROR);
            return;
        }
    }

    if (!check_pcb_configuration())
    {
        std::string message("pcbEN134: I2C bus ");
        message.append(i2c_bus_name);
        message.append(" has a problem.");
        debugOutput::sendMessage(message, MSG_ERROR);
        return;
    }

    debugOutput::sendMessage("I2C bus configuration appears correct.", MSG_INFO);

    // Everything checks out so get it all set up
    initialize_pcb();

    debugOutput::sendMessage("Initialized I2C bus components.", MSG_INFO);

} // End of setup_i2c_bus()

///////////////////////////////////////////////////////////////////////////
uint8_t pcbEN134::get_PCA9534_address_from_slot(uint8_t slot)
{
    if (slot == 0)
    {
        debugOutput::sendMessage("ASSERT ERROR: slot numbers start at 1", MSG_ERROR);
    }

    uint8_t slot_index = slot - 1;
#if SLOT_COUNT == 4
    uint8_t slot_addresses[4] = {PCA9534_ADDRESS_SLOT_1, PCA9534_ADDRESS_SLOT_2, PCA9534_ADDRESS_SLOT_3, PCA9534_ADDRESS_SLOT_4};

#elif SLOT_COUNT == 8
    debugOutput::sendMessage("NOT SET YET FOR MORE THAN 4 PUMPS", MSG_ERROR);
    //  uint8_t slot_addresses [8] = {PCA9534_ADDRESS_SLOT_1, PCA9534_ADDRESS_SLOT_2, PCA9534_ADDRESS_SLOT_3, PCA9534_ADDRESS_SLOT_4};
#endif
    return slot_addresses[slot_index];
}

bool pcbEN134::check_pcb_configuration(void)
{
    unsigned char i2c_probe_address;
    bool config_valid = true;

#if SLOT_COUNT == 4
    bool slot_pca9534_found[SLOT_COUNT] = {false, false, false, false};
#elif SLOT_COUNT == 8
    bool slot_pca9534_found[SLOT_COUNT] = {false, false, false, false, false, false, false, false};
#endif

    for (i2c_probe_address = 0x03; i2c_probe_address <= 0x77; i2c_probe_address++)
    {
        // Go through all the addresses
        debugOutput::sendMessage("will test i2c address: " + to_string(i2c_probe_address), MSG_INFO);

        if (!set_i2c_address(i2c_probe_address))
        {
            std::string message("Error with i2c protocol");
            return false;
        }

        if (i2c_smbus_read_byte(i2c_handle) < 0)
        {
            // // error, check which device has error
            // if (i2c_probe_address == PCA9534_TMP_SLOT2_ADDRESS)
            // {
            //     std::string message("PCA9534 not found on I2C bus ");
            //     message.append(i2c_bus_name);
            //     debugOutput::sendMessage(message, MSG_ERROR);
            //     debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
            //     config_valid = false;
            // }
        }
        else
        {
            if (i2c_probe_address == PCA9534_ADDRESS_SLOT_1)
            {
                debugOutput::sendMessage("Slot 1 PCA9534 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[0] = true;
            }
            else if (i2c_probe_address == PCA9534_ADDRESS_SLOT_2)
            {
                debugOutput::sendMessage("Slot 2 PCA9534 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[1] = true;
            }
            else if (i2c_probe_address == PCA9534_ADDRESS_SLOT_3)
            {
                debugOutput::sendMessage("Slot 3 PCA9534 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[2] = true;
            }
            else if (i2c_probe_address == PCA9534_ADDRESS_SLOT_4)
            {
                debugOutput::sendMessage("Slot 4 PCA9534 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[3] = true;
            }
            else if (i2c_probe_address == PIC_ADDRESS)
            {
                pic_pwm_found = true;
                debugOutput::sendMessage("PIC found on I2C bus for motor PWM and speed feedback", MSG_INFO);
            }
            else
            {
                std::string message("Unknown device found on I2C bus ");
                message.append(i2c_bus_name);
                debugOutput::sendMessage(message, MSG_ERROR);
                config_valid = false;
            }
        }
    }

    for (uint8_t i = 0; i < SLOT_COUNT; i++)
    {
        if (!slot_pca9534_found[i])
        {
            debugOutput::sendMessage("No controller found for slot" + to_string(i + 1), MSG_ERROR);
        }
    }

    if (!pic_pwm_found)
    {
        std::string message("No PWM generator found on I2C bus ");
        message.append(i2c_bus_name);
        debugOutput::sendMessage(message, MSG_ERROR);
        debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
        config_valid = false;
    }

    return config_valid;

} // End of check_pcb_configuration()

///////////////////////////////////////////////////////////////////////////
void pcbEN134::initialize_pcb(void)
{
    // Initialize the PCA9534
    for (uint8_t slot = 1; slot <= SLOT_COUNT; slot++)
    {
        SendByte(get_PCA9534_address_from_slot(slot), 0x01, 0b00100000); // Output pin values register (has no influence on input values)
        // SendByte(get_PCA9534_address_from_slot(i), 0x01, 0b00000000); // Output pin values register (has no influence on input values)
        SendByte(get_PCA9534_address_from_slot(slot), 0x03, 0b00011010); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
    }

    // SendByte(get_PCA9534_address_from_slot(2), 0x01, 0b00100000); // Output pin values register (has no influence on input values)
    setSingleDispenseButtonLight(2, true);
    //  setSingleDispenseButtonLight(2, false);
    // Set PWM value
    SendByte(PIC_ADDRESS, 0x00, 50);

    while (true)
    {
        usleep(1000000);
        dispenseButtonRefresh();
        // usleep(50000);
        for (uint8_t buttonindex = 0; buttonindex < SLOT_COUNT; buttonindex++)
        {
            debugOutput::sendMessage("button state: " + to_string(buttonindex) + " " + to_string(getDispenseButtonStateDebounced(buttonindex + 1)), MSG_INFO);
            debugOutput::sendMessage("button state: " + to_string(buttonindex) + " " + to_string(getDispenseButtonState(buttonindex + 1)), MSG_INFO);
        }
    };

} // End of initialize_pcb ()
