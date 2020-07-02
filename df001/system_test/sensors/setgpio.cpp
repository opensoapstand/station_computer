//***************************************
//
// <gpio.cpp>
// <Contians functions that activates and manipulates UDOO braswell GPIOs>


//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************


#include "setgpio.h"

void setgpio::activate_GPIO(int gpio){

  std::string GPIO = std::to_string(gpio);

  string chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/export'");
  system(chmod_command.c_str());

  string export_command("sudo sh -c 'echo ");
  export_command += GPIO;
  export_command += "  > /sys/class/gpio/export'";
  system(export_command.c_str());
}

void setgpio::deactivate_GPIO(int gpio){

  std::string GPIO = std::to_string(gpio);

  string chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/export'");
  system(chmod_command.c_str());

  string export_command("sudo sh -c 'echo ");
  export_command += GPIO;
  export_command += "  > /sys/class/gpio/unexport'";
  system(export_command.c_str());
}

void setgpio::setGPIO_input(int gpio){

  std::string GPIO = std::to_string(gpio);

  //First, modify directory
  string dir_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  dir_chmod_command += GPIO;
  dir_chmod_command += "/direction'";
  system(dir_chmod_command.c_str());

  //Second, edit the directory
  string dir_echo_command("sudo sh -c 'echo in >/sys/class/gpio/gpio");
  dir_echo_command += GPIO;
  dir_echo_command += "/direction'";
  system(dir_echo_command.c_str());

  //Third, verify directory status
  string dir_cat_command("sudo sh -c 'cat /sys/class/gpio/gpio");
  dir_cat_command += GPIO;
  dir_cat_command += "/direction'";
  system(dir_cat_command.c_str());

}

void setgpio::setGPIO_output(int gpio){

  std::string GPIO = std::to_string(gpio);

  //First, modify directory
  string dir_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  dir_chmod_command += GPIO;
  dir_chmod_command += "/direction'";
  system(dir_chmod_command.c_str());

  //Second, edit the directory
  string dir_echo_command("sudo sh -c 'echo out >/sys/class/gpio/gpio");
  dir_echo_command += GPIO;
  dir_echo_command += "/direction'";
  system(dir_echo_command.c_str());

  //Third, verify directory status
  string dir_cat_command("sudo sh -c 'cat /sys/class/gpio/gpio");
  dir_cat_command += GPIO;
  dir_cat_command += "/direction'";
  system(dir_cat_command.c_str());
}

void setgpio::setGPIO_high(int gpio){

  std::string GPIO = std::to_string(gpio);

  string val_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  val_chmod_command += GPIO;
  val_chmod_command += "/value'";
  system(val_chmod_command.c_str());

  string val_echo_command("sudo sh -c 'echo 1 >/sys/class/gpio/gpio");
  val_echo_command += GPIO;
  val_echo_command += "/value'";
  system(val_echo_command.c_str());
}

void setgpio::setGPIO_low(int gpio){

  std::string GPIO = std::to_string(gpio);

  string val_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  val_chmod_command += GPIO;
  val_chmod_command += "/value'";
  system(val_chmod_command.c_str());

  string val_echo_command("sudo sh -c 'echo 0 >/sys/class/gpio/gpio");
  val_echo_command += GPIO;
  val_echo_command += "/value'";
  system(val_echo_command.c_str());
}

void setgpio::setGPIO_edge_both(int gpio){

  std::string GPIO = std::to_string(gpio);

  string edg_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  edg_chmod_command += GPIO;
  edg_chmod_command += "/edge'";
  system(edg_chmod_command.c_str());

  string edg_echo_command1("sudo sh -c 'echo both >/sys/class/gpio/gpio");
  edg_echo_command1 += GPIO;
  edg_echo_command1 += "/edge'";
  system(edg_echo_command1.c_str());

}

void setgpio::setGPIO_edge_rising(int gpio){

  std::string GPIO = std::to_string(gpio);

  string edg_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  edg_chmod_command += GPIO;
  edg_chmod_command += "/edge'";
  system(edg_chmod_command.c_str());

  string edg_echo_command1("sudo sh -c 'echo rising >/sys/class/gpio/gpio");
  edg_echo_command1 += GPIO;
  edg_echo_command1 += "/edge'";
  system(edg_echo_command1.c_str());

}

void setgpio::setGPIO_edge_falling(int gpio){

  std::string GPIO = std::to_string(gpio);

  string edg_chmod_command("sudo sh -c 'chmod -R 777 /sys/class/gpio/gpio");
  edg_chmod_command += GPIO;
  edg_chmod_command += "/edge'";
  system(edg_chmod_command.c_str());

  string edg_echo_command1("sudo sh -c 'echo falling >/sys/class/gpio/gpio");
  edg_echo_command1 += GPIO;
  edg_echo_command1 += "/edge'";
  system(edg_echo_command1.c_str());
}
