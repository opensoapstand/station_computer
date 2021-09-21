

#include "dsed8344.h"


#define __USE_SMBUS_I2C_LIBRARY__  1


// Constructor
dsed8344::dsed8344 (const char *bus)
{
    // Get a handle to the bus device if we haven't already
    if (i2c_handle < 0)
    {
	i2c_handle = open (bus, O_RDWR);
	if (i2c_handle < 0)
	{
	    std::string message("dsed8344: Error opening");
	    message.append (bus);
	    debugOutput::sendMessage(message, ERROR);
	    return;
	}
    }

    // Make a copy of the bus name for later use
    i2c_bus_name = (char *) calloc (strlen (bus) + 1, sizeof (char));
    if (i2c_bus_name == NULL)
    { 
	debugOutput::sendMessage("dsed8344: Unable to allocate memory.", ERROR);
	close (i2c_handle);
	return;
    }
    strcpy (i2c_bus_name, bus);
    
    if (!check_8344_configuration ())
    {
	std::string message("dsed8344: I2C bus ");
	message.append (bus);
	message.append (" has a problem.");
	debugOutput::sendMessage(message, ERROR);
	return;
    }
    debugOutput::sendMessage  ("I2C bus configuration appears correct.", INFO);
    
    
    // Everything checks out so get it all set up
    initialize_8344 ();

    debugOutput::sendMessage  ("Initialized I2C bus components.", INFO);
    
}  // End of dsed8344() constructor


// Destructor
dsed8344::~dsed8344 (void) 
{
    free (i2c_bus_name);
    close (i2c_handle);
}  // End of dsed8344() destructor


///////////////////////////////////////////////////////////////////////////
// Public methods
///////////////////////////////////////////////////////////////////////////

unsigned char dsed8344::getPumpPWM (void)
{
    return ReadByte (MAX31760_ADDRESS, 0x50); // PWM value
}   // End of getPumpRPM()


bool dsed8344::setPumpPWM (unsigned char pwm_val)
{
    return SendByte (MAX31760_ADDRESS, 0x50, pwm_val); // PWM value
}   // End of setPumpPWM()


bool dsed8344::setPumpDirection (bool direction)
{
    unsigned char pwm_value;
    unsigned char reg_value;
    
    // Set the RPM to zero to make sure any running pumps stop.
    // Changing the direction without stopping the pump can damage the
    // pump.
    pwm_value = getPumpPWM();
    setPumpPWM (0);

    // Set the direction
    reg_value = ReadByte (PCA9534_ADDRESS, 0x01);
    if (direction)
    {
	reg_value = reg_value | 0b00100000;
    }
    else
    {
	reg_value = reg_value & 0b11011111;
    }
    SendByte (PCA9534_ADDRESS, 0x01, reg_value);

    // Restore the pump RPM value
    setPumpPWM(pwm_value);

    return true;
}   // End of seyPumpDirection()


bool dsed8344::startPump (unsigned char pump_number)
{
    unsigned char reg_value;
    
    reg_value = ReadByte (PCA9534_ADDRESS, 0x01);
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
    SendByte (PCA9534_ADDRESS, 0x01, reg_value);
    
    return true;
}   // End startPump()


bool dsed8344::stopPump ()
{
    unsigned char reg_value;
    
    reg_value = ReadByte (PCA9534_ADDRESS, 0x01);
    reg_value = reg_value & 0b11111000;
    SendByte (PCA9534_ADDRESS, 0x01, reg_value);

    return true;
}   // End stopPump()


unsigned short dsed8344::getPumpTach (void)
{
    return ((ReadByte (MAX31760_ADDRESS, 0x52) << 8) |
	    ReadByte (MAX31760_ADDRESS, 0x53));

}   // End of getpumpTach()


bool dsed8344::getButton (void)
{
    return ((ReadByte (PCA9534_ADDRESS, 0x00) & 0x80) ? false : true);
}   // End of getButton()


