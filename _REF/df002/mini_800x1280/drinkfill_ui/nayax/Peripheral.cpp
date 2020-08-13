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
*  Description  : This module is a Marshall peripheral example
*
*/

#include "Peripheral.h"

				/************************************************/
				/*				  D E F I N E S					*/
				/************************************************/

#define PERIPHERAL_MAIN_TICK_MS												1
#define PERIPHERAL_NEXT_SESSION_DELAY										(PERIPHERAL_MAIN_TICK_MS * 1000)
#define PERIPHERAL_WAIT_FOR_READER_TO_DISPLAY_VENDING_STATUS_MSG_DELAY		(PERIPHERAL_MAIN_TICK_MS * 2000)
#define PERIPHERAL_WAIT_FOR_VEND											(PERIPHERAL_MAIN_TICK_MS * 10000)
#define PERIPHERAL_WAIT_FOR_BEGIN_SESSION_TIMEOUT							(PERIPHERAL_MAIN_TICK_MS * 30000)
#define PERIPHERAL_WAIT_FOR_CANCELLING_SESSION_TIMEOUT                      (PERIPHERAL_MAIN_TICK_MS * 10000)
#define PERIPHERAL_CLOSE_SESSION_MSG_DELAY								    (PERIPHERAL_MAIN_TICK_MS * 5000)
#define PERIPHERAL_WAIT_FOR_APPROVING_VEND								    (PERIPHERAL_MAIN_TICK_MS * 5000)
#define PERIPHERAL_WAIT_FOR_END_SESSION								        (PERIPHERAL_MAIN_TICK_MS * 90000)

#define PERIPHERAL_VERSION_STR                                              "01_01"

#define PERIPHERAL_TARGET_RESOLUTION                                        1         // 1-millisecond target resolution


				/************************************************/
				/*				    E N U M S					*/
				/************************************************/
typedef enum E_Pripheral_State_tag
{
    PERIPHERAL_STATE_WAIT_FOR_PAIRING,
    PERIPHERAL_STATE_ENABLE_READER,
    PERIPHERAL_STATE_WAIT_FOR_STOP_MSG,
    PERIPHERAL_STATE_WAIT_FOR_BEGIN_SESSION,
    PERIPHERAL_STATE_WAIT_FOR_VEND_APPROVE,
    PERIPHERAL_STATE_WAIT_FOR_MACHINE_TO_APPROVE,
    PERIPHERAL_STATE_WAIT_FOR_VEND,
    PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE,
    PERIPHERAL_STATE_WAIT_FOR_END_SESSION,
    PERIPHERAL_STATE_WAIT_FOR_NEXT_SESSION,

    PERIPHERAL_STATE_MAX,
}E_Pripheral_State;

typedef enum E_Pripheral_ModemTestState_tag
{
    PERIPHERAL_MODEM_TEST_IDLE_STATE,
    PERIPHERAL_MODEM_TEST_OPEN_SOCKET_STATE,    
    PERIPHERAL_MODEM_TEST_SEND_DATA_STATE,
    PERIPHERAL_MODEM_TEST_WAIT_MODEM_RESPONSE,

    PERIPHERAL_MODEM_TEST_STATE_MAX,
}E_Pripheral_ModemTestState;


typedef enum E_Peripheral_RcvMsg_tag
{
	PERIPHERAL_RCVMSG_RESET,
	PERIPHERAL_RCVMSG_CONFIG,
	PERIPHERAL_RCVMSG_BEGIN_SESSION,
	PERIPHERAL_RCVMSG_SESSION_CANCELED,
	PERIPHERAL_RCVMSG_VEND_APPROVE,
	PERIPHERAL_RCVMSG_VEND_DENIED,
	PERIPHERAL_RCVMSG_END_SESSION,
	PERIPHERAL_RCVMSG_EVENT_COM_ERROR,
	PERIPHERAL_RCVMSG_EVENT_KEEP_ALIVE_TICK,
    PERIPHERAL_RCVMSG_EVENT_SETTLEMENT_STATUS,
    PERIPHERAL_RCVMSG_EVENT_CARD_TYPE,
    PERIPHERAL_RCVMSG_MODEM_STATUS,
    PERIPHERAL_RCVMSG_MODEM_GET_DATA,

	PERIPHERAL_RCVMSG_NONE,
}E_Peripheral_RcvMsg;


                /************************************************/
                /*				   S T R U C T S 			    */
                /************************************************/
typedef struct S_Peripheral_TaskInput_tag
{
    uint8_t			ComId;			    /** COM ID	                */
    uint8_t			PeripheralType;		/**	Peripheral Type		    */
    uint8_t		    PeripheralSubType;	/** Peripheral Sub Type	    */

}S_Peripheral_TaskInput;


				/************************************************/
				/*			  S T A T I C    D A T A			*/
				/************************************************/
U_MarshallApi_Functions		    *pRcvFunction;
int                            pPeripherlaThread;
S_Marshall_PeripheralInfo	    PeripheralInfo;
uint8_t							SendData[MARSHALLAPI_MAX_PACKET_LENGTH - 11];
wchar_t                         UserWindowStr[20];
U_MarshallApi_Functions			*pSendFunction = (U_MarshallApi_Functions *)SendData;
S_Peripheral_TaskInput          TaskInput;
const uint16_t                  ItemPriceTable[] = { 20, 30, 40, 50 };
uint32_t                        PeripheralTxReference = 0;
uint32_t                        ExternalCmd;
uint32_t                        ExternalCmdData;
uint32_t                        NeedToApproveFlag = 0;
uint32_t                        UserCanceledTheSession;
uint32_t                        BeginSessionTimeout;
uint32_t                        CurrentTick = 0;
uint32_t                        StateStartTick = 0;
uint16_t                        ItemNumber = 0;
uint16_t                        ItemPrice = 0;
E_Peripheral_SessionType        SessionType = PERIPHERAL_SESSION_TYPE_VENDING;
E_Pripheral_State				State = PERIPHERAL_STATE_WAIT_FOR_PAIRING;
uint8_t                         DecimalPlace = 2;
E_Peripheral_RcvMsg				RcvMsg;

