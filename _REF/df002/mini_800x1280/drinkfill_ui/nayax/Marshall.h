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

#ifndef __MARSHALL_H__
#define __MARSHALL_H__

#include<stdint.h>
#include"Status.h"
#include"MarshallApi.h"
#include"Hal.h"

		    /************************************************/
		    /*				  D E F I N E S					*/
		    /************************************************/
/**
* @brief	Application message - event data size
*/
#define MARSHALL_APP_MSG_EVENT_SIZE		    1

/**
* @brief	Application message - the header size
*/
#define MARSHALL_APP_MSG_HEADER_SIZE		3

		    /************************************************/
		    /*				    E N U M S					*/
		    /************************************************/
/**
* @name 	E_Marshall_AppMsgType
* @brief	The type of messages that the system send to the application
*/
typedef enum E_Marshall_AppMsgType_tag
{
	MARSHALL_INTERNAL_EVENT = 1,
	MARSHALL_EXTERNAL_MSG,

}E_Marshall_AppMsgType;

/**
* @name 	E_Marshall_AppMsgType
* @brief	The type of messages that the system send to the application
*/
typedef enum E_Marshall_AppMsgEvents_tag
{
	MARSHALL_NON_CONSECUTIVE_PACKET_ID_EVENT = 1,
	MARSHALL_KEEP_ALIVE_TICK,
	MARSHALL_RCV_NACK,

	MARSHALL_EVENT_MAX,

}E_Marshall_AppMsgEvents;

		    /************************************************/
		    /*				   S T R U C T S 			    */
		    /************************************************/

/**
* @name 	S_Marshall_AppMsg
* @brief	The message structure that the system send to the application
*/
typedef struct S_Marshall_AppMsg_tag
{
	uint8_t			MsgType;			/** Message type - E_Marshall_AppMsgType	*/
	uint8_t			Event;				/**	In case of MsgType is event				*/
	uint16_t		DataSize;			/** The message data size					*/
	uint8_t		   *pData;				/** The message data						*/

}S_Marshall_AppMsg;


/**
* @name 	S_Marshall_PeripheralInfo_tag
* @brief	The peripheral info structure
*/
# pragma pack (1)
typedef struct S_Marshall_PeripheralInfo_tag
{
	uint8_t			Type;						/** Peripheral type			 			*/
	uint8_t			SubType;					/** Peripheral sub type		 			*/
	uint16_t	    Capabilities;			/** Peripheral Capabilities	 			*/
	uint8_t			Model[20];					/** Peripheral Model	 				*/
	uint8_t			SerialNum[20];				/** Peripheral SerialNum 				*/
	uint8_t			SwVer[20];					/** Peripheral Application SW Version	*/

}S_Marshall_PeripheralInfo;
# pragma pack ()

typedef void(*HalTracePrintCB)(char * pStr, uint32_t Src);
		    /****************************************************/
		    /*		F U N C T I O N S   P R O T O T Y P E S		*/
		    /****************************************************/

/**
* @name 	Marshall_Init
* @brief	Initialize the module
*/
void Marshall_Init(HalTracePrintCB PrintCB);

/**
* @name 	Marshall_Close
* @brief 	Close the module handles
*/
void Marshall_Close(void);

/**
* @name 	Marshall_Registration
* @brief 	Register to a Marshall channel
*
* @param [in]	pPeripheralInfo		    Peripheral info
* @param [in]	AppThreadID		        The application thread ID (the thread that will receive messages from the Marshall thread)
* @param [in]	ComId				    The com ID in this type
*
* @retval 		The function status
*/
E_Status_Ret Marshall_Registration(S_Marshall_PeripheralInfo	*pPeripheralInfo,
                                    uint32_t                    AppThreadID,
                                    uint8_t 				    ComId);

