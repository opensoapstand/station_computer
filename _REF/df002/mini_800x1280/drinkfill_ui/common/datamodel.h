//***************************************
//
// datamodel.h
// System status class
//
// created: 16-MAY-2019
// by: Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef DATAMODEL_H__
#define DATAMODEL_H__


#define MAX_BEVS 4
#define MAX_DISPENSE 1000
#include<iostream>
#include "../fsm.h"

class datamodel {
public:

    datamodel();
    ~datamodel();

    enum df_State {
        df_INIT,
        df_IDLE,
        df_DISPENSE,
        df_DISPENSE_PAUSED,
        df_DISPENSE_ERROR,
        df_DISPENSE_ENDED,
        df_UPDATE_INVENTORY,
        df_ERROR
    };

    uint8_t getNumberofBeverages();

    bool setDispenseParameters(uint8_t bevToDispense, uint16_t volume);

    df_State getSystemState();
    void setCurrentState(df_State currentState);

    uint16_t getDispensingVolume() const;
    uint16_t getDispensedVolume();
    void setDispensedVolume(uint16_t volume);
    uint8_t getDispensingBeverage();
    bool setDispensingParameters();


private:


    uint8_t bevToDispense;
    uint16_t dispensingVolume;
    uint16_t dispensedVolume;
    df_State currentState;
};


#endif //DATAMODEL_H__
