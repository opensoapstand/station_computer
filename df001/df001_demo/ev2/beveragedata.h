//***************************************
//
// countdownWindow
// User pause window
//
// created: 29-AUG-2019
// by: Jason Wang
//
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef BEVERAGEDATA_H
#define BEVERAGEDATA_H

#define NUM_BEV 2;

class beverageData
{
public:
    beverageData();
    ~beverageData();
    beverageData(int op, int initialVolume);

    int getBeverageOption();
    int getInventoryVolume();
    void setDispenseVolume(int volume);
    void updateInventoryVolume();

    void updateBeverage(beverageData* bev);

private:
    int bevDispense;
    int dispenseVolume;
    int currentInventory;
};

#endif // BEVERAGEDATA_H
