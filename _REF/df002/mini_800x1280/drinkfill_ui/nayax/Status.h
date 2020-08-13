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
 *  Description  : This header contain a general status definition
 *
 */


#ifndef __STATUS_H__
#define __STATUS_H__


		/************************************************/
		/*				    E N U M S					*/
		/************************************************/

/**
 *  @brief 	functions return value
 *
 */
typedef enum
{
	STATUS_RET_SUCCESS,
	STATUS_RET_FAIL,
	STATUS_RET_UNEXPECTED_ERROR,
	STATUS_RET_NOT_SUPPORTED_PARAM_IN,
	STATUS_RET_NO_AVAILABLE_RESOURCE,
	STATUS_RET_ERROR_PARAM_IN,
	STATUS_RET_TIMEOUT,
	STATUS_RET_MODULE_ALREADY_OPEN,
	STATUS_RET_MODULE_IS_CLOSE,
	STATUS_RET_BUFF_ALLOCATION_ERROR,
	STATUS_RET_BUFF_EMPTY,
	STATUS_RET_BUFF_FULL,
	STATUS_RET_RCV_PACKET_OK,
	STATUS_RET_NO_PACKET_AVAILABLE,
	STATUS_RET_PACKET_CRC_ERROR,
	STATUS_RET_COM_ERROR,
	STATUS_RET_COM_CHANNEL_IS_OPEN,
	STATUS_RET_COM_CHANNEL_IS_CLOSE,
	STATUS_RET_COM_CHANNEL_UNABLE_TO_OPEN_COM,
	STATUS_RET_COM_CHANNEL_UNABLE_TO_CONFIG_COM,
	STATUS_RET_COM_CHANNEL_NOT_CONNECTED,
	STATUS_RET_FAIL_BECAUSE_OF_RESET,

}E_Status_Ret;

#endif  //__STATUS_H__