uint32_t                        ModemTestBusy = 0;
uint32_t						ModemTestSendDataCtr = 0;
E_Pripheral_ModemTestState		ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
uint32_t                        ModemTestCurrentTick = 0;
uint32_t                        ModemTestStateStartTick = 0;
uint16_t                        SendCrc;
uint16_t                        LastSandCrc;
uint16_t                        RcvCrc;
uint16_t                        PortNumber = 0;
char                            IpAddr[40];

const char                     *ModemTestData[10] = {   "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                                                        "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
                                                        "222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222",
                                                        "333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333",
                                                        "444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444",
                                                        "555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555",
                                                        "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666",
                                                        "777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777",
                                                        "888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888",
                                                        "999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999" };


                /********************************************************/
                /*	  V E N D I N G    S T A T E    M A C H I N E		*/
                /********************************************************/
static void Peripheral_WaitForPairingState(void);
static void Peripheral_EnableReaderState(void);
static void Peripheral_WaitForStopState(void);
static void Peripheral_WaitForBeginSession(void);
static void Peripheral_WaitForVendApprove(void);
static void Peripheral_WaitForMachineToApprove(void);
static void Peripheral_WaitForVend(void);
static void Peripheral_WaitToSendSessionComplete(void);
static void Peripheral_WaitForEndSession(void);
static void Peripheral_WaitForNextSession(void);

typedef void(*Peripheral_StateFunc) (void);
Peripheral_StateFunc PeripheralStateFunc[PERIPHERAL_STATE_MAX] = { Peripheral_WaitForPairingState,
                                                                    Peripheral_EnableReaderState,
                                                                    Peripheral_WaitForStopState,
                                                                    Peripheral_WaitForBeginSession,
                                                                    Peripheral_WaitForVendApprove,
                                                                    Peripheral_WaitForMachineToApprove,
                                                                    Peripheral_WaitForVend,
                                                                    Peripheral_WaitToSendSessionComplete,
                                                                    Peripheral_WaitForEndSession,
                                                                    Peripheral_WaitForNextSession };


                /**********************************************************/
                /*	 M O D E M 	 T E S T    S T A T E    M A C H I N E	  */
                /**********************************************************/
static void Peripheral_ModemTestIdleState(void);
static void Peripheral_ModemTestOpenSocketState(void);
static void Peripheral_ModemTestSendDataState(void);
static void Peripheral_ModemTestWaitForModemResponseState(void);

Peripheral_StateFunc PeripheralModemStateFunc[PERIPHERAL_MODEM_TEST_STATE_MAX] = { Peripheral_ModemTestIdleState,
                                                                                    Peripheral_ModemTestOpenSocketState,
                                                                                    Peripheral_ModemTestSendDataState,
                                                                                    Peripheral_ModemTestWaitForModemResponseState };


                /************************************************/
                /*			  F U N C T I O N S			        */
                /************************************************/

static void * Peripheral_Task(void *pParams);

/**
* @name 	Peripheral_InitlInfo
* @brief 	Init the peripheral general info - serial,version...
*/
static void Peripheral_InitlInfo(void)
{
    /* initialize random seed: */
    srand((uint32_t)time(NULL));

    snprintf((char *)PeripheralInfo.Model, 20, "Model_%d", rand());
    snprintf((char *)PeripheralInfo.SerialNum, 20, "Serial_%d", rand());
    snprintf((char *)PeripheralInfo.SwVer, 20, "Simulator_Ver_%s", PERIPHERAL_VERSION_STR);
}

/**
* @name 	Peripheral_InMsgParser
* @brief 	Translate the marshall command to E_Peripheral_RcvMsg
* @param [in]	pAppMsg		The message to translate.
*
* @retval 	The message enum (E_Peripheral_RcvMsg)
*/
static E_Peripheral_RcvMsg Peripheral_InMsgParser(S_Marshall_AppMsg *pAppMsg)
{
    E_Peripheral_RcvMsg			RcvMsg = PERIPHERAL_RCVMSG_NONE;


    if (pAppMsg->MsgType == MARSHALL_EXTERNAL_MSG)
    {
        cout<<pAppMsg->MsgType<<endl;

        pRcvFunction = (U_MarshallApi_Functions *)&pAppMsg->pData[1];
        switch (pAppMsg->pData[0])
        {
        case MARSHALLAPI_CODE_RESET:
            RcvMsg = PERIPHERAL_RCVMSG_RESET;
            break;

        case MARSHALLAPI_CODE_CONFIG:
            RcvMsg = PERIPHERAL_RCVMSG_CONFIG;
            break;

        case MARSHALLAPI_CODE_TRANSFER_DATA:
            if (pRcvFunction->TransferData.Tag == MARSHALLAPI_TRANSFER_DATA_CARD_TYPE)
                RcvMsg = PERIPHERAL_RCVMSG_EVENT_CARD_TYPE;
            break;

        case MARSHALLAPI_CODE_STATUS:
            if (pRcvFunction->Status.Type == MARSHALLAPI_STATUS_TYPE_CLOSE_SESSION_STATUS)
                RcvMsg = PERIPHERAL_RCVMSG_EVENT_SETTLEMENT_STATUS;
            break;

        case MARSHALLAPI_CODE_MDB_CMD:
            switch (pRcvFunction->MdbCashlessDevCmd.MdbCashlessDevCmdHeader.Cmd)
            {
            case MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_BEGIN_SESSION:
                RcvMsg = PERIPHERAL_RCVMSG_BEGIN_SESSION;
                break;
            case MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_CANCEL_REQ:
                RcvMsg = PERIPHERAL_RCVMSG_SESSION_CANCELED;
                break;
            case MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_VEND_APPROVE:
                RcvMsg = PERIPHERAL_RCVMSG_VEND_APPROVE;
                break;
            case MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_VEND_DENIED:
                RcvMsg = PERIPHERAL_RCVMSG_VEND_DENIED;
                break;
            case MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_END_SESSION:
                RcvMsg = PERIPHERAL_RCVMSG_END_SESSION;
                break;
            default:
                break;
            }
            break;

        case MARSHALLAPI_CODE_MODEM_STATUS:
            RcvMsg = PERIPHERAL_RCVMSG_MODEM_STATUS;
            break;

        case MARSHALLAPI_CODE_RCV_DATA:
            RcvMsg = PERIPHERAL_RCVMSG_MODEM_GET_DATA;
            break;

        default:
            break;
        }
    }
    else
    {
        if (pAppMsg->Event == MARSHALL_NON_CONSECUTIVE_PACKET_ID_EVENT
            || pAppMsg->Event == MARSHALL_RCV_NACK)
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION | HAL_TRACE_SRC_ERROR, "Error - _tmain() event - %d", pAppMsg->Event);
            RcvMsg = PERIPHERAL_RCVMSG_EVENT_COM_ERROR;
        }
        else if (pAppMsg->Event == MARSHALL_KEEP_ALIVE_TICK)
        {
            RcvMsg = PERIPHERAL_RCVMSG_EVENT_KEEP_ALIVE_TICK;
        }
    }

    return RcvMsg;
}

