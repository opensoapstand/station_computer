//***************************************
//
// gpio.h
// GPIO Abstract Class Implementation
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
//***************************************

#include "gpio.h"

// CTOR
gpio::gpio()
{
        // m_nAddress = -1; //set negative one for illegal i2c address
        m_nPin = -1; // set negative one for illegal pin address
        m_stop = false;
        m_input = false;
        m_i2c = false; // no i2c chip associate with it
        gpioThread = nullptr;
        // m_pDispenser = nullptr; // UNCOMMENT LODE
}

// DTOR
gpio::~gpio()
{
        #ifdef INTERRUPT_DRIVE_FLOW_SENSOR_TICKS
        // kill thread!
        this->stopListener_flowsensor();
        #endif
}
// void gpio::setPinNumber(int pinNumber){

// }
#ifdef INTERRUPT_DRIVE_FLOW_SENSOR_TICKS
// kick off a listener_flowsensor
void gpio::startListener_flowsensor()
{
        debugOutput::sendMessage("Thread init: Flowsensor tick listener", MSG_INFO);
        DF_ERROR df_ret = ERROR_BAD_PARAMS;

        // if ((nullptr == gpioThread) && (nullptr != m_pDispenser)) // UNCOMMENT LODE
        if ((nullptr == gpioThread))
        {
                gpioThread = new std::thread(&gpio::listener_flowsensor, this);
                df_ret = OK;
        }
        else
        {
                debugOutput::sendMessage("Did not pass null check", MSG_INFO);
        }
}
#endif
// void gpio::startListener_buttons_powerAndMaintenance()
// {
//         debugOutput::sendMessage("Thread init: Maintenance Button and Shutdown Button listener", MSG_INFO);
//         DF_ERROR df_ret = ERROR_BAD_PARAMS;

//         //        if ((nullptr ==  gpioThread) && (nullptr != m_pDispenser)){
//         gpioThread = new std::thread(&gpio::listener_buttons_powerAndMaintenance, this);
//         df_ret = OK;
//         //        } else {
//         //                debugOutput::sendMessage("Did not pass null check", MSG_INFO);
//         //        }
// }

// void gpio::startButtonListener()
//{
//         debugOutput::sendMessage("-----startListener-----", MSG_INFO);
//         DF_ERROR df_ret = ERROR_BAD_PARAMS;

//        if ((nullptr ==  gpioThread)){
//                gpioThread = new std::thread(&gpio::listener, this);
//                df_ret = OK;
//        } else {
//                debugOutput::sendMessage("Did not pass null check", MSG_INFO);
//        }
//}

// TODO: Threaded GPIO interrupt...
// call this with code that looks like
// std::thread tGPIOListener tgpio = <gpioinstance>->listener();
// std::thread gpio::listener()
#ifdef INTERRUPT_DRIVE_FLOW_SENSOR_TICKS
void gpio::listener_flowsensor()
{
        // cout << "Spin up GPIO Thread" << endl;
        DF_ERROR df_ret = ERROR_BAD_PARAMS;
        m_stop = false;

        // while (!m_stop)
        // {
                monitorGPIO_Flowsensor(&m_stop);
        //         usleep(1000);
        // }

        m_stop = true; // reset
                       //  return;
}
#endif
// void gpio::listener_buttons_powerAndMaintenance()
// {
//         // cout << "Spin up GPIO Thread" << endl;
//         DF_ERROR df_ret = ERROR_BAD_PARAMS;
//         m_stop = false;

//         while (!m_stop)
//         {
//                 monitorGPIO_Buttons_powerAndMaintenance();
//                 usleep(100000);
//         }

//         m_stop = true; // reset
//                        //  return;
// }
