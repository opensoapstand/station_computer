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


#endif