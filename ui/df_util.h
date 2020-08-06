#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <includefiles.h>

using namespace std;

struct DrinkSelection{
    const char* brandName;
    const char* brandImageRef;
    const char* drinkType;
    const char* drinkFlavor;
    const char* drinkImageRef;
    int optionNumber;
    const char* drinkSubtitle;
    const char* drinktDescription;
    double drinkML;
    double drinkPrice;
};

class df_util
{
public:
    df_util();
    void send_to_FSM(QString * msg);

};

#endif // DF_UTIL_H
