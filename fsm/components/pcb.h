#ifndef _pcb_H
#define _pcb_H

#include <cstddef>
#include <cstdint>

// This is a bit of a hack, but on the Odyssey we need to explicitly
// include code to access the I2C bus.  We could probably make this
// work cleanly on the rPi but I got tired of fighting compiler
// warnings...
#ifdef __arm__
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#else
#include "../smbus/smbus.h"
#endif

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../dftypes.h"

#include "../objects/debugOutput.h"

#define MAX_BUF 64

#define MAX_SLOT_COUNT 8
#define PCA9534_ADDRESS_SLOT_1 0b0100000 // EN134
#define PCA9534_ADDRESS_SLOT_2 0b0100001 // EN134
#define PCA9534_ADDRESS_SLOT_3 0b0100010 // EN134
#define PCA9534_ADDRESS_SLOT_4 0b0100011 // EN134

#define MCP23017_ADDRESS_SLOT_1 0b0100000 // EN258
#define MCP23017_ADDRESS_SLOT_2 0b0100001 // EN258
#define MCP23017_ADDRESS_SLOT_3 0b0100010 // EN258
#define MCP23017_ADDRESS_SLOT_4 0b0100011 // EN258
#define MCP23017_ADDRESS_SLOT_5 0b0100100 // EN258
#define MCP23017_ADDRESS_SLOT_6 0b0100101 // EN258
#define MCP23017_ADDRESS_SLOT_7 0b0100110 // EN258
#define MCP23017_ADDRESS_SLOT_8 0b0100111 // EN258

#define PIC_ADDRESS 0b0110000      // EN134 and DSED8344
#define MAX31760_ADDRESS 0b1010000 // DSED8344
// #define DS2485Q_ADDRESS 0b1000000
// #define MCP3424T_ADDRESS 0b1101000
#define ADC081C021_CURRENT_SENSOR_ADDRESS 0b1010100 // EN134 EN258

#define TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_1_ADDRESS 0b0011000 // EN134 EN258
#define TEMPERATURE_OPTIONAL_EXTERNAL_MCP9808_2_ADDRESS 0b0011001 // EN134 EN258
#define TEMPERATURE_ADC_ADS7830 0b1001000                        // EN258

#define DUMMY_ADDRESS 0b0100011

#define PCA9534_TMP_SLOT2_ADDRESS PCA9534_ADDRESS_SLOT_2

#define PCA9534_DSED8344_PIN_IN_BUTTON 7

#define PCA9534_EN134_PIN_OUT_SOLENOID 0
#define PCA9534_EN134_PIN_OUT_PUMP_ENABLE 1
#define PCA9534_EN134_PIN_OUT_PUMP_DIR 2
#define PCA9534_EN134_PIN_IN_PUMP_FAULT 3
#define PCA9534_EN134_PIN_IN_BUTTON 4
#define PCA9534_EN134_PIN_OUT_BUTTON_LED_LOW_IS_ON 5
#define PCA9534_EN134_PIN_IN_FLOW_SENSOR_TICKS 6
#define PCA9534_EN134_PIN_OUT_FLOW_SENSOR_ENABLE 7



#define MCP23017_REGISTER_GPA 0x09
#define MCP23017_REGISTER_GPB 0x19

#define MCP23017_EN258_GPA0_PIN_OUT_SOLENOID_6 0
#define MCP23017_EN258_GPA1_PIN_OUT_SOLENOID_7 1
#define MCP23017_EN258_GPA2_PIN_OUT_SOLENOID_8 2
#define MCP23017_EN258_GPA3_NOT_USED 3
#define MCP23017_EN258_GPA4_NOT_USED 4
#define MCP23017_EN258_GPA5_NOT_USED 5
#define MCP23017_EN258_GPA6_PIN_IN_FLOW_SENSOR_AICHI 6
#define MCP23017_EN258_GPA7_PIN_OUT_FLOW_SENSOR_DIGMESA 7

