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

/************************************************/
/*				  I N C L U D E S				*/
/************************************************/
#include "stdafx.h"

#include "Hal.h"
#include "Status.h"




/************************************************/
/*				  D E F I N E S					*/
/************************************************/

#define HAL_SERIAL_PORT_BR		B115200

#define  ComName "/dev/ttyUSB0"
int SerialHandle;
char readBuffer[1024];
/************************************************/
/*					C O M	 					*/
/************************************************/

/**
* @name 	Hal_CloseCom
* @brief 	Close the COM handle
*
* @param [in]	SerialHandle	Serial COM handle
*/
void Hal_CloseCom()
{
    if (SerialHandle)
    {
        close(SerialHandle);
        SerialHandle = -1;
    }
}

/**
* @name 	Hal_OpenCom
* @brief 	Open the COM
*
* @param [in]	pSerialHandle	Serial COM handle
* @param [in]	ComId			The com ID
*
* @retval	The function status
*/
E_Status_Ret Hal_OpenCom()
{
    SerialHandle == -1;
    //COMMTIMEOUTS		    ComTimeouts;
    struct termios          SerialParams;


    /** Create the port name */

    /** Create the serial port handle */
    SerialHandle = open(ComName ,O_RDWR | O_NOCTTY | O_NDELAY);//CreateFileA((char *)ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (SerialHandle == -1)
    {
        //		if (GetLastError() == ERROR_FILE_NOT_FOUND)
        //		    return STATUS_RET_COM_CHANNEL_UNABLE_TO_OPEN_COM;

        return STATUS_RET_UNEXPECTED_ERROR;
    }

    /** Clear the COM buffers */
    tcflush(SerialHandle, HAL_PURGE_COM_TXCLEAR);

    /** Setup the serial port */
    if (tcgetattr(SerialHandle, &SerialParams)!=0)
    {
        Hal_CloseCom();
        return STATUS_RET_UNEXPECTED_ERROR;
    }
    //	if (!SetupComm(SerialHandle, 1024, 1024))
    //	{
    //		Hal_CloseCom(SerialHandle);
    //		return STATUS_RET_UNEXPECTED_ERROR;
    //	}
    //SerialParams.dcb.DCBlength = sizeof(SerialParams.dcb);

    cfsetispeed(&SerialParams,B19200); /* Set Read  Speed as 19200                       */
    cfsetospeed(&SerialParams,B19200);
    SerialParams.c_cflag |=  CS8;
    SerialParams.c_cflag &= ~CSTOPB;
    SerialParams.c_cflag &= ~PARENB;

    SerialParams.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    SerialParams.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    SerialParams.c_cflag &= ~CRTSCTS;

    //SerialParams.dcb.fAbortOnError = TRUE;

    //	if (!SetCommState(SerialHandle, &SerialParams.dcb))
    //	{
    //		Hal_CloseCom(SerialHandle);
    //		return STATUS_RET_COM_CHANNEL_UNABLE_TO_CONFIG_COM;
    //	}

    if((tcsetattr(SerialHandle,TCSANOW,&SerialParams)) != 0) {
        Hal_CloseCom();
        return STATUS_RET_COM_CHANNEL_UNABLE_TO_CONFIG_COM;
    } else {
        tcflush(SerialHandle,TCIOFLUSH);
    }

//    GetCommTimeouts(SerialHandle, &ComTimeouts);

//    ComTimeouts.ReadIntervalTimeout = 1;
//    ComTimeouts.ReadTotalTimeoutMultiplier = 10;
//    ComTimeouts.ReadTotalTimeoutConstant = 1;
//    ComTimeouts.WriteTotalTimeoutConstant = 50;
//    ComTimeouts.WriteTotalTimeoutMultiplier = 10;

//    if (!SetCommTimeouts(SerialHandle, &ComTimeouts))
//    {
//        Hal_CloseCom(SerialHandle);
//        return STATUS_RET_COM_CHANNEL_UNABLE_TO_CONFIG_COM;
//    }


    return STATUS_RET_SUCCESS;
}


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
E_Status_Ret Hal_ReadCom(uint8_t * pBuff, uint32_t BytesToRead, uint32_t *pBytesReceived)
{
    int         readsize= -1;
    LPDWORD		LpErrors = NULL;
  //  LPCOMSTAT	LpStat = NULL;

    readsize = read(SerialHandle, pBuff, BytesToRead);
    *pBytesReceived = (uint32_t)readsize;
    if (readsize == -1)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "Hal_ReadCom() - ClearCommError");

//        /** Clear the hardware errors */
//        if (!ClearCommError(SerialHandle, LpErrors, LpStat))
//            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error Hal_ReadCom() - ClearCommError");

//        if (LpErrors != NULL)
//            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "Hal_ReadCom() - ClearCommError - error - %d", *LpErrors);

        return STATUS_RET_UNEXPECTED_ERROR;
    }
    return STATUS_RET_SUCCESS;
}

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
E_Status_Ret Hal_WriteCom(uint8_t * pBuff, uint32_t BytesToWrite, uint32_t *pBytesWritten)
{
    int         writesize = -1;
    LPDWORD		LpErrors = NULL;
   // LPCOMSTAT	LpStat = NULL;
    writesize = write(SerialHandle, pBuff, BytesToWrite);
    *pBytesWritten = (uint32_t)writesize;

    if (writesize == -1)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "Hal_WriteCom() - ClearCommError");

