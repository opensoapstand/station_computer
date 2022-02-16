

#include "dsed8344.h"

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

    setup_i2c_bus();

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

unsigned char dsed8344::getPumpPWM(void)
{
    return ReadByte(MAX31760_ADDRESS, 0x50); // PWM value
} // End of getPumpRPM()

bool dsed8344::setPumpPWM(uint8_t pwm_val)
{
    return SendByte(MAX31760_ADDRESS, 0x50, pwm_val); // PWM value
} // End of setPumpPWM()

bool dsed8344::setPumpDirectionForwardElseReverse(bool direction)
{
    unsigned char pwm_value;
    unsigned char reg_value;

    // Set the RPM to zero to make sure any running pumps stop.
    // Changing the direction without stopping the pump can damage the
    // pump.
    pwm_value = getPumpPWM();
    setPumpPWM(0);

    // Set the direction
    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);
    if (direction)
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

    return true;
} // End of seyPumpDirection()

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
    // 2 byte value. standstill is max 65535 , lower is faster. (about 200 is max?! (tested with an idle pump))
    return ((ReadByte(MAX31760_ADDRESS, 0x52) << 8) |
            ReadByte(MAX31760_ADDRESS, 0x53));

} // End of getPumpSpeed()

bool dsed8344::getButton(void)
{
    return ((ReadByte(PCA9534_ADDRESS, 0x00) & 0x80) ? false : true);
} // End of getButton()

void dsed8344::setDispenseButtonLight(bool poweron)
{
    unsigned char reg_value;

    reg_value = ReadByte(PCA9534_ADDRESS, 0x01);
    if (poweron)
    {
        reg_value = reg_value | 0b01000000;
    }
    else
    {
        reg_value = reg_value & 0b10111111;
    }
    SendByte(PCA9534_ADDRESS, 0x01, reg_value);

} // End of setDispenseButtonLight()

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

    for (i2c_probe_address = 0x03; i2c_probe_address <= 0x77; i2c_probe_address++)
    {
        // Go through all the devices
        if (!set_i2c_address(i2c_probe_address))
        {
            return false;
        }
        if (i2c_smbus_read_byte(i2c_handle) < 0)
        {
            if (i2c_probe_address == PCA9534_ADDRESS)
            {
                std::string message("PCA9534 not found on I2C bus ");
                message.append(i2c_bus_name);
                debugOutput::sendMessage(message, MSG_ERROR);
                debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
                return false;
            }
            if (i2c_probe_address == MAX31760_ADDRESS)
            {
                std::string message("MAX31760 not found on I2C bus ");
                message.append(i2c_bus_name);
                debugOutput::sendMessage(message, MSG_ERROR);
                debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
                return false;
            }
        }
        else
        {
            if ((i2c_probe_address != PCA9534_ADDRESS) &&
                (i2c_probe_address != MAX31760_ADDRESS))
            {
                std::string message("Unknown device found on I2C bus ");
                message.append(i2c_bus_name);
                debugOutput::sendMessage(message, MSG_ERROR);
                return false;
            }
        }
    }

    return true;

} // End of check_8344_configuration()

///////////////////////////////////////////////////////////////////////////
void dsed8344::initialize_8344(void)
{
    // Initialize the PCA9534
    SendByte(PCA9534_ADDRESS, 0x01, 0b11000000); // Output pin values
    SendByte(PCA9534_ADDRESS, 0x03, 0b10011000); // Enable outputs

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

} // End of initialize_8344 ()