/**
* @name 	Peripheral_DisableUser
* @brief 	Disable user GUI buttens
*/
static void Peripheral_DisableUser(void)
{

}


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
pthread_t Peripheral_Init(uint8_t ComId, uint8_t PeripheralType, uint8_t PeripheralSubType, uint16_t PerCapabilities)
{
    pthread_t ThreadID;
//    TIMECAPS TimeCaps;
    uint     TimerRes;

    /** Get target resolution */
//    if (timeGetDevCaps(&TimeCaps, sizeof(TIMECAPS)) != TIMERR_NOERROR)
//    {
//        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_GENERAL, "Err - Task timer");
//        return NULL;
//    }
    /** Init the min timer period */
//    TimerRes = min(max(TimeCaps.wPeriodMin, PERIPHERAL_TARGET_RESOLUTION), TimeCaps.wPeriodMax);
//    timeBeginPeriod(TimerRes);

    PeripheralInfo.Capabilities = PerCapabilities;

    TaskInput.ComId = ComId;
    TaskInput.PeripheralType = PeripheralType;
    TaskInput.PeripheralSubType = PeripheralSubType;

    //CreateThread(NULL, 0, &Peripheral_Task, PrintCB, 0, &ThreadID);
    pPeripherlaThread = pthread_create(&ThreadID, nullptr, &Peripheral_Task, nullptr);

    return ThreadID;
}

/**
* @name 	Peripheral_Close
* @brief 	Close the peripherals and marhsall handels
*/
void Peripheral_Close(void)
{
    Marshall_Close();

//    CloseHandle(pPeripherlaThread);
    pthread_exit((void*) pPeripherlaThread);
}

/**
* @name 	Peripheral_UpdateSessionType
* @brief 	Update the session type
* @param [in]	SessionType
*/
void Peripheral_UpdateSessionType(E_Peripheral_SessionType Type)
{
    SessionType = Type;
}


/**
* @name 	Peripheral_UpdatePortNubmer
* @brief 	Update the port number
* @param [in]	port numer
*/
void Peripheral_UpdatePortNubmer(uint16_t Port)
{
    PortNumber = Port;
}

/**
* @name 	Peripheral_UpdateIpAddr
* @brief 	Update the IP address
* @param [in]	IP address
*/
void Peripheral_UpdateIpAddr(char *Str)
{
    std::strcpy(IpAddr, Str);
}


                /********************************************************/
                /*	  V E N D I N G    S T A T E    M A C H I N E		*/
                /********************************************************/

/**
* @name 	Peripheral_WaitForPairingState
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_PAIRING
*/
static void Peripheral_WaitForPairingState(void)
{
    if (RcvMsg == PERIPHERAL_RCVMSG_CONFIG)
    {
        DecimalPlace = pRcvFunction->ConfigParams.DecimalPlace;
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Config");

        State = PERIPHERAL_STATE_ENABLE_READER;
    }
}

