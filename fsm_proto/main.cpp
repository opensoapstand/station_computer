//***************************************
//
// <main.cpp>
// <Main function of FSM>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved

#include "fsm.h"
#include "levelSensor.h"

int main(){

  fsm fsm;
  levelSensor levelSensor;

  std::thread thread1(&fsm::startFSM, fsm);
  std::thread thread2(&levelSensor::readValue, levelSensor);

  thread1.join();
  thread2.join();

  return 0;
}
