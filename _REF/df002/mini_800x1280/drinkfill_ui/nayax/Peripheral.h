/*                  - Nayax Confidential and Proprietary -
*
*  Copyright (C) June 2000, Nayax.  ALL RIGHTS RESERVED.
*
*  Except as specifically permitted herein, no portion of the information,
*  including but not limited to object code and source code, may be reproduced,
*  modified, distributed, republished or otherwise exploited in any form or by
*  any means for any purpose without the prior written permission of Nayax
*  Israel. Use of software subject to the terms and conditions
*  detailed in the file "LICENSE.txt".
*
*  Description  :
*
*/

#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include "stdafx.h"
#include <string.h>
#include "Status.h"
#include "Hal.h"
#include "MarshallApi.h"
#include "Marshall.h"
#include "Utils.h"

/** Window GUI messages */
typedef enum E_Peipheral_Msg_tag
{
    PERIPHERAL_CHOOSE_PRODUCT_MSG   = 4000,
    PERIPHERAL_TAKE_PRODUCT_MSG,
    PERIPHERAL_CLOSE_SESSION_MSG,
    PERIPHERAL_CASH_SALE_MSG,
    PERIPHERAL_CANCEL_MSG,
    PERIPHERAL_TEST_MODEM_MSG,

    PERIPHERAL_MAX_MSG,
}E_Peipheral_Msg;


/**
* @name 	E_Marshall_AppMsgType
* @brief	The session type
*/
typedef enum E_Peripheral_SessionType_tag
{
    PERIPHERAL_SESSION_TYPE_VENDING = 0,
    PERIPHERAL_SESSION_TYPE_INFO,

    PERIPHERAL_SESSION_TYPE_MAX,
}E_Peripheral_SessionType;


                /****************************************************/
                /*		F U N C T I O N S   P R O T O T Y P E S		*/
                /****************************************************/

/** A call back function for the MarshallHal_TracePrint() */
typedef void(*PeripheralHalTracePrintCB)(char * pStr, uint32_t Src);

/**
* @name 	Peripheral_Init
* @brief 	Init the ComId, Peripheral type and subtype and create the periheral thread
* @param [in]	PrintCB             A call back function for the MarshallHal_TracePrint()
*                                   After the MarshallHal_TracePrint() assemble the message it uses the call back to pass the message forword
* @param [in]	ComId               The serial COM ID
* @param [in]	PeripheralType      The peripheral type (E_MarshallApi_FirmInfoType)
* @param [in]	PeripheralSubType   The peripheral sub type (E_MarshallApi_InfoNayaxRdrSubType, E_MarshallApi_InfoPhotoBoothSubType...)
* @param [in]	PerCapabilities     The peripheral Capabilities
*/
pthread_t Peripheral_Init(uint8_t ComId, uint8_t PeripheralType, uint8_t PeripheralSubType, uint16_t PerCapabilities);

/**
* @name 	Peripheral_Close
* @brief 	Close the peripherals and marhsall handels
*/
void Peripheral_Close(void);

/**
* @name 	Peripheral_UpdateSessionType
* @brief 	Update the session type
* @param [in]	SessionType
*/
void Peripheral_UpdateSessionType(E_Peripheral_SessionType Type);

/**
* @name 	Peripheral_UpdatePortNubmer
* @brief 	Update the port number
* @param [in]	port numer
*/
void Peripheral_UpdatePortNubmer(uint16_t Port);

/**
* @name 	Peripheral_UpdateIpAddr
* @brief 	Update the IP address
* @param [in]	IP address
*/
void Peripheral_UpdateIpAddr(char *Str);

#endif      //__PERIPHERAL_H__