/**
* @name 	Peripheral_EnableReaderState
* @brief 	State machine, state - PERIPHERAL_STATE_ENABLE_READER
*/
static void Peripheral_EnableReaderState(void)
{
    /** The user choosed a product */
    if (PERIPHERAL_CHOOSE_PRODUCT_MSG == ExternalCmd)
    {
        NeedToApproveFlag = 0;
        UserCanceledTheSession = 0;
        ItemNumber = ExternalCmdData;
        ItemPrice = ItemPriceTable[ItemNumber % 4];
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Reader enable");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_READER;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_READER_ENABLE;
        Marshall_SendPacket((uint8_t*)pSendFunction, sizeof(pSendFunction->MdbCmd.CmdHeader), MARSHALLAPI_CODE_MDB_CMD, 1);

        BeginSessionTimeout = PERIPHERAL_WAIT_FOR_BEGIN_SESSION_TIMEOUT;
        State = PERIPHERAL_STATE_WAIT_FOR_BEGIN_SESSION;
    }
    /** Cash sale */
    else if (PERIPHERAL_CASH_SALE_MSG == ExternalCmd)
    {
        ItemNumber = ExternalCmdData;
        ItemPrice = ItemPriceTable[ItemNumber % 4];
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Cash Sale, Item number = 0x%X, Item price = %d", ItemNumber, ItemPrice);
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_CASH_SALE;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendCashSale.ItemNumber = ItemNumber;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendCashSale.ItemPrice = ItemPrice;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.CmdHeader) + sizeof(S_MarshallApi_MdbVendCashSale),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
    }
    /** If the peripheral had a session time-out and the Amit wasn't in idle state */
    else if (RcvMsg == PERIPHERAL_RCVMSG_BEGIN_SESSION)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Vend cancel");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_CANCEL;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.CmdHeader),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
        State = PERIPHERAL_STATE_WAIT_FOR_VEND_APPROVE;
    }
    /** The user stop the session */
    else if ((PeripheralInfo.Capabilities & MARSHALLAPI_DEV_CAPB_MULTI_SESSION_BIT_MASK)
        && PERIPHERAL_CLOSE_SESSION_MSG == ExternalCmd)
    {
        ItemNumber = ExternalCmdData;
        if (PeripheralInfo.Capabilities & MARSHALLAPI_DEV_CAPB_VEND_REQ_PRICE_IS_NOT_FINAL_BIT_MASK)
            ItemPrice = ItemPriceTable[ItemNumber % 4] - 10;
        else
            ItemPrice = ItemPriceTable[ItemNumber % 4];


        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Close session, Item number = 0x%X, Item price = %d, Quantity = %d", ItemNumber, ItemPrice, ItemPrice * 100);
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_CLOSE_SESSION;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendCloseSession.SessionStatus = MARSHALLAPI_MDB_CLOSE_SESSION_CMD_STATUS_OK;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendCloseSession.ProductID = ItemNumber;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendCloseSession.Price = ItemPrice;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendCloseSession.Quantity = ItemPrice * 100;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.CmdHeader) + sizeof(S_MarshallApi_MdbVendCloseSession),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
    }
    /** The Amit send settlement status */
    else if (RcvMsg == PERIPHERAL_RCVMSG_EVENT_SETTLEMENT_STATUS)
    {
        if (!pRcvFunction->Status.DataFirstByte)
        {
            swprintf(UserWindowStr, sizeof(UserWindowStr), L"\r\nCharge Was %4.2f\r\n", (double)ItemPrice / pow((double)10, DecimalPlace));
        }
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Settlement status - %d", pRcvFunction->Status.DataFirstByte);
        State = PERIPHERAL_STATE_WAIT_FOR_STOP_MSG;
    }
}

/**
* @name 	Peripheral_WaitForStopState
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_STOP_MSG
*/
static void Peripheral_WaitForStopState(void)
{
    /** Delay for displaing the charged amount */
    if ((CurrentTick - StateStartTick) >= PERIPHERAL_CLOSE_SESSION_MSG_DELAY)
    {

        State = PERIPHERAL_STATE_ENABLE_READER;
    }
}

/**
* @name 	Peripheral_WaitForBeginSession
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_BEGIN_SESSION
*/
static void Peripheral_WaitForBeginSession(void)
{
    if (RcvMsg == PERIPHERAL_RCVMSG_BEGIN_SESSION)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Begin session, transaction credit = %d ",
            pRcvFunction->MdbCashlessDevCmd.MdbCashlessDevCmdFunctions.MdbCashlessDevBeginSession.TransCredit);
        if (pRcvFunction->MdbCashlessDevCmd.MdbCashlessDevCmdFunctions.MdbCashlessDevBeginSession.TransCredit >= ItemPrice
            && !UserCanceledTheSession)
        {
            if (MARSHALLAPI_MDB_SESSION_TYPE_INFO == SessionType)
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Session type - info");
                pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
                pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_SESSION_TYPE;
                pSendFunction->MdbCmd.MdbFunctions.MdbVendSessionType.SessionType = MARSHALLAPI_MDB_SESSION_TYPE_INFO;
                Marshall_SendPacket((uint8_t*)pSendFunction,
                    sizeof(pSendFunction->MdbCmd.MdbFunctions.MdbVendSessionType) + sizeof(pSendFunction->MdbCmd.CmdHeader),
                    MARSHALLAPI_CODE_MDB_CMD,
                    1);

                ItemPrice = 0;
                ItemNumber = 0xffff;
            }
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Vend reqeust, Item price = %d, Item number = 0x%X", ItemPrice, ItemNumber);
            pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
            pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_REQEUST;
            pSendFunction->MdbCmd.MdbFunctions.MdbVendRequest.ItemPrice = ItemPrice;
            pSendFunction->MdbCmd.MdbFunctions.MdbVendRequest.ItemNumber = ItemNumber;
            Marshall_SendPacket((uint8_t*)pSendFunction,
                sizeof(pSendFunction->MdbCmd.MdbFunctions.MdbVendRequest) + sizeof(pSendFunction->MdbCmd.CmdHeader),
                MARSHALLAPI_CODE_MDB_CMD,
                1);
        }
        else
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Vend cancel");
            pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
            pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_CANCEL;
            Marshall_SendPacket((uint8_t*)pSendFunction,
                sizeof(pSendFunction->MdbCmd.CmdHeader),
                MARSHALLAPI_CODE_MDB_CMD,
                1);
        }
        if (PeripheralInfo.Capabilities & (MARSHALLAPI_DEV_CAPB_PROP_MIFARE_CARD_APPROVED_BY_VMC_BIT_MASK | MARSHALLAPI_DEV_CAPB_PROP_MAG_CARD_APPROVED_BY_VMC_BIT_MASK))
        {
            if (NeedToApproveFlag)
            {
                NeedToApproveFlag = 0;
                State = PERIPHERAL_STATE_WAIT_FOR_MACHINE_TO_APPROVE;
            }
            else
            {
                State = PERIPHERAL_STATE_WAIT_FOR_VEND_APPROVE;
            }sizeof(IpAddr);
        }
        else
            State = PERIPHERAL_STATE_WAIT_FOR_VEND_APPROVE;
    }
    /** The Amit send session canceled */
    else if (RcvMsg == PERIPHERAL_RCVMSG_SESSION_CANCELED)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Session cancelled");
        State = PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE;
    }
    /** The user cancel the session */
    else if (ExternalCmd == PERIPHERAL_CANCEL_MSG)
    {


        /** Disable the Amit */
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Reader disable");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_READER;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_READER_DISABLE;
        Marshall_SendPacket((uint8_t*)pSendFunction, sizeof(pSendFunction->MdbCmd.CmdHeader), MARSHALLAPI_CODE_MDB_CMD, 1);

        /** Notice - The Amit might not be in Idla state and will not accept the disable command.
        Wait for timeout or for begin session message and disable again (if the user allready presented a card) */
        BeginSessionTimeout = PERIPHERAL_WAIT_FOR_CANCELLING_SESSION_TIMEOUT;
        StateStartTick = CurrentTick;
        UserCanceledTheSession = 1;
    }
    else if (RcvMsg == PERIPHERAL_RCVMSG_EVENT_CARD_TYPE)            
    {
        S_MarshallApi_TLV   *pTlv = (S_MarshallApi_TLV *)&pRcvFunction->TransferData;
        uint8_t             CardType;
        uint8_t             EntryMode;
        uint8_t             Uid[9];
        uint8_t             i;

        CardType = pTlv->DataFirstByte;
        pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 1);
        EntryMode = pTlv->DataFirstByte;
        pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 1);

        /** Checking the type for errors, the Amit shouldn't send card details if it's not a Mifare card (at this point of time)  */
        if (MARSHALLAPI_TRANSFER_DATA_PROPRIETARY_CARD_TYPE == CardType)
        {
            if (MARSHALLAPI_TRANSFER_DATA_MIFARE_CARD_ENTRY_TYPE == EntryMode)
            {
                Uid[8] = 0;
                for (i = 0; i < 4; i++)
                    Utils_ByteToXAscii(*((&pTlv->DataFirstByte) + i), &Uid[2 * i]);

                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Card Details, Card Type - %d,Entry Mode - %d, UID - %s", CardType, EntryMode, Uid);                
                if (PeripheralInfo.Capabilities & MARSHALLAPI_DEV_CAPB_PROP_MIFARE_CARD_APPROVED_BY_VMC_BIT_MASK)
                    NeedToApproveFlag = 1;
            }
            else if (MARSHALLAPI_TRANSFER_DATA_MSR_SWIPE_CARD_ENTRY_TYPE == EntryMode)
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Card Details, Card Type - %d, Entry Mode - %d, UID - %s", CardType, EntryMode, &pTlv->DataFirstByte);
                if (PeripheralInfo.Capabilities & MARSHALLAPI_DEV_CAPB_PROP_MAG_CARD_APPROVED_BY_VMC_BIT_MASK)
                    NeedToApproveFlag = 1;
            }            
        }
    }
    /** Time out */
    else if ((CurrentTick - StateStartTick) >= BeginSessionTimeout)
    {        
        if (!UserCanceledTheSession)
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Begin session timeout, Snd - Reader disable");
            pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_READER;
            pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_READER_DISABLE;
            Marshall_SendPacket((uint8_t*)pSendFunction, sizeof(pSendFunction->MdbCmd.CmdHeader), MARSHALLAPI_CODE_MDB_CMD, 1);
        }
        State = PERIPHERAL_STATE_WAIT_FOR_NEXT_SESSION;
    }
}