void dsed8344::setButtonPower (bool poweron)
{
    unsigned char reg_value;

    reg_value = ReadByte (PCA9534_ADDRESS, 0x01);
    if (poweron) 
    {
	reg_value = reg_value | 0b01000000;
    }
    else
    {
	reg_value = reg_value & 0b10111111;
    }
    SendByte (PCA9534_ADDRESS, 0x01, reg_value);

}   // End of setButtonPower()



///////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////
bool dsed8344::SendByte (unsigned char address, unsigned char reg, unsigned char byte)
{

#ifdef __USE_SMBUS_I2C_LIBRARY__
    set_i2c_address (address);
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
    
    packets.msgs  = messages;
    packets.nmsgs = 1;
    if (ioctl(i2c_handle, I2C_RDWR, &packets) < 0) 
    {
	debugOutput::sendMessage (strerror(errno), ERROR);
	return false;
    }
#endif
    
    return true;
}   // End of SendByte()


///////////////////////////////////////////////////////////////////////////
unsigned char dsed8344::ReadByte (unsigned char address, unsigned char reg)
{
    unsigned char buffer[2];

#ifdef __USE_SMBUS_I2C_LIBRARY__
    set_i2c_address (address);
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
    
    packets.msgs  = messages;
    packets.nmsgs = 2;
    if (ioctl(i2c_handle, I2C_RDWR, &packets) < 0) 
    {
	debugOutput::sendMessage (strerror(errno), ERROR);
	return false;
    }
#endif
    
    return buffer[1];
}   // End of ReadByte()


///////////////////////////////////////////////////////////////////////////
bool dsed8344::set_i2c_address (unsigned char address)
{
    if (ioctl(i2c_handle, I2C_SLAVE, address) < 0)
    {
	if (errno == EBUSY)
	{
	    debugOutput::sendMessage ("I2C bus busy.  Is something else running?", INFO);
	}
	else 
	{
	    debugOutput::sendMessage ("Could not set I2C address to " + to_string (address), ERROR);
	    return false;
	}
    }

    return true;
}  // End of set_i2c_address()


///////////////////////////////////////////////////////////////////////////
bool dsed8344::check_8344_configuration (void)
{
    // Go through all the devices
    if (!set_i2c_address (PCA9534_ADDRESS))
    {
	return false;
    }
    if (i2c_smbus_write_quick (i2c_handle, I2C_SMBUS_WRITE) < 0)
    {
	std::string message("PCA9534 not found on I2C bus ");
	message.append (i2c_bus_name);
	debugOutput::sendMessage(message, ERROR);
	debugOutput::sendMessage ("Pump control impossible.", ERROR);
	return false;
    }
    
    if (!set_i2c_address (MAX31760_ADDRESS))
    {
	return false;
    }
    if (i2c_smbus_write_quick (i2c_handle, I2C_SMBUS_WRITE) < 0)
    {
	std::string message("MAX31760 not found on I2C bus ");
	message.append (i2c_bus_name);
	debugOutput::sendMessage(message, ERROR);
	debugOutput::sendMessage ("Pump PWM control unavailable.", ERROR);
	return false;
    }

    return true;
    
}  // End of check_8344_configuration()


///////////////////////////////////////////////////////////////////////////
void dsed8344::initialize_8344 (void)
{
    // Initialize the PCA9534
    SendByte (PCA9534_ADDRESS, 0x01, 0b11000000); // Output pin values
    SendByte (PCA9534_ADDRESS, 0x03, 0b10011000); // Enable outputs

    // Initialize the MAX31760
    SendByte (MAX31760_ADDRESS, 0x00, 0b10011100);
    SendByte (MAX31760_ADDRESS, 0x01, 0b00010001);
    SendByte (MAX31760_ADDRESS, 0x02, 0b00110000);
    SendByte (MAX31760_ADDRESS, 0x50, 0x80); // PWM value
    // Enable TACH1 input
    SendByte (MAX31760_ADDRESS, 0x02, 0b00110001);

}  // End of initialize_8344 ()
