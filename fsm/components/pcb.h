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

enum pcb_version
{
    INVALID,
    DSED8344_NO_PIC,
    DSED8344_PIC,
    EN134_4SLOTS,
    EN134_8SLOTS
};

class pcb
{
public:
    pcb(void);
    pcb(const char *);
    ~pcb();
    void setup();
    void refresh();
    void initialize_pcb(void);
    bool check_pcb_version(void);
    bool isSlotAvailable(uint8_t slot);

    unsigned char getPumpPWM();
    bool setPumpPWM(uint8_t pwm_val);
    bool setPumpSpeedPercentage(uint8_t speed_percentage);
    bool setPumpsDisableAll();
    bool setPumpEnable(uint8_t slot);

    bool getDispenseButtonStateDebounced(uint8_t slot);
    bool getDispenseButtonEdge(uint8_t slot);
    bool getDispenseButtonEdgePositive(uint8_t slot);
    bool getDispenseButtonEdgeNegative(uint8_t slot);
    void setSingleDispenseButtonLight(uint8_t slot, bool onElseOff);
    void virtualButtonPressHack(void);
    void virtualButtonUnpressHack(void);
    void dispenseButtonRefresh();
    bool getDispenseButtonState(uint8_t slot);

    void setSolenoid(uint8_t slot, bool onElseOff);

    void refreshFlowSensors();
    void flowSensorEnable(uint8_t slot);
    void flowSensorsDisableAll();

private:

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

    void refreshFlowSensor(uint8_t slot);

    uint64_t flowSensorTickReceivedEpoch [SLOT_COUNT];
    bool flowSensorStateMemory[SLOT_COUNT];
    

    bool is_initialized;
    int i2c_handle = -1;
    char *i2c_bus_name;

    bool pic_pwm_found = false;
    bool max31760_pwm_found = false;

    enum pcb_version pcb_version;
};

#endif