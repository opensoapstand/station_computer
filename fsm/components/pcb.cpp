

#include "pcb.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <bitset>
#include <cstdint>
#define DEFAULT_I2C_BUS "/dev/i2c-1"

#define __USE_SMBUS_I2C_LIBRARY__ 1

// Constructor that works out the name of the I2C bus
pcb::pcb(void)
{

#ifdef __arm__

    i2c_bus_name = (char *)calloc(strlen(DEFAULT_I2C_BUS) + 1, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("pcb: Unable to allocate memory.", MSG_ERROR);
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
        debugOutput::sendMessage("pcb: Unable to allocate memory.", MSG_ERROR);
        return;
    }
    strcpy(i2c_bus_name, "/dev/");
    strcpy(i2c_bus_name + 5, path);
    i2c_bus_name[strlen(i2c_bus_name) - 1] = 0;

#endif
    pcb_version = INVALID;
    get_pcb_version();
}

// Constructor where you can specify the name of the I2C bus
pcb::pcb(const char *bus)
{
    // Make a copy of the bus name for later use
    i2c_bus_name = (char *)calloc(strlen(bus) + 1, sizeof(char));
    if (i2c_bus_name == NULL)
    {
        debugOutput::sendMessage("pcb: Unable to allocate memory.", MSG_ERROR);
        close(i2c_handle);
        return;
    }
    strcpy(i2c_bus_name, bus);

    setup_i2c_bus();

} // End of pcb() constructor

// Destructor
pcb::~pcb(void)
{
    free(i2c_bus_name);
    close(i2c_handle);
} // End of pcb() destructor

///////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////

bool pcb::SendByte(unsigned char address, unsigned char reg, unsigned char byte)
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

void pcb::PCA9534SendByteToSlot(uint8_t slot, unsigned char reg, unsigned char byte)
{

    SendByte(get_PCA9534_address_from_slot(slot), reg, byte);
}

uint8_t pcb::PCA9534ReadRegisterFromSlot(uint8_t slot, uint8_t reg)
{
    return ReadByte(get_PCA9534_address_from_slot(slot), reg);
}

unsigned char pcb::ReadByte(unsigned char address, unsigned char reg)
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
bool pcb::set_i2c_address(unsigned char address)
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
void pcb::setup_i2c_bus(void)
{
    // Get a handle to the bus device if we haven't already
    if (i2c_handle < 0)
    {
        i2c_handle = open(i2c_bus_name, O_RDWR);
        if (i2c_handle < 0)
        {
            std::string message("pcb: Error opening");
            message.append(i2c_bus_name);
            debugOutput::sendMessage(message, MSG_ERROR);
            return;
        }
    }

    debugOutput::sendMessage("I2C bus configuration appears correct.", MSG_INFO);
    debugOutput::sendMessage("Initialized I2C bus components.", MSG_INFO);
} // End of setup_i2c_bus()

///////////////////////////

bool pcb::getPCA9534Input(uint8_t slot, int posIndex)
{
    return (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & (1 << posIndex));
}

void pcb::setPCA9534Output(uint8_t slot, int posIndex, bool onElseOff)
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
    // debugOutput::sendMessage("value to be sent: " + to_string(reg_value) + " to address: " + to_string(get_PCA9534_address_from_slot(slot)), MSG_INFO);

    SendByte(get_PCA9534_address_from_slot(slot), 0x01, reg_value);
}

uint8_t pcb::get_PCA9534_address_from_slot(uint8_t slot)
{
    if (slot == 0)
    {
        debugOutput::sendMessage("ASSERT ERROR: slot numbers start at 1", MSG_ERROR);
    }

    uint8_t slot_index = slot - 1;

    uint8_t slot_addresses[MAX_SLOT_COUNT] = {PCA9534_ADDRESS_SLOT_1, PCA9534_ADDRESS_SLOT_2, PCA9534_ADDRESS_SLOT_3, PCA9534_ADDRESS_SLOT_4, DUMMY_ADDRESS, DUMMY_ADDRESS, DUMMY_ADDRESS, DUMMY_ADDRESS};
    return slot_addresses[slot_index];
}

////////////////////////

///////////////////////////

void pcb::setMCP23017Register(uint8_t slot, uint8_t reg, uint8_t value)
{

    SendByte(get_MCP23017_address_from_slot(slot), reg, value);
}

uint8_t pcb::getMCP23017Register(uint8_t slot, uint8_t reg)
{

    return (ReadByte(get_MCP23017_address_from_slot(slot), reg));
}

bool pcb::getMCP23017Input(uint8_t slot, int posIndex, uint8_t GPIORegister)
{

    return (ReadByte(get_MCP23017_address_from_slot(slot), GPIORegister) & (1 << posIndex));
}

void pcb::displayMCP23017IORegisters(uint8_t slot)
{
    switch (pcb_version)
    {

    case EN258_4SLOTS:
    case EN258_8SLOTS:
    {

        uint8_t register_gpioA_value = getMCP23017Register(slot, MCP23017_REGISTER_GPA);
        uint8_t register_gpioB_value = getMCP23017Register(slot, MCP23017_REGISTER_GPB);
        std::bitset<8> binaryA(register_gpioA_value);
        std::bitset<8> binaryB(register_gpioB_value);
        debugOutput::sendMessage("Pcb: MCP23017 status for slot: " + std::to_string(slot) + " GPA : " + binaryA.to_string() + " GPB : " + binaryB.to_string(), MSG_INFO);
    }
    break;
    default:;
    }
}

void pcb::setMCP23017Output(uint8_t slot, int posIndex, bool onElseOff, uint8_t GPIORegister)
{
    // slot starts at 1!

    // GPIORegister --> gpioA or gpioB register value

    unsigned char reg_value;

    reg_value = ReadByte(get_MCP23017_address_from_slot(slot), GPIORegister);

    if (onElseOff)
    {
        reg_value = reg_value | (1UL << posIndex);
    }
    else
    {
        reg_value = reg_value & ~(1UL << posIndex);
    }
    // debugOutput::sendMessage("value to be sent: " + to_string(reg_value) + " to address: " + to_string(get_PCA9534_address_from_slot(slot)), MSG_INFO);

    SendByte(get_MCP23017_address_from_slot(slot), GPIORegister, reg_value);
}

uint8_t pcb::get_MCP23017_address_from_slot(uint8_t slot)
{
    if (slot == 0)
    {
        debugOutput::sendMessage("ASSERT ERROR: slot numbers start at 1", MSG_ERROR);
    }

    uint8_t slot_index = slot - 1;

    uint8_t slot_addresses[MAX_SLOT_COUNT] = {MCP23017_ADDRESS_SLOT_1, MCP23017_ADDRESS_SLOT_2, MCP23017_ADDRESS_SLOT_3, MCP23017_ADDRESS_SLOT_4, MCP23017_ADDRESS_SLOT_5, MCP23017_ADDRESS_SLOT_6, MCP23017_ADDRESS_SLOT_7, MCP23017_ADDRESS_SLOT_8};
    return slot_addresses[slot_index];
}

////////////////////////
void pcb::pcb_refresh()
{
    usleep(100);
    if (get_pcb_version() != INVALID)
    {
        dispenseButtonRefresh();
        refreshFlowSensors();
        // independentDispensingRefresh(); // ATTENTION:  this is the state machine. involves more than just pumps....
    }
    else
    {

        usleep(5000000);
        debugOutput::sendMessage("ASSERT ERROR: PCB not detected. Replace pcb? i2c working correctly?", MSG_ERROR);
    }
}

bool pcb::isPcbValid()
{
    return get_pcb_version() != INVALID;
}
void pcb::setup()
{

    if (!is_initialized)
    {

        setup_i2c_bus();
        for (uint8_t i = 0; i < MAX_SLOT_COUNT; i++)
        {
            dispenseButtonStateMemory[i] = false;
            dispenseButtonStateDebounced[i] = false;
            dispenseButtonIsDebounced[i] = true;

            pumpCycle_state[i] = state_init;
            button_light_blink_on_else_off[i] = true;
        }
        is_initialized = true;
    }

    if (!define_pcb_version())
    {
        std::string message("pcb: I2C bus ");
        // message.append(i2c_bus_name);
        message.append("has a problem.");
        debugOutput::sendMessage(message, MSG_ERROR);
        return;
    }

    // set values to default
    initialize_pcb();
}