#define MCP23017_EN258_GPB0_PIN_IN_BUTTON 0
#define MCP23017_EN258_GPB1_PIN_OUT_BUTTON_LED_LOW_IS_ON 1
#define MCP23017_EN258_GPB2_PIN_OUT_PUMP 2
#define MCP23017_EN258_GPB3_PIN_OUT_SOLENOID_1 3
#define MCP23017_EN258_GPB4_PIN_OUT_SOLENOID_2 4
#define MCP23017_EN258_GPB5_PIN_OUT_SOLENOID_3 5
#define MCP23017_EN258_GPB6_PIN_OUT_SOLENOID_4 6
#define MCP23017_EN258_GPB7_PIN_OUT_SOLENOID_5 7


#define PUMP_START_DELAY_MILLIS 100
#define PUMP_STOP_BEFORE_BACKTRACK_TIME_MILLIS 0
#define PUMP_BACKTRACK_TIME_MILLIS 100
#define SOLENOID_STOP_DELAY_MILLIS 500

#define SLOT_ENABLED_BLINK_BUTTON_ON_MILLIS 200
#define SLOT_ENABLED_BLINK_BUTTON_OFF_MILLIS 500

class pcb
{
public:
    enum PumpSolenoidInteractionState
    {
        state_init,
        state_idle,
        state_slot_enabled,
        state_button_pressed,
        state_pumping,
        state_button_released_pump_stopped,
        state_pump_stopped_before_backtrack,
        state_pump_backtracking,
        state_stop_pump,
        state_wait_solenoid_delay
    };
    enum TemperatureSensor
    {
        external_sensor_fridge,
        external_sensor_cavity,
        pcb_sensor_fridge,
        pcb_sensor_cavity
    };

    enum PcbVersion
    {
        INVALID,
        DSED8344_NO_PIC,          // pre v3 board for 4 slots, with 1 button
        DSED8344_PIC_MULTIBUTTON, // modified v3 version for 4 buttons. It has never been deployed without the modification!!
        EN134_4SLOTS,
        EN134_8SLOTS,
        EN258_4SLOTS,
        EN258_8SLOTS
    };

    pcb(void);
    pcb(const char *);
    ~pcb();
    void setup();
    void pcb_refresh();
    void initialize_pcb(void);
    bool define_pcb_version(void);
    PcbVersion get_pcb_version();
    bool isSlotAvailable(uint8_t slot);
    bool isTemperatureSensorMCP9808Available_1();
    bool isTemperatureSensorMCP9808Available_2(); // 2nd sensor
    bool isTemperatureSensorADS7830Available();   // 2nd sensor

    
    unsigned char getPumpPWM();
    bool setPumpPWM(uint8_t pwm_val);
    bool setPumpSpeedPercentage(uint8_t speed_percentage);
    bool setPumpsDisableAll();
    bool setPumpEnable(uint8_t slot);
    void independentDispensingRefresh();
    bool setPumpDirection(uint8_t slot, bool forwardElseReverse);
    bool startPump(uint8_t slot);
    bool stopPump(uint8_t slot);

    double getTemperature(TemperatureSensor sensor);

    double getTemperatureFromADS7830(uint8_t temperatureSensorI2CAddress, uint8_t position);
    double getTemperatureFromMCP9808(uint8_t temperatureSensorI2CAddress);

    void setSingleDispenseButtonLight(uint8_t slot, bool onElseOff);
    bool getDispenseButtonStateDebounced(uint8_t slot);
    bool getDispenseButtonEdge(uint8_t slot);
    bool getDispenseButtonEdgePositive(uint8_t slot);
    bool getDispenseButtonEdgeNegative(uint8_t slot);
    void virtualButtonPressHack(uint8_t slot);
    void virtualButtonUnpressHack(uint8_t slot);
    void dispenseButtonRefresh();
    void dispenseButtonRefreshPerSlot(uint8_t slot);
    bool getDispenseButtonState(uint8_t slot);

    void disableAllSolenoidsOfSlot(uint8_t slot);
    void setSolenoidFromArray(uint8_t slot, uint8_t position, bool onElseOff);
    void setSolenoidOnePerSlot(uint8_t slot, bool onElseOff);
    uint64_t getFlowSensorTotalPulses(uint8_t slot);
    uint64_t getFlowSensorPulsesSinceEnabling(uint8_t slot);
    void resetFlowSensorTotalPulses(uint8_t slot);
    void refreshFlowSensors();
    void flowSensorEnable(uint8_t slot);
    void flowSensorsDisableAll();
    // PcbVersion enum PcbVersion;
    enum PcbVersion pcb_version;
    uint8_t PCA9534ReadRegisterFromSlot(uint8_t slot, uint8_t reg);

