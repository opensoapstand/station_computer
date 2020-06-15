//***************************************
//
// <levelsensor.cpp>
// <Contains functions that monitor the level sensors...
// <... and shows should sent a warning when the sensor is triggered>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "levelsensor.h"

void levelSensor::readValue(){

  string command("sudo sh -c 'cat /sys/class/gpio/gpio493/value'");
  string sys_command = command_to_string(command.c_str());

  int sensorValue = stoi(sys_command);  //convert string to integer

  while(1){

    string sys_command = command_to_string(command.c_str());
    sensorValue = stoi(sys_command);
    sleep(1);

    if(sensorValue == 1){
      std::cout << "Waste tank at critical level" << '\n';
    }

    else{
      std::cout << "Waste tank isn't full yet" << '\n';
    }
  }
}

string levelSensor::command_to_string(string cmd) {

  string data;
  FILE * stream;
  const int max_buffer = 256;
  char buffer[max_buffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");
  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
  return data;
}