pcb::PcbVersion pcb::get_pcb_version(void)
{
    return pcb_version;
}

std::string pcb::toString(PcbVersion version)
{
    switch (version)
    {
    case INVALID:
        return "INVALID";
    case DSED8344_NO_PIC:
        return "DSED8344_NO_PIC";
    case DSED8344_PIC_MULTIBUTTON:
        return "DSED8344_PIC_MULTIBUTTON";
    case EN134_4SLOTS:
        return "EN134_4SLOTS";
    case EN134_8SLOTS:
        return "EN134_8SLOTS";
    case EN258_4SLOTS:
        return "EN258_4SLOTS";
    case EN258_8SLOTS:
        return "EN258_8SLOTS";
    default:
        return "Unknown pcb Version";
    }
}

bool pcb::define_pcb_version(void)
{
    unsigned char i2c_probe_address;
    bool config_valid = true;

    for (uint8_t i = 0; i < MAX_SLOT_COUNT; i++)
    {
        slot_pca9534_found[i] = false;
        slot_mcp23017_found[i] = false;
    }

    for (i2c_probe_address = 0x03; i2c_probe_address <= 0x77; i2c_probe_address++)
    {
        // Go through all the addresses
        debugOutput::sendMessage("Check I2C address " + to_string(i2c_probe_address), MSG_INFO);

        if (!set_i2c_address(i2c_probe_address))
        {
            debugOutput::sendMessage("Error with i2c protocol", MSG_INFO);
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
            if (i2c_probe_address == PCA9534_ADDRESS_SLOT_1 || i2c_probe_address == MCP23017_ADDRESS_SLOT_1)
            {
                debugOutput::sendMessage("Slot 1 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[0] = true;
                slot_mcp23017_found[0] = true;
            }
            else if (i2c_probe_address == PCA9534_ADDRESS_SLOT_2 || i2c_probe_address == MCP23017_ADDRESS_SLOT_2)
            {
                debugOutput::sendMessage("Slot 2 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[1] = true;
                slot_mcp23017_found[1] = true;
            }
            else if (i2c_probe_address == PCA9534_ADDRESS_SLOT_3 || i2c_probe_address == MCP23017_ADDRESS_SLOT_3)
            {
                debugOutput::sendMessage("Slot 3 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[2] = true;
                slot_mcp23017_found[2] = true;
            }
            else if (i2c_probe_address == PCA9534_ADDRESS_SLOT_4 || i2c_probe_address == MCP23017_ADDRESS_SLOT_4)
            {
                debugOutput::sendMessage("Slot 4 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[3] = true;
                slot_mcp23017_found[3] = true;
            }
            else if (i2c_probe_address == MCP23017_ADDRESS_SLOT_5)
            {
                debugOutput::sendMessage("Slot 4 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[4] = true;
                slot_mcp23017_found[4] = true;
            }
            else if (i2c_probe_address == MCP23017_ADDRESS_SLOT_6)
            {
                debugOutput::sendMessage("Slot 4 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[5] = true;
                slot_mcp23017_found[5] = true;
            }
            else if (i2c_probe_address == MCP23017_ADDRESS_SLOT_7)
            {
                debugOutput::sendMessage("Slot 4 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[6] = true;
                slot_mcp23017_found[6] = true;
            }
            else if (i2c_probe_address == MCP23017_ADDRESS_SLOT_8)
            {
                debugOutput::sendMessage("Slot 4 PCA9534 OR mcp23017 found on I2C bus for pcb I/O", MSG_INFO);
                slot_pca9534_found[7] = true;
                slot_mcp23017_found[7] = true;
            }
            else if (i2c_probe_address == MAX31760_ADDRESS)
            {
                max31760_pwm_found = true;
                debugOutput::sendMessage("MAX31760 found on I2C bus for PWM and speed feedback", MSG_INFO);
            }
            else if (i2c_probe_address == ADC081C021_CURRENT_SENSOR_ADDRESS)
            {
                // ADC081C021
                current_sensor_found = true;

                debugOutput::sendMessage("ADC081C021 current sensor found. NOT IN USE YET.", MSG_INFO);
            }
            else if (i2c_probe_address == TEMPERATURE_ADC_ADS7830)
            {
                temperature_ads7830_found = true;

                debugOutput::sendMessage("ads7830. Temperature sensor.", MSG_INFO);
            }

            else if (i2c_probe_address == TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_2_ADDRESS)
            {
                debugOutput::sendMessage("MCP9808 Temperature Sensor 2 found.", MSG_INFO);
                mcp9808_temperature_sensor_2_found = true;
                double temp = getTemperatureFromMCP9808(TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_2_ADDRESS);
                char temp_celcius_chars[MAX_BUF];
                snprintf(temp_celcius_chars, sizeof(temp_celcius_chars), "%.2f", temp);
                string temp_celcius = (temp_celcius_chars);
                debugOutput::sendMessage("Temperature at startup: " + std::string(temp_celcius), MSG_INFO);
            }
            else if (i2c_probe_address == TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_1_ADDRESS)
            {
                debugOutput::sendMessage("MCP9808 Temperature Sensor 1 found.", MSG_INFO);
                mcp9808_temperature_sensor_1_found = true;
                double temp = getTemperatureFromMCP9808(TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_1_ADDRESS);
                char temp_celcius_chars[MAX_BUF];
                snprintf(temp_celcius_chars, sizeof(temp_celcius_chars), "%.2f", temp);
                string temp_celcius = (temp_celcius_chars);
                debugOutput::sendMessage("Temperature at startup: " + std::string(temp_celcius), MSG_INFO);
            }
            else if (i2c_probe_address == PIC_ADDRESS)
            {
                pic_pwm_found = true;
                debugOutput::sendMessage("PIC found on I2C bus for motor PWM and speed feedback", MSG_INFO);
            }
            else
            {
                std::string message("Unknown device found on I2C bus ");
                // int address = ADC081C021_CURRENT_SENSOR_ADDRESS;
                // std::string addressStr = std::to_string(address);
                // debugOutput::sendMessage(addressStr, MSG_INFO);
                message.append(i2c_bus_name);
                debugOutput::sendMessage(message, MSG_ERROR);
                config_valid = false;
            }
        }
    }

    if (max31760_pwm_found)
    {
        // definitely 8344
        debugOutput::sendMessage("DSED8344 board because max31760 is found.", MSG_INFO);
        pcb_version = DSED8344_NO_PIC;
    }
    else if (temperature_ads7830_found)
    {
        // only the EN258 has a temperature chip on the pcb
        pcb_version = EN258_8SLOTS;
        if (slot_mcp23017_found[4] || slot_mcp23017_found[5] || slot_mcp23017_found[6] || slot_mcp23017_found[7])
        {
            pcb_version = EN258_8SLOTS;
        }
        else if (slot_mcp23017_found[0] || slot_mcp23017_found[1] || slot_mcp23017_found[2] || slot_mcp23017_found[3])
        {
            pcb_version = EN258_4SLOTS;
        }
        else
        {
            pcb_version = INVALID;
            debugOutput::sendMessage("No pca9534 I/O expanders found. Impossible to determine board type.", MSG_ERROR);
        }
    }
    else if (pic_pwm_found)
    {
        debugOutput::sendMessage("PCB has PIC. Refine pcb type... ", MSG_INFO);

        if (slot_pca9534_found[4] || slot_pca9534_found[5] || slot_pca9534_found[6] || slot_pca9534_found[7])
        {
            // for sure 8 slots board
            // if (slot_pca9534_found[0] && slot_pca9534_found[1] && slot_pca9534_found[2] &&slot_pca9534_found[3] && slot_pca9534_found[4] && slot_pca9534_found[5] && slot_pca9534_found[6] &&slot_pca9534_found[7]){
            pcb_version = EN134_8SLOTS;
        }
        else if (slot_pca9534_found[1] || slot_pca9534_found[2] || slot_pca9534_found[3])
        {
            // for sure 4 slots board
            pcb_version = EN134_4SLOTS;
        }
        else if (slot_pca9534_found[0])
        {
            pcb_version = DSED8344_PIC_MULTIBUTTON;
        }
        else
        {
            debugOutput::sendMessage("No pca9534 I/O expanders found. Impossible to determine board type.", MSG_INFO);
            pcb_version = INVALID;
        }
    }
    else
    {
        debugOutput::sendMessage("Board type could not be determined.", MSG_INFO);
        pcb_version = INVALID;
    }

    switch (pcb_version)
    {
    case (INVALID):
    {
        debugOutput::sendMessage("Pcb: Pcb not valid. Is it connected? Check found i2c devices vs expected ones.", MSG_ERROR);
    };
    break;

    case (DSED8344_NO_PIC):
    {
        debugOutput::sendMessage("Pcb: DSED8344 without pic found. pwm generated by MAX31760", MSG_INFO);
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        debugOutput::sendMessage("Pcb: DSED8344 with pwm generating pic found", MSG_INFO);
        if (!slot_pca9534_found[0])
        {
            debugOutput::sendMessage("PCA9534 not found! ", MSG_ERROR);
        }
    };
    break;

    case (EN134_4SLOTS):
    {
        debugOutput::sendMessage("Pcb: EN-134 4 slots found. ", MSG_INFO);
        for (uint8_t i = 0; i < 4; i++)
        {
            if (!slot_pca9534_found[i])
            {
                debugOutput::sendMessage("No controller found for slot" + to_string(i + 1), MSG_ERROR);
            }
        }
    };
    break;
    case (EN134_8SLOTS):
    {
        debugOutput::sendMessage("Pcb: EN-134 8 slots found.", MSG_INFO);
        for (uint8_t i = 0; i < 8; i++)
        {
            if (!slot_pca9534_found[i])
            {
                debugOutput::sendMessage("No controller found for slot" + to_string(i + 1), MSG_ERROR);
            }
        }
    };
    case (EN258_8SLOTS):
    {
        debugOutput::sendMessage("Pcb: EN-258 8 slots found.", MSG_INFO);
        for (uint8_t i = 0; i < 8; i++)
        {
            if (!slot_mcp23017_found[i])
            {
                debugOutput::sendMessage("No controller found for slot" + to_string(i + 1), MSG_ERROR);
            }
        }
    };
    break;
    case (EN258_4SLOTS):
    {
        debugOutput::sendMessage("Pcb: EN-258 4 slots found.", MSG_INFO);
        for (uint8_t i = 0; i < 4; i++)
        {
            if (!slot_mcp23017_found[i])
            {
                debugOutput::sendMessage("No controller found for slot" + to_string(i + 1), MSG_ERROR);
            }
        }
    };
    break;
    }

    return config_valid;
}

void pcb::sendEN134DefaultConfigurationToPCA9534(uint8_t slot, bool reportIfModified)
{
    // sendByteIfNotSetToSlot(slot, 0x01, 0b00100000, reportIfModified); // BAAAD load  this to emulate a glitch in the chip
    // sendByteIfNotSetToSlot(slot, 0x03, 0b11111111, reportIfModified); //BAAAD load  this to emulate a glitch in the chip
    sendByteIfNotSetToSlot(slot, 0x01, 0b00100000, reportIfModified);
    sendByteIfNotSetToSlot(slot, 0x03, 0b01011000, reportIfModified);
    debugOutput::sendMessage("Default config sent to PCA9534 for slot: " + to_string(slot), MSG_INFO);
}

void pcb::sendByteIfNotSetToSlot(uint8_t slot, unsigned char reg, unsigned char value, bool reportIfModified)
{
    int attempts = 10;
    uint8_t readVal = PCA9534ReadRegisterFromSlot(slot, reg);
    while (readVal != value)
    {
        if (attempts < 0)
        {
            debugOutput::sendMessage("Too many attempts. Could not set register 0x01 of PCA9534 for slot " + to_string(slot), MSG_ERROR);
            break;
        }
        attempts--;
        PCA9534SendByteToSlot(slot, reg, value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
        debugOutput::sendMessage("PCA9534 register " + to_string(reg) + " of slot: " + to_string(slot) + ": " + to_string(readVal) + ". Not configured right. Set to value: " + to_string(value), MSG_INFO);
        if (reportIfModified)
        {
            debugOutput::sendMessage("WARNING: This register was changed. Was this a glitch?", MSG_WARNING);
        }
        readVal = PCA9534ReadRegisterFromSlot(slot, reg);
    }
}

void pcb::initialize_pcb()
{
    // set initial values. depending on pcb version
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
        SendByte(get_PCA9534_address_from_slot(1), 0x03, 0b10000000); // 0 = output
        SendByte(get_PCA9534_address_from_slot(1), 0x01, 0b11100000); // Output pin values
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
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

        // SendByte(get_PCA9534_address_from_slot(1), 0x01, 0b11100000); // Output pin values
        SendByte(get_PCA9534_address_from_slot(1), 0x03, 0b10000000); // 0 = output
    };
    break;
    case (EN134_4SLOTS):
    {
        // Initialize the PCA9534
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            sendEN134DefaultConfigurationToPCA9534(slot, true);
        }

        setPumpsDisableAll();
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            setSpoutSolenoid(slot, false);
            setSingleDispenseButtonLight(slot, false);
        }
    };
    break;
    case (EN134_8SLOTS):
    {
        // Initialize the PCA9534
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            // SendByte(get_PCA9534_address_from_slot(slot), 0x01, 0b00100000); // Output pin values register (has no influence on input values)
            // SendByte(get_PCA9534_address_from_slot(slot), 0x03, 0b01011000); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
            sendEN134DefaultConfigurationToPCA9534(slot, true);
        }
        setPumpsDisableAll();
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            setSpoutSolenoid(slot, false);
            setSingleDispenseButtonLight(slot, false);
        }
    };
    break;
    case (EN258_4SLOTS):
    {
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            uint8_t IOCON_value;
            // IOCON_value |= 0x02;                          // INTPOL...
            IOCON_value |= 0x80;                          // BANK disable.
                                                          // IOCON_value |= 0x80; // BANK enable.
            setMCP23017Register(slot, 0x0A, IOCON_value); // IOCON (IOCON.bank = 0)
            setMCP23017Register(slot, 0x00, 0xC0);        // IODIRA (IOCON.bank = 1)
            setMCP23017Register(slot, 0x10, 0x01);        // IODIRB (IOCON.bank = 1
                                                          // sendEN134DefaultConfigurationToPCA9534(slot, true);

            uint8_t GPIOA_value = 0x00;
            uint8_t GPIOB_value = 0x00;

            // GPIOB_value |= 0x01 << MCP23017_EN258_GPB1_PIN_OUT_BUTTON_LED_LOW_IS_ON; // switch off button light

            setMCP23017Register(slot, MCP23017_REGISTER_GPA, GPIOA_value); // GPIOA (IOCON.bank = 1 // button off (0 for ON)
            setMCP23017Register(slot, MCP23017_REGISTER_GPB, GPIOB_value); // GPIOB (IOCON.bank = 1 // button off (0 for ON)
        }

        setPumpsDisableAll();
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            for (uint8_t position = 1; position <= 8; position++)
            {
                setSolenoidFromArray(slot, position, false);
                setSingleDispenseButtonLight(slot, false);
            }
        }
    };
    break;
    case (EN258_8SLOTS):
    {
        debugOutput::sendMessage("TODO INITIALIZE PCB!!!!!!!", MSG_ERROR);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!1", MSG_ERROR);
    }
    break;
    }

    setFlowSensorTypeDefaults();
    flowSensorsDisableAll();
    debugOutput::sendMessage("pcb initialized. version: " + toString(pcb_version), MSG_INFO);
}

