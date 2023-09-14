//***************************************
//
// stateinit.h
// init state class
//
// Set Objects for FSM and Addresses for
// GPIO's through XML Reference.
// Check all Hardware is operational.
// Initialize threads for operation and
// communication in machine.
// Connect or create database.
//
// created: 01-2022
// by: Lode Ameije, Ash Singla, Udbhav Kansal & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#ifndef STATEINIT__H_
#define STATEINIT__H_

#include "../dftypes.h"
#include "../objects/messageMediator.h"
#include "stateVirtual.h"
#include "../../library/tinyxml/tinyxml.h"
#include "../objects/machine.h"

class stateInit : public stateVirtual
{
public:
    stateInit();
    stateInit(messageMediator *message); //debug through local network
    ~stateInit();

    string toString();

    TiXmlElement *getSolenoid(char *dispenserID);

    DF_ERROR onEntry();
    DF_ERROR onAction();
    DF_ERROR onExit();

    DF_ERROR dispenserSetup();

private:
    TiXmlDocument *m_pXMLSettings;
    TiXmlElement *m_pRoot, *m_pHardware, *m_pDispenser;

    const char *dispenserId[PRODUCT_DISPENSERS_MAX];

    DF_ERROR setDispenserId();
    DF_ERROR setDispenserSolenoid(TiXmlElement *dispenserEle, int dispenserIdx, dispenser *productDispensers);
    DF_ERROR setDispenserFlowSensor(TiXmlElement *dispenserEle, int dispenserIdx, dispenser *productDispensers);
    DF_ERROR setDispenserPump(TiXmlElement *dispenserEle, int dispenserIdx, dispenser *productDispensers);
    DF_ERROR setButton(TiXmlElement *hardwareEle, int dispenserIdx);
    DF_ERROR setProducts();

    const char *getXML(const char *subHeader, TiXmlElement *childEle);

    sqlite3 *db;
    int rc;

    gpio* pin_vpp;
    gpio* pin_pgc;
    gpio* pin_pgd;
    
    int slot;
    string name;
    double volume_dispensed;
    double volume_target_l;
    double volume_target_s;
    double calibration_const;
    double price_l;
    double price_s;
    int is_still;
    double volume_per_tick;

    bool level = true;

protected:
};

// **** XML DEPTH ***
//first layer
#define DRINKFILL_STRING "DRINKFILL"

//second layer
#define HARDWARE_STRING "hardware"

//third layer
#define DISPENSER_STRING "dispenser"
#define ID_STRING "id"

#define BUTTON_STRING "button"
#define BUTTONPRESS_STRING "buttonpress"

//forth layer
#define SOLENOID_STRING "solenoid"
#define TYPE_STRING "type"
#define I2CADDRESS_STRING "i2caddress"
#define IO_STRING "io"
#define FLOWSENSOR_STRING "flowsensor"
#define X86_STRING "x86"
#define MCP_STRING "mcp"
#define ARD_STRING "ard"
#define PUMP_STRING "pump"

#endif
