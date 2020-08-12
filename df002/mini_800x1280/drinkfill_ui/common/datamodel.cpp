//***************************************
//
// datamodel.cpp
// System status class
//
// created: 16-MAY-2019
// by: Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "datamodel.h"
#include <iostream>
#include <string>
#include <string.h>
#include <chrono>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <thread>

datamodel::datamodel(){
    currentState = df_State::df_INIT;
    setDispensedVolume(0);
}

datamodel::~datamodel() {
    // no actions

}

void datamodel::setCurrentState(df_State state) {
    //!!! do a test for valid entries
    this->currentState = state;
}

datamodel::df_State datamodel::getSystemState(){
    return this->currentState;
}

uint16_t datamodel::getDispensingVolume() const {
    return dispensingVolume;
}

void datamodel::setDispensedVolume(uint16_t volume) {
    this->dispensedVolume = volume;
}

uint16_t datamodel::getDispensedVolume(){
    return dispensedVolume;
}