bool pcb::isTemperatureSensorMCP9808Available_1()
{
    return mcp9808_temperature_sensor_1_found;
}
bool pcb::isTemperatureSensorMCP9808Available_2()
{
    return mcp9808_temperature_sensor_2_found;
}

bool pcb::isTemperatureSensorADS7830Available()
{
    return temperature_ads7830_found;
}

bool pcb::isSlotAvailable(uint8_t slot)
{

    if (slot == 0)
    {
        debugOutput::sendMessage("Slot numbering starts at 1", MSG_ERROR);
    }

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
        return true;
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        return true;
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        return slot_pca9534_found[slot - 1];
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        return slot_mcp23017_found[slot - 1];
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!! check for slot available", MSG_ERROR);
    }
    break;
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////
// BUTTON FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::setDispenseButtonLightsAllOff()
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
        setPCA9534Output(1, 6, true);
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    case (EN134_4SLOTS):
    case (EN258_4SLOTS):
    {
        debugOutput::sendMessage("All 4 button lights off.", MSG_INFO);

        for (int slot = 1; slot < 5; slot++)
        {
            setSingleDispenseButtonLight(slot, false);
        }
    };
    break;
    case (EN134_8SLOTS):
    case (EN258_8SLOTS):
    {
        debugOutput::sendMessage("All 8 button lights off.", MSG_INFO);

        for (int slot = 1; slot < 9; slot++)
        {
            setSingleDispenseButtonLight(slot, false);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!setSingleDispenseButtonLight", MSG_ERROR);
    }
    break;
    }
}

