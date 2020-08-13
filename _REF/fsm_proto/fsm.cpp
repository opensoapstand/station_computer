//***************************************
//
// <fsm.cpp>
// <Contians functions that activates finiate state machine>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "fsm.h"

void fsm::startFSM(){

  dispense dispense;
  gpio gpio;

  gpio.activateGPIO(249); //Activate Level Sensor Pin
  gpio.setGPIO_input(249);

  int SELECT;
  int SENSOR1 = 0, DRINK1  = 0, WATER1 = 0, AIR1 = 0;
  int SENSOR2 = 1, DRINK2  = 1, WATER2 = 1, AIR2 = 1;
  int SENSOR3 = 2, DRINK3  = 2, WATER3 = 2, AIR3 = 2;
  int SENSOR4 = 3, DRINK4  = 3, WATER4 = 3, AIR4 = 3;
  int SENSOR5 = 4, DRINK5  = 4, WATER5 = 4, AIR5 = 4;
  int SENSOR6 = 5, DRINK6  = 5, WATER6 = 5, AIR6 = 5;
  int SENSOR7 = 6, DRINK7  = 6, WATER7 = 6, AIR7 = 6;
  int SENSOR8 = 7, DRINK8  = 7, WATER8 = 7, AIR8 = 7;
  int SENSOR9 = 8, DRINK9  = 8, WATER9 = 8, AIR9 = 8;

  std::cout << "\n\n=====================================" << endl;
  std::cout << "Case 1:   Drink 1, low volume" << endl;
  std::cout << "Case 2:   Drink 1, high volume" << endl;
  std::cout << "Case 3:   Drink 2, low volume" << endl;
  std::cout << "Case 4:   Drink 2, high volume" << endl;
  std::cout << "Case 5:   Drink 3, low volume" << endl;
  std::cout << "Case 6:   Drink 3, high volume" << endl;
  std::cout << "Case 7:   Drink 4, low volume" << endl;
  std::cout << "Case 8:   Drink 4, high volume" << endl;
  std::cout << "Case 9:   Drink 5, low volume" << endl;
  std::cout << "Case 10:  Drink 5, high volume" << endl;
  std::cout << "Case 11:  Drink 6, low volume" << endl;
  std::cout << "Case 12:  Drink 6, high volume" << endl;
  std::cout << "Case 13:  Drink 7, low volume" << endl;
  std::cout << "Case 14:  Drink 7, high volume" << endl;
  std::cout << "Case 15:  Drink 8, low volume" << endl;
  std::cout << "Case 16:  Drink 8, high volume" << endl;
  std::cout << "Case 17:  Drink 9, low volume" << endl;
  std::cout << "Case 18:  Drink 9, high volume" << endl;
  std::cout << "=====================================\n\n" << endl;

  while(true){
    
    std::cout << "Please input case (0 - 8):";
    std::cin >> SELECT;
    std::cout << "\nSystem getting ready for dispense sequence...\n\n";

      switch(SELECT){

        case 1: dispense.dispenseLOW(SENSOR1, DRINK1, WATER1, AIR1);
                break;

        case 2: dispense.dispenseHIGH(SENSOR1, DRINK1, WATER1, AIR1);
                break;

        case 3: dispense.dispenseLOW(SENSOR2, DRINK2, WATER2, AIR2);
                break;

        case 4: dispense.dispenseHIGH(SENSOR2, DRINK2, WATER2, AIR2);
                break;

        case 5: dispense.dispenseLOW(SENSOR3, DRINK3, WATER3, AIR3);
                break;

        case 6: dispense.dispenseHIGH(SENSOR3, DRINK3, WATER3, AIR3);
                break;

        case 7: dispense.dispenseLOW(SENSOR4, DRINK4, WATER4, AIR4);
                break;

        case 8: dispense.dispenseHIGH(SENSOR4, DRINK4, WATER4, AIR4);
                break;

        case 9: dispense.dispenseLOW(SENSOR5, DRINK5, WATER5, AIR5);
                break;

        case 10: dispense.dispenseHIGH(SENSOR5, DRINK5, WATER5, AIR5);
                break;

        case 11: dispense.dispenseLOW(SENSOR6, DRINK6, WATER6, AIR6);
                break;

        case 12: dispense.dispenseHIGH(SENSOR6, DRINK6, WATER6, AIR6);
                break;

        case 13: dispense.dispenseLOW(SENSOR7, DRINK7, WATER7, AIR7);
                break;

        case 14: dispense.dispenseHIGH(SENSOR7, DRINK7, WATER7, AIR7);
                break;

        case 15: dispense.dispenseLOW(SENSOR8, DRINK8, WATER8, AIR8);
                break;

        case 16: dispense.dispenseHIGH(SENSOR8, DRINK8, WATER8, AIR8);
                break;

        case 17: dispense.dispenseLOW(SENSOR9, DRINK9, WATER9, AIR9);
                break;

        case 18: dispense.dispenseHIGH(SENSOR9, DRINK9, WATER9, AIR9);
                break;

        default:  std::cout << "\nInvalid input. Please try again:";
                            break;
                          }
   exit(0);
  }
}
