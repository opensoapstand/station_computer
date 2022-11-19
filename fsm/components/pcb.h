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

#include "pcbEN134.h"

#define MAX_SLOT_COUNT 8
#define PCA9534_ADDRESS_SLOT_1 0b0100000
#define PCA9534_ADDRESS_SLOT_2 0b0100001
#define PCA9534_ADDRESS_SLOT_3 0b0100010
#define PCA9534_ADDRESS_SLOT_4 0b0100011
#define DUMMY_ADDRESS 0b0100011

#define PCA9534_TMP_SLOT2_ADDRESS PCA9534_ADDRESS_SLOT_2

#define PCA9534_PIN_OUT_SOLENOID 0
#define PCA9534_PIN_OUT_PUMP_ENABLE 1
#define PCA9534_PIN_OUT_PUMP_DIR 2
#define PCA9534_PIN_IN_PUMP_FAULT 3
#define PCA9534_PIN_IN_BUTTON 4
#define PCA9534_PIN_OUT_BUTTON_LED_LOW_IS_ON 5
#define PCA9534_PIN_IN_FLOW_SENSOR_TICKS 6
#define PCA9534_PIN_OUT_FLOW_SENSOR_ENABLE 7

#define PIC_ADDRESS 0b0110000
#define MAX31760_ADDRESS 0b1010000
#define DS2485Q_ADDRESS 0b1000000
#define MCP3424T_ADDRESS 0b1101000

#define PUMP_START_DELAY_MILLIS 500
#define PUMP_STOP_BEFORE_BACKTRACK_TIME_MILLIS 0
#define PUMP_BACKTRACK_TIME_MILLIS 0
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

    enum PcbVersion
    {
        INVALID,
        DSED8344_NO_PIC, // pre v3 board for 4 slots, but 1 button
        DSED8344_PIC,    // modified v3 version for 4 buttons. It has never been deployed without the modification
        EN134_4SLOTS,
        EN134_8SLOTS
    };

    pcb(void);
    pcb(const char *);
    ~pcb();
    void setup();
    void refresh();
    void initialize_pcb(void);
    bool define_pcb_version(void);
    PcbVersion get_pcb_version();
    bool isSlotAvailable(uint8_t slot);

    unsigned char getPumpPWM();
    bool setPumpPWM(uint8_t pwm_val);
    bool setPumpSpeedPercentage(uint8_t speed_percentage);
    bool setPumpsDisableAll();
    bool setPumpEnable(uint8_t slot);
    void pumpRefresh();
    bool setPumpDirection(uint8_t slot, bool forwardElseReverse);

    void setSingleDispenseButtonLight(uint8_t slot, bool onElseOff);
    bool getDispenseButtonStateDebounced(uint8_t slot);
    bool getDispenseButtonEdge(uint8_t slot);
    bool getDispenseButtonEdgePositive(uint8_t slot);
    bool getDispenseButtonEdgeNegative(uint8_t slot);
    void virtualButtonPressHack(uint8_t slot);
    void virtualButtonUnpressHack(uint8_t slot);
    void dispenseButtonRefresh();
    bool getDispenseButtonState(uint8_t slot);

    void setSolenoid(uint8_t slot, bool onElseOff);
    uint64_t getFlowSensorTotalPulses(uint8_t slot);
    uint64_t getFlowSensorPulsesSinceEnabling(uint8_t slot);
    void resetFlowSensorTotalPulses(uint8_t slot);
    void refreshFlowSensors();
    void flowSensorEnable(uint8_t slot);
    void flowSensorsDisableAll();
    // PcbVersion enum PcbVersion;

private:
    void EN134_PumpCycle_refresh(uint8_t slots);
    bool slot_pca9534_found[MAX_SLOT_COUNT];

    bool set_i2c_address(unsigned char address);
    void setup_i2c_bus(void);
    unsigned char ReadByte(unsigned char address, unsigned char reg);
    bool SendByte(unsigned char address, unsigned char reg, unsigned char byte);

    bool getPCA9534Input(uint8_t slot, int posIndex);
    void setPCA9534Output(uint8_t slot, int posIndex, bool onElseOff);
    uint8_t get_PCA9534_address_from_slot(uint8_t slot);

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

    uint64_t pump_start_delay_start_epoch[MAX_SLOT_COUNT];
    uint64_t pump_stop_before_backtrack_delay_start_epoch[MAX_SLOT_COUNT];
    uint64_t pump_backtrack_start_epoch[MAX_SLOT_COUNT];
    uint64_t solenoid_stop_delay_start_epoch[MAX_SLOT_COUNT];
    enum PcbVersion pcb_version;
    PumpSolenoidInteractionState pumpCycle_state[MAX_SLOT_COUNT];
    uint64_t button_light_blink_period_start_millis[MAX_SLOT_COUNT];
    bool button_light_blink_on_else_off[MAX_SLOT_COUNT];
};

#endif