void pcb::setSingleDispenseButtonLight(uint8_t slot, bool onElseOff)
{

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
        setPCA9534Output(1, 6, !onElseOff);
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        switch (slot)
        {

        case 1:
        {
            // has a mosfet in between
            // board has only one PCA9534 (as if it were slot one on multi slots boards)
            setPCA9534Output(1, 6, !onElseOff);
            break;
        }
        case 2:
        {
            setPCA9534Output(1, 3, !onElseOff);
            break;
        }
        case 3:
        {
            setPCA9534Output(1, 4, !onElseOff);
            break;
        }
        case 4:
        {
            // work on the gpio of the linux board directly.
            // has a level shifter 3.3v to 5v

            // if (getSlot() == 4)
            // {
            //    m_pDispenseButton4[0]->writePin(!enableElseDisable);
            // }
            // break;
            debugOutput::sendMessage("ASSERT ERROR: wrong way of setting light four for multi dispense light. The hack is not through the PCA but the odyssey.", MSG_ERROR);
            break;
        }
        default:
        {
            debugOutput::sendMessage("Slot not found for setting dispense button light.", MSG_ERROR);
            break;
        }
        }
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        if (onElseOff)
        {
            // debugOutput::sendMessage("Set button light on: " + to_string(slot), MSG_INFO);
            setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_BUTTON_LED_LOW_IS_ON, false);
        }
        else
        {
            // debugOutput::sendMessage("Set button light off: " + to_string(slot), MSG_INFO);
            setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_BUTTON_LED_LOW_IS_ON, true);
        }
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        if (onElseOff)
        {
            // debugOutput::sendMessage("Pcb: slot " + to_string(slot) + ": Button light ON " , MSG_INFO);
            setMCP23017Output(slot, MCP23017_EN258_GPB1_PIN_OUT_BUTTON_LED_LOW_IS_ON, false, MCP23017_REGISTER_GPB);
        }
        else
        {
            // debugOutput::sendMessage("Pcb: slot " + to_string(slot) + ": Button light OFF " , MSG_INFO);
            setMCP23017Output(slot, MCP23017_EN258_GPB1_PIN_OUT_BUTTON_LED_LOW_IS_ON, true, MCP23017_REGISTER_GPB);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!setSingleDispenseButtonLight", MSG_ERROR);
    }
    break;
    }

} // End of setSingleDispenseButtonLight()

// void pcb::virtualButtonPressHack(uint8_t slot)
// {

//     switch (pcb_version)
//     {

//     case (DSED8344_NO_PIC):
//     {
//     };
//     case (DSED8344_PIC_MULTIBUTTON):
//     {
//         // // WARNING: This overrides the physical dispense button. As such, there is no fail safe mechanism.
//         // // If the program crashes while the button is pressed, it might keep on dispensing *forever*.
//         // unsigned char reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03);
//         // reg_value = reg_value & 0b01111111;
//         // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)

//         // reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);
//         // reg_value = reg_value & 0b01111111; // virtual button press
//         // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);
//     };
//     break;
//     case (EN134_4SLOTS):
//     case (EN134_8SLOTS):
//     {
//     };
//     break;
//     default:
//     {
//         debugOutput::sendMessage("Error PCB NOT VALID!!3", MSG_ERROR);
//     }
//     break;
//     }
// }

// void pcb::virtualButtonUnpressHack(uint8_t slot)
// {
//     switch (pcb_version)
//     {

//     case (DSED8344_NO_PIC):
//     {
//     };
//     case (DSED8344_PIC_MULTIBUTTON):
//     {
//     };
//     break;
//     case (EN134_4SLOTS):
//     case (EN134_8SLOTS):
//     {
//     };
//     break;
//     default:
//     {
//         debugOutput::sendMessage("Error PCB NOT VALID!!4", MSG_ERROR);
//     }
//     break;
//     }
// }

bool pcb::getDispenseButtonState(uint8_t slot)
{
    bool isPressed = false;

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
        isPressed = ((ReadByte(get_PCA9534_address_from_slot(1), 0x00) & 0x80) ? false : true); // low = pressed
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        // isPressed = ((ReadByte(PCA9534_PIN_IN_BUTTON, 0x00) & 0x80) ? false : true); // low = pressed
        // isPressed = ((ReadByte(PCA9534_PIN_IN_BUTTON, 0x00) & 0x80) ? false : true); // low = pressed
        bool val = (ReadByte(get_PCA9534_address_from_slot(1), 0x00) & (1 << PCA9534_DSED8344_PIN_IN_BUTTON));
        isPressed = !val;
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        bool val = (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & (1 << PCA9534_EN134_PIN_IN_BUTTON));
        isPressed = !val;
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {

        bool val = (ReadByte(get_MCP23017_address_from_slot(slot), MCP23017_REGISTER_GPB) & (1 << MCP23017_EN258_GPB0_PIN_IN_BUTTON));

        isPressed = !val;
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No button available for this pcb.", MSG_ERROR);
    }
    break;
    }

    // bool isPressed = ((ReadByte(PCA9534_PIN_IN_BUTTON, 0x00) & 0x80) ? false : true); // low = pressed

    // bool isPressed = (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & PCA9534_PIN_IN_BUTTON) ;

    return isPressed;

} // End of getDispenseButtonState()

bool pcb::getDispenseButtonEdge(uint8_t slot)
{
    if (isSlotAvailable(slot))
    {
        return (positive_edge_detected[slot - 1] || negative_edge_detected[slot - 1]);
    }
    else
    {
        return false;
    }
} // End of getDispenseButtonState()

bool pcb::getDispenseButtonEdgePositive(uint8_t slot)
{
    if (isSlotAvailable(slot))
    {
        return (positive_edge_detected[slot - 1]);
    }
    else
    {
        return false;
    }
} // End of getDispenseButtonState()

bool pcb::getDispenseButtonEdgeNegative(uint8_t slot)
{
    if (isSlotAvailable(slot))
    {
        return (negative_edge_detected[slot - 1]);
    }
    else
    {
        return false;
    }
} // End of getDispenseButtonState()

void pcb::dispenseButtonRefresh()
{
    // as this is not in a separate thread, we'll need to call it periodically...
    // up edge: state wait for debouncing.
    // down edge: instant react. Because the PWM is hardware disconnected right away. We don't want jitter on that (aka disable right away when negative edge is detected).

    bool isPressed = false;

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
        dispenseButtonRefreshPerSlot(1);
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        dispenseButtonRefreshPerSlot(1);
        // for (uint8_t slot_index = 0; slot_index < MAX_SLOT_COUNT; slot_index++)
        // {
        //     // if (isSlotAvailable(slot_index + 1))
        //     // {

        //         dispenseButtonRefreshPerSlot(slot_index + 1);
        //     // }
        // }
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        for (uint8_t slot_index = 0; slot_index < MAX_SLOT_COUNT; slot_index++)
        {
            if (isSlotAvailable(slot_index + 1))
            {

                dispenseButtonRefreshPerSlot(slot_index + 1);
            }
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!5", MSG_ERROR);
    }
    break;
    }
}

