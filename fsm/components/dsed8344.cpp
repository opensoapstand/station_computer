

#include "dsed8344.h"
#include <chrono>
#include <cmath>

#define DEFAULT_I2C_BUS "/dev/i2c-1"

#define __USE_SMBUS_I2C_LIBRARY__ 1

// Constructor that works out the name of the I2C bus
dsed8344::dsed8344(void)
{

#ifdef __arm__

    i2c_bus_name = (char *)calloc(strlen(DEFAULT_I2C_BUS) + 1, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("dsed8344: Unable to allocate memory.", MSG_ERROR);
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
        debugOutput::sendMessage("dsed8344: Unable to allocate memory.", MSG_ERROR);
        return;
    }
    strcpy(i2c_bus_name, "/dev/");
    strcpy(i2c_bus_name + 5, path);
    i2c_bus_name[strlen(i2c_bus_name) - 1] = 0;

#endif

} // End of dsed8344() constructor

// Constructor where you can specify the name of the I2C bus
dsed8344::dsed8344(const char *bus)
{
    // Make a copy of the bus name for later use
    i2c_bus_name = (char *)calloc(strlen(bus) + 1, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("dsed8344: Unable to allocate memory.", MSG_ERROR);
        close(i2c_handle);
        return;
    }
    strcpy(i2c_bus_name, bus);

    setup_i2c_bus();
    dispenseButtonStateMemory = false;
    dispenseButtonStateDebounced = false;
    dispenseButtonIsDebounced = true;

} // End of dsed8344() constructor

// Destructor
dsed8344::~dsed8344(void)
{
    free(i2c_bus_name);
    close(i2c_handle);
} // End of dsed8344() destructor

///////////////////////////////////////////////////////////////////////////
// Public methods
///////////////////////////////////////////////////////////////////////////

void dsed8344::setup()
{

    if (!is_initialized)
    {

        setup_i2c_bus();

        dispenseButtonStateMemory = false;
        dispenseButtonStateDebounced = false;
        dispenseButtonIsDebounced = true;
        is_initialized = true;
    }
}
unsigned char dsed8344::getPumpPWM(void)
{
    if (max31760_pwm_found)
    {
        return ReadByte(MAX31760_ADDRESS, 0x50); // PWM value
    }

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

bool dsed8344::setPumpPWM(uint8_t pwm_val)
{
    if (max31760_pwm_found)
    {
        // debugOutput::sendMessage("Motor speed set to (pwm byte): " + to_string(pwm_val), MSG_INFO);
        return SendByte(MAX31760_ADDRESS, 0x50, pwm_val); // PWM value
    }
    else if (pic_pwm_found)
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

bool dsed8344::setPumpDirection(bool forwardElseReverse)
{
    unsigned char pwm_value;
    unsigned char reg_value;

    // Set the RPM to zero to make sure any running pumps stop.
    // Changing the direction without stopping the pump can damage the
    // pump.

    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);

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
        SendByte(PCA9534_ADDRESS, 0x01, reg_value);

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

bool dsed8344::setPumpEnable(unsigned char pump_number)
{
    unsigned char reg_value;

    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);
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
    SendByte(PCA9534_ADDRESS, 0x01, reg_value);

    return true;
} // End setPumpEnable()

void dsed8344::virtualButtonPressHack()
{
    // WARNING: This overrides the physical dispense button. As such, there is no fail safe mechanism.
    // If the program crashes while the button is pressed, it might keep on dispensing *forever*.

    unsigned char reg_value = ReadByte(PCA9534_ADDRESS, 0x03);
    reg_value = reg_value & 0b01111111;
    SendByte(PCA9534_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)

    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);
    reg_value = reg_value & 0b01111111; // virtual button press
    SendByte(PCA9534_ADDRESS, 0x01, reg_value);
}

void dsed8344::virtualButtonUnpressHack()
{
    unsigned char reg_value = ReadByte(PCA9534_ADDRESS, 0x03);
    reg_value = reg_value | 0b10000000;
    SendByte(PCA9534_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
}

bool dsed8344::setPumpsDisableAll()
{
    unsigned char reg_value;

    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);
    reg_value = reg_value & 0b11111000;
    SendByte(PCA9534_ADDRESS, 0x01, reg_value);

    return true;
} // End setPumpsDisableAll()

