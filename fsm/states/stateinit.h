//***************************************
//
// stateinit.h
// init state class
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
        stateInit(int inputDigit); //debug through terminal
        ~stateInit();

        TiXmlElement *getSolenoid(char* dispenserID); 

        DF_ERROR onEntry();
        DF_ERROR onAction();
        DF_ERROR onExit();

    private:    
        TiXmlDocument *m_pXMLSettings;
        TiXmlElement *m_pRoot, *m_pHardware, *m_pDispenser;

        const char* dispenserId[CASSETTES_MAX];

        DF_ERROR setDispenserId();
        DF_ERROR setDispenser(TiXmlElement *dispenserEle, int index);

        const char* getXML(const char* subHeader, TiXmlElement *solenoidEle);
};

//first layer
#define DRINKFILL_STRING "DRINKFILL"

//second layer
#define HARDWARE_STRING "hardware"

//third layer
#define DISPENSER_STRING "dispenser"
#define ID_STRING "id"

//forth layer
#define SOLENOID_STRING "solenoid"
#define TYPE_STRING "type"
#define IO_STRING "io"
#define FLOWSENSOR_STRING "flowsensor"

#define X86_STRING "x86"
#define MCP_STRING "mcp"
#define ARD_STRING "ard"

#endif