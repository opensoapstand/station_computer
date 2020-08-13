#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

class levelSensor{

  public:

    void readValue(void);
    string command_to_string(string cmd);

};