unsigned short dsed8344::getPumpSpeed(void)
{
    if (max31760_pwm_found)
    {
        // 2 byte value. standstill is max 65535 , lower is faster. (about 200 is max?! (tested with an idle pump))
        return ((ReadByte(MAX31760_ADDRESS, 0x52) << 8) |
                ReadByte(MAX31760_ADDRESS, 0x53));
    }

    if (pic_pwm_found)
    {
        return (ReadByte(PIC_ADDRESS, 0x01));
    }

} // End of getPumpSpeed()

bool dsed8344::getDispenseButtonState(void)
{

    bool isPressed = ((ReadByte(PCA9534_ADDRESS, 0x00) & 0x80) ? false : true); // low = pressed

    return isPressed;

} // End of getDispenseButtonState()

bool dsed8344::getDispenseButtonEdge(void)
{

} // End of getDispenseButtonState()
void dsed8344::dispenseButtonRefresh()
{
    // as this is not in a separate thread, we'll need to call it some times...
    // up edge: state wait for debouncing.
    // down edge: instant react. Because the PWM is hardware disconnected right away. We don't want jitter on that (aka disable right away when negative edge is detected).
    using namespace std::chrono;
    uint64_t now_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    bool state = getDispenseButtonState();

    if (state != dispenseButtonStateMemory)
    {
        dispenseButtonDebounceStartEpoch = now_epoch;
        dispenseButtonIsDebounced = false;
        // debugOutput::sendMessage("edge detected!" + std::to_string(dispenseButtonDebounceStartEpoch) + "state: " + std::to_string(state), MSG_INFO);
    }

    if (!dispenseButtonIsDebounced)
    {
        if (state)
        {
            // up edge wait for stable
            if ((now_epoch > (dispenseButtonDebounceStartEpoch + DISPENSE_BUTTON_DEBOUNCE_MILLIS)) && !dispenseButtonIsDebounced && state != dispenseButtonStateDebounced)
            {
                dispenseButtonIsDebounced = true;
                // debugOutput::sendMessage("commit edge to state" + std::to_string(millis_since_epoch - dispenseButtonDebounceStartEpoch), MSG_INFO);
                // debugOutput::sendMessage("debounced" + to_string(state), MSG_INFO);
                dispenseButtonStateDebounced = state;
            }
        }
        else
        {
            // down edge do not wait for stable
            dispenseButtonIsDebounced = true;
            dispenseButtonStateDebounced = state;
        }
    }

    dispenseButtonStateMemory = state;
}

bool dsed8344::getDispenseButtonStateDebounced()
{

    return dispenseButtonStateDebounced;
}

void dsed8344::setPCA9534Output(int posIndex, bool onElseOff)
{
    unsigned char reg_value;

    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);

    if (onElseOff)
    {
        reg_value = reg_value | (1UL << posIndex);
    }
    else
    {
        reg_value = reg_value & ~(1UL << posIndex);
    }

    SendByte(PCA9534_ADDRESS, 0x01, reg_value);
}
void dsed8344::setSingleDispenseButtonLight(bool poweron)

{
    setPCA9534Output(6, poweron);
    // unsigned char reg_value;

    // reg_value = ReadByte(PCA9534_ADDRESS, 0x01);
    // if (poweron)
    // {
    //     reg_value = reg_value | 0b01000000;
    // }
    // else
    // {
    //     reg_value = reg_value & 0b10111111;
    // }
    // SendByte(PCA9534_ADDRESS, 0x01, reg_value);

} // End of setSingleDispenseButtonLight()

///////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////
bool dsed8344::SendByte(unsigned char address, unsigned char reg, unsigned char byte)
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
unsigned char dsed8344::ReadByte(unsigned char address, unsigned char reg)
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
bool dsed8344::set_i2c_address(unsigned char address)
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
void dsed8344::setup_i2c_bus(void)
{
    // Get a handle to the bus device if we haven't already
    if (i2c_handle < 0)
    {
        i2c_handle = open(i2c_bus_name, O_RDWR);
        if (i2c_handle < 0)
        {
            std::string message("dsed8344: Error opening");
            message.append(i2c_bus_name);
            debugOutput::sendMessage(message, MSG_ERROR);
            return;
        }
    }

    if (!check_8344_configuration())
    {
        std::string message("dsed8344: I2C bus ");
        message.append(i2c_bus_name);
        message.append(" has a problem.");
        debugOutput::sendMessage(message, MSG_ERROR);
        return;
    }

    debugOutput::sendMessage("I2C bus configuration appears correct.", MSG_INFO);

    // Everything checks out so get it all set up
    initialize_8344();

    debugOutput::sendMessage("Initialized I2C bus components.", MSG_INFO);

} // End of setup_i2c_bus()

///////////////////////////////////////////////////////////////////////////
bool dsed8344::check_8344_configuration(void)
{
    unsigned char i2c_probe_address;
    bool config_valid = true;

    for (i2c_probe_address = 0x03; i2c_probe_address <= 0x77; i2c_probe_address++)
    {
        // Go through all the addresses

        if (!set_i2c_address(i2c_probe_address))
        {
            std::string message("Error with i2c protocol");
            return false;
        }

        if (i2c_smbus_read_byte(i2c_handle) < 0)
        {
            // error, check which device has error
            if (i2c_probe_address == PCA9534_ADDRESS)
            {
                std::string message("PCA9534 not found on I2C bus ");
                message.append(i2c_bus_name);
                debugOutput::sendMessage(message, MSG_ERROR);
                debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
                config_valid = false;
            }
        }
        else
        {
            if (i2c_probe_address == PCA9534_ADDRESS)
            {
                debugOutput::sendMessage("PCA9534 found on I2C bus for pcb I/O", MSG_INFO);
            }
            else if (i2c_probe_address == PIC_ADDRESS)
            {
                pic_pwm_found = true;
                debugOutput::sendMessage("PIC found on I2C bus for motor PWM and speed feedback", MSG_INFO);
            }
            else if (i2c_probe_address == MAX31760_ADDRESS)
            {
                max31760_pwm_found = true;
                debugOutput::sendMessage("MAX31760 found on I2C bus for PWM and speed feedback", MSG_INFO);
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

    if (!pic_pwm_found && !max31760_pwm_found)
    {
        std::string message("No PWM generator found on I2C bus ");
        message.append(i2c_bus_name);
        debugOutput::sendMessage(message, MSG_ERROR);
        debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
        config_valid = false;
    }

    if (pic_pwm_found && max31760_pwm_found)
    {
        std::string message("Two PWM generators found on I2C bus ");
        message.append(i2c_bus_name);
        debugOutput::sendMessage(message, MSG_ERROR);
        debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
        config_valid = false;
    }
    return config_valid;

} // End of check_8344_configuration()

///////////////////////////////////////////////////////////////////////////
void dsed8344::initialize_8344(void)
{
    // Initialize the PCA9534

    // 0bxxxxxxxx
    //          x // output 1:enable selected pump 0:disable all pumps
    //        xx  // output select pump. 00:pump1 , 01:2, 10:3, 11:4
    //       x    // output if multibutton: enable button x (low=active)
    //      x     // output if multibutton: enable button x (low=active)
    //     x      // output: pump direction
    //    x       // output: button light , if multibutton: enable button x (low=active)
    //   x        // input: button in.

    SendByte(PCA9534_ADDRESS, 0x01, 0b11100000); // Output pin values

// #ifdef ENABLE_MULTI_BUTTON
    SendByte(PCA9534_ADDRESS, 0x03, 0b10000000); // 0 = output
// #else
    // SendByte(PCA9534_ADDRESS, 0x03, 0b10011000); 
// #endif

    if (max31760_pwm_found)
    {
        // Initialize the MAX31760

        // Temperature alerts masked
        // PWM frequency 25 kHz
        // PWM polarity negative (100% setting is low)
        SendByte(MAX31760_ADDRESS, 0x00, 0b10011100);

        // Set direct fan control
        SendByte(MAX31760_ADDRESS, 0x01, 0b00010001);

        // Set ramp to immediate
        // Enable TACH1 input
        SendByte(MAX31760_ADDRESS, 0x02, 0b00110001);

        // Disable all alerts
        SendByte(MAX31760_ADDRESS, 0x04, 0b11111111);

        // Set PWM value
        SendByte(MAX31760_ADDRESS, 0x50, 0x80);
    }
    if (pic_pwm_found)
    {
        // Set PWM value
        SendByte(PIC_ADDRESS, 0x00, 50);
    }

} // End of initialize_8344 ()
