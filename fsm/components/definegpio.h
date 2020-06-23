//***************************************
//
// pinout.h
// define all pin related to i2c/gpio readings
//
// created: 10-06-2020
// by: <author>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

//first two digit is address and later 2 digit is pin number
//2000 -> x20 (i2c address) and 00 pin 0 for i2c chip

#ifndef _DEFINEGPIO_H
#define _DEFINEGPIO_H

#define FSM_VERSION "0.1.1"
#define button 2000

#define drink1 2001
#define water1 2002
#define air1   2003
#define flow1  337

#define drink2 2004
#define water2 2005
#define air2   2006
#define flow2  364

#define drink3 2007  
#define water3 2009  //skipped x20_0b (2008) because of led config changed
#define air3   2010
#define flow3  412

#define drink4 2011
#define water4 2012
#define air4   2013
#define flow4  413

#define drink5 2014
#define water5 2015
#define air5   2100
#define flow5  415

#define drink6 2101
#define water6 2102
#define air6   2103
#define flow6  416
#define pump6_F 2200
#define pump6_R 2201

#define drink7 2104
#define water7 2105
#define air7   2106
#define flow7  417
#define pump7_F 2202
#define pump7_R 2203

#define drink8 2107
#define water8 2108
#define air8   2109
#define flow8  339
#define pump8_F 2204
#define pump8_R 2205

#define drink9 2110
#define water9 2111
#define air9   2112
#define flow9  419
#define pump9_F 2206
#define pump9_R 2207

//future digital control pin
#define FC1 2208
#define FC2 2209
#define FC3 2210
#define FC4 2211
#define FC5 2212
#define FC6 2213
#define FC7 2214


/*
int drinkP[] = {drink1, drink2, drink3, drink4, drink5, drink6, drink7, drink8, drink9};

int waterP[] = {water1, water2, water3, water4, water5, water6, water7, water8, water9};

int airP[] = {air1, air2, air3, air4, air5, air6, air7, air8, air9};

int flowP[] = {flow1, flow2, flow3, flow4, flow5, flow6, flow7, flow8, flow9};

int pumpFwdP[] = {pump6_F, pump7_F, pump8_F, pump9_F};

int pumpRevP[] = {pump6_R, pump7_R, pump8_R, pump9_R};
*/

#endif //_GPIOS_H