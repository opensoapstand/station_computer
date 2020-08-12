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
*
*/
		/************************************************/
		/*				  I N C L U D E S				*/
		/************************************************/
#include "stdafx.h"
#include "Hal.h"
#include "Utils.h"
#include "Status.h"
#include "MarshallApi.h"
#include "Marshall.h"



		/************************************************/
		/*				  D E F I N E S					*/
		/************************************************/

/**
* @brief	Application message queue definitions 
*/
#define MARSHALL_APP_MSG_QUEUE_LENGTH				10
#define MARSHALL_APP_MSG_QUEUE_ITEM_SIZE			sizeof(S_Marshall_AppMsg *)

/**
* @brief	Tx packet queue definitions
*/
#define MARSHALL_TX_QUEUE_LENGTH					10
#define MARSHALL_TX_QUEUE_ITEM_SIZE					sizeof(S_Marshall_TxPacket *)

/**
* @brief	Max wait time for Tx done
*/
#define MARSHALL_WAIT_FOR_TX_DONE_TIMEOUT			10000

/**
* @brief	MS timeout for sending message to the Q
*/
#define MARSHALL_SEND_MSG_TO_Q_MS_TIMEOUT           1000

/**
* @brief	The task wait delay
*/
#define MARSHALL_TASK_WAIT_DLY						1

/**
* @brief	Min time between retries in msec
*/
#define MARSHALL_MIN_TIME_BETWEEN_RETRIES_MS		50

/**
* @brief	Max time between retries in msec
*/
#define MARSHALL_MAX_TIME_BETWEEN_RETRIES_MS		100

/**
* @brief	Wait delay for second byte in msec
*/
#define MARSHALL_WAIT_DELAY_FOR_SECOND_BYTE_MS     	5

/**
* @brief	Maximum retries number
*/
#define MARSHALL_MAX_RETRIES_NUM     				3

/**
* @brief	Maximum COM Rx timeout in msec
*/
#define MARSHALL_MAX_COM_PACKET_RCV_TIMEOUT			100

/**
* @brief	Maximum packet length
*/
#define MARSHALL_MAX_PACKET_LENGTH     				512

/**
* @brief	Queue messages type
*/
#define MARSHALL_TX_QUEUE_MSG_TYPE                  0x7001
#define MARSHALL_APP_QUEUE_MSG_TYPE                 0x7002
#define MARSHALL_APP_QUEUE_BLOCKING_TYPE            0x7003


		/************************************************/
		/*				    E N U M S					*/
		/************************************************/

/**
* @name 	E_Marshall_ChkComRetVal
* @brief	The check COM Rx return values
*/
typedef enum E_Marshall_ChkComRetVal_tag
{
	MARSHALL_CHK_COM_RET_VAL_ERROR,
	MARSHALL_CHK_COM_RET_VAL_PARTIAL_PACKET,
	MARSHALL_CHK_COM_RET_VAL_PACKET_OK,
	MARSHALL_CHK_COM_RET_VAL_NO_PACKET_AVAILABLE,
	MARSHALL_CHK_COM_RET_VAL_TIMEOUT,
	MARSHALL_CHK_COM_RET_VAL_CRC_ERROR,

}E_Marshall_ChkComRetVal;

/**
* @name 	E_Marshall_HandleState
* @brief	The handler state
*/
typedef enum
{
	MARSHALL_HANDLE_STATE_IDLE,
	MARSHALL_HANDLE_WAIT_FOR_ACK,
	MARSHALL_HANDLE_GET_THE_REST_OF_RX_PACKET,
	MARSHALL_HANDLE_GET_THE_REST_OF_ACK,

	MARSHALL_HANDLE_STATE_MAX,

}E_Marshall_HandleState;

/**
* @name 	E_Marshall_TxType
* @brief	Task message type
*           The task accept external message or internal event
*/
typedef enum E_Marshall_TxType_tag
{
	MARSHALL_TX_TYPE_INTERNAL_EVENT = 1,
	MARSHALL_TX_TYPE_EXTERNAL_MSG,

	MARSHALL_TX_TYPE_MAX,

}E_Marshall_TxType;

/**
* @name 	E_Marshall_TxEvent
* @brief	Task event type
*/
typedef enum E_Marshall_TxEvent_tag
{
	MARSHALL_TX_EVENT_RST_Q = 1,

	MARSHALL_NO_EVENT,

}E_Marshall_TxEvent;

/**
* @name 	S_Marshall_TxPacketParams
* @brief
*/
typedef struct S_Marshall_TxPacketParams_tag
{
    uint8_t                    *pData;
    uint32_t                    ThreadId;
    uint16_t 					DataSize;
    E_MarshallApi_FuncCode		FuncCode;
    uint8_t						IsBlocking;
    E_Marshall_TxEvent			TxEvent;
    E_Marshall_TxType           TxType;
}S_Marshall_TxPacketParams;



/**
* @name 	S_Marshall_TxPacket
* @brief
*/
typedef struct S_Marshall_TxPacket_tag
{
	uint8_t					    *pPacket;
    uint32_t                    ThreadId;
	uint8_t					    IsBlocking;
    E_Marshall_TxType		    TxType;
}S_Marshall_TxPacket;



		/************************************************/
		/*			   S T A T I C   D A T A			*/
		/************************************************/
/**
* @name 	S_Marshall_StaticData
* @brief	The module static data
*/
typedef struct S_Marshall_StaticData_tag
{
    pthread_t                       MarshallThreadID;
    uint32_t                        AppThreadID;

    int                             pMarshalSysThread;

	S_MarshallApi_ConfigParams		Cfg;
	S_Marshall_PeripheralInfo		PeripheralInfo;

    Hal_Clock						LastActivityTimeStamp;
	uint8_t							SndPacketID;
	uint8_t							ExpectedRcvPacketID;
	E_Status_Ret                    CurrentTxStatus;

	uint8_t							IsRegistered;
	uint8_t							IsPaired;

    int						   *pSerialHandle;
	S_Marshall_TxPacket			   *pCurrentTxPacket;
	uint8_t						   *pCurrentRxPacket;

    Hal_Clock						LastTxTimeStamp;
	uint32_t						StartOfRcvProcessTimeStamp;
	uint16_t						BytesReceived;
	uint16_t						DelayMsBeforeNextRetry;
	uint8_t							RetryNumber;

	E_Marshall_HandleState			HandleState;
 

}S_Marshall_StaticData;

S_Marshall_StaticData			MarshallStaticData;


		/************************************************/
		/*		 S T A T I C   F U N C T I O N S		*/
		/************************************************/
static void Marshall_HandleRxPacket(void);
static void *Marshall_Task(void *pParameters);
static E_Status_Ret Marshall_BuildPacket(S_Marshall_TxPacketParams  *pTxPacketParams);