/**
* @name 	Marshall_SendPacket
* @brief 	Send a packet
*
* @param [in]	pData		The packet data
* @param [in]	DataSize	The packet data size
* @param [in]	FuncCode	The packet function code
* @param [in]	IsBlocking	If it need to block the sender until the Tx is done (ACK,Time-out...)
*
* @retval 					The function status
*/
E_Status_Ret Marshall_SendPacket(uint8_t 			   		*pData,
								 uint16_t 					DataSize,
								 E_MarshallApi_FuncCode 	FuncCode,
								 uint8_t					IsBlocking);
/**
* @name 	Marshall_GetAppMsg
* @brief 	Get the application message
*
* @param [out]	pAppMsg		Application message
* @param [in]   WaitTime	wait time in milli second for receiving a message,
*								INFINITE - for endless wait
*
* @retval 					The function status
*/
E_Status_Ret Marshall_GetAppMsg(S_Marshall_AppMsg	**pAppMsg, uint32_t WaitTime);

/**
* @name 	Marshall_StartPairingProcess
* @brief 	Start
*
* @retval 	The function
*/
E_Status_Ret Marshall_StartPairingProcess(void);

/**
* @name 	Hal_CreateThread
* @brief 	Create thread
*
* @param [in]	Thread			A pointer to the application-defined function to be executed by the thread
* @param [in]	pParameter		A pointer to a variable to be passed to the thread.
* @param [in]	pThreadID		A pointer to a variable that receives the thread identifier.
*
* @retval	If the function succeeds, the return value is a handle to the newly created tread object.
*			If the function fails, the return value is NULL.
*/
int Hal_CreateThread(pthread_t pThreadID);

/**
* @name 	Hal_CloseThread
* @brief 	Close the Thread
*
* @param [in]	pThreadHandle	 Handle to the Thread object
*/
void Hal_CloseThread(int pThreadHandle);

/**
* @name 	Hal_SuspendThread
* @brief 	Suspends the execution of the current thread
*
* @param [in]	Milliseconds	 The time interval for which execution is to be suspended, in milliseconds.
*/
void Hal_SuspendThread(uint32_t Milliseconds);

/**
* @name 	Hal_CreateThreadQueue
* @brief 	Create thread queue
*
* @param [in]	ThreadID		The thread identifier.
* @param [in]	QueueLength		The queue length .
* @param [in]	ItemSize		The queue item size.
*
* @retval	If the function succeeds, it return TRUE.
*/
int Hal_CreateThreadQueue(uint32_t ThreadID, uint32_t QueueLength, uint32_t ItemSize);

/**
* @name 	Hal_DestroyThreadQueue
* @brief 	Destroy thread queue
*
* @param [in]	ThreadID		The thread identifier.
*
*/
void Hal_DestroyThreadQueue(uint32_t ThreadID);

/**
* @name 	Hal_CreateThreadQueue
* @brief 	Create thread queue
*
* @param [in]	ThreadID		The thread identifier.
* @param [in]	Param		    Additional message-specific information.
* @param [in]	WaitMs		    Wait time in mSec.
* @param [in]	MsgType         The message type
*
* @retval	If the function succeeds, it return TRUE.
*/
int Hal_SendMsgToThreadQueue(uint32_t ThreadID, void *pParam, uint32_t WaitMs, uint32_t MsgType);

/**
* @name 	Hal_CreateThreadQueue
* @brief 	Create thread queue
*
* @param [in]	ThreadID		The thread identifier.
* @param [in]	pParam		    Additional message-specific information.
* @param [in]	WaitMs		    Wait time in mSec.
* @param [in]	MsgType         The message type
*
* @retval	If the function succeeds, it return TRUE.
*/
int Hal_GetThreadQueueMsg(uint32_t ThreadID, void **pParam, uint32_t WaitMs, uint32_t MsgType);

/**
* @name 	Hal_GetCurrentThreadID
* @brief 	Get the current thread ID
*
* @retval	current thread ID.
*/
uint32_t Hal_GetCurrentThreadID(void);



#endif  //__MARSHALL_H__