/**
* @name 	Peripheral_WaitForVendApprove
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_VEND_APPROVE
*/
static void Peripheral_WaitForVendApprove(void)
{
    if (RcvMsg == PERIPHERAL_RCVMSG_VEND_APPROVE)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Vend approve, amount - %d", pRcvFunction->MdbCashlessDevCmd.MdbCashlessDevCmdFunctions.MdbCashlessDevVendApprove.VendAmount);

        State = PERIPHERAL_STATE_WAIT_FOR_VEND;
    }
    else if (RcvMsg == PERIPHERAL_RCVMSG_VEND_DENIED)
    {

        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Vend denied");
        State = PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE;
    }
    else if (RcvMsg == PERIPHERAL_RCVMSG_EVENT_CARD_TYPE)
    {
        S_MarshallApi_TLV   *pTlv = (S_MarshallApi_TLV *)&pRcvFunction->TransferData;
        uint8_t             CardType;
        uint8_t             EntryMode;
        uint8_t            *pTxnId;
        uint8_t             ComStatus;
        uint8_t             Bin[7];
        uint8_t             BinHash[41];
        uint8_t             Uid[9];
        uint8_t             i;

        CardType = pTlv->DataFirstByte;
        pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 1);
        EntryMode = pTlv->DataFirstByte;
        pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 1);

        if (MARSHALLAPI_TRANSFER_DATA_PROPRIETARY_CARD_TYPE == CardType)
        {
            if (MARSHALLAPI_TRANSFER_DATA_MIFARE_CARD_ENTRY_TYPE == EntryMode)
            {
                Uid[8] = 0;
                for (i = 0; i < 4; i++)
                    Utils_ByteToXAscii(*((&pTlv->DataFirstByte) + i), &Uid[2 * i]);

                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Card Details, Card Type - %d, Entry Mode - %d, UID - %s", CardType, EntryMode, Uid);
            }
            else
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Card Details, Card Type - %d, Entry Mode - %d, UID - %s", CardType, EntryMode, &pTlv->DataFirstByte);
            }
            pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + pTlv->Length);
        }
        else
        {
            memcpy(Bin, &pTlv->DataFirstByte, 6);
            Bin[6] = 0;
            pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 6);
            for (i = 0; i < 20; i++)
                Utils_ByteToXAscii(*((&pTlv->DataFirstByte) + i), &BinHash[2 * i]);
            BinHash[40] = 0;
            pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 20);

            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION,
                "Rcv - Card Details, Card Type - %d, Entry Mode - %d, BIN - %s,\nBIN Hash - %s", CardType, EntryMode, Bin, BinHash);
        }

        pTxnId = (uint8_t *)&pTlv->DataFirstByte;
        pTlv = (S_MarshallApi_TLV *)((uint8_t *)&pTlv->DataFirstByte + 8);
        ComStatus = pTlv->DataFirstByte;
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION,
            "Rcv Txn ID - %s, Com Status - %d", pTxnId, ComStatus);

        if (MARSHALLAPI_MDB_SESSION_TYPE_INFO != SessionType)
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Peripheral transaction reference - %d", PeripheralTxReference);
            pSendFunction->TransferData.Tag = MARSHALLAPI_TRANSFER_DATA_TAG_TXN_ID;
            memset(&pSendFunction->TransferData.DataFirstByte, 0, 20);
            pSendFunction->TransferData.Length = 20;
            *(uint32_t*)&pSendFunction->TransferData.DataFirstByte = PeripheralTxReference++;
            Marshall_SendPacket((uint8_t*)pSendFunction,
                sizeof(S_MarshallApi_TLV) + 19,
                MARSHALLAPI_CODE_TRANSFER_DATA,
                1);
        }
    }
    else if (ExternalCmd == PERIPHERAL_CANCEL_MSG)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Vend cancel");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_CANCEL;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.CmdHeader),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
    }
}