void pcb::dispenseButtonRefreshPerSlot(uint8_t slot)
{

    uint8_t slot_index = slot - 1;
    using namespace std::chrono;
    uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    bool state = getDispenseButtonState(slot);

    if (state != dispenseButtonStateMemory[slot_index])
    {
        dispenseButtonDebounceStartEpoch[slot_index] = now_epoch_millis;
        dispenseButtonIsDebounced[slot_index] = false;
        debugOutput::sendMessage("Pcb: Slot: " + std::to_string(slot) + " . Button edge detected! timestamp:" + std::to_string(dispenseButtonDebounceStartEpoch[slot_index]) + "state: " + std::to_string(state), MSG_INFO);
    }

    positive_edge_detected[slot_index] = false;
    negative_edge_detected[slot_index] = false;

    // up edge wait for stable
    if ((now_epoch_millis > (dispenseButtonDebounceStartEpoch[slot_index] + DISPENSE_BUTTON_DEBOUNCE_MILLIS)) && !dispenseButtonIsDebounced[slot_index] && state != dispenseButtonStateDebounced[slot_index])
    {
        // stable up edge detected
        dispenseButtonIsDebounced[slot_index] = true;
        debugOutput::sendMessage("Button press debounced. Slot:" + to_string(slot) + " State:" + to_string(state), MSG_INFO);
        dispenseButtonStateDebounced[slot_index] = state;
        if (state)
        {
            positive_edge_detected[slot_index] = true;
        }
        else
        {
            negative_edge_detected[slot_index] = true;
        }
    }

    dispenseButtonStateMemory[slot_index] = state;
}

bool pcb::getDispenseButtonStateDebounced(uint8_t slot)
{
    if (isSlotAvailable(slot))
    {
        return dispenseButtonStateDebounced[slot - 1];
    }
    else
    {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////
// FLOW SENSOR FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::flowSensorEnable(uint8_t slot)
{

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        // goes together with motor enable
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {

        flowSensorsDisableAll(); // reset flow sensor pulse count
        // enable only the active slot flow sensor
        setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_FLOW_SENSOR_ENABLE, true);
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        flowSensorsDisableAll(); // reset flow sensor pulse count
        // enable not needed for EN258 board.
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Flow sensor enable not available for this pcb.", MSG_ERROR);
    }
    break;
    }

    // Only one sensor can be enabled at a time to be safe (it causes the pulses to be combined from all sensors to a separate input in the Odyssey.)
}

int pcb::getSlotCountByPcbType()
{
    switch (get_pcb_version())
    {

    case (DSED8344_PIC_MULTIBUTTON):
    case (DSED8344_NO_PIC):
    case (EN258_4SLOTS):
    case (EN134_4SLOTS):
    {
        return 4;
    }
    break;
    case (EN258_8SLOTS):
    case (EN134_8SLOTS):
    {
        return 8;
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Unrecognized pcb.", MSG_ERROR);
        return 1;
    }
    break;
    }
}
void pcb::flowSensorsDisableAll()
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        for (uint8_t slot = 1; slot <= getSlotCountByPcbType(); slot++)
        {
            setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_FLOW_SENSOR_ENABLE, false);
            flow_sensor_pulses_since_enable[slot - 1] = 0;
        }
    }
    break;
        break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        // enable of pins not needed for EN258 board.
        for (uint8_t slot = 1; slot <= getSlotCountByPcbType(); slot++)
        {
            flow_sensor_pulses_since_enable[slot - 1] = 0;
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Flow sensor DISABLE not available for this pcb.", MSG_ERROR);
    }
    break;
    }
}

void pcb::refreshFlowSensors()
{
    // this is: Polling: periodically read flow rates
    // this is not: Interrupt:  there is a specific line that goes the the Odyssey as an interrupt (which is how the soapstand app traditionally worked)
    // if the interrupt is in use then, the reading of the register is not needed

    switch (pcb_version)
    {
    case (EN258_4SLOTS):
    case (EN134_4SLOTS):
    case (EN258_8SLOTS):
    case (EN134_8SLOTS):
    {
        for (uint8_t slot = 1; slot <= getSlotCountByPcbType(); slot++)
        {
            pollFlowSensor(slot);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No flowsensors to refresh on this pcb", MSG_ERROR);
    }
    break;
    }
}

uint64_t pcb::getFlowSensorPulsesSinceEnabling(uint8_t slot)
{
    uint8_t slot_index = slot - 1;
    return flow_sensor_pulses_since_enable[slot_index];
}

// void pcb::resetFlowSensorPulsesForDispenser(uint8_t slot)
// {
//     uint8_t slot_index = slot - 1;
//     flow_sensor_pulses_for_dispenser[slot_index] = 0;
// }

// uint64_t pcb::getFlowSensorPulsesForDispenser(uint8_t slot)
// {
//     // warning: the flow sensor ticks also come in straight to the Odyssey IO. The controller handles the volume calculation from there not from here. The advantage: no missed pulses (interrupt per pulse),the disadvantage: every slot uses the same pin.
//     uint8_t slot_index = slot - 1;
//     return flow_sensor_pulses_for_dispenser[slot_index];
// }

pcb::FlowSensorType pcb::getFlowSensorType(uint8_t slot)
{
    return flowSensorsType[slot - 1];
}

void pcb::setFlowSensorTypeDefaults()
{
    switch (pcb_version)
    {
    case (EN134_4SLOTS):
    {
        for (uint8_t slot = 1; slot <= getSlotCountByPcbType(); slot++)
        {
            setFlowSensorType(slot, pcb::AICHI);
        }
    };
    break;
    case (EN258_4SLOTS):
    {
        for (uint8_t slot = 1; slot <= getSlotCountByPcbType(); slot++)
        {
            setFlowSensorType(slot, pcb::DIGMESA);
        }
    };
    break;
    default:
    {
        // pcb::FlowSensorType::AICHI
        for (uint8_t slot = 1; slot <= getSlotCountByPcbType(); slot++)
        {
            setFlowSensorType(slot, pcb::NOTSET);
        }
        debugOutput::sendMessage("Pcb: No default flowsensors to set for this pcb", MSG_ERROR);
    }
    break;
    }
}

void pcb::setFlowSensorType(uint8_t slot, FlowSensorType sensorType)
{
    switch (sensorType)
    {
    case pcb::AICHI:
        debugOutput::sendMessage("Pcb: Slot: " + to_string(slot) + ". Flow sensor set to AICHI.", MSG_INFO);
        break;
    case pcb::DIGMESA:
        debugOutput::sendMessage("Pcb: Slot: " + to_string(slot) + ". Flow sensor set to DIGMESA.", MSG_INFO);
        break;
    default:
        debugOutput::sendMessage("Pcb: ASSERT ERROR. Slot: " + to_string(slot) + "Unknown flow sensor type!", MSG_ERROR);
    }

    switch (pcb_version)
    {
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        if (sensorType == pcb::DIGMESA)
        {
            debugOutput::sendMessage("Pcb: WARNING: digmesa flowsensor selected for AICHI only pcb. Ensure cable pinout is configured correctly, with added resistor!", MSG_ERROR);
        }
        flowSensorsType[slot - 1] = sensorType;
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        flowSensorsType[slot - 1] = sensorType;
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No flowsensors to set on this pcb", MSG_ERROR);
    }
    break;
    }
}

void pcb::registerFlowSensorTickCallback(int slot, std::function<void()> callback)
{
    debugOutput::sendMessage("Flow sensor tick callback registered!", MSG_ERROR);
    flowSensorTickCallbacks[slot - 1] = callback;
}