/**
* @name 	Marshall_ComRegistration
* @brief 	Registration to a communication channel
*
* @param [in]	ComId		The COM ID
*
* @retval 					The function status
*/
static E_Status_Ret Marshall_ComRegistration(uint8_t  ComId) ////Opening port change uint8t to string 
{
	E_Status_Ret			Ret;

//	/** Create the Tx packet queue */
//    if (TRUE != Hal_CreateThreadQueue(MarshallStaticData.MarshallThreadID, MARSHALL_TX_QUEUE_LENGTH, MARSHALL_TX_QUEUE_ITEM_SIZE))
//        return STATUS_RET_NO_AVAILABLE_RESOURCE;

	/** Open the serial port */

    Ret = Hal_OpenCom();
	if (STATUS_RET_SUCCESS != Ret)
	{
        //Hal_DestroyThreadQueue(MarshallStaticData.MarshallThreadID);
		return Ret;
	}

	MarshallStaticData.HandleState = MARSHALL_HANDLE_STATE_IDLE;

	return STATUS_RET_SUCCESS;
}

/**
* @name 	Marshall_CheckForAvailableRxPacket
* @brief 	Check for available COM Rx packets
*
* @retval 		E_Marshall_ChkComRetVal
*/
static E_Marshall_ChkComRetVal Marshall_CheckForAvailableRxPacket(void) //Read Com Port
{
	uint32_t					ComRcvLength;
	uint32_t					i;
	uint16_t					RxPacketLength = 0;
	uint16_t					PacketCrc;
	uint8_t						PacketLengthSecondByte;

	/** If available, get the next packet length */
    if (STATUS_RET_SUCCESS != Hal_ReadCom((uint8_t*)&RxPacketLength, MARSHALLAPI_PACKET_LENGTH_SIZE, &ComRcvLength))
	    return MARSHALL_CHK_COM_RET_VAL_ERROR;

	if (ComRcvLength)
	{
		if (ComRcvLength == 1)
		{
            /** Wait for the packet length second byte */
			for (i = 0; i < MARSHALL_WAIT_DELAY_FOR_SECOND_BYTE_MS; i++)
			{
                if (STATUS_RET_SUCCESS != Hal_ReadCom( &PacketLengthSecondByte, 1, &ComRcvLength))
				    return MARSHALL_CHK_COM_RET_VAL_ERROR;

				if (ComRcvLength)
				{
					RxPacketLength |= (((uint16_t)PacketLengthSecondByte) << 8);
					break;
				}
                //Hal_SuspendThread(1);
			}
		}
        
        if(!ComRcvLength)
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_CheckForAvailableRxPacket() - Received just 1 byte");
			return MARSHALL_CHK_COM_RET_VAL_ERROR;           
        }
        
		if (RxPacketLength > MARSHALL_MAX_PACKET_LENGTH)
		{
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_CheckForAvailableRxPacket() - Rx packet overflow, packet length - %d", RxPacketLength);
			return MARSHALL_CHK_COM_RET_VAL_ERROR;
		}

		MarshallStaticData.pCurrentRxPacket = (uint8_t *)MarshallHal_Malloc(RxPacketLength + MARSHALLAPI_PACKET_LENGTH_SIZE);

        /** Check Malloc */
		if (MarshallStaticData.pCurrentRxPacket == NULL)
		{
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_CheckForAvailableRxPacket() - Malloc Rx packet");
			return MARSHALL_CHK_COM_RET_VAL_ERROR;
		}
		else
		{
			MarshallStaticData.StartOfRcvProcessTimeStamp = MarshallHal_Clock();
			*(uint16_t*)&MarshallStaticData.pCurrentRxPacket[0] = RxPacketLength;
            if (STATUS_RET_SUCCESS != Hal_ReadCom(
				                                    &MarshallStaticData.pCurrentRxPacket[MARSHALLAPI_PACKET_LENGTH_SIZE],
				                                    RxPacketLength,
				                                    &ComRcvLength))
			{
				free(MarshallStaticData.pCurrentRxPacket);
				return MARSHALL_CHK_COM_RET_VAL_ERROR;
			}
			if (ComRcvLength)
			{
				if (ComRcvLength < RxPacketLength)
				{
                    /** Rceveived part of the packet */
					MarshallStaticData.BytesReceived = (uint16_t)ComRcvLength;
					return MARSHALL_CHK_COM_RET_VAL_PARTIAL_PACKET;
				}
				else
				{
                    MarshallHal_TracePrint(RxPacketLength + 2, MarshallStaticData.pCurrentRxPacket, HAL_TRACE_SRC_COM_PACKETS, "Rx: ");
					
                    /** Check CRC */
					PacketCrc = Utils_Calc_CRC_CCITT(MarshallStaticData.pCurrentRxPacket, RxPacketLength, UTILS_SEED_CCITT);
					if (*((uint16_t*)(&MarshallStaticData.pCurrentRxPacket[RxPacketLength])) == PacketCrc)
					{
						return MARSHALL_CHK_COM_RET_VAL_PACKET_OK;
					}
					else
					{
						MarshallStaticData.BytesReceived = 0;
						free(MarshallStaticData.pCurrentRxPacket);
                        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_CheckForAvailableRxPacket() - CRC error");

						/** Clear the Rx buffers */
                        Hal_PurgeCom( HAL_PURGE_COM_RXCLEAR);
						return MARSHALL_CHK_COM_RET_VAL_CRC_ERROR;
					}
				}
			}
			else
			{
				return MARSHALL_CHK_COM_RET_VAL_PARTIAL_PACKET;
			}
		}
	}
	return MARSHALL_CHK_COM_RET_VAL_NO_PACKET_AVAILABLE;
}