/**
* @name 	Peripheral_WaitForMachineToApprove
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_MACHINE_TO_APPROVE
*/
static void Peripheral_WaitForMachineToApprove(void)
{
    if (RcvMsg == PERIPHERAL_RCVMSG_VEND_DENIED)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Vend denied");
        State = PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE;
    }
    else if ((CurrentTick - StateStartTick) >= PERIPHERAL_WAIT_FOR_APPROVING_VEND)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - auth status - ok");
        pSendFunction->TransferData.Tag = MARSHALLAPI_TRANSFER_DATA_VMC_AUTH_STATUS;
        pSendFunction->TransferData.Length = 1;
        pSendFunction->TransferData.DataFirstByte = 0;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(S_MarshallApi_TLV),
            MARSHALLAPI_CODE_TRANSFER_DATA,
            1);
        State = PERIPHERAL_STATE_WAIT_FOR_VEND_APPROVE;
    }
}

/**
* @name 	Peripheral_WaitForVend
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_VEND
*/
static void Peripheral_WaitForVend(void)
{
    /** Time out / the user canceled */
    if (ExternalCmd == PERIPHERAL_CANCEL_MSG
        || (CurrentTick - StateStartTick) >= PERIPHERAL_WAIT_FOR_VEND)
    {

        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Vend cancel");
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Vend failure");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_FAILURE;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.CmdHeader),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
        State = PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE;
    }
    /** The user start charging */
    else if (ExternalCmd == PERIPHERAL_TAKE_PRODUCT_MSG)
    {

        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Vend success, Item - 0x%X", ItemNumber);
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_SUCCESS;
        pSendFunction->MdbCmd.MdbFunctions.MdbVendSuccess.ItemNumber = ItemNumber;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.MdbFunctions.MdbVendSuccess) + sizeof(pSendFunction->MdbCmd.CmdHeader),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
        State = PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE;
    }
}

/**
* @name 	Peripheral_WaitToSendSessionComplete
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_TO_SEND_SESSION_COMPLETE
*/
static void Peripheral_WaitToSendSessionComplete(void)
{
    /** A delay for the reader to display the status messages */
    if ((CurrentTick - StateStartTick) >= PERIPHERAL_WAIT_FOR_READER_TO_DISPLAY_VENDING_STATUS_MSG_DELAY)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Session complete");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_VEND;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_VEND_SESSION_COMPLETE;
        Marshall_SendPacket((uint8_t*)pSendFunction,
            sizeof(pSendFunction->MdbCmd.CmdHeader),
            MARSHALLAPI_CODE_MDB_CMD,
            1);
        State = PERIPHERAL_STATE_WAIT_FOR_END_SESSION;
    }
}

/**
* @name 	Peripheral_WaitForEndSession
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_END_SESSION
*/
static void Peripheral_WaitForEndSession(void)
{
    /** The Amit send end-session */
    if (RcvMsg == PERIPHERAL_RCVMSG_END_SESSION
        || ((CurrentTick - StateStartTick) >= PERIPHERAL_WAIT_FOR_END_SESSION))
    {
        if (RcvMsg == PERIPHERAL_RCVMSG_END_SESSION)
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - End session");
        else
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION | HAL_TRACE_SRC_ERROR, "Didn't Rcv - End session");

        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Reader disable");
        pSendFunction->MdbCmd.CmdHeader.Cmd = MARSHALLAPI_MDB_CMD_CODE_READER;
        pSendFunction->MdbCmd.CmdHeader.SubCmd = MARSHALLAPI_MDB_CMD_CODE_READER_DISABLE;
        Marshall_SendPacket((uint8_t*)pSendFunction, sizeof(pSendFunction->MdbCmd.CmdHeader), MARSHALLAPI_CODE_MDB_CMD, 1);
        State = PERIPHERAL_STATE_WAIT_FOR_NEXT_SESSION;
    }
}

/**
* @name 	Peripheral_WaitForNextSession
* @brief 	State machine, state - PERIPHERAL_STATE_WAIT_FOR_NEXT_SESSION
*/
static void Peripheral_WaitForNextSession(void)
{
    /** Timeout for next session */
    if ((CurrentTick - StateStartTick) >= PERIPHERAL_NEXT_SESSION_DELAY)
    {

        State = PERIPHERAL_STATE_ENABLE_READER;
    }
}

            /**********************************************************/
            /*	 M O D E M 	 T E S T    S T A T E    M A C H I N E	  */
            /**********************************************************/

/**
* @name 	Peripheral_ModemTestIdleState
* @brief 	Modem test state machine, state - PERIPHERAL_MODEM_TEST_IDLE_STATE
*/
static void Peripheral_ModemTestIdleState(void)
{
    if (PERIPHERAL_TEST_MODEM_MSG == ExternalCmd)
    {
        SendCrc = UTILS_POLYNOMIAL_CCITT;
        RcvCrc = UTILS_POLYNOMIAL_CCITT;
        ModemTestSendDataCtr = 0;
        ModemTestBusy = 0;
        ModemTestState = PERIPHERAL_MODEM_TEST_OPEN_SOCKET_STATE;
    }        
}

