#include <iostream>
#include "local_db_logger.h"
using namespace std;

int main()
{
    local_db_logger db;
    db.update_drink_inventory_local(1,2.89);

    return 0;
}
