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
*  Description  : This is the Marshall hardware abstraction layer (for windows PC)
*
*/

#ifndef __HAL_H__
#define __HAL_H__

#ifdef WIN32
	#include <Windows.h>
	#include <time.h>
#endif


#include "Status.h"


				/************************************************/
				/*				 T Y P E S						*/
				/************************************************/

//typedef signed char			int8_t;
//typedef short				int16_t;
//typedef int					int32_t;
//typedef long long			int64_t;
//typedef unsigned char		uint8_t;
//typedef unsigned short      uint16_t;
//typedef unsigned int        uint32_t;
//typedef unsigned long long	uint64_t;

//typedef int					int;

#ifndef FALSE
#define FALSE				0
#endif

#ifndef TRUE
#define TRUE				1
#endif

#include<stdint.h>
#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <Marshall.h>
#include <ctime>
#include <cstring>
using namespace std;

				/************************************************/
				/*					C O M	 					*/
				/************************************************/

typedef unsigned long DWORD;
typedef DWORD* LPDWORD;

#define HAL_PURGE_COM_TXCLEAR		TCOFLUSH
#define HAL_PURGE_COM_RXCLEAR		TCIFLUSH


/**
* @name 	Hal_CloseCom
* @brief 	Close the COM handle
*
* @param [in]	SerialHandle	Serial COM handle
*/
void Hal_CloseCom();

/**
* @name 	Hal_OpenCom
* @brief 	Open the COM
*
* @param [in]	pSerialHandle	Serial COM handle
* @param [in]	ComId			The com ID
*
* @retval	The function status
*/
E_Status_Ret Hal_OpenCom();

/**
* @name 	Hal_ReadCom
* @brief 	Read a buffer from the  COM
*
* @param [in]	SerialHandle	Serial COM handle
* @param [in]	pBuff			The buffer to read the data to
* @param [in]	BytesToRead		The bytes to read
* @param [out]	pBytesReceived	Bytes received
*
* @retval	The function status
*/
E_Status_Ret Hal_ReadCom(uint8_t * pBuff, uint32_t BytesToRead, uint32_t *pBytesReceived);

/**
* @name 	Hal_WriteCom
* @brief 	Send a buffer to a COM
*
* @param [in]	SerialHandle	Serial COM handle
* @param [in]	pBuff			The buffer
* @param [in]	BytesToWrite	The bytes to write
* @param [out]	pBytesWritten	Bytes written
*
* @retval	The function status
*/
E_Status_Ret Hal_WriteCom(uint8_t * pBuff, uint32_t BytesToWrite, uint32_t *pBytesWritten);

/**
* @name 	Hal_PurgeCom
* @brief 	Clear the COM Rx buffer
*
* @param [in]	SerialHandle	Serial COM handle
* @param [in]	CmdFlags		Command (bits) - HAL_PURGE_COM_TXCLEAR,HAL_PURGE_COM_RXCLEAR
*/
void Hal_PurgeCom(uint32_t Flags);



				/************************************************/
				/*						O S 	 				*/
				/************************************************/

/**
* @name 	HAL_INFINITE
* @brief 	Wait infinite
*/
#ifdef WIN32
	#define HAL_INFINITE		INFINITE 
#else
	#define HAL_INFINITE		0XFFFFFFFF
#endif

/**
* @name 	HAL_THREAD_PROTOTYPE
* @brief 	Thread function prototype
*/
#ifdef WIN32
	#define HAL_THREAD_PROTOTYPE	WINAPI
#else
	#define HAL_THREAD_PROTOTYPE			
#endif

/**
* @name 	E_Hal_WaitRetVal
* @brief 	Wait mutex/semaphore return value
*/
typedef enum E_Hal_WaitRetVal_tag
{
	HAL_WAIT_OBJECT_SIGNALED,
	HAL_WAIT_TIMEOUT,
	HAL_WAIT_FAILED,
}E_Hal_WaitRetVal;

///**
//* @name 	HAL_THREAD
//* @brief 	Thread prototype
//*/
typedef uint32_t(HAL_THREAD_PROTOTYPE HAL_THREAD) (void *pParameters);


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
void * Hal_CreateThread(HAL_THREAD Thread, void *pParameter, uint32_t * pThreadID);

///**
//* @name 	Hal_CloseThread
//* @brief 	Close the Thread
//*
//* @param [in]	pThreadHandle	 Handle to the Thread object
//*/
//void Hal_CloseThread(void *pThreadHandle);

///**
//* @name 	Hal_SuspendThread
//* @brief 	Suspends the execution of the current thread
//*
//* @param [in]	Milliseconds	 The time interval for which execution is to be suspended, in milliseconds.
//*/
//void Hal_SuspendThread(uint32_t Milliseconds);

///**
//* @name 	Hal_CreateThreadQueue
//* @brief 	Create thread queue
//*
//* @param [in]	ThreadID		The thread identifier.
//* @param [in]	QueueLength		The queue length .
//* @param [in]	ItemSize		The queue item size.
//*
//* @retval	If the function succeeds, it return TRUE.
//*/
//int Hal_CreateThreadQueue(uint32_t ThreadID, uint32_t QueueLength, uint32_t ItemSize);

///**
//* @name 	Hal_DestroyThreadQueue
//* @brief 	Destroy thread queue
//*
//* @param [in]	ThreadID		The thread identifier.
//*
//*/
//void Hal_DestroyThreadQueue(uint32_t ThreadID);

///**
//* @name 	Hal_CreateThreadQueue
//* @brief 	Create thread queue
//*
//* @param [in]	ThreadID		The thread identifier.
//* @param [in]	Param		    Additional message-specific information.
//* @param [in]	WaitMs		    Wait time in mSec.
//* @param [in]	MsgType         The message type
//*
//* @retval	If the function succeeds, it return TRUE.
//*/
//int Hal_SendMsgToThreadQueue(uint32_t ThreadID, void *pParam, uint32_t WaitMs, uint32_t MsgType);

///**
//* @name 	Hal_CreateThreadQueue
//* @brief 	Create thread queue
//*
//* @param [in]	ThreadID		The thread identifier.
//* @param [in]	pParam		    Additional message-specific information.
//* @param [in]	WaitMs		    Wait time in mSec.
//* @param [in]	MsgType         The message type
//*
//* @retval	If the function succeeds, it return TRUE.
//*/
//int Hal_GetThreadQueueMsg(uint32_t ThreadID, void **pParam, uint32_t WaitMs, uint32_t MsgType);

///**
//* @name 	Hal_GetCurrentThreadID
//* @brief 	Get the current thread ID
//*
//* @retval	current thread ID.
//*/
//uint32_t Hal_GetCurrentThreadID(void);

                    /************************************************/
                    /*					C L O C K	   				*/
                    /************************************************/

/**
* @name 	Hal_Clock
* @brief	Clock return value
*/
#ifdef WIN32
    typedef clock_t Hal_Clock;
#else
    typedef long Hal_Clock;
#endif

/**
* @name 	MarshallHal_Clock
* @brief	wall-clock time.(it is used for relative time calculation)
*
* @retval	The elapsed wall-clock time. (in windows it is from the start of the process)
*/
Hal_Clock MarshallHal_Clock(void);

/**
* @name 	MarshalMac_GetDiffTimeInMillSec
* @brief	Get diff time in milli sec
*
* @retval	If the function succeeds, the return value is a handle to the newly created tread object.
*/
uint32_t MarshallHal_GetDiffTimeInMillSec(Hal_Clock t1, Hal_Clock t2);

                    /************************************************/
                    /*				 M A L L O C					*/
                    /************************************************/

/**
* @name 	MarshallHal_Malloc
* @brief	Get diff time in milli sec
*
* @param [in]	size	Bytes to allocate
*
* @retval	returns a void pointer to the allocated space, or NULL if there is insufficient memory available.
*/
void * MarshallHal_Malloc(uint32_t size);


					/************************************************/
					/*					T R A C E 	   				*/
					/************************************************/

/**
* @name 	E_Hal_TraceSrc
* @brief	Trace source
*/

#define HAL_TRACE_SRC_COM_PACKETS   (1<<0)
#define HAL_TRACE_SRC_ERROR         (1<<1)
#define HAL_TRACE_SRC_APPLICATION   (1<<2)
#define HAL_TRACE_SRC_GENERAL       (1<<3)


typedef void(*HalTracePrintCB)(char * pStr, uint32_t Src);

/**
* @name 	MarshallHal_TraceInit
* @brief	Init the trace mutex
*/
void MarshallHal_TraceInit(HalTracePrintCB PrintCB);

/**
* @name 	MarshallHal_TracePrint
* @brief	Print a trace message
*
* @param [in]	Size		The data size
* @param [in]	pData		The data
* @param [in]	Src		    The trace source
* @param [in]	Fmt			Formated data
*/
void MarshallHal_TracePrint(uint32_t Size, uint8_t * pData, uint32_t Src, const char * Fmt, ...);

#endif	//__HAL_H__