/**
* @name 	Marshall_GetTheRestOfRxPacket
* @brief 	Get the rest of Rx packet
* 			(when Marshall_CheckForAvailableRxPacket() get only part of the packet)
*
* @retval 		E_Marshall_ChkComRetVal
*/
static E_Marshall_ChkComRetVal Marshall_GetTheRestOfRxPacket(void)
{
	uint32_t							ComRcvLength;
	uint32_t							PacketLength;
	Hal_Clock							TimeStamp;
	uint16_t							PacketCrc;
	E_Marshall_ChkComRetVal				RetVal = MARSHALL_CHK_COM_RET_VAL_NO_PACKET_AVAILABLE;

	PacketLength = *(uint16_t*)&MarshallStaticData.pCurrentRxPacket[MARSHALLAPI_PACKET_HEADR_PACKET_LENGTH_OFFSET] - MarshallStaticData.BytesReceived;

    /** Get COM data */
    if (STATUS_RET_SUCCESS != Hal_ReadCom(
		                                    &MarshallStaticData.pCurrentRxPacket[MarshallStaticData.BytesReceived + MARSHALLAPI_PACKET_LENGTH_SIZE],
		                                    PacketLength,
		                                    &ComRcvLength))
	{
		MarshallStaticData.BytesReceived = 0;
		free(MarshallStaticData.pCurrentRxPacket);
		return MARSHALL_CHK_COM_RET_VAL_ERROR;
	}

	if (ComRcvLength)
	{
        /** Add to the bytes received counter */
		MarshallStaticData.BytesReceived += (uint16_t)ComRcvLength;
		
        /** Check if we received the entire packet */
		if (MarshallStaticData.BytesReceived == *(uint16_t*)&MarshallStaticData.pCurrentRxPacket[MARSHALLAPI_PACKET_HEADR_PACKET_LENGTH_OFFSET])
		{
            MarshallHal_TracePrint(MarshallStaticData.BytesReceived + 2, MarshallStaticData.pCurrentRxPacket, HAL_TRACE_SRC_COM_PACKETS, "Rx:\n");
			
            /** Check CRC */
			PacketCrc = Utils_Calc_CRC_CCITT(MarshallStaticData.pCurrentRxPacket, MarshallStaticData.BytesReceived, UTILS_SEED_CCITT);
			if (*((uint16_t*)(&MarshallStaticData.pCurrentRxPacket[MarshallStaticData.BytesReceived])) == PacketCrc)
			{
				RetVal = MARSHALL_CHK_COM_RET_VAL_PACKET_OK;
			}
			else
			{
				free(MarshallStaticData.pCurrentRxPacket);
				RetVal = MARSHALL_CHK_COM_RET_VAL_CRC_ERROR;

                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_GetTheRestOfRxPacket() - CRC error");

				/** Clear the Rx buffers */
                Hal_PurgeCom( HAL_PURGE_COM_RXCLEAR);
			}
			MarshallStaticData.BytesReceived = 0;

			/** return, no need to check for timeout */
			return RetVal;
		}
	}

	/** Timeout for receiving the entire packet */
	TimeStamp = MarshallHal_Clock();
	if (MarshallHal_GetDiffTimeInMillSec(MarshallStaticData.StartOfRcvProcessTimeStamp, TimeStamp) >= MARSHALL_MAX_COM_PACKET_RCV_TIMEOUT)
	{
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_GetTheRestOfRxPacket() - Rx timeout error");
		free(MarshallStaticData.pCurrentRxPacket);
		MarshallStaticData.BytesReceived = 0;
		RetVal = MARSHALL_CHK_COM_RET_VAL_TIMEOUT;
	}

	return RetVal;
}


/**
* @name 	Marshall_SendComTx
* @brief 	Send a packet to the COM channel
*/
static void Marshall_SendComTx(void)
{
	uint32_t		BytesToWrite;
	uint32_t		BytesWritten;


	BytesToWrite = *(uint16_t *)&MarshallStaticData.pCurrentTxPacket->pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_LENGTH_OFFSET] + MARSHALLAPI_PACKET_LENGTH_SIZE;

    MarshallHal_TracePrint(BytesToWrite, MarshallStaticData.pCurrentTxPacket->pPacket, HAL_TRACE_SRC_COM_PACKETS, "Tx: ");

	MarshallStaticData.LastTxTimeStamp = MarshallHal_Clock();

	/** Delay = random time between MIN to MAX time */
	MarshallStaticData.DelayMsBeforeNextRetry =
		(rand() % (MARSHALL_MAX_TIME_BETWEEN_RETRIES_MS - MARSHALL_MIN_TIME_BETWEEN_RETRIES_MS + 1))
		+ MARSHALL_MIN_TIME_BETWEEN_RETRIES_MS;

    Hal_WriteCom(	MarshallStaticData.pCurrentTxPacket->pPacket,
					BytesToWrite,
					&BytesWritten);

	if (BytesWritten != BytesToWrite)
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_SendComTx() - didn't write the whole Tx packet");

	MarshallStaticData.RetryNumber++;
}


/**
* @name 	Marshall_ChkPacketForAck
* @brief 	Check if received a valid ACK
*
* @retval   1 - Received ACK
*/
static uint32_t Marshall_ChkPacketForAck(void)
{
	S_MarshallApi_PacketHeader *pTxPacket, *pRxPacket;

	pTxPacket = (S_MarshallApi_PacketHeader *)MarshallStaticData.pCurrentTxPacket->pPacket;
	pRxPacket = (S_MarshallApi_PacketHeader *)MarshallStaticData.pCurrentRxPacket;

	if (pRxPacket->FuncCode == MARSHALLAPI_CODE_RESPONSE
		&& MarshallStaticData.pCurrentRxPacket[MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET] == MARSHALLAPI_RESPONSE_VAL_ACK)
	{
		if (pTxPacket->Dest         == pRxPacket->Source
			    && pTxPacket->DestLsb   == pRxPacket->SourceLsb
			    && pTxPacket->Source    == pRxPacket->Dest
			    && pTxPacket->SourceLsb == pRxPacket->DestLsb
			    && pTxPacket->PacketId  == pRxPacket->PacketId)
		    return 1;
	}

	return 0;
}


/**
* @name 	Marshall_TxEnd
* @brief 	Handle the end of Tx process
*
* @param [in]	pTxPacket	Tx packet
* @param [in]	TxStatus	The Tx status
*/
static void Marshall_TxEnd(S_Marshall_TxPacket *pTxPacket, E_Status_Ret TxStatus)
{
    if (pTxPacket->IsBlocking)
	{
		MarshallStaticData.CurrentTxStatus = TxStatus;
        if (FALSE == Hal_SendMsgToThreadQueue(pTxPacket->ThreadId, NULL, MARSHALL_SEND_MSG_TO_Q_MS_TIMEOUT, MARSHALL_APP_QUEUE_BLOCKING_TYPE))
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_ComHdlr() - release blocking");
    }
	if (pTxPacket->TxType == MARSHALL_TX_TYPE_EXTERNAL_MSG)
        free(pTxPacket->pPacket);

	free(pTxPacket);
	MarshallStaticData.RetryNumber = 0;
}

/**
* @name 	Marshall_ChangeRetryNumInPacket
* @brief 	Change the packet retry number and calculate the packet CRC
*
* @param [in]	pPacket		The packet to change it retry number
* @param [in]	RetryNum	The new retry number
*/
static void Marshall_ChangeRetryNumInPacket(uint8_t *pPacket, uint8_t RetryNum)
{
	uint16_t    PacketCrc;
	uint16_t    PacketLength;

	pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_OPTIONS_OFFSET] &= ~MARSHALLAPI_OPTIONS_RETRY_NUMBER_MASK;
	pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_OPTIONS_OFFSET] |= (RetryNum << MARSHALLAPI_OPTIONS_RETRY_NUMBER_OFFSET);

	PacketLength = *(uint16_t*)&pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_LENGTH_OFFSET];

	PacketCrc = Utils_Calc_CRC_CCITT(pPacket, PacketLength, UTILS_SEED_CCITT);

	*(uint16_t*)&pPacket[PacketLength] = PacketCrc;
}

/**
* @name 	Marshall_ClrTxQAndRxFifo
* @brief 	Clear the Tx message queue and the hardware buffers
*
*/
static void Marshall_ClrTxQAndRxFifo(void)
{
	S_Marshall_TxPacket		    *pTxPacket = NULL;

	/** Clear the Tx packets */
	do
	{
//        if (FALSE == Hal_GetThreadQueueMsg(MarshallStaticData.MarshallThreadID, (void **)&pTxPacket, 0, MARSHALL_TX_QUEUE_MSG_TYPE))
//		    break;

		Marshall_TxEnd(pTxPacket, STATUS_RET_FAIL_BECAUSE_OF_RESET);
	} while (1);

	/** Clear the Rx and Tx buffers */
    Hal_PurgeCom(HAL_PURGE_COM_RXCLEAR | HAL_PURGE_COM_TXCLEAR);
}