    void PCA9534SendByteToSlot(uint8_t slot, unsigned char reg, unsigned char byte);
    void sendEN134DefaultConfigurationToPCA9534(uint8_t slot, bool reportIfModified);
    void sendByteIfNotSetToSlot(uint8_t slot, unsigned char reg, unsigned char value, bool reportIfModified);

    void EN134_PumpCycle_refresh(uint8_t slots);

    bool set_i2c_address(unsigned char address);
    void setup_i2c_bus(void);
    unsigned char ReadByte(unsigned char address, unsigned char reg);
    bool SendByte(unsigned char address, unsigned char reg, unsigned char byte);
    bool getPCA9534Input(uint8_t slot, int posIndex);
    void setPCA9534Output(uint8_t slot, int posIndex, bool onElseOff);
    uint8_t get_PCA9534_address_from_slot(uint8_t slot);
    bool getMCP23017Input(uint8_t slot, int posIndex, uint8_t GPIORegister);
    void setMCP23017Output(uint8_t slot, int posIndex, bool onElseOff, uint8_t GPIORegister);
    uint8_t get_MCP23017_address_from_slot(uint8_t slot);
    uint8_t getMCP23017Register(uint8_t slot, uint8_t reg);
    void setMCP23017Register(uint8_t slot, uint8_t reg, uint8_t value);
    void setFlowSensorTypeEN258(uint8_t slot, bool isDigmesaElseAichi);
    bool getFlowSensorTypeEN258DigmesaElseAichi(uint8_t slot);
private:


    bool flowSensorDigmesaElseAichi[MAX_SLOT_COUNT];
    bool dispenseButtonStateMemory[MAX_SLOT_COUNT];
    bool dispenseButtonIsDebounced[MAX_SLOT_COUNT];
    bool dispenseButtonStateDebounced[MAX_SLOT_COUNT];
    bool positive_edge_detected[MAX_SLOT_COUNT];
    bool negative_edge_detected[MAX_SLOT_COUNT];
    uint64_t dispenseButtonDebounceStartEpoch[MAX_SLOT_COUNT];

    bool slotEnabled[MAX_SLOT_COUNT]; // in SED8433 this is not needed(pump ON =  pump enable  with button hardwired on pcb ). in EN-134: this enable high AND button press -> pump ON
    // bool slotEnabledMemory[MAX_SLOT_COUNT];
    void refreshFlowSensor(uint8_t slot);

    uint64_t flowSensorTickReceivedEpoch[MAX_SLOT_COUNT];
    bool flowSensorStateMemory[MAX_SLOT_COUNT];

    uint64_t flow_sensor_total_pulses[MAX_SLOT_COUNT];
    uint64_t flow_sensor_pulses_since_enable[MAX_SLOT_COUNT];

    uint64_t button_[MAX_SLOT_COUNT];

    bool is_initialized;
    int i2c_handle = -1;
    char *i2c_bus_name;

    bool pic_pwm_found = false;
    bool max31760_pwm_found = false;
    bool current_sensor_found = false;
    bool temperature_ads7830_found = false;
    bool mcp9808_temperature_sensor_1_found = false;
    bool mcp9808_temperature_sensor_2_found = false;
    bool slot_pca9534_found[MAX_SLOT_COUNT];
    bool slot_mcp23017_found[MAX_SLOT_COUNT];

    uint64_t pump_start_delay_start_epoch[MAX_SLOT_COUNT];
    uint64_t pump_stop_before_backtrack_delay_start_epoch[MAX_SLOT_COUNT];
    uint64_t pump_backtrack_start_epoch[MAX_SLOT_COUNT];
    uint64_t solenoid_stop_delay_start_epoch[MAX_SLOT_COUNT];

    PumpSolenoidInteractionState pumpCycle_state[MAX_SLOT_COUNT];
    uint64_t button_light_blink_period_start_millis[MAX_SLOT_COUNT];
    bool button_light_blink_on_else_off[MAX_SLOT_COUNT];
};

#endif