void pcb::pollFlowSensor(uint8_t slot)
{
    if (slot == 0)
    {
        debugOutput::sendMessage("Slot error! Starts at 1!", MSG_ERROR);
    }

    bool state = false;
    switch (pcb_version)
    {
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        state = getPCA9534Input(slot, PCA9534_EN134_PIN_IN_FLOW_SENSOR_TICKS);
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        // debugOutput::sendMessage("wARNING: todo: set manually which flow sensor is being used! BEST to only use one pin. or it will be a hassle in the future (define which sensor is used,....)", MSG_ERROR);

        switch (getFlowSensorType(slot))
        {
        case (pcb::DIGMESA):
        {
            state = getMCP23017Input(slot, MCP23017_EN258_GPA7_PIN_IN_FLOW_SENSOR_DIGMESA, MCP23017_REGISTER_GPA);
        }
        break;
        case (pcb::AICHI):
        {
            state = getMCP23017Input(slot, MCP23017_EN258_GPA6_PIN_IN_FLOW_SENSOR_AICHI, MCP23017_REGISTER_GPA);
        }
        break;
        default:
        {
            debugOutput::sendMessage("Pcb: ASSERT ERROR No flowsensors configured.", MSG_ERROR);
        }
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No flowsensors on this pcb", MSG_ERROR);
    }
    break;
    }

    uint8_t slot_index = slot - 1;

    using namespace std::chrono;
    uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    if (now_epoch_millis > (flowSensorTickReceivedEpoch[slot_index] + FLOW_SENSOR_DEBOUNCE_MILLIS))
    {

        if (flowSensorStateMemory[slot_index] != state)
        {
            // flow_sensor_pulses_for_dispenser[slot_index]++;

            if (flowSensorTickCallbacks[slot_index])
            {
                flowSensorTickCallbacks[slot_index]();
            }

            flow_sensor_pulses_since_enable[slot_index]++;

            // debugOutput::sendMessage("Flow sensor pulse detected by PCA chip. Slot: " + to_string(slot) + ". Pulse total: " + to_string(flow_sensor_pulses_since_enable[slot_index]), MSG_INFO);
            //debugOutput::sendMessage("Flow sensor pulse detected while polling", MSG_INFO);

            flowSensorTickReceivedEpoch[slot_index] = now_epoch_millis;
        }
        flowSensorStateMemory[slot_index] = state;
    }
}

///////////////////////////////////////////////////////////////////////////
// PUMP FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::independentDispensingRefresh()
{
    switch (pcb_version)
    {
    case DSED8344_NO_PIC:
    {
        break;
    }
    case (DSED8344_PIC_MULTIBUTTON):
    {
        break;
    }
    case (EN134_4SLOTS):
    {
        EN134_PumpCycle_refresh(4);
    }
    break;

    case (EN134_8SLOTS):
    {
        EN134_PumpCycle_refresh(8);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No independent dispensing refresh cycle for this pcb. ", MSG_ERROR);
    }
    break;
    }
}

void pcb::EN134_PumpCycle_refresh(uint8_t slots)
{
    // low level state machine, to prevent damage to the solenoid. i.e. do not run pump with closed solenoid.

    switch (pcb_version)
    {
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {

        for (uint8_t slot_index = 0; slot_index < 4; slot_index++)
        {
            uint8_t slot = slot_index + 1;
            using namespace std::chrono;
            uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            switch (pumpCycle_state[slot_index])
            {

            case state_init:
            {
                setSpoutSolenoid(slot, false);
                setSingleDispenseButtonLight(slot, false);
                stopPump(slot);
                pumpCycle_state[slot_index] = state_idle;
            }
            break;
            case state_idle:
            {
                if (slotEnabled[slot_index])
                {
                    pumpCycle_state[slot_index] = state_slot_enabled;
                    button_light_blink_on_else_off[slot_index] = false;
                    button_light_blink_period_start_millis[slot_index] = now_epoch_millis - SLOT_ENABLED_BLINK_BUTTON_OFF_MILLIS - 10;
                }
            }
            break;
            case state_slot_enabled:
            {
                if (!slotEnabled[slot_index])
                {
                    pumpCycle_state[slot_index] = state_idle;
                }

                if (button_light_blink_on_else_off[slot_index])
                {
                    // button blink light ON
                    if (now_epoch_millis > (button_light_blink_period_start_millis[slot_index] + SLOT_ENABLED_BLINK_BUTTON_ON_MILLIS))
                    {
                        button_light_blink_on_else_off[slot_index] = false;
                        setSingleDispenseButtonLight(slot, false);
                        button_light_blink_period_start_millis[slot_index] = now_epoch_millis;
                    }
                }
                else
                {
                    // button blink light OFF
                    if (now_epoch_millis > (button_light_blink_period_start_millis[slot_index] + SLOT_ENABLED_BLINK_BUTTON_OFF_MILLIS))
                    {
                        button_light_blink_on_else_off[slot_index] = true;
                        setSingleDispenseButtonLight(slot, true);
                        button_light_blink_period_start_millis[slot_index] = now_epoch_millis;
                    }
                }

                if (getDispenseButtonStateDebounced(slot))
                {
                    pumpCycle_state[slot_index] = state_button_pressed;
                    pump_start_delay_start_epoch[slot_index] = now_epoch_millis;
                    setSpoutSolenoid(slot, true);
                    setSingleDispenseButtonLight(slot, true);
                }
            }
            break;
            case state_button_pressed:
            {
                // wait state. handle releasing button/ losing slot enable
                if (!getDispenseButtonStateDebounced(slot) || !slotEnabled[slot_index])
                {
                    pumpCycle_state[slot_index] = state_init;
                }

                if (now_epoch_millis > (pump_start_delay_start_epoch[slot_index] + PUMP_START_DELAY_MILLIS))
                {
                    pumpCycle_state[slot_index] = state_pumping;
                    startPump(slot);
                }
            }
            break;
            case state_pumping:
            {
                if (!getDispenseButtonStateDebounced(slot) || !slotEnabled[slot_index])
                {

                    if (PUMP_BACKTRACK_TIME_MILLIS > 0)
                    {
                        // only chose this path if there is any backtracking at all.

                        stopPump(slot);
                        pumpCycle_state[slot_index] = state_button_released_pump_stopped;
                        pump_stop_before_backtrack_delay_start_epoch[slot_index] = now_epoch_millis;
                    }
                    else
                    {
                        pumpCycle_state[slot_index] = state_stop_pump;
                    }
                }
            }
            break;
            case state_button_released_pump_stopped:
            {
                if (now_epoch_millis > (pump_stop_before_backtrack_delay_start_epoch[slot_index] + PUMP_STOP_BEFORE_BACKTRACK_TIME_MILLIS))
                {
                    pumpCycle_state[slot_index] = state_pump_backtracking;
                    setPumpDirection(slot, false);
                    startPump(slot);
                    pump_backtrack_start_epoch[slot_index] = now_epoch_millis;
                }
            }
            break;
            case state_pump_backtracking:
            {

                if (now_epoch_millis > (pump_backtrack_start_epoch[slot_index] + PUMP_BACKTRACK_TIME_MILLIS))
                {

                    setPumpDirection(slot, true);

                    pumpCycle_state[slot_index] = state_stop_pump;
                }
            }
            break;

            case state_stop_pump:
            {
                pumpCycle_state[slot_index] = state_wait_solenoid_delay;
                stopPump(slot);
                solenoid_stop_delay_start_epoch[slot_index] = now_epoch_millis;
            }
            break;

            case state_wait_solenoid_delay:
            {
                if (getDispenseButtonStateDebounced(slot))
                {
                    pumpCycle_state[slot_index] = state_pumping;
                    startPump(slot);
                }

                if (now_epoch_millis > (solenoid_stop_delay_start_epoch[slot_index] + SOLENOID_STOP_DELAY_MILLIS))
                {
                    pumpCycle_state[slot_index] = state_slot_enabled;
                    setSpoutSolenoid(slot, false);
                    setSingleDispenseButtonLight(slot, false);
                }
            }
            default:
            {
                break;
            }
            }
        };
        break;
    default:
    {
        debugOutput::sendMessage("Pcb: DO NOT execute independent dispense cycle with this PCB", MSG_ERROR);
    }
    break;
    }
    }
}
unsigned char pcb::getPumpPWM()
{
    // pump speed is set globally. Not set per slot!
    // pwm_val = byte value max = 255
    switch (pcb_version)
    {
    case DSED8344_NO_PIC:
    {
        return ReadByte(MAX31760_ADDRESS, 0x50); // PWM value
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        float f_value;

        // Rescale so the PIC values match the old MAX31760 values.
        // The PIC takes PWM values in the 0-100 range.
        f_value = (float)ReadByte(PIC_ADDRESS, 0x00); // PWM value
        f_value = floor(f_value * 2.55);

        return ((unsigned char)f_value);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No PWM setting available for this pcb.", MSG_ERROR);
        return 0;
    }
    break;
    }

} // End of getPumpRPM()

