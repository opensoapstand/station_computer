#include <iostream>
#include "local_db_logger.h"
using namespace std;

//ideally this should be input for every machine for very first time
static const int number_of_Drinks = 9;
const int machine = 1;

int main()
{
    local_db_logger db;
    db.create_machine_inDB(machine);
    db.insert_drink_inventory(machine, 1, 2.89);
    db.insert_temperature(machine, 2.12, 23.21);
    db.insert_waste_level(machine, 0, 0);
    db.update_machine_option(machine, 3, 4);
    db.update_machine_option(machine, 2, 4);
    db.update_machine_option(machine, 1, 1);



    return 0;
}