/**
* @name 	Peripheral_ModemTestOpenSocketState
* @brief 	Modem test state machine, state - PERIPHERAL_MODEM_TEST_OPEN_SOCKET_STATE
*/
static void Peripheral_ModemTestOpenSocketState(void)
{    
    pSendFunction->OpenSocket.ServerDestination = MARSHALLAPI_OPEN_SOCKET_DEST_TRD_PARTIY_SERVER;
    pSendFunction->OpenSocket.Protocol = MARSHALLAPI_OPEN_SOCKET_PROTOCOL_TCP;
    pSendFunction->OpenSocket.Port = PortNumber;
    MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - Open socket, IP - %s, Port - %d", &pSendFunction->OpenSocket.ServerAddrFirstByte, pSendFunction->OpenSocket.Port);
    Marshall_SendPacket((uint8_t*)pSendFunction, 17, MARSHALLAPI_CODE_OPEN_SOCKET, 1);
    ModemTestState = PERIPHERAL_MODEM_TEST_WAIT_MODEM_RESPONSE;
}

/**
* @name 	Peripheral_ModemTestSendDataState
* @brief 	Modem test state machine, state - PERIPHERAL_MODEM_TEST_SEND_DATA_STATE
*/
static void Peripheral_ModemTestSendDataState(void)
{
    if (PERIPHERAL_TEST_MODEM_MSG == ExternalCmd)
    {
        Marshall_SendPacket(NULL, 0, MARSHALLAPI_CODE_CLOSE_SOCKET, 1);
        ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
        return;
    }
    else if (ModemTestBusy)
    {
        if ((ModemTestCurrentTick - ModemTestStateStartTick) >= (ModemTestBusy * 1000))
            ModemTestBusy = 0;
    }
    else if ((ModemTestCurrentTick - ModemTestStateStartTick) >= (PERIPHERAL_MAIN_TICK_MS * 100))
    {
        ModemTestSendDataCtr++;
        if (ModemTestSendDataCtr > 100)
        {
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Snd - close socket");
            Marshall_SendPacket(NULL, 0, MARSHALLAPI_CODE_CLOSE_SOCKET, 1);
            ModemTestState = PERIPHERAL_MODEM_TEST_WAIT_MODEM_RESPONSE;
            return;
        }
        pSendFunction->SendData.DataLength = 400;

        LastSandCrc = SendCrc;
        SendCrc = Utils_Calc_CRC_CCITT(&pSendFunction->SendData.FirstByteOfData, 400, SendCrc);
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Send - Buffer size - %d data - %s", pSendFunction->SendData.DataLength, &pSendFunction->SendData.FirstByteOfData);
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Send CRC - %X", SendCrc);
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Send packet - %d", ModemTestSendDataCtr);

        Marshall_SendPacket((uint8_t*)pSendFunction, 402, MARSHALLAPI_CODE_SEND_DATA, 1);
        ModemTestState = PERIPHERAL_MODEM_TEST_WAIT_MODEM_RESPONSE;
    }
}

/**
* @name 	Peripheral_ModemTestWaitForModemResponseState
* @brief 	Modem test state machine, state - PERIPHERAL_MODEM_TEST_WAIT_FOR_MODEM_RESPONSE
*/
static void Peripheral_ModemTestWaitForModemResponseState(void)
{
    if (RcvMsg == PERIPHERAL_RCVMSG_MODEM_STATUS)
    {
        switch (pRcvFunction->ModemStatus.Cmd)
        {
        case MARSHALLAPI_CODE_OPEN_SOCKET:
            if (pRcvFunction->ModemStatus.ModemRetVal == MARSHALLAPI_MODEM_RET_VAL_OK)
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Socket open - ok");
                ModemTestState = PERIPHERAL_MODEM_TEST_SEND_DATA_STATE;
            }
            else
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Socket open - error, ret val - %d, extra data - %d",
                    pRcvFunction->ModemStatus.ModemRetVal, pRcvFunction->ModemStatus.ModemExtraData);
                ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
            }
            break;
        case MARSHALLAPI_CODE_CLOSE_SOCKET:
            if (pRcvFunction->ModemStatus.ModemRetVal == MARSHALLAPI_MODEM_RET_VAL_OK)
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Socket close - ok");
            else
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Socket close - error, ret val - %d, extra data - %d",
                pRcvFunction->ModemStatus.ModemRetVal, pRcvFunction->ModemStatus.ModemExtraData);
            ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
            break;
        case MARSHALLAPI_CODE_SEND_DATA:
            switch (pRcvFunction->ModemStatus.ModemRetVal)
            {
            case MARSHALLAPI_MODEM_RET_VAL_OK:
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Transmission state OK");
                pSendFunction->ReqData.DataLength = 400;
                pSendFunction->ReqData.TimeOutSec = 30;
                Marshall_SendPacket((uint8_t*)pSendFunction, sizeof(S_MarshallApi_ReqData), MARSHALLAPI_CODE_REQ_DATA, 1);
                break;
            case MARSHALLAPI_MODEM_RET_VAL_BUSY:
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Transmission state BUSY timeout - %d", pRcvFunction->ModemStatus.ModemExtraData);
                ModemTestBusy = pRcvFunction->ModemStatus.ModemExtraData;
                ModemTestState = PERIPHERAL_MODEM_TEST_SEND_DATA_STATE;
                break;
            default:
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Transmission status - %d extra data - %d",
                    pRcvFunction->ModemStatus.ModemRetVal, pRcvFunction->ModemStatus.ModemExtraData);
                ModemTestSendDataCtr = 0;
                Marshall_SendPacket(NULL, 0, MARSHALLAPI_CODE_CLOSE_SOCKET, 1);
                ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
                break;
            }
            if (pRcvFunction->ModemStatus.ModemRetVal != MARSHALLAPI_MODEM_RET_VAL_OK)
            {
                SendCrc = LastSandCrc;
                ModemTestSendDataCtr--;
            }
            break;
        default:
            if (pRcvFunction->ModemStatus.ModemRetVal == MARSHALLAPI_MODEM_RET_VAL_SOCKET_CLOSE)
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Socket state close");
                ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
            }
            break;
        }

    }
    else if (RcvMsg == PERIPHERAL_RCVMSG_MODEM_GET_DATA)
    {
        if (pRcvFunction->RcvData.DataLength)
        {
            (&pRcvFunction->RcvData.FirstByteOfData)[pRcvFunction->RcvData.DataLength] = '\0';
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Buffer size - %d data - %s", pRcvFunction->RcvData.DataLength, &pRcvFunction->RcvData.FirstByteOfData);
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Buffer size - %d", pRcvFunction->RcvData.DataLength);
            RcvCrc = Utils_Calc_CRC_CCITT(&pRcvFunction->RcvData.FirstByteOfData, pRcvFunction->RcvData.DataLength, RcvCrc);
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv CRC - %X", RcvCrc);
        }
        else
        {
            SendCrc = LastSandCrc;
            MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Rcv - Buffer size - 0");
        }
        ModemTestState = PERIPHERAL_MODEM_TEST_SEND_DATA_STATE;
    }
    else if ((ModemTestCurrentTick - ModemTestStateStartTick) >= (PERIPHERAL_MAIN_TICK_MS * 120000))
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION, "Modem timeout");
        ModemTestState = PERIPHERAL_MODEM_TEST_IDLE_STATE;
    }
}