void pcb::setPumpPWM(uint8_t pwm_val)
{
    // pump speed is set globally. Not set per slot!
    // pwm_val = byte value max = 255
    switch (pcb_version)
    {
    case DSED8344_NO_PIC:
    {
        SendByte(MAX31760_ADDRESS, 0x50, pwm_val); // PWM value
    };
    break;
    case (DSED8344_PIC_MULTIBUTTON):
    {
        // debugOutput::sendMessage("incoming=================" + std::to_string(pwm_val) , MSG_ERROR);
        float f_value;

        // Rescale so the PIC values match the old MAX31760 values.
        // The PIC takes PWM values in the [0-100] range. (includes 100)

        // f_value = (float)pwm_val;
        // f_value = floor(f_value / 100);

        // return SendByte(PIC_ADDRESS, 0x00, (unsigned char)f_value); // PWM value
        f_value = (float)pwm_val;
        f_value = floor(f_value / 2.55);
        unsigned char speed_percentage = (unsigned char)f_value;
        uint8_t inverted_percentage = 100 - (uint8_t)speed_percentage;
        setPumpSpeedPercentage(speed_percentage);
        debugOutput::sendMessage("speed percentage sent =" + std::to_string(speed_percentage), MSG_ERROR);
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        float f_value;

        // Rescale so the PIC values match the old MAX31760 values.
        // The PIC takes PWM values in the [0-100] range. (includes 100)

        // f_value = (float)pwm_val;
        // f_value = floor(f_value / 100);

        // return SendByte(PIC_ADDRESS, 0x00, (unsigned char)f_value); // PWM value
        f_value = (float)pwm_val;
        f_value = floor(f_value / 2.55);
        unsigned char speed_percentage = 100 - (unsigned char)f_value; // invert speed. pwm is inverted.
        setPumpSpeedPercentage((uint8_t)speed_percentage);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Pcb has No pwm setting available", MSG_ERROR);
    }
    break;
    }

} // End of setPumpPWM()

void pcb::setPumpSpeedPercentage(uint8_t speed_percentage)
{
    bool success = false;
    // pump speed is set globally. Not set per slot!
    // pwm_val = byte value max = 255
    switch (pcb_version)
    {
    case DSED8344_NO_PIC:
    case (DSED8344_PIC_MULTIBUTTON):
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {

        if (speed_percentage > 100)
        {
            debugOutput::sendMessage("Speed invalid. Will set to max. Please provide argument in [0..100] interval. Provided: " + to_string(speed_percentage), MSG_WARNING);
            speed_percentage = 100;
        }
        debugOutput::sendMessage("Speed percentage set: " + to_string(speed_percentage), MSG_INFO);
        success = SendByte(PIC_ADDRESS, 0x00, speed_percentage); // PWM value
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Pcb has no setPumpSpeedPercentage available", MSG_ERROR);
    }
    break;
    }
}

void pcb::setPumpsDisableAll()
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
    {
        unsigned char reg_value;

        reg_value = ReadByte(get_PCA9534_address_from_slot(1), 0x01);
        reg_value = reg_value & 0b11111000;
        SendByte(get_PCA9534_address_from_slot(1), 0x01, reg_value);
    };
    break;
    case (EN258_4SLOTS):
    case (EN134_4SLOTS):
    {
        uint8_t reg_value;
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            stopPump(slot);
            slotEnabled[slot - 1] = false;
        }
    };
    break;
    case (EN258_8SLOTS):
    case (EN134_8SLOTS):
    {
        uint8_t reg_value;
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            stopPump(slot);
            slotEnabled[slot - 1] = false;
        }
    };
    break;

    default:
    {
        debugOutput::sendMessage("Pcb: Error PCB NOT VALID!!11", MSG_ERROR);
    }
    break;
    }

} // End setPumpsDisableAll()

void pcb::setPumpEnable(uint8_t slot)
{
    // in 8344 one button: do not take into account button press, it is hardwired on the pcb
    // in 8344 multibutton:
    // in EN-134 version: enable = motor runs at set speed.
    // --> motor enable should be depending on button press.
    // slot = pump_number
    uint8_t slot_index = slot - 1;
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
    {

        unsigned char reg_value;

        reg_value = ReadByte(get_PCA9534_address_from_slot(1), 0x01); // slot 1 because there is only one PCA9534
        switch (slot)
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
            ;
        }
        SendByte(get_PCA9534_address_from_slot(1), 0x01, reg_value); // slot 1 because there is only one PCA9534

    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        debugOutput::sendMessage("Pcb: Enable pump " + to_string(slot), MSG_INFO);
        slotEnabled[slot_index] = true;
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!12", MSG_ERROR);
    }
    break;
    }
}

void pcb::startPump(uint8_t slot)
{
    switch (pcb_version)
    {
    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
        break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        if (slotEnabled[slot - 1])
        {
            debugOutput::sendMessage("Pcb: Start pump " + to_string(slot), MSG_INFO);
            setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_PUMP_ENABLE, true); // start pump
        }
        else
        {
            debugOutput::sendMessage("Pcb: Cannot start pump. Not enabled for slot:" + to_string(slot), MSG_WARNING);
        }
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        if (slotEnabled[slot - 1])
        {
            debugOutput::sendMessage("Pcb: Start pump " + to_string(slot), MSG_INFO);
            setMCP23017Output(slot, MCP23017_EN258_GPB2_PIN_OUT_PUMP, true, MCP23017_REGISTER_GPB); // stop pump
        }
        else
        {
            debugOutput::sendMessage("Pcb: Cannot start pump. Not enabled for slot:" + to_string(slot), MSG_WARNING);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: no pump available for unknown pcb", MSG_ERROR);
    }
    break;
    }
}

void pcb::stopPump(uint8_t slot)
{
    switch (pcb_version)
    {
    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
        break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        debugOutput::sendMessage("Pcb: Stop pump " + to_string(slot), MSG_INFO);
        setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_PUMP_ENABLE, false); // stop pump
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        debugOutput::sendMessage("Pcb: Stop pump " + to_string(slot), MSG_INFO);
        setMCP23017Output(slot, MCP23017_EN258_GPB2_PIN_OUT_PUMP, false, MCP23017_REGISTER_GPB); // stop pump
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Unknown Pcb has No pump", MSG_ERROR);
    }
    break;
    }
}

void pcb::setPumpDirection(uint8_t slot, bool forwardElseReverse)
{
    switch (pcb_version)
    {
    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        bool reverseElseForward = !forwardElseReverse;

        setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_PUMP_DIR, reverseElseForward);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Pcb has No direction setting", MSG_ERROR);
    }
    break;
    }

    // slotEnabled[slot-1] = slotEnabled;
}

