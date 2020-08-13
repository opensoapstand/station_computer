//***************************************
//
// <dispense.cpp>
// <Contians functions that executes beverage dispense sequences>
//
// --> remove i2c control, i2c related is in i2cgpios.h/i2cgpios.cpp due to pcb change (06/12/2020)
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "dispense.h"

//not updated yet --> refer to dispenseLOW
void dispense::dispenseHIGH(int drink, int water, int air, int flowSensor, double calibration)
{
  //setgpio gpio;
  //interrupt interrupt;
  //i2c i2c;

  //int sensorGPIO[] = {359,358,355,388,415,416,417,339,419};
  //int sensorGPIO_size = sizeof(sensorGPIO)/sizeof(*sensorGPIO);
  //int I2C_size = 16;
  //int H2O_activation_time = 2;  //Duration(s) - H2O solenoid activation
  //int calibration = 1;

  //1.1.1 activate i2c channel 1 for Drinks
  //i2c.activate_Channel1();

  //1.1.1 activate i2c channel 2 for Water
  //i2c.activate_Channel2();

  //1.2 activate UDOO pins
  //for(int i = 0; i < sensorGPIO_size; i++){
  //  gpio.activateGPIO(sensorGPIO[i]);
  //}

  //2.1 setup sensor (input) pin
  //for (int i = 0; i < sensorGPIO_size; i++){

  //   if(i == sensor){
  //     gpio.setGPIO_input(sensorGPIO[i]);
  //     gpio.setGPIO_edge_both(sensorGPIO[i]);
  //     gpio.setGPIO_edge_rising(sensorGPIO[i]);
  //   }

  // //2.2 disable other sensor pins
  //   else{
  //     gpio.setGPIO_output(sensorGPIO[i]);
  //     gpio.setGPIO_low(sensorGPIO[i]);
  //   }
  // }

  //3.1 setup drink solenoid (output) pin
  // for (int i = 0; i < I2C_size; i++){

  //   if(i == drink){
  //     i2c.setpin_HIGH_Channel1(i);
  //   }
  // //3.2 disable other drink solenoid pins
  //   else{
  //     i2c.setpin_LOW_Channel1(i);
  //   }
  // }

  //3 disable all H2O solenoid pins

  //4 Initiate interrupt
  //interrupt.countEDGE_HighVolume(sensorGPIO[sensor], calibration);

  //5 Turn off drink solenoid
  //i2c.setpin_LOW_Channel1(drink);

  //6 Turn on water solenoid for x seconds
  // i2c.setpin_HIGH_Channel1(water);
  // sleep(H2O_activation_time);
  // i2c.setpin_LOW_Channel1(water);
}

void dispense::dispenseLOW(int sensor, int drink, int water, int air, double calibration)
{

  // gpio gpio;
  // interrupt interrupt;
  // //i2c i2c;

  // int sensorGPIO[] = { 359,358,355,388,415,416,417,339,419 };
  // int sensorGPIO_size = sizeof(sensorGPIO)/sizeof(*sensorGPIO);
  // int H2O_activation_time = 2;  //Duration(s) - H2O solenoid activation
  // int calibration = 1;

  // //1.1 activate i2c channel 1 pins
  // //i2c.activate_Channel1();

  // //1.2 activate UDOO pins
  // for(int i = 0; i < sensorGPIO_size; i++){
  //   gpio.activateGPIO(sensorGPIO[i]);
  // }

  // //2.1 setup sensor (input) pin
  // for (int i = 0; i < sensorGPIO_size; i++){

  //   if(i == sensor){
  //     gpio.setGPIO_input(sensorGPIO[i]);
  //     gpio.setGPIO_edge_both(sensorGPIO[i]);
  //     gpio.setGPIO_edge_rising(sensorGPIO[i]);
  //   }

  // //2.2 disable other sensor pins
  //   else {
  //     gpio.setGPIO_output(sensorGPIO[i]);
  //     gpio.setGPIO_low(sensorGPIO[i]);
  //   }
  // }

  // //3. setup drink solenoid (output) pin
  // //i2c.setpin_HIGH_Channel1(drink); //Set drink pin HIGH (Use i2c0-PA0 for testing)

  // //4 Initiate interrupt
  // interrupt.countEDGE_LowVolume(sensorGPIO[sensor], calibration);

  // //5 Turn off drink solenoid
  // //i2c.setpin_LOW_Channel1(drink);

  // //6 Turn on water solenoid for x seconds (Use i2c1-PA0 for testing)
  // // i2c.setpin_HIGH_Channel2(water);
  // // sleep(H2O_activation_time);
  // // i2c.setpin_LOW_Channel2(water);

  // //7 Turn on air solenoid for x seconds (use i2c0)

  // std::cout << "\nDispense sequence completed.\n\n";
}


void dispense::read_FlowSensor(int flowSensor, double calibration)
{
  //setup flow sensor
  gpio.activate_GPIO(flowSensor);
  gpio.setGPIO_input(flowSensor);
  //gpio.setGPIO_edge_both(flowSensor);
  gpio.setGPIO_edge_rising(flowSensor);

  std::cout << flowSensorInterrupt.countEDGE_HighVolume(flowSensor, calibration) << endl;
}