/**
* @name 	Peripheral_Task
* @brief 	The peripheral task, it handle the transaction session
* @param [in]	pParams		A call back function for the MarshallHal_TracePrint()
*                           After the MarshallHal_TracePrint() assemble the message it uses the call back to pass the message forword  
*/
static void * Peripheral_Task(void *pParams)
{
    E_Status_Ret					Ret;    
    S_Marshall_AppMsg			    *pAppMsg = NULL;
    E_Pripheral_State				LastState;
    string                             Msg;

    E_Pripheral_ModemTestState		ModemTestLastState;

    /** Get the input arguments */
    PeripheralInfo.Type = TaskInput.PeripheralType;
    PeripheralInfo.SubType = TaskInput.PeripheralSubType;

    Peripheral_InitlInfo();

    /** Init and registration */
   Marshall_Init((HalTracePrintCB)pParams);
    Ret = Marshall_Registration(&PeripheralInfo, Hal_GetCurrentThreadID(), TaskInput.ComId);
   // cout<<Ret<<endl;
    if (Ret != STATUS_RET_SUCCESS)
    {
        MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION | HAL_TRACE_SRC_ERROR,"Error - peripheral - Marshall_Registration");
        Peripheral_Close();
        //return 0;
    }

    for (;;)
    {
        ExternalCmd = 0;
        ExternalCmdData = 0;
        Ret = STATUS_RET_NO_PACKET_AVAILABLE;

        /** Get the user message */
//        if (PeekMessage(&Msg, NULL, PERIPHERAL_CHOOSE_PRODUCT_MSG, PERIPHERAL_MAX_MSG, PM_REMOVE))
//        {
//            ExternalCmd = Msg.message;
//            ExternalCmdData = Msg.wParam;
//        }
      
        /** Wait for marshall messages */
        if (!ExternalCmd)
            Ret = Marshall_GetAppMsg(&pAppMsg, PERIPHERAL_MAIN_TICK_MS);


        CurrentTick++;
        ModemTestCurrentTick++;
        cout<<Ret<<endl;
        RcvMsg = PERIPHERAL_RCVMSG_NONE;
        cout<<RcvMsg<<endl;
        if (Ret == STATUS_RET_SUCCESS)
        {

            RcvMsg = Peripheral_InMsgParser(pAppMsg);
            cout<<RcvMsg<<endl;
            if (RcvMsg == PERIPHERAL_RCVMSG_RESET)
            {
                MarshallHal_TracePrint(0, NULL, HAL_TRACE_SRC_APPLICATION,"Rcv - Reset");
                Marshall_StartPairingProcess();
                Peripheral_DisableUser();
                State = PERIPHERAL_STATE_WAIT_FOR_PAIRING;
                continue;
            }
            else if (RcvMsg == PERIPHERAL_RCVMSG_EVENT_COM_ERROR)
            {
                Peripheral_DisableUser();
                State = PERIPHERAL_STATE_WAIT_FOR_PAIRING;
                continue;
            }
            else if (RcvMsg == PERIPHERAL_RCVMSG_EVENT_KEEP_ALIVE_TICK)
            {
                if (State != PERIPHERAL_STATE_WAIT_FOR_PAIRING)
                {
                    if (STATUS_RET_SUCCESS != Marshall_SendPacket(NULL, 0, MARSHALLAPI_CODE_KEEP_ALIVE, 1))
                    {
                        Peripheral_DisableUser();
                        State = PERIPHERAL_STATE_WAIT_FOR_PAIRING;
                    }                        
                }
            }
        }        
        
        /** Peripheral state machine */
        LastState = State;        
        PeripheralStateFunc[State]();        
        if (LastState != State)
            StateStartTick = CurrentTick;

        /** Modem test state machine */
        ModemTestLastState = ModemTestState;        
        PeripheralModemStateFunc[ModemTestState]();
        if (ModemTestLastState != ModemTestState)
            ModemTestStateStartTick = ModemTestCurrentTick;

        if (Ret == STATUS_RET_SUCCESS)
        {
            if (pAppMsg->MsgType == MARSHALL_EXTERNAL_MSG)
                free(pAppMsg->pData);

            free(pAppMsg);
        }
    }
}