///////////////////////////////////////////////////////////////////////////
// SOLENOID FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::setSolenoidFromArray(uint8_t slot, uint8_t position, bool onElseOff)
{
    // position starts from 1
    // slot starts from 1
    uint8_t solenoid_positions[8] = {MCP23017_EN258_GPB3_PIN_OUT_SOLENOID_1,
                                     MCP23017_EN258_GPB4_PIN_OUT_SOLENOID_2,
                                     MCP23017_EN258_GPB5_PIN_OUT_SOLENOID_3,
                                     MCP23017_EN258_GPB6_PIN_OUT_SOLENOID_4,
                                     MCP23017_EN258_GPB7_PIN_OUT_SOLENOID_5,
                                     MCP23017_EN258_GPA0_PIN_OUT_SOLENOID_6,
                                     MCP23017_EN258_GPA1_PIN_OUT_SOLENOID_7,
                                     MCP23017_EN258_GPA2_PIN_OUT_SOLENOID_8};
    uint8_t solenoid_positions_register[8] = {MCP23017_REGISTER_GPB,
                                              MCP23017_REGISTER_GPB,
                                              MCP23017_REGISTER_GPB,
                                              MCP23017_REGISTER_GPB,
                                              MCP23017_REGISTER_GPB,
                                              MCP23017_REGISTER_GPA,
                                              MCP23017_REGISTER_GPA,
                                              MCP23017_REGISTER_GPA};
    bool isValid = false;

    if (position <= 0)
    {
        debugOutput::sendMessage("Pcb ASSERT ERROR: First Solenoid position is 1  !!!!", MSG_ERROR);
    }

    switch (pcb_version)
    {

    case (EN258_4SLOTS):
    {

        if (slot > 0 && slot <= 4)
        {
            isValid = true;
        }
    }
    break;
    case (EN258_8SLOTS):
    {
        if (slot > 0 && slot <= 8)
        {
            isValid = true;
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No solenoid array function available for this pcb.", MSG_ERROR);
    }
    break;
    }

    if (isValid)
    {
        setMCP23017Output(slot, solenoid_positions[position - 1], onElseOff, solenoid_positions_register[position - 1]);
        debugOutput::sendMessage("Pcb: Array Solenoid# : " + std::to_string(position) + ". Slot: " + std::to_string(slot) + ". Enabled: " + std::to_string(onElseOff), MSG_INFO);
    }
}

void pcb::disableAllSolenoidsOfSlot(uint8_t slot)
{
    switch (pcb_version)
    {
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        setSpoutSolenoid(slot, false);
    };
    break;
    case (EN258_4SLOTS):
    {
        for (uint8_t position = 1; position <= 4; position++)
        {
            setSolenoidFromArray(slot, position, false);
        }
    }
    break;
    case (EN258_8SLOTS):
    {
        for (uint8_t position = 1; position <= 8; position++)
        {
            setSolenoidFromArray(slot, position, false);
        }
    }
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: No solenoid function available for this pcb.", MSG_ERROR);
    }
    break;
    }
}

void pcb::setAdditiveSolenoid(uint8_t slot, int additivePosition, bool onElseOff)
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
    {
        debugOutput::sendMessage("No solenoid used in pcb. ", MSG_INFO);
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        debugOutput::sendMessage("No base solenoid used in pcb. ", MSG_INFO);
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        switch (additivePosition)
        {
        case (1):
        {
            setSolenoidFromArray(slot, EN258_SOLENOID_ADDITIVE_1, onElseOff);
        }
        break;
        case (2):
        {
            setSolenoidFromArray(slot, EN258_SOLENOID_ADDITIVE_2, onElseOff);
        }
        break;
        case (3):
        {
            setSolenoidFromArray(slot, EN258_SOLENOID_ADDITIVE_3, onElseOff);
        }
        break;
        case (4):
        {
            setSolenoidFromArray(slot, EN258_SOLENOID_ADDITIVE_4, onElseOff);
        }
        break;
        case (5):
        {
            setSolenoidFromArray(slot, EN258_SOLENOID_ADDITIVE_5, onElseOff);
        }
        break;
        default:
        {
            debugOutput::sendMessage("Pcb: Additive position not available." + std::to_string(additivePosition), MSG_ERROR);
        }
        break;
        }
    }
    break;

    default:
    {
        debugOutput::sendMessage("Pcb: No single solenoid function available for this pcb.", MSG_ERROR);
    }
    break;
    }
}
void pcb::setBaseSolenoid(uint8_t slot, bool onElseOff)
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
    {
        debugOutput::sendMessage("No solenoid used in pcb. ", MSG_INFO);
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        debugOutput::sendMessage("No base solenoid used in pcb. ", MSG_INFO);
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        setSolenoidFromArray(slot, EN258_SOLENOID_BASE, onElseOff);
    }
    break;

    default:
    {
        debugOutput::sendMessage("Pcb: No single solenoid function available for this pcb.", MSG_ERROR);
    }
    break;
    }
}

void pcb::setSpoutSolenoid(uint8_t slot, bool onElseOff)
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC_MULTIBUTTON):
    {
        debugOutput::sendMessage("No solenoid used in pcb. ", MSG_INFO);
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        setPCA9534Output(slot, PCA9534_EN134_PIN_OUT_SOLENOID, onElseOff);
    };
    break;
    case (EN258_4SLOTS):
    case (EN258_8SLOTS):
    {
        setSolenoidFromArray(slot, EN258_SOLENOID_SPOUT, onElseOff);
    }
    break;

    default:
    {
        debugOutput::sendMessage("Pcb: No single solenoid function available for this pcb.", MSG_ERROR);
    }
    break;
    }
}

double pcb::getTemperature(TemperatureSensor sensor)
{

    double temperature = 666.0;

    switch (sensor)
    {
    case (external_sensor_fridge):
    {
        if (mcp9808_temperature_sensor_1_found)
        {
            temperature = getTemperatureFromMCP9808(TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_1_ADDRESS);
        }
        else
        {
            debugOutput::sendMessage("Pcb: temperature sensor not available (external_sensor_fridge)", MSG_ERROR);
        }
    }
    break;
    case (external_sensor_cavity):
    {
        if (mcp9808_temperature_sensor_2_found)
        {
            temperature = getTemperatureFromMCP9808(TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_2_ADDRESS);
        }
        else
        {
            debugOutput::sendMessage("Pcb: temperature sensor not available (external_sensor_cavity)", MSG_ERROR);
        }
    }
    break;
    case (pcb_sensor_fridge):
    {
        if (isTemperatureSensorADS7830Available())
        {
            temperature = getTemperatureFromADS7830(TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_2_ADDRESS, 0);
        }
        else
        {
            debugOutput::sendMessage("Pcb: temperature sensor not available (pcb_sensor_fridge)", MSG_ERROR);
        }
    }
    break;
    case (pcb_sensor_cavity):
    {
        if (isTemperatureSensorADS7830Available())
        {
            temperature = getTemperatureFromADS7830(TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_2_ADDRESS, 1);
        }
        {
            debugOutput::sendMessage("Pcb: temperature sensor not available (pcb_sensor_cavity)", MSG_ERROR);
        }
    }
    break;
    default:
    {
        debugOutput::sendMessage("Pcb: Unknown indicated temperature sensor.)", MSG_ERROR);
    }
    break;
    }
    return temperature;
}

double pcb::getTemperatureFromADS7830(uint8_t temperatureSensorI2CAddress, uint8_t position)
{
    debugOutput::sendMessage("Not yet implemented for postion" + std::to_string(position), MSG_WARNING);
    return 666.1;
}

double pcb::getTemperatureFromMCP9808(uint8_t temperatureSensorI2CAddress)
{

    double return_temperature;
    set_i2c_address(temperatureSensorI2CAddress);
    int temperature_bytes = i2c_smbus_read_word_data(i2c_handle, 0x05);
    if (temperature_bytes < 0)
    {
        debugOutput::sendMessage("Error did not read from temperature sensor mcp9808", MSG_INFO);
        return 1;
    }

    uint16_t msbint = temperature_bytes >> 8;
    uint16_t lsbint = temperature_bytes & 0x00FF;
    uint16_t temperature_bytes_swapped = (lsbint << 8) | msbint;

    temperature_bytes = temperature_bytes_swapped;
    uint16_t signBit = (temperature_bytes >> 12) & 0x01;   // Sign bit is at bit 12
    uint16_t temperatureData = temperature_bytes & 0x0FFF; // Temperature data is bits 11-0

    if ((temperatureData & 0x0800) != 0)
    {
        // Negative temperature
        temperatureData = (~temperatureData & 0x0FFF) + 1; // Two's complement conversion
        return_temperature = -1.0 * temperatureData * 0.0625;
    }
    else
    {
        // Positive temperature
        return_temperature = temperatureData * 0.0625;
    }

    return return_temperature;
}