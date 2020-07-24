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
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#ifndef STATEINIT__H_
#define STATEINIT__H_

#include "../dftypes.h"
#include "../objects/messagemediator.h"
#include "statevirtual.h"
#include "../tinyxml/tinyxml.h"

class stateInit : public stateVirtual
{
    public:
        stateInit();
        stateInit(messageMediator * message); //debug through local network 
        ~stateInit();

        string toString();

        TiXmlElement *getSolenoid(char* dispenserID); 

        DF_ERROR onEntry();
        DF_ERROR onAction(dispenser* cassettes);
        DF_ERROR onExit();

        dispenser* dispenserSetup();


    private:   
        TiXmlDocument *m_pXMLSettings;
        TiXmlElement *m_pRoot, *m_pHardware, *m_pDispenser;

        const char* dispenserId[CASSETTES_MAX];

        DF_ERROR setDispenserId();
        DF_ERROR setDispenserSolenoid(TiXmlElement *dispenserEle, int dispenserIdx, dispenser cassettes[]);
        DF_ERROR setDispenserFlowSensor(TiXmlElement *dispenserEle, int dispenserIdx, dispenser cassettes[]);
        DF_ERROR setDispenserPump(TiXmlElement *dispenserEle, int dispenserIdx, dispenser cassettes[]);
        DF_ERROR setButton(TiXmlElement *hardwareEle, int dispenserIdx);
        const char* getXML(const char* subHeader, TiXmlElement *childEle);

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