/**
* @name 	Marshall_ComHdlr
* @brief 	The COM handler -
* 				Check for Rx available in the COM channel
* 				Check the Tx queue for outgoing packets
* 				handle the COM state machine
*/
static void inline Marshall_ComHdlr(void)
{
	E_Marshall_ChkComRetVal		    ChkComRetVal;
    S_Marshall_TxPacketParams       *pTxPacketParams;
    E_Status_Ret                    Ret;

	switch (MarshallStaticData.HandleState)
	{
	case MARSHALL_HANDLE_STATE_IDLE:
		/** Check for available Rx packets */
		ChkComRetVal = Marshall_CheckForAvailableRxPacket();
		if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_PARTIAL_PACKET)
		{
			MarshallStaticData.HandleState = MARSHALL_HANDLE_GET_THE_REST_OF_RX_PACKET;
			break;
		}
		else if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_PACKET_OK)
		{
			/** Handle Rx packet */
			Marshall_HandleRxPacket();
		}
        
		/** Check for available Tx packets */
        if (Hal_GetThreadQueueMsg(MarshallStaticData.MarshallThreadID, (void **)&pTxPacketParams, 0, MARSHALL_TX_QUEUE_MSG_TYPE))
        {
            //if (pTxPacketParams->TxType == MARSHALL_TX_TYPE_EXTERNAL_MSG)
            {
                /** Build the packet */
                Ret = Marshall_BuildPacket(pTxPacketParams);
                if (STATUS_RET_SUCCESS != Ret)
                {
                    MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_HandleTxPacket() - Marshall_BuildPacket");
                    break;
                }

                /** Send the packet */
                Marshall_SendComTx();

                /** Check if it is a broadcast (no need to wait for ACK or to perform retries)*/
                if (MarshallStaticData.pCurrentTxPacket->pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_OPTIONS_OFFSET]
                    & MARSHALLAPI_OPTIONS_ACK_REQUESTED_MASK)
                {
                    MarshallStaticData.HandleState = MARSHALL_HANDLE_WAIT_FOR_ACK;
                }
                else
                {
                    free(MarshallStaticData.pCurrentTxPacket->pPacket);
                    free(MarshallStaticData.pCurrentTxPacket);
                }
            }
            /** If it is an event */
            //else
            {
                switch (pTxPacketParams->TxEvent)
                {
                    case MARSHALL_TX_EVENT_RST_Q:
                        Marshall_ClrTxQAndRxFifo();
                        break;

                    default:
                        break;
                }

                if (pTxPacketParams->IsBlocking)
                {
                    MarshallStaticData.CurrentTxStatus = STATUS_RET_SUCCESS;
                    if (FALSE == Hal_SendMsgToThreadQueue(pTxPacketParams->ThreadId, NULL, MARSHALL_SEND_MSG_TO_Q_MS_TIMEOUT, MARSHALL_APP_QUEUE_BLOCKING_TYPE))
                        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_ComHdlr() - release blocking");
                }
            }


            /** Free the pTxPacketParams */
            if (pTxPacketParams->DataSize)
                free(pTxPacketParams->pData);
            free(pTxPacketParams);
        }
		break;

	case MARSHALL_HANDLE_GET_THE_REST_OF_RX_PACKET:
		ChkComRetVal = Marshall_GetTheRestOfRxPacket();

        /** Handle Rx packet */
		if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_PACKET_OK)
		    Marshall_HandleRxPacket();

		/** If Packet OK,packet time out or CRC error */
		if (ChkComRetVal != MARSHALL_CHK_COM_RET_VAL_NO_PACKET_AVAILABLE)
		    MarshallStaticData.HandleState = MARSHALL_HANDLE_STATE_IDLE;

		break;

	case MARSHALL_HANDLE_WAIT_FOR_ACK:
		/** check the COM for available packets */
		ChkComRetVal = Marshall_CheckForAvailableRxPacket();
		if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_PARTIAL_PACKET)
		{
			MarshallStaticData.HandleState = MARSHALL_HANDLE_GET_THE_REST_OF_ACK;
		}
		else if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_PACKET_OK)
		{
			if (Marshall_ChkPacketForAck())
			{
				Marshall_TxEnd(MarshallStaticData.pCurrentTxPacket, STATUS_RET_SUCCESS);
				MarshallStaticData.HandleState = MARSHALL_HANDLE_STATE_IDLE;
			}
			/** Handle Rx packet */
			Marshall_HandleRxPacket();
		}
		if (MarshallStaticData.HandleState != MARSHALL_HANDLE_WAIT_FOR_ACK)
		    break;

		/** Send retry if necessary */
		if (MarshallHal_GetDiffTimeInMillSec(MarshallStaticData.LastTxTimeStamp, MarshallHal_Clock())
			>= MarshallStaticData.DelayMsBeforeNextRetry)
		{
			if (MarshallStaticData.RetryNumber < MARSHALL_MAX_RETRIES_NUM)
			{
				Marshall_ChangeRetryNumInPacket(MarshallStaticData.pCurrentTxPacket->pPacket, MarshallStaticData.RetryNumber);
				Marshall_SendComTx();
			}
			else
			{
				/** Time-out */
				MarshallStaticData.IsPaired = 0;
				Marshall_TxEnd(MarshallStaticData.pCurrentTxPacket, STATUS_RET_TIMEOUT);
				MarshallStaticData.HandleState = MARSHALL_HANDLE_STATE_IDLE;
			}
		}
		break;

	case MARSHALL_HANDLE_GET_THE_REST_OF_ACK:
		/** Set next state - wait for ack */
		MarshallStaticData.HandleState = MARSHALL_HANDLE_WAIT_FOR_ACK;

		/** Get the rest of the packet */
		ChkComRetVal = Marshall_GetTheRestOfRxPacket();
		if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_PACKET_OK)
		{
			if (Marshall_ChkPacketForAck())
			{
				Marshall_TxEnd(MarshallStaticData.pCurrentTxPacket, STATUS_RET_SUCCESS);
				MarshallStaticData.HandleState = MARSHALL_HANDLE_STATE_IDLE;
			}
			/** Handle Rx packet */
			Marshall_HandleRxPacket();
		}

		if (ChkComRetVal == MARSHALL_CHK_COM_RET_VAL_NO_PACKET_AVAILABLE)
		    MarshallStaticData.HandleState = MARSHALL_HANDLE_GET_THE_REST_OF_ACK;
		break;

	default:
		break;
	}
}

/**
* @name 	Marshall_SendPacketToTxQ
* @brief 	
*
* @param [in]	pTxPacket	The Tx packet
*
* @retval 					The function status
*/
E_Status_Ret Marshall_SendPacketToTxQ(S_Marshall_TxPacketParams *pTxPacket)
{
	if (pTxPacket == NULL)
	    return STATUS_RET_ERROR_PARAM_IN;

//    if (TRUE == Hal_SendMsgToThreadQueue(MarshallStaticData.MarshallThreadID, pTxPacket, MARSHALL_SEND_MSG_TO_Q_MS_TIMEOUT, MARSHALL_TX_QUEUE_MSG_TYPE))
        return STATUS_RET_SUCCESS;

    return STATUS_RET_NO_AVAILABLE_RESOURCE;
}

/**
*
* @name 	Marshall_BuildResponse
* @brief	Build the response packet
*
* @param [in]	pData				Packet data
* @param [in]	pPacketToResponseTo	Packet to response to (if it need to send a response to a received packet)
* @param [out]	pBuildedPacket		The builded packet (if it wasn't set to NULL)
* @retval 					        The function status
*/
static E_Status_Ret Marshall_BuildResponse(uint8_t 			   		   *pData,
                                            S_Marshall_TxPacket	      **pBuildedPacket,
                                            S_MarshallApi_Packet	   *pPacketToResponseTo)
{
    S_MarshallApi_PacketHeader *pPacketHeader;
    S_Marshall_TxPacket	       *pTxPacket;
    uint16_t					PacketLength;
    E_Status_Ret				Ret = STATUS_RET_SUCCESS;

    if (pBuildedPacket == NULL)
        return STATUS_RET_ERROR_PARAM_IN;

    *pBuildedPacket = NULL;

    /** Allocate the Tx packet structure */
    pTxPacket = (S_Marshall_TxPacket *)MarshallHal_Malloc(sizeof(S_Marshall_TxPacket));
    if (pTxPacket == NULL)
        return STATUS_RET_BUFF_ALLOCATION_ERROR;

    pTxPacket->IsBlocking = 0;
    pTxPacket->TxType = MARSHALL_TX_TYPE_EXTERNAL_MSG;

    /** Allocate a data buffer, It will free it in the end of the Tx process */
    pTxPacket->pPacket = (uint8_t *)MarshallHal_Malloc(MARSHALLAPI_RESPONSE_CMD_DATA_SIZE + MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET + MARSHALLAPI_PACKET_CRC_SIZE);
    if (pTxPacket->pPacket == NULL)
    {
        free(pTxPacket);
        return STATUS_RET_BUFF_ALLOCATION_ERROR;
    }

    pPacketHeader = (S_MarshallApi_PacketHeader	*)pTxPacket->pPacket;

    /** Prepare the packet header (the packet length include the CRC but exclude itself)*/
    PacketLength = MARSHALLAPI_RESPONSE_CMD_DATA_SIZE + MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET;
    pPacketHeader->PacketLength     = PacketLength;
    pPacketHeader->PacketId         = pPacketToResponseTo->Header.PacketId;
    pPacketHeader->Source           = pPacketToResponseTo->Header.Dest;
    pPacketHeader->SourceLsb        = pPacketToResponseTo->Header.DestLsb;
    pPacketHeader->Dest             = pPacketToResponseTo->Header.Source;
    pPacketHeader->DestLsb          = pPacketToResponseTo->Header.SourceLsb;
    pPacketHeader->PacketOptions    = MARSHALLAPI_OPTIONS_ACK_NOT_REQUESTED;
    pPacketHeader->FuncCode         = MARSHALLAPI_CODE_RESPONSE;

    /** Copy the packet data */
    memcpy(&pTxPacket->pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET], pData, MARSHALLAPI_RESPONSE_CMD_DATA_SIZE);

    /**
    * The CRC calculation is including the PacketLength and excluding itself
    * CRC size is equal to PacketLength size so the CRC size to calculate is equal to the PacketLength
    */
    *((uint16_t*)&pTxPacket->pPacket[PacketLength]) = Utils_Calc_CRC_CCITT(pTxPacket->pPacket, PacketLength, UTILS_SEED_CCITT);

    *pBuildedPacket = pTxPacket;
    return Ret;
}


/**
*
* @name 	Marshall_BuildPacket
* @brief	Build the packet
*
* @param [in]	pTxPacketParams     Tx packet parameters
* @retval 					        The function status
*/
static E_Status_Ret Marshall_BuildPacket(S_Marshall_TxPacketParams  *pTxPacketParams)
{
	S_MarshallApi_PacketHeader *pPacketHeader;
	S_Marshall_TxPacket	       *pTxPacket;
	uint16_t					PacketLength;
	E_Status_Ret				Ret = STATUS_RET_SUCCESS;

    if (pTxPacketParams == NULL)
        return STATUS_RET_ERROR_PARAM_IN;

    MarshallStaticData.pCurrentTxPacket = NULL;

	/** Allocate the Tx packet structure */
	pTxPacket = (S_Marshall_TxPacket *)MarshallHal_Malloc(sizeof(S_Marshall_TxPacket));
	if (pTxPacket == NULL)
	    return STATUS_RET_BUFF_ALLOCATION_ERROR;

    pTxPacket->IsBlocking = pTxPacketParams->IsBlocking;
    pTxPacket->TxType = pTxPacketParams->TxType;
    pTxPacket->ThreadId = pTxPacketParams->ThreadId;

	/** Allocate a buffer, It will free it in the end of the Tx process */
    pTxPacket->pPacket = (uint8_t *)MarshallHal_Malloc(pTxPacketParams->DataSize +
                                                        MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET +
                                                        MARSHALLAPI_PACKET_CRC_SIZE);
	if (pTxPacket->pPacket == NULL)
	{
		free(pTxPacket);
		return STATUS_RET_BUFF_ALLOCATION_ERROR;
	}

	pPacketHeader = (S_MarshallApi_PacketHeader	*)pTxPacket->pPacket;

	/** Prepare the packet header (the packet length include the CRC but exclude itself)*/
    PacketLength = pTxPacketParams->DataSize + MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET;
	pPacketHeader->PacketLength = PacketLength;

    if (pTxPacketParams->FuncCode == MARSHALLAPI_CODE_FW_INFO)
		MarshallStaticData.SndPacketID = 0;

	pPacketHeader->PacketId         = MarshallStaticData.SndPacketID++;
    pPacketHeader->Source           = MarshallStaticData.Cfg.PeripheralID;
    pPacketHeader->SourceLsb        = MarshallStaticData.PeripheralInfo.SerialNum[0];
    pPacketHeader->Dest             = MARSHALLAPI_AMIT_SOURCE_ADDR;
    pPacketHeader->DestLsb          = MarshallStaticData.Cfg.AmitSerialNumber[0];
    pPacketHeader->PacketOptions    = MARSHALLAPI_OPTIONS_ACK_REQUESTED;

    if (pTxPacketParams->FuncCode == MARSHALLAPI_CODE_FW_INFO)
        pPacketHeader->PacketOptions = MARSHALLAPI_OPTIONS_ACK_NOT_REQUESTED;

    pPacketHeader->FuncCode = pTxPacketParams->FuncCode;

	/** Copy the packet data */
    memcpy(&pTxPacket->pPacket[MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET], pTxPacketParams->pData, pTxPacketParams->DataSize);

	/**
	* The CRC calculation is including the PacketLength and excluding itself
	* CRC size is equal to PacketLength size so the CRC size to calculate is equal to the PacketLength
	*/
	*((uint16_t*)&pTxPacket->pPacket[PacketLength]) = Utils_Calc_CRC_CCITT(pTxPacket->pPacket, PacketLength, UTILS_SEED_CCITT);

    MarshallStaticData.pCurrentTxPacket = pTxPacket;
	return Ret;
}


/**
* @name 	Marshall_SendMsgToApp
* @brief	Send a message to the application layer
*
* @param [in]	pPacket		The packet to send (in case of external message)
* @param [in]   Event		The event to send (in case of internal event)
*
* @retval 		None
*/
static void Marshall_SendMsgToApp(S_MarshallApi_Packet *pPacket,
									E_Marshall_AppMsgType MsgType,
									E_Marshall_AppMsgEvents Event)
{
	S_Marshall_AppMsg	    *pAppMsg;
	uint16_t				 MsgSize;


	pAppMsg = (S_Marshall_AppMsg	*)MarshallHal_Malloc(sizeof(S_Marshall_AppMsg));
	if (pAppMsg == NULL)
	{
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_SendMsgToApp() - MarshallHal_Malloc");
		return;
	}

	pAppMsg->Event = Event;

	if (MsgType == MARSHALL_INTERNAL_EVENT)
	{
		pAppMsg->MsgType = MARSHALL_INTERNAL_EVENT;
		pAppMsg->DataSize = 0;
		pAppMsg->pData = NULL;
	}
	else
	{
		MsgSize = pPacket->Header.PacketLength - (MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET - 1);
		/** It is the application responsibility to free the buffer
		* 	 Send to the application the function code + the function data */
		pAppMsg->pData = (uint8_t *)MarshallHal_Malloc(MsgSize);

		if (pAppMsg->pData != NULL)
		{
			pAppMsg->MsgType = MARSHALL_EXTERNAL_MSG;
			pAppMsg->Event = 0;
			pAppMsg->DataSize = MsgSize;
			memcpy(pAppMsg->pData, &pPacket->Header.FuncCode, MsgSize);
		}
		else
		{
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_SendMsgToApp() - MarshallHal_Malloc(MsgSize)");
			free(pAppMsg);
		}
	}

    if (FALSE == Hal_SendMsgToThreadQueue(MarshallStaticData.AppThreadID, pAppMsg, MARSHALL_SEND_MSG_TO_Q_MS_TIMEOUT, MARSHALL_APP_QUEUE_MSG_TYPE))
    {
        if (MsgType == MARSHALL_EXTERNAL_MSG)
            free(pAppMsg->pData);

        free(pAppMsg);
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_SendMsgToApp() - CyclicBuff_Put");
    }
}

/**
* @name 	Marshall_HandleRxPacket
* @brief	Handle the Rx packet
*
*/
static void Marshall_HandleRxPacket(void)
{
	S_MarshallApi_Packet 	   *pPacket;
	S_Marshall_TxPacket 	   *pPacketResponse = NULL;
	uint8_t					   *pMsgResponse = NULL;
	uint32_t					ResponseSize;
	uint32_t					BytesWritten;
	uint8_t						ResponseExtraData = MARSHALLAPI_RESPONSE_VAL_MAX;
	uint8_t						ForwordPacketToApp = 0;


	pPacket = (S_MarshallApi_Packet *)MarshallStaticData.pCurrentRxPacket;

	/** if it is a reset packet, disable the Marshall communication*/
	if (pPacket->Header.FuncCode == MARSHALLAPI_CODE_RESET)
	{
		MarshallStaticData.IsPaired = 0;
		ForwordPacketToApp			= 1;
	}
    /** Check for configuration message*/
    else if (pPacket->Header.FuncCode == MARSHALLAPI_CODE_CONFIG
                && pPacket->Header.DestLsb == MarshallStaticData.PeripheralInfo.SerialNum[0] )
    {
        /** Update last activity */
        MarshallStaticData.LastActivityTimeStamp = MarshallHal_Clock();

        ForwordPacketToApp = 1;
        MarshallStaticData.IsPaired = 1;
        MarshallStaticData.ExpectedRcvPacketID = (pPacket->Header.PacketId + 1);
        memcpy(&MarshallStaticData.Cfg, &pPacket->FunctionsParams.ConfigParams, sizeof(S_MarshallApi_ConfigParams));        
    }
	/** Check the packet address */
	else if ((pPacket->Header.Source	== MARSHALLAPI_AMIT_SOURCE_ADDR
		        && pPacket->Header.SourceLsb	== MarshallStaticData.Cfg.AmitSerialNumber[0]
		        && pPacket->Header.Dest			== MarshallStaticData.Cfg.PeripheralID
		        && pPacket->Header.DestLsb		== MarshallStaticData.PeripheralInfo.SerialNum[0]))
	{
		/** Update last activity */
		MarshallStaticData.LastActivityTimeStamp = MarshallHal_Clock();

		/** check for response message*/
		if (pPacket->Header.FuncCode == MARSHALLAPI_CODE_RESPONSE)
		{
			if (MarshallStaticData.pCurrentRxPacket[MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET] != MARSHALLAPI_RESPONSE_VAL_ACK)
			{
				Marshall_SendMsgToApp(NULL, MARSHALL_INTERNAL_EVENT, MARSHALL_RCV_NACK);
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_HandleRxPacket() - Rcv NACK");
			}
			pPacketResponse = NULL;
		}
		else
		{
			if (pPacket->Header.PacketOptions & MARSHALLAPI_OPTIONS_ACK_REQUESTED_MASK)
			    ResponseExtraData = MARSHALLAPI_RESPONSE_VAL_ACK;

			if (MarshallStaticData.ExpectedRcvPacketID == pPacket->Header.PacketId)
			{
				ForwordPacketToApp = 1;
				MarshallStaticData.ExpectedRcvPacketID++;
			}
			/** In case we already received the packet and the other side didn't get the ACK */
			else if ((pPacket->Header.PacketOptions & MARSHALLAPI_OPTIONS_RETRY_NUMBER_MASK)
				&& MarshallStaticData.ExpectedRcvPacketID == (uint8_t)(pPacket->Header.PacketId + 1))	//Casting - handling rape around
			{

			}
			else
			{
				Marshall_SendMsgToApp(NULL, MARSHALL_INTERNAL_EVENT, MARSHALL_NON_CONSECUTIVE_PACKET_ID_EVENT);
				ResponseExtraData = MARSHALLAPI_RESPONSE_VAL_NON_CONSECUTIVE_PACKET_ID;
			}
		}
	}

	/** If it need to forward the packet to the application */
	if (ForwordPacketToApp)
	{
		Marshall_SendMsgToApp(pPacket, MARSHALL_EXTERNAL_MSG, MARSHALL_EVENT_MAX);
	}

	if (ResponseExtraData != MARSHALLAPI_RESPONSE_VAL_MAX)
	{
		/** Build the response */
        if (STATUS_RET_SUCCESS != Marshall_BuildResponse(&ResponseExtraData,
														&pPacketResponse,
														pPacket             ))
		{
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_HandleRxPacket() - Marshall_BuildPacket");
		}
		else
		{
			/** Send the response packet */
			pMsgResponse = pPacketResponse->pPacket;
			ResponseSize = *(uint16_t *)&pMsgResponse[MARSHALLAPI_PACKET_HEADR_PACKET_LENGTH_OFFSET] + MARSHALLAPI_PACKET_LENGTH_SIZE;
            MarshallHal_TracePrint(ResponseSize, pMsgResponse, HAL_TRACE_SRC_COM_PACKETS, "ACK:\n");

            Hal_WriteCom(pMsgResponse, ResponseSize, &BytesWritten);

			if (BytesWritten != ResponseSize)
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_HandleRxPacket() - didn't write the whole Tx packet");

			free(pMsgResponse);
		}
		free(pPacketResponse);
	}

	free(MarshallStaticData.pCurrentRxPacket);
}

/**
* @name 	Marshall_SendAndBlock
* @brief 	Send a packet
*
* @param [in]	pData			The packet data
* @param [in]	DataSize		The packet data size
* @param [in]	FuncCode		The packet function code
* @param [in]	IsBlocking		If it need to block the sender until the Tx is done (ACK,Timeout...)
* @param [in]	TxEvent			If it need to send an event
*
* @retval 					The function status
*/
static E_Status_Ret Marshall_SendAndBlock(uint8_t 			   			*pData,
                                            uint16_t 					DataSize,
                                            E_MarshallApi_FuncCode 		FuncCode,
                                            uint8_t						IsBlocking,
                                            E_Marshall_TxEvent			TxEvent)
{
    S_Marshall_TxPacketParams       *pTxPacketParams;
    E_Status_Ret 		            Ret;
    uint8_t                         *pMsg;

    

    /** Allocate the Tx packet parameters structure */
    pTxPacketParams = (S_Marshall_TxPacketParams *)MarshallHal_Malloc(sizeof(S_Marshall_TxPacketParams));
    if (pTxPacketParams == NULL)
        return STATUS_RET_BUFF_ALLOCATION_ERROR;

    /** Allocate the Tx packet data */
    if (DataSize)
    {        
        pTxPacketParams->pData = (uint8_t *)MarshallHal_Malloc(DataSize);
        if (pTxPacketParams->pData == NULL)
        {
            free(pTxPacketParams);
            return STATUS_RET_BUFF_ALLOCATION_ERROR;
        }            
    }

    /** Copy message parameters */
    memcpy(pTxPacketParams->pData, pData, DataSize);
    pTxPacketParams->DataSize   = DataSize;
    pTxPacketParams->FuncCode   = FuncCode;
    pTxPacketParams->IsBlocking = IsBlocking;
    pTxPacketParams->TxEvent    = TxEvent;
    pTxPacketParams->ThreadId   = Hal_GetCurrentThreadID();

    if (TxEvent == MARSHALL_NO_EVENT)
        pTxPacketParams->TxType = MARSHALL_TX_TYPE_EXTERNAL_MSG;
    else
        pTxPacketParams->TxType = MARSHALL_TX_TYPE_INTERNAL_EVENT;

    /** Send it to the task */
    Ret = Marshall_SendPacketToTxQ(pTxPacketParams);
    if (Ret != STATUS_RET_SUCCESS)
    {
        if (DataSize)
            free(pTxPacketParams->pData);
        free(pTxPacketParams);
    }
            
    /** If it need to block the user until it receive a response or timeout */
    if (IsBlocking)
    {
        if (FALSE == Hal_GetThreadQueueMsg(pTxPacketParams->ThreadId,
                                            (void **)&pMsg,
                                            MARSHALL_WAIT_FOR_TX_DONE_TIMEOUT,
                                            MARSHALL_APP_QUEUE_BLOCKING_TYPE))
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_SendPacket() - time out");
            Ret = STATUS_RET_TIMEOUT;
        }
        else
        {
            Ret = MarshallStaticData.CurrentTxStatus;
        }

    }    
    return Ret;
}

		/************************************************/
		/*			   F U N C T I O N S				*/
		/************************************************/

/**
* @name 	Marshall_Init
* @brief	Initialize the module
*/
void Marshall_Init(HalTracePrintCB PrintCB)
{
	memset(&MarshallStaticData, 0, sizeof(S_Marshall_StaticData));

    MarshallHal_TraceInit(PrintCB);
}

/**
* @name 	Marshall_Close
* @brief 	Close the module handles
*/
void Marshall_Close(void)
{
    Hal_CloseCom();

    Hal_DestroyThreadQueue(MarshallStaticData.MarshallThreadID);
    Hal_DestroyThreadQueue(MarshallStaticData.AppThreadID);

    if (MarshallStaticData.pMarshalSysThread)
    {
        Hal_CloseThread(MarshallStaticData.pMarshalSysThread);
        MarshallStaticData.pMarshalSysThread = NULL;
    }
}


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
									uint8_t 				    ComId)
{
	E_Status_Ret		  Ret;

	/** Check the in parameters */
	if (pPeripheralInfo == NULL)
	    return STATUS_RET_ERROR_PARAM_IN;

	if (MarshallStaticData.IsRegistered)
	    return STATUS_RET_MODULE_ALREADY_OPEN;

	memcpy(&MarshallStaticData.PeripheralInfo, pPeripheralInfo, sizeof(S_Marshall_PeripheralInfo));

	/** Create the application message queue */
    MarshallStaticData.AppThreadID = AppThreadID;
//    if (TRUE != Hal_CreateThreadQueue(AppThreadID, MARSHALL_APP_MSG_QUEUE_LENGTH, MARSHALL_APP_MSG_QUEUE_ITEM_SIZE))
//        return STATUS_RET_NO_AVAILABLE_RESOURCE;

	/** Register to the COM */
	Ret = Marshall_ComRegistration(ComId);
	if (Ret != STATUS_RET_SUCCESS)
	{
		Marshall_Close();
		return Ret;
	}

    /** Create the Marshall task */
    MarshallStaticData.pMarshalSysThread = Hal_CreateThread(MarshallStaticData.MarshallThreadID);
   // cout<<MarshallStaticData.pMarshalSysThread<<endl;
    if (MarshallStaticData.pMarshalSysThread != 0)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_ComRegistration() - CreateThread");
        Marshall_Close();
        return STATUS_RET_UNEXPECTED_ERROR;
    }

    MarshallStaticData.LastActivityTimeStamp = MarshallHal_Clock();
    MarshallStaticData.Cfg.PeripheralID = pPeripheralInfo->SubType; /** Start with ID = info sub type */
    MarshallStaticData.IsRegistered = 1;

	return STATUS_RET_SUCCESS;
}

/**
* @name 	Marshall_SendPacket
* @brief 	Send a packet
*
* @param [in]	pData		The packet data
* @param [in]	DataSize	The packet data size
* @param [in]	FuncCode	The packet function code
* @param [in]	IsBlocking	If it need to block the sender until the Tx is done (ACK,Timeout...)
*
* @retval 					The function status
*/
E_Status_Ret Marshall_SendPacket(uint8_t 			   		*pData,
									uint16_t 				DataSize,
									E_MarshallApi_FuncCode 	FuncCode,
									uint8_t					IsBlocking)
{

	if (FuncCode > MARSHALLAPI_CODE_MAX || (DataSize && pData == NULL))
	    return STATUS_RET_ERROR_PARAM_IN;

	if (!MarshallStaticData.IsPaired)
	    return STATUS_RET_COM_ERROR;

	return Marshall_SendAndBlock(pData, DataSize, FuncCode, IsBlocking, MARSHALL_NO_EVENT);
}


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
E_Status_Ret Marshall_GetAppMsg(S_Marshall_AppMsg	**pAppMsg, uint32_t WaitTime)
{
//    if (TRUE == Hal_GetThreadQueueMsg(MarshallStaticData.AppThreadID, (void **)pAppMsg, WaitTime, MARSHALL_APP_QUEUE_MSG_TYPE))
        return STATUS_RET_SUCCESS;

    return STATUS_RET_NO_PACKET_AVAILABLE;
}

/**
* @name 	Marshall_StartPairingProcess
* @brief 	Start
*
* @retval 	The function status
*/
E_Status_Ret Marshall_StartPairingProcess(void)
{
	S_MarshallApi_FirmInfoParams	*pData;
	S_Marshall_AppMsg				*pAppMsg = NULL;
	E_Status_Ret					 Ret = STATUS_RET_SUCCESS;
	uint32_t						 RetryNum = 0;

	/** Clear the application message and Tx packets queues */
	do
	{
		if (Ret != STATUS_RET_SUCCESS)
		{
			Ret = STATUS_RET_SUCCESS;
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_ERROR, "error - Marshall_StartPairingProcess() - ClrQueues");
		}

        /** clear the application message queue */        
//        while (TRUE == Hal_GetThreadQueueMsg(MarshallStaticData.AppThreadID, (void **)&pAppMsg, 0, MARSHALL_APP_QUEUE_MSG_TYPE))
//        {
//            if (pAppMsg->DataSize)
//                free(pAppMsg->pData);
//            free(pAppMsg);
//        }

		/** Send an event to the marshall task to clear the Tx queue */
		Ret = Marshall_SendAndBlock(NULL, 0, MARSHALLAPI_CODE_MAX, 1, MARSHALL_TX_EVENT_RST_Q);

	} while (Ret != STATUS_RET_SUCCESS);

	pData = (S_MarshallApi_FirmInfoParams *)MarshallHal_Malloc(sizeof(S_MarshallApi_FirmInfoParams));
	if (pData == NULL)
	    return STATUS_RET_BUFF_ALLOCATION_ERROR;

	/** set the data */
	pData->ProtocolVerMajor = MARSHALLAPI_MAJOR_VERSION;
	pData->ProtocolVerMinor = MARSHALLAPI_MINOR_VERSION;
	memcpy(&pData->PeripheralType, &MarshallStaticData.PeripheralInfo, sizeof(S_Marshall_PeripheralInfo));

	/** Send firmware info packet */
	Ret = Marshall_SendAndBlock((uint8_t *)pData, sizeof(S_MarshallApi_FirmInfoParams), MARSHALLAPI_CODE_FW_INFO, 0, MARSHALL_NO_EVENT);
	free(pData);

	return Ret;
}


//        /************************************************/
//        /*		    M A R S H A L L		T A S K		    */
//        /************************************************/

/**
* @name 	Marshall_Task
* @brief 	Handle the Tx packet and COM RX
* 			Handle the peripherals keep alive indication
* @param [in]	pParameters		The value passed into the task.
*/
static void*  Marshall_Task(void *pParameters)
{
    Hal_Clock		TimeStamp;
    uint32_t		DiffMilliSec;

    /** Handle the keep alive */
    for (;;)
    {
        /** Handle the COM channels (the COM channels state machine)*/
        Marshall_ComHdlr();

        /** Handle the keep alive (will send a notification to the application) */
        if (MarshallStaticData.IsPaired)
        {
            TimeStamp = MarshallHal_Clock();
            DiffMilliSec = (uint32_t)MarshallHal_GetDiffTimeInMillSec(MarshallStaticData.LastActivityTimeStamp, TimeStamp);
            if (DiffMilliSec >= MarshallStaticData.Cfg.KeepAlivePollInterval)
            {
                MarshallStaticData.LastActivityTimeStamp = TimeStamp;
                /** Notify the application to send keep alive */
                Marshall_SendMsgToApp(NULL, MARSHALL_INTERNAL_EVENT, MARSHALL_KEEP_ALIVE_TICK);
            }
        }

        /** Suspend the thread */
        Hal_SuspendThread(MARSHALL_TASK_WAIT_DLY);
    }
}


/************************************************/
/*						O S						*/
/************************************************/

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
//void * Hal_CreateThread(HAL_THREAD Thread, void *pParameter, uint32_t * pThreadID)
int  Hal_CreateThread(pthread_t pThreadID)
{
    //return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, pParameter, 0, (LPDWORD)pThreadID);
    //pthread_create(pThreadID, pParameter, Thread, nullptr);
    return  pthread_create(&pThreadID, nullptr, &Marshall_Task, nullptr);
}

/**
* @name 	Hal_CloseThread
* @brief 	Close the Thread
*
* @param [in]	pThreadHandle	 Handle to the Thread object
*/
void Hal_CloseThread(int pThreadHandle)
{
    pthread_exit((void*) pThreadHandle);
}

/**
* @name 	Hal_SuspendThread
* @brief 	Suspends the execution of the current thread
*
* @param [in]	Milliseconds	 The time interval for which execution is to be suspended, in milliseconds.
*/
void Hal_SuspendThread(uint32_t Milliseconds)
{
    sleep(Milliseconds);
}


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
int Hal_CreateThreadQueue(uint32_t ThreadID, uint32_t QueueLength, uint32_t ItemSize)
{
    return TRUE;
}

/**
* @name 	Hal_DestroyThreadQueue
* @brief 	Destroy thread queue
*
* @param [in]	ThreadID		The thread identifier.
*
*/
void Hal_DestroyThreadQueue(uint32_t ThreadID)
{

}

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
int Hal_SendMsgToThreadQueue(uint32_t ThreadID, void *pParam, uint32_t WaitMs, uint32_t MsgType)
{
    while (1)
    {
//        if (PostThreadMessage(ThreadID, MsgType, 0, *pParam))
            return TRUE;

        if (!WaitMs)
            break;

        WaitMs--;
        sleep(1);
    }
    return FALSE;
}

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
int Hal_GetThreadQueueMsg(uint32_t ThreadID, void **pParam, uint32_t WaitMs, uint32_t MsgType)
{
    string     QueueMsg;

    while (1)
    {
        //if (PeekMessage(&QueueMsg, NULL, MsgType, MsgType, ))
        {
            //*pParam = (void *)QueueMsg.lParam;
            return TRUE;
        }
        if (!WaitMs)
            break;

        WaitMs--;
        sleep(1);
    }

    return FALSE;
}

/**
* @name 	Hal_GetCurrentThreadID
* @brief 	Get the current thread ID
*
* @retval	current thread ID.
*/
uint32_t Hal_GetCurrentThreadID(void)
{
    return (uint32_t)getpid();
}
