

#include "pcb.h"
#include <chrono>
#include <cmath>

#define DEFAULT_I2C_BUS "/dev/i2c-1"

#define __USE_SMBUS_I2C_LIBRARY__ 1

// Constructor that works out the name of the I2C bus
pcb::pcb(void)
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
            std::string message("pcbEN134: Error opening");
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

void pcb::refresh()
{
    usleep(100);
    dispenseButtonRefresh();
    refreshFlowSensors();
    pumpRefresh();
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

            pumpCycle_state[i] = state_idle;
            button_light_blink_on_else_off[i] = true;
        }
        is_initialized = true;
    }

    if (!define_pcb_version())
    {
        std::string message("pcbEN134: I2C bus ");
        // message.append(i2c_bus_name);
        message.append(" has a problem.");
        debugOutput::sendMessage(message, MSG_ERROR);
        return;
    }

    // set values to default
    initialize_pcb();
}

PcbVersion pcb::get_pcb_version(void)
{
    return pcb_version;
}

bool pcb::define_pcb_version(void)
{
    unsigned char i2c_probe_address;
    bool config_valid = true;

    for (uint8_t i = 0; i < MAX_SLOT_COUNT; i++)
    {
        slot_pca9534_found[i] = false;
    }

    for (i2c_probe_address = 0x03; i2c_probe_address <= 0x77; i2c_probe_address++)
    {
        // Go through all the addresses
        debugOutput::sendMessage("will test i2c address: " + to_string(i2c_probe_address), MSG_INFO);

        if (!set_i2c_address(i2c_probe_address))
        {
            std::string message("Error with i2c protocol");
            // return false;
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
            else if (i2c_probe_address == MAX31760_ADDRESS)
            {
                max31760_pwm_found = true;
                debugOutput::sendMessage("MAX31760 found on I2C bus for PWM and speed feedback", MSG_INFO);
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

    if (max31760_pwm_found)
    {
        // definitely 8344
        pcb_version = DSED8344_NO_PIC;
    }
    else if (pic_pwm_found)
    {
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
            //
            pcb_version = DSED8344_NO_PIC;
        }
        else
        {
            pcb_version = INVALID;
        }
    }
    else
    {
        pcb_version = INVALID;
    }

    switch (pcb_version)
    {
    case (INVALID):
    {
        debugOutput::sendMessage("Pcb not valid. Is it connected?", MSG_ERROR);

        if (!pic_pwm_found)
        {
            std::string message("No PWM generator found on I2C bus ");
            message.append(i2c_bus_name);
            debugOutput::sendMessage(message, MSG_ERROR);
            debugOutput::sendMessage("Pump control impossible.", MSG_ERROR);
            config_valid = false;
        }
    };
    break;

    case (DSED8344_NO_PIC):
    {
        debugOutput::sendMessage("Pcb DSED8344 without pic found. pwm generated by MAX31760", MSG_ERROR);
    };
    break;
    case (DSED8344_PIC):
    {
        debugOutput::sendMessage("Pcb DSED8344 with pwm generating pic found", MSG_ERROR);
        if (!slot_pca9534_found[0])
        {
            debugOutput::sendMessage("PCA9534 not found! ", MSG_ERROR);
        }
    };
    break;

    case (EN134_4SLOTS):
    {
        debugOutput::sendMessage("Pcb EN-134 4 slots found. ", MSG_ERROR);
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
        debugOutput::sendMessage("Pcb EN-134 8 slots found.", MSG_ERROR);
        for (uint8_t i = 0; i < 8; i++)
        {
            if (!slot_pca9534_found[i])
            {
                debugOutput::sendMessage("No controller found for slot" + to_string(i + 1), MSG_ERROR);
            }
        }
    };
    break;
    }

    return config_valid;
}

void pcb::initialize_pcb()
{
    // set initial values. depending on pcb version

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    break;
    case (DSED8344_PIC):
    {
    };
    break;
    case (EN134_4SLOTS):
    {
        // Initialize the PCA9534
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            SendByte(get_PCA9534_address_from_slot(slot), 0x01, 0b00100000); // Output pin values register (has no influence on input values)
            SendByte(get_PCA9534_address_from_slot(slot), 0x03, 0b01011000); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
        }

        setPumpsDisableAll();
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            setSolenoid(slot, false);
            setSingleDispenseButtonLight(slot, false);
        }
    };
    break;
    case (EN134_8SLOTS):
    {
        // Initialize the PCA9534
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            SendByte(get_PCA9534_address_from_slot(slot), 0x01, 0b00100000); // Output pin values register (has no influence on input values)
            SendByte(get_PCA9534_address_from_slot(slot), 0x03, 0b01011000); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
        }
        setPumpsDisableAll();
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            setSolenoid(slot, false);
            setSingleDispenseButtonLight(slot, false);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

bool pcb::isSlotAvailable(uint8_t slot)
{
    if (slot == 0)
    {
        debugOutput::sendMessage("Slot numbering starts at 1", MSG_ERROR);
    }

    return slot_pca9534_found[slot - 1];
}
///////////////////////////////////////////////////////////////////////////
// BUTTON FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::setSingleDispenseButtonLight(uint8_t slot, bool onElseOff)
{

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    break;
    case (DSED8344_PIC):
    {
        switch (slot)
        {

        case 1:
        {
            // has a mosfet in between
            setPCA9534Output(slot, 6, !onElseOff);
            break;
        }
        case 2:
        {
            setPCA9534Output(slot, 3, !onElseOff);
            break;
        }
        case 3:
        {
            setPCA9534Output(slot, 4, !onElseOff);
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
            // debugOutput::sendMessage("Set button light on", MSG_INFO);
            setPCA9534Output(slot, PCA9534_PIN_OUT_BUTTON_LED_LOW_IS_ON, false);
        }
        else
        {
            // debugOutput::sendMessage("Set button light off", MSG_INFO);
            setPCA9534Output(slot, PCA9534_PIN_OUT_BUTTON_LED_LOW_IS_ON, true);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }

} // End of setSingleDispenseButtonLight()

void pcb::virtualButtonPressHack(uint8_t slot)
{

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    case (DSED8344_PIC):
    {
        // // WARNING: This overrides the physical dispense button. As such, there is no fail safe mechanism.
        // // If the program crashes while the button is pressed, it might keep on dispensing *forever*.
        // unsigned char reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03);
        // reg_value = reg_value & 0b01111111;
        // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)

        // reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01);
        // reg_value = reg_value & 0b01111111; // virtual button press
        // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x01, reg_value);
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

void pcb::virtualButtonUnpressHack(uint8_t slot)
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    case (DSED8344_PIC):
    {
        // unsigned char reg_value = ReadByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03);
        // reg_value = reg_value | 0b10000000;
        // SendByte(PCA9534_TMP_SLOT2_ADDRESS, 0x03, reg_value); // Config register 0 = output, 1 = input (https://www.nxp.com/docs/en/data-sheet/PCA9534.pdf)
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

bool pcb::getDispenseButtonState(uint8_t slot)
{

    // bool isPressed = ((ReadByte(PCA9534_PIN_IN_BUTTON, 0x00) & 0x80) ? false : true); // low = pressed
    // bool isPressed = (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & PCA9534_PIN_IN_BUTTON) ;
    bool val = (ReadByte(get_PCA9534_address_from_slot(slot), 0x00) & (1 << PCA9534_PIN_IN_BUTTON));

    bool isPressed = !val;

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
    // as this is not in a separate thread, we'll need to call it some times...
    // up edge: state wait for debouncing.
    // down edge: instant react. Because the PWM is hardware disconnected right away. We don't want jitter on that (aka disable right away when negative edge is detected).

    for (uint8_t slot_index = 0; slot_index < SLOT_COUNT; slot_index++)
    {
        if (isSlotAvailable(slot_index + 1))
        {

            using namespace std::chrono;
            uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            bool state = getDispenseButtonState(slot_index + 1);

            if (state != dispenseButtonStateMemory[slot_index])
            {
                dispenseButtonDebounceStartEpoch[slot_index] = now_epoch_millis;
                dispenseButtonIsDebounced[slot_index] = false;
                // debugOutput::sendMessage("edge detected!" + std::to_string(dispenseButtonDebounceStartEpoch[slot_index]) + "state: " + std::to_string(state), MSG_INFO);
            }

            positive_edge_detected[slot_index] = false;
            negative_edge_detected[slot_index] = false;

            // up edge wait for stable
            if ((now_epoch_millis > (dispenseButtonDebounceStartEpoch[slot_index] + DISPENSE_BUTTON_DEBOUNCE_MILLIS)) && !dispenseButtonIsDebounced[slot_index] && state != dispenseButtonStateDebounced[slot_index])
            {
                // stable up edge detected
                dispenseButtonIsDebounced[slot_index] = true;
                debugOutput::sendMessage("Button press debounced. Slot:" + to_string(slot_index + 1) + " State:" + to_string(state), MSG_INFO);
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
    }
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
    case (DSED8344_PIC):
    {
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        flowSensorsDisableAll();
        setPCA9534Output(slot, PCA9534_PIN_OUT_FLOW_SENSOR_ENABLE, true);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }

    // Only one sensor can be enabled at a time to be safe (it causes the pulses to be combined from all sensors to a separate input in the Odyssey.)
}

void pcb::flowSensorsDisableAll()
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    break;
    case (DSED8344_PIC):
    {
    };
    break;
    case (EN134_4SLOTS):
    {
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            setPCA9534Output(slot, PCA9534_PIN_OUT_FLOW_SENSOR_ENABLE, false);
            flow_sensor_pulses_since_enable[slot - 1] = 0;
        }
    }
    break;
    case (EN134_8SLOTS):
    {
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            setPCA9534Output(slot, PCA9534_PIN_OUT_FLOW_SENSOR_ENABLE, false);
            flow_sensor_pulses_since_enable[slot - 1] = 0;
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

void pcb::refreshFlowSensors()
{
    // this is only need if flow rates are read from the PCA9534.
    // there is a specific line that goes the the Odyssey as an interrupt (which is how the soapstand app traditionally worked)
    // then, the reading of the register is not needed

    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    {
    };
    break;
    case (DSED8344_PIC):
    {
    };
    break;
    case (EN134_4SLOTS):
    {
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            refreshFlowSensor(slot);
        }
    };
    break;
    case (EN134_8SLOTS):
    {
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            refreshFlowSensor(slot);
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

void pcb::resetFlowSensorTotalPulses(uint8_t slot)
{
    uint8_t slot_index = slot - 1;
    flow_sensor_total_pulses[slot_index] = 0;
}

uint64_t pcb::getFlowSensorPulsesSinceEnabling(uint8_t slot)
{
    uint8_t slot_index = slot - 1;
    return flow_sensor_pulses_since_enable[slot_index];
}
uint64_t pcb::getFlowSensorTotalPulses(uint8_t slot)
{
    uint8_t slot_index = slot - 1;
    return flow_sensor_total_pulses[slot_index];
}
void pcb::refreshFlowSensor(uint8_t slot)
{
    if (slot == 0)
    {
        debugOutput::sendMessage("Slot error! Starts at 1!", MSG_ERROR);
    }
    uint8_t slot_index = slot - 1;

    using namespace std::chrono;
    uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    bool state = getPCA9534Input(slot, PCA9534_PIN_IN_FLOW_SENSOR_TICKS);

    if (now_epoch_millis > (flowSensorTickReceivedEpoch[slot_index] + FLOW_SENSOR_DEBOUNCE_MILLIS))
    {

        if (flowSensorStateMemory[slot_index] != state)
        {
            flow_sensor_total_pulses[slot_index]++;
            flow_sensor_pulses_since_enable[slot_index]++;
            debugOutput::sendMessage("Flow sensor at slot" + to_string(slot) + ". Pulse total: " + to_string(flow_sensor_pulses_since_enable[slot_index]), MSG_INFO);
            flowSensorTickReceivedEpoch[slot_index] = now_epoch_millis;
        }
        flowSensorStateMemory[slot_index] = state;
    }
}

///////////////////////////////////////////////////////////////////////////
// PUMP FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::pumpRefresh()
{
    switch (pcb_version)
    {
    case DSED8344_NO_PIC:
    {
        break;
    }
    case (DSED8344_PIC):
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
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

void pcb::EN134_PumpCycle_refresh(uint8_t slots)
{
    // low level state machine, to prevent damage to the solenoid. i.e. do not run pump with closed solenoid.

    for (uint8_t slot_index = 0; slot_index < 4; slot_index++)
    {
        uint8_t slot = slot_index + 1;
        using namespace std::chrono;
        uint64_t now_epoch_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        switch (pumpCycle_state[slot_index])
        {

        case state_init:
        {
            setSolenoid(slot, false);
            setSingleDispenseButtonLight(slot, false);
            setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, false);
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
                setSolenoid(slot, true);
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
                setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, true);
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

                    setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, false); // stop pump
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
                setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, true); // start pump
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
            setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, false);
            solenoid_stop_delay_start_epoch[slot_index] = now_epoch_millis;
        }
        break;

        case state_wait_solenoid_delay:
        {
            if (getDispenseButtonStateDebounced(slot))
            {
                pumpCycle_state[slot_index] = state_pumping;
                setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, true);
            }

            if (now_epoch_millis > (solenoid_stop_delay_start_epoch[slot_index] + SOLENOID_STOP_DELAY_MILLIS))
            {
                pumpCycle_state[slot_index] = state_slot_enabled;
                setSolenoid(slot, false);
                setSingleDispenseButtonLight(slot, false);
            }
        }
        default:
        {
            break;
        }
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
    case (DSED8344_PIC):
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
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
        return 0;
    }
    break;
    }

} // End of getPumpRPM()

bool pcb::setPumpPWM(uint8_t pwm_val)
{
    // pump speed is set globally. Not set per slot!
    // pwm_val = byte value max = 255
    switch (pcb_version)
    {
    case DSED8344_NO_PIC:
    {
        return SendByte(MAX31760_ADDRESS, 0x50, pwm_val); // PWM value
    };
    break;
    case (DSED8344_PIC):
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
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }

} // End of setPumpPWM()

bool pcb::setPumpSpeedPercentage(uint8_t speed_percentage)
{
    if (speed_percentage > 100)
    {
        debugOutput::sendMessage("Speed invalid. Will set to max. Please provide argument in [0..100] interval. Provided: " + to_string(speed_percentage), MSG_WARNING);
        speed_percentage = 100;
    }
    return SendByte(PIC_ADDRESS, 0x00, speed_percentage); // PWM value
}

bool pcb::setPumpsDisableAll()
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC):
    {
        unsigned char reg_value;

        reg_value = ReadByte(get_PCA9534_address_from_slot(1), 0x01);
        reg_value = reg_value & 0b11111000;
        SendByte(get_PCA9534_address_from_slot(1), 0x01, reg_value);

        return true;
    };
    break;
    case (EN134_4SLOTS):
    {
        uint8_t reg_value;
        for (uint8_t slot = 1; slot <= 4; slot++)
        {
            setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, false);
            slotEnabled[slot - 1] = false;
        }
    };
    break;
    case (EN134_8SLOTS):
    {
        uint8_t reg_value;
        for (uint8_t slot = 1; slot <= 8; slot++)
        {
            setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_ENABLE, false);
            slotEnabled[slot - 1] = false;
        }
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }

} // End setPumpsDisableAll()

bool pcb::setPumpEnable(uint8_t slot)
{
    // in 8344 versions: do not take into account button press, it is hardwire on the pcb
    // in EN-134 version: enable = motor runs at set speed.
    // --> motor enable should be depending on button press.
    // slot = pump_number
    uint8_t slot_index = slot - 1;
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC):
    {

        unsigned char reg_value;

        reg_value = ReadByte(get_PCA9534_address_from_slot(slot), 0x01);
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
            return false;
        }
        SendByte(get_PCA9534_address_from_slot(slot), 0x01, reg_value);

        return true;
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        slotEnabled[slot_index] = true;
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

bool pcb::setPumpDirection(uint8_t slot, bool forwardElseReverse)
{
    // remember rotating or not.
    // bool slotEnabled = slotEnabled[slot-1];
    // setPumpsDisableAll();
    // usleep(1000000);
    bool reverseElseForward = !forwardElseReverse;

    setPCA9534Output(slot, PCA9534_PIN_OUT_PUMP_DIR, reverseElseForward);
    // slotEnabled[slot-1] = slotEnabled;
}

///////////////////////////////////////////////////////////////////////////
// SOLENOID FUNCTIONS
///////////////////////////////////////////////////////////////////////////

void pcb::setSolenoid(uint8_t slot, bool onElseOff)
{
    switch (pcb_version)
    {

    case (DSED8344_NO_PIC):
    case (DSED8344_PIC):
    {
        debugOutput::sendMessage("No solenoid used in pcb. ", MSG_INFO);
    };
    break;
    case (EN134_4SLOTS):
    case (EN134_8SLOTS):
    {
        setPCA9534Output(slot, PCA9534_PIN_OUT_SOLENOID, onElseOff);
    };
    break;
    default:
    {
        debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
    }
    break;
    }
}

// switch (pcb_version)
// {

// case (DSED8344_NO_PIC):
// {
// };
// break;
// case (DSED8344_PIC):
// {
// };
// break;
// case (EN134_4SLOTS):
// {
// };
// break;
// case (EN134_8SLOTS):
// {
// };
// break;
// default:
// {
//     debugOutput::sendMessage("Error PCB NOT VALID!!", MSG_ERROR);
// }
// break;
// }