//        /** Clear the hardware errors */
//        if (!ClearCommError(SerialHandle, LpErrors, LpStat))
//            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error Hal_WriteCom() - ClearCommError");

//        if (LpErrors != NULL)
//            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "Hal_WriteCom() - ClearCommError - error - %d", *LpErrors);

//        if (!WriteFile(SerialHandle,
//                       pBuff,
//                       BytesToWrite,
//                       (LPDWORD)pBytesWritten,
//                       NULL))
//        {
//            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Hal_WriteCom() - error writing");
//            return STATUS_RET_FAIL;
//        }
    }
    return STATUS_RET_SUCCESS;
}


/**
* @name 	Hal_PurgeCom
* @brief 	Clear the COM Rx buffer
*
* @param [in]	SerialHandle	Serial COM handle
* @param [in]	CmdFlags		Command (bits) - HAL_PURGE_COM_TXCLEAR,HAL_PURGE_COM_RXCLEAR
*/
void Hal_PurgeCom( uint32_t Flags)
{
    //DWORD dFlags = 0;
    int dFlags = 0;

    if (Flags & HAL_PURGE_COM_TXCLEAR)
        dFlags |= HAL_PURGE_COM_TXCLEAR;

    if (Flags & HAL_PURGE_COM_RXCLEAR)
        dFlags |= HAL_PURGE_COM_RXCLEAR;

    if (tcflush(SerialHandle, dFlags) != 0)
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Hal_PurgeCom() - PurgeComm");
}







/************************************************/
/*					C L O C K	   				*/
/************************************************/

/**
* @name 	MarshallHal_Clock
* @brief	wall-clock time.(it is used for relative time calculation)
*
* @retval	The elapsed wall-clock time. (in windows it is from the start of the process)
*/
Hal_Clock MarshallHal_Clock(void)
{
    return((Hal_Clock)clock());
}

/**
* @name 	MarshalMac_GetDiffTimeInMillSec
* @brief	Get diff time in milli sec
*
* @retval	If the function succeeds, the return value is a handle to the newly created tread object.
*/
uint32_t MarshallHal_GetDiffTimeInMillSec(Hal_Clock t1, Hal_Clock t2)
{
    return((uint32_t)((t2 - t1) / (CLOCKS_PER_SEC / 1000)));
}

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

void * MarshallHal_Malloc(uint32_t size)
{
    /** keep the allocation to be alligned to 8 */
    return malloc(size + 8 - size % 8);
}

/************************************************/
/*					T R A C E 	   				*/
/************************************************/
#define TRACE_MAX_BUFF_SIZE			2048

HalTracePrintCB  TracePrintCB = NULL;

/**
* @name 	MarshallHal_TraceInit
* @brief	Init the trace mutex
*/
void MarshallHal_TraceInit(HalTracePrintCB PrintCB)
{
    TracePrintCB = PrintCB;
}

/**
* @name 	MarshallHal_TracePrint
* @brief	Print a trace message
*
* @param [in]	Size		The data size
* @param [in]	pData		The data
* @param [in]	Src		    The trace source
* @param [in]	Fmt			Formated data
*/
void MarshallHal_TracePrint(uint32_t Size, uint8_t * pData, uint32_t Src, const char * Fmt, ...)
{
    va_list		Args;
    uint32_t	i;
    time_t	Aclock;
    uint32_t	PrintCnt;
    uint32_t	SizeBuff = TRACE_MAX_BUFF_SIZE;
    struct tm	Newtime;
    char		TimeStamp[32];
    char		*pOutString;


    pOutString = (char *)MarshallHal_Malloc(TRACE_MAX_BUFF_SIZE);

    if (pOutString == NULL)
        return;

    /** Get time in seconds */
    time(&Aclock);
    /** Convert time to struct tm form. */
    localtime(&Aclock);
    asctime(&Newtime);

    /** insert milli sec */
    snprintf(&TimeStamp[19], 13, ":%0.3d", MarshallHal_Clock() % 1000);
    /** Print the time */
    PrintCnt = snprintf(pOutString, SizeBuff, "\n\r%s\n", TimeStamp);

    /** Print the user args */
    va_start(Args, Fmt);
    PrintCnt += vsnprintf(&pOutString[PrintCnt], TRACE_MAX_BUFF_SIZE - PrintCnt, Fmt, Args);
    va_end(Args);

    /** Print the user data */
    for (i = 0; i < Size; i++)
        PrintCnt += snprintf(&pOutString[PrintCnt], TRACE_MAX_BUFF_SIZE - PrintCnt, "%0.2X ", pData[i]);

    /** Print to the I/O */
    if (TracePrintCB)
        TracePrintCB(pOutString, Src);

    free(pOutString);
}

