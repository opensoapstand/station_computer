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
 *  Description  : This header file conatain the Marshall API 
 *
 */
 
#ifndef __MARSHALL_API_H__
#define __MARSHALL_API_H__

#include<stdint.h>
/**
* @brief	STM32F2XX - Nayax Amit 
* @brief	VPOS_PORJ - Nayax Vpos 
*/
#ifndef P_STRUCT
  #if defined STM32F2XX
	#include "CM3_wrapper.h"
  #elif defined VPOS_PORJ
    #define P_STRUCT __packed struct 
  #else
	#define P_STRUCT struct
	/* All the structures in this API are 1 byte packed structures !! */
	# pragma pack (1)
  #endif
#endif



					/************************************************/
					/*				  D E F I N E S					*/
					/************************************************/

/**
* @brief	Marshall version
*/
#define MARSHALLAPI_VERSION_STR             "Ver 00.11"

/**
 * @brief	Marshall version
 */
#define MARSHALLAPI_MAJOR_VERSION			0
#define MARSHALLAPI_MINOR_VERSION			11

/**
 * @brief	Amit source address
 */
#define MARSHALLAPI_AMIT_SOURCE_ADDR		0

/**
 * @brief	Max packet length
 */
#define MARSHALLAPI_MAX_PACKET_LENGTH		512

					/************************************************/
					/*				    E N U M S					*/
					/************************************************/

/**
 * @name 	E_MarshallApi_FuncCode
 * @brief	The Marshall functions code
 */
typedef enum    E_MarshallApi_FuncCode_tag
{
    MARSHALLAPI_CODE_RESPONSE               = 0x00,         /** Response code (ack/nak...) */
    MARSHALLAPI_CODE_RESET                  = 0x01,         /** Reset the Peripheral */
    
    MARSHALLAPI_CODE_FW_INFO                = 0x05,         /** Contains specific FW details */
    MARSHALLAPI_CODE_CONFIG                 = 0x06,         /** Set parameters for communication */
    MARSHALLAPI_CODE_KEEP_ALIVE             = 0x07,         /** Indication that the D200 is still connected */
    MARSHALLAPI_CODE_DISPLAY_MSG            = 0x08,        	/** Displays Message on D200 LCD */
    MARSHALLAPI_CODE_DISPLAY_MSG_STATUS     = 0x09,        	/** Response for displays Message */
	MARSHALLAPI_CODE_TRANSFER_DATA			= 0x0A,        	/** Transfer specific data  */
	MARSHALLAPI_CODE_STATUS					= 0x0B,        	/** Status message */
    
    
    MARSHALLAPI_CODE_MODEM_STATUS           = 0x20,         /** Modem status */
    MARSHALLAPI_CODE_OPEN_SOCKET            = 0x21,         /** Open a socket for comm. with a server */
    MARSHALLAPI_CODE_CLOSE_SOCKET           = 0x22,         /** Close socket opened for comm. with server */
    MARSHALLAPI_CODE_SEND_DATA              = 0x23,         /** Send Data to server */
    MARSHALLAPI_CODE_REQ_DATA               = 0x24,         /** Reqeust data received from server */
    MARSHALLAPI_CODE_RCV_DATA               = 0x25,         /** data received from server */
    
    MARSHALLAPI_CODE_TRACE                  = 0x30,         /** Send Trace parameters to Amit */
    MARSHALLAPI_CODE_ALERT                  = 0x31,         /** Send Alert to Amit */   
    
    MARSHALLAPI_CODE_MDB_CMD                = 0x80,         /** Retrieve received Data message from server */ 
    
    MARSHALLAPI_CODE_PP_SET_EVENT           = 0x90,         /** Pin pad - Set event type to be activated on D200 */
    MARSHALLAPI_CODE_PP_EVENT               = 0x91,       	/** Pin pad - Event type and its specific data */
    MARSHALLAPI_CODE_PP_TRANS_REQUEST       = 0x92,       	/** Pin pad - Provide to D200 Transaction’s parameters */
    MARSHALLAPI_CODE_PP_TRANS_DETAILS       = 0x93,      	/** Pin pad - Provides the Amit with Transaction’s details to be sent to Nayax’ Server */
    MARSHALLAPI_CODE_PP_DISP_CONFIRM        = 0x94,         /** Pin pad - Indication whether the product was dispensed correctly or not */
    MARSHALLAPI_CODE_PP_TRANS_CANCEL        = 0x95,         /** Pin pad - Two-way command in order to cancel a transaction */
    MARSHALLAPI_CODE_PP_END_TRANS           = 0x96,         /** Pin pad - Indicates end of transaction */    

    MARSHALLAPI_CODE_MAX,

} E_MarshallApi_FuncCode;



					/************************************************/
					/*			P A C K E T    H E A D E R  	    */
					/************************************************/

/**
 * @name 	E_MarshallApi_FuncCode
 * @brief	The Marshall header structure
 */
typedef P_STRUCT S_MarshallApi_PacketHeader_tag
{
	uint16_t	PacketLength;			                    /** Length of Packet Message (It include the CRC but exclude itself)*/
	uint8_t		PacketOptions;			                    /** Packet Options bitmap */
	uint8_t		PacketId;				                    /** Packet ID - Running Identifier */
	uint8_t		Source;					                    /** Source of Packet */
	uint8_t		SourceLsb;				                    /** LSB Serial Number of Source */
	uint8_t		Dest;					                    /** Destination of Packet */
	uint8_t		DestLsb;				                    /** LSB Serial Number of Destination */
	uint8_t		FuncCode;				                    /** Specific Code of Function */

}S_MarshallApi_PacketHeader;

/**
 * @brief	Packet length size
 */
#define MARSHALLAPI_PACKET_LENGTH_SIZE		                2


/**
 * @brief 	Packet header members offset
 */
#define MARSHALLAPI_PACKET_HEADR_PACKET_LENGTH_OFFSET		0
#define MARSHALLAPI_PACKET_HEADR_PACKET_OPTIONS_OFFSET		2
#define MARSHALLAPI_PACKET_HEADR_PACKET_ID_OFFSET			3
#define MARSHALLAPI_PACKET_HEADR_PACKET_SRC_ADDR_OFFSET		4
#define MARSHALLAPI_PACKET_HEADR_PACKET_SRC_LSB_OFFSET		5
#define MARSHALLAPI_PACKET_HEADR_PACKET_DST_ADDR_OFFSET		6
#define MARSHALLAPI_PACKET_HEADR_PACKET_DST_LSB_OFFSET		7
#define MARSHALLAPI_PACKET_HEADR_PACKET_FUNC_CODE_OFFSET	8
#define MARSHALLAPI_PACKET_HEADR_PACKET_DATA_OFFSET			9


/**
 * @brief	Packet options
 */
#define	MARSHALLAPI_OPTIONS_ACK_REQUESTED_OFFSET		    0
#define	MARSHALLAPI_OPTIONS_ACK_REQUESTED_MASK			    0x01
#define	MARSHALLAPI_OPTIONS_ACK_NOT_REQUESTED			    0
#define	MARSHALLAPI_OPTIONS_ACK_REQUESTED				    0x1

#define	MARSHALLAPI_OPTIONS_PACKET_ENCRYPTED_OFFSET		    1
#define	MARSHALLAPI_OPTIONS_PACKET_ENCRYPTED_MASK		    0x02
#define	MARSHALLAPI_OPTIONS_PACKET_NOT_ENCRYPTED		    0
#define	MARSHALLAPI_OPTIONS_PACKET_ENCRYPTED			    1

#define	MARSHALLAPI_OPTIONS_RETRY_NUMBER_OFFSET			    2
#define	MARSHALLAPI_OPTIONS_RETRY_NUMBER_MASK			    0x0C

#define	MARSHALLAPI_OPTIONS_RFU_OFFSET					    4
#define	MARSHALLAPI_OPTIONS_RFU_MASK					    0xF0

					/************************************************/
					/*			P A C K E T    F O O T E R  	    */
					/************************************************/

/**
 * @brief	Packet length size
 */
#define MARSHALLAPI_PACKET_CRC_SIZE		                    2

/**
 * @name 	S_MarshallApi_PacketFooter
 * @brief	The Marshall packet structure
 */
typedef P_STRUCT S_MarshallApi_PacketFooter_tag
{
	uint16_t	Crc;					/** Packet CRC signature */

}S_MarshallApi_PacketFooter;

					/************************************************/
					/*				 R E S P O N S E    	   	 	*/
					/************************************************/

/**
 * @brief	Response data size
 */
#define MARSHALLAPI_RESPONSE_CMD_DATA_SIZE				    1

/**
 * @name 	E_MarshallApi_ResponseVal
 * @brief	Function - Response
 */
typedef enum E_MarshallApi_ResponseVal_tag
{
	MARSHALLAPI_RESPONSE_VAL_ACK 						    = 0,
	MARSHALLAPI_RESPONSE_VAL_CRC_ERROR 					    = 1,
	MARSHALLAPI_RESPONSE_VAL_WRONG_DST_ADDR 			    = 2,
	MARSHALLAPI_RESPONSE_VAL_NON_CONSECUTIVE_PACKET_ID 	    = 3,
	MARSHALLAPI_RESPONSE_VAL_NONE_REGISTERED_SRC	 	    = 4,
	MARSHALLAPI_RESPONSE_VAL_UNEXPECTED_ERROR 			    = 10,

	MARSHALLAPI_RESPONSE_VAL_MAX,

}E_MarshallApi_ResponseVal;

					/************************************************/
					/*		    F I R M W A R E   I N F O   		*/
					/************************************************/

/**
 * @name 	S_MarshallApi_FirmInfoParams
 * @brief	The Firm Info structure
 */
typedef P_STRUCT S_MarshallApi_FirmInfoParams_tag
{

	uint8_t			ProtocolVerMajor;					                /** Protocol version (Major) 			*/
	uint8_t			ProtocolVerMinor;					                /** Protocol version (Minor) 			*/
	uint8_t			PeripheralType;						                /** Peripheral type			 			*/
	uint8_t			PeripheralSubType;					                /** Peripheral sub type		 			*/
	uint16_t		PeripheralCapabilities;			                    /** Peripheral Capabilities	 			*/
	uint8_t			PeripheralModel[20];				                /** Peripheral Model	 				*/
	uint8_t			PeripheralSerialNum[20];			                /** Peripheral SerialNum 				*/
	uint8_t			PeripheralAppSwVer[20];				                /** Peripheral Application SW Version	*/

}S_MarshallApi_FirmInfoParams;

/**
 * @name 	E_MarshallApi_FirmInfoType
 * @brief	Firmware info - type
 */
typedef enum E_MarshallApi_FirmInfoType_tag
{
    MARSHALLAPI_INFO_TYPE_RESERVED_1                        = 0,        /** Reserved                            */
    MARSHALLAPI_INFO_TYPE_RESERVED_2                        = 1,        /** Reserved                            */
    MARSHALLAPI_INFO_TYPE_CRD_RDR                           = 2,        /** Card Readers                        */
    MARSHALLAPI_INFO_TYPE_PHOTO_BOOTH                       = 3,        /** Photo Booth                         */
    MARSHALLAPI_INFO_TYPE_EV_CHARGER                        = 4,        /** EV (electric vehicle) Charger       */
    MARSHALLAPI_INFO_TYPE_BEVERAGE_MACHINE                  = 5,        /** Beverage machines                   */
    MARSHALLAPI_INFO_TYPE_WATER_FILLING_MACHINE             = 6,        /** Water filling machine               */
    MARSHALLAPI_INFO_TYPE_LOCKER                            = 7,        /** Locker                              */
    MARSHALLAPI_INFO_TYPE_RETAIL                            = 8,        /** Retail                              */
    MARSHALLAPI_INFO_TYPE_LAUNDRY_MACHINE                   = 9,        /** Laundry machine                     */
    MARSHALLAPI_INFO_TYPE_E_MONEY                           = 10,       /** E money                             */

    MARSHALLAPI_INFO_TYPE_MAX,

} E_MarshallApi_FirmInfoType;

/**
 * @name 	E_MarshallApi_InfoCardRdrSubType
 * @brief	Firmware info - Nayax card reader sub types
 */
typedef enum E_MarshallApi_CardRdrSubType_tag
{
    MARSHALLAPI_INFO_CARD_RDR_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_CARD_RDR_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_CARD_RDR_SUB_TYPE_2                    = 2,   

    MARSHALLAPI_INFO_CARD_RDR_SUB_TYPE_MAX,

} E_MarshallApi_InfoNayaxRdrSubType;

/**
 * @name 	E_MarshallApi_InfoPhotoBoothSubType
 * @brief	Firmware info - Photo Booth sub types
 */
typedef enum E_MarshallApi_InfoPhotoBoothSubType_tag
{
    MARSHALLAPI_INFO_PHOTO_BOOTH_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_PHOTO_BOOTH_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_PHOTO_BOOTH_SUB_TYPE_2                    = 2,
    
    MARSHALLAPI_INFO_PHOTO_BOOTH_SUB_TYPE_MAX,

} E_MarshallApi_InfoPhotoBoothSubType;

/**
 * @name 	E_MarshallApi_InfoEvChargerSubType
 * @brief	Firmware info - EV (electric vehicle) Charger sub types
 */
typedef enum E_MarshallApi_InfoEvChargerSubType_tag
{
    MARSHALLAPI_INFO_EV_CHARGER_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_EV_CHARGER_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_EV_CHARGER_SUB_TYPE_2                    = 2,
    
    MARSHALLAPI_INFO_EV_CHARGER_SUB_TYPE_MAX,

} E_MarshallApi_InfoEvChargerSubType;


/**
 * @name 	E_MarshallApi_InfoBeverageMachineSubType
 * @brief	Firmware info - Beverage Machine sub types
 */
typedef enum E_MarshallApi_InfoBeverageMachineSubType_tag
{
    MARSHALLAPI_INFO_BEVERAGE_MACHINE_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_BEVERAGE_MACHINE_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_BEVERAGE_MACHINE_SUB_TYPE_2                    = 2,
    
    MARSHALLAPI_INFO_BEVERAGE_MACHINE_SUB_TYPE_MAX,

} E_MarshallApi_InfoBeverageMachineSubType;

/**
* @name 	E_MarshallApi_InfoWaterFillingSubType
* @brief	Firmware info - Water filling sub types
*/
typedef enum E_MarshallApi_InfoWaterFillingSubType_tag
{
    MARSHALLAPI_INFO_WATER_FILLING_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_WATER_FILLING_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_WATER_FILLING_SUB_TYPE_2                    = 2,

    MARSHALLAPI_INFO_WATER_FILLING_SUB_TYPE_MAX,

} E_MarshallApi_InfoWaterFillingSubType;

/**
* @name 	E_MarshallApi_InfoLockerSubType
* @brief	Firmware info - Locker sub types
*/
typedef enum E_MarshallApi_InfoLockerSubType_tag
{
    MARSHALLAPI_INFO_LOCKER_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_LOCKER_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_LOCKER_SUB_TYPE_2                    = 2,

    MARSHALLAPI_INFO_LOCKER_SUB_TYPE_MAX,

} E_MarshallApi_InfoLockerSubType;

/**
* @name 	E_MarshallApi_InfoRetailSubType
* @brief	Firmware info - Retail sub types
*/
typedef enum E_MarshallApi_InfoRetailSubType_tag
{
    MARSHALLAPI_INFO_RETAIL_SUB_TYPE_RESERVED             = 0,   
    MARSHALLAPI_INFO_RETAIL_SUB_TYPE_1                    = 1,   
    MARSHALLAPI_INFO_RETAIL_SUB_TYPE_2                    = 2,

    MARSHALLAPI_INFO_RETAIL_SUB_TYPE_MAX,

} E_MarshallApi_InfoRetailSubType;


/**
* @name 	E_MarshallApi_InfoLaundryMachineSubType
* @brief	Firmware info - Laundry sub types
*/
typedef enum E_MarshallApi_InfoLaundryMachineSubType_tag
{
    MARSHALLAPI_INFO_LAUNDRY_MACHINE_SUB_TYPE_RESERVED              = 0,   
    MARSHALLAPI_INFO_LAUNDRY_MACHINE_SUB_TYPE_1                     = 1,   
    MARSHALLAPI_INFO_LAUNDRY_MACHINE_SUB_TYPE_2                     = 2,

    MARSHALLAPI_INFO_LAUNDRY_MACHINE_SUB_TYPE_MAX,

} E_MarshallApi_InfoLaundryMachineSubType;

/**
* @name 	E_MarshallApi_InfoEMoneySubType
* @brief	Firmware info - E Money sub types
*/
typedef enum E_MarshallApi_InfoEMoneySubType_tag
{
    MARSHALLAPI_INFO_E_MONEY_SUB_TYPE_RESERVED                      = 0,   
    MARSHALLAPI_INFO_E_MONEY_SUB_TYPE_1                             = 1,   
    MARSHALLAPI_INFO_E_MONEY_SUB_TYPE_2                             = 2,

    MARSHALLAPI_INFO_E_MONEY_SUB_TYPE_MAX,

} E_MarshallApi_InfoEMoneySubType;


/**
 * @brief	Device Capabilities 
 */
#define	MARSHALLAPI_DEV_CAPB_MAGSTRIPE_BIT_OFFSET                           0
#define	MARSHALLAPI_DEV_CAPB_MAGSTRIPE_BIT_MASK			                    0x0001
#define	MARSHALLAPI_DEV_CAPB_CONTACT_BIT_OFFSET                             1
#define	MARSHALLAPI_DEV_CAPB_CONTACT_BIT_MASK			                    0x0002
#define	MARSHALLAPI_DEV_CAPB_CONTACTLESS_BIT_OFFSET                         2
#define	MARSHALLAPI_DEV_CAPB_CONTACTLESS_BIT_MASK		                    0x0004  
#define	MARSHALLAPI_DEV_CAPB_KEYPAD_BIT_OFFSET                              3
#define	MARSHALLAPI_DEV_CAPB_KEYPAD_BIT_MASK		                        0x0008 
#define	MARSHALLAPI_DEV_CAPB_DISPLAY_BIT_OFFSET                             4
#define	MARSHALLAPI_DEV_CAPB_DISPLAY_BIT_MASK		                        0x0010 
#define	MARSHALLAPI_DEV_CAPB_TOUCH_SCREEN_BIT_OFFSET                        5
#define	MARSHALLAPI_DEV_CAPB_TOUCH_SCREEN_BIT_MASK		                    0x0020 
#define	MARSHALLAPI_DEV_CAPB_MULTI_SESSION_BIT_OFFSET                       6
#define	MARSHALLAPI_DEV_CAPB_MULTI_SESSION_BIT_MASK		                    0x0040 
#define	MARSHALLAPI_DEV_CAPB_PROP_MIFARE_CARD_APPROVED_BY_VMC_BIT_OFFSET    7
#define	MARSHALLAPI_DEV_CAPB_PROP_MIFARE_CARD_APPROVED_BY_VMC_BIT_MASK		0x0080 
#define	MARSHALLAPI_DEV_CAPB_VEND_REQ_PRICE_IS_NOT_FINAL_BIT_OFFSET         8
#define	MARSHALLAPI_DEV_CAPB_VEND_REQ_PRICE_IS_NOT_FINAL_BIT_MASK           0x0100 
#define	MARSHALLAPI_DEV_CAPB_PROP_MAG_CARD_APPROVED_BY_VMC_BIT_OFFSET       9
#define	MARSHALLAPI_DEV_CAPB_PROP_MAG_CARD_APPROVED_BY_VMC_BIT_MASK		    0x0200


					/************************************************/
					/*		    		C O N F I G	  	 			*/
					/************************************************/
/**
 * @name 	S_MarshallApi_ConfigParams
 * @brief	The Config Params structure
 */
typedef P_STRUCT S_MarshallApi_ConfigParams_tag
{

	uint8_t			ProtocolVerMajor;					        /** Protocol version (Major) 			*/
	uint8_t			ProtocolVerMinor;					        /** Protocol version (Minor) 			*/
	uint8_t			PeripheralID;						        /** Peripheral ID			 			*/
	uint32_t		KeepAlivePollInterval;				        /** Keep Alive/Poll Interval in mSec 	*/
	uint8_t			Language;							        /** Language		 					*/
	uint8_t			CountryCode[3];						        /** Country Code 	 					*/
	uint8_t			CurrencyCode[3];					        /** Currency Code 	 					*/
	uint8_t			DecimalPlace;						        /** Decimal Place 	 					*/
	uint8_t			AmitSerialNumber[16];				        /** Amit SerialNumber					*/
	uint16_t		MaxMsgLength;						        /** Max Message Length 					*/

}S_MarshallApi_ConfigParams;

					/************************************************/
					/*		   T R A N S F E R      D A T A 		*/
					/************************************************/
/**
 * @name 	E_MarshallApi_TransferDataTags
 * @brief	Transfer data tags
 */
typedef enum E_MarshallApi_TransferDataTags_tag
{
    MARSHALLAPI_TRANSFER_DATA_TAG_TXN_ID                        = 1,    /** Transaction ID                      */
    MARSHALLAPI_TRANSFER_DATA_TAG_CHOOSE_PRODUCT_TIMEOUT        = 2,    /** The peripheral can update the Amit timeout “Choose Product Timeout”.
                                                                            If it set to zero the Amit will not play and display the choose product message.
                                                                            It is possible to update this parameter only when the Amit is in idle state and not during a session.   */
    MARSHALLAPI_TRANSFER_DATA_CARD_TYPE                         = 3,    /** Card type       (Data - E_MarshallApi_TDCardType,           Data size - 1 byte) */
    MARSHALLAPI_TRANSFER_DATA_CARD_ENTRY_MODE                   = 4,    /** Card entry mode (Data - E_MarshallApi_TDCardEntryType,      Data size - 1 byte) */
    MARSHALLAPI_TRANSFER_DATA_CARD_BIN                          = 5,    /** Card BIN        (Data - ascii,                              Data size - 6 bytes) */
    MARSHALLAPI_TRANSFER_DATA_CARD_BIN_HASH                     = 6,    /** Card BIN Hash   (Data - int,                                Data size - 20 bytes) */ 
    MARSHALLAPI_TRANSFER_DATA_CARD_MIFARE_UID                   = 7,    /** Card UID        (Data - int,                                Data size - 4,7 or 10 bytes) */ 
    MARSHALLAPI_TRANSFER_DATA_VMC_AUTH_STATUS                   = 8,    /** When the VMC finishing authorize a propriety card it will send auth status to the Amit  */
    MARSHALLAPI_TRANSFER_DATA_COM_STATUS                        = 9,    /** The Amit - server comunication status (0 - OK, else - error, Data size  - 1) */
    

    MARSHALLAPI_TRANSFER_DATA_TAG_MAX,

} E_MarshallApi_TransferDataTags;

#define MARSHALL_TXN_ID_SIZE                                    8

/**
 * @name 	S_MarshallApi_TLV
 * @brief	The transfer data TLV structur
 */
typedef P_STRUCT S_MarshallApi_TLV_tag
{    
	uint8_t			Tag;
    uint8_t			Length;
    uint8_t         DataFirstByte;

}S_MarshallApi_TLV;


/**
 * @name 	E_MarshallApi_TDCardType
 * @brief	Transfer data card type
 */
typedef enum E_MarshallApi_TDCardType_tag
{
    MARSHALLAPI_TRANSFER_DATA_UNKOWN_CARD_TYPE = 0,                 /* Unknown	            */
    MARSHALLAPI_TRANSFER_DATA_PROPRIETARY_CARD_TYPE,                /* Proprietary	        */
    MARSHALLAPI_TRANSFER_DATA_VISA_CARD_TYPE,                       /* Visa	                */
    MARSHALLAPI_TRANSFER_DATA_MASTER_CARD_CARD_TYPE,                /* Master Card          */
    MARSHALLAPI_TRANSFER_DATA_CHINA_UNION_PAY_CARD_TYPE,            /* China Union Pay	    */
    MARSHALLAPI_TRANSFER_DATA_MAESTRO_CARD_TYPE,                    /* Maestro  	        */
    MARSHALLAPI_TRANSFER_DATA_INTERAC_CARD_TYPE,                    /* Interac  	        */
                                                                    
    MARSHALLAPI_TRANSFER_DATA_CARD_TYPE_MAX,                       

}E_MarshallApi_TDCardType;

/**
 * @name 	E_MarshallApi_TDCardEntryType
 * @brief	Transfer data card entry type
 */
typedef enum E_MarshallApi_TDCardEntryType_tag
{ 
    MARSHALLAPI_TRANSFER_DATA_MSR_SWIPE_CARD_ENTRY_TYPE = 1,         /* Mag stripes	        */
    MARSHALLAPI_TRANSFER_DATA_CONTACTLESS_CARD_ENTRY_TYPE,           /* Contactless	        */
    MARSHALLAPI_TRANSFER_DATA_CONTACT_CARD_ENTRY_TYPE,               /* Contact	            */
    MARSHALLAPI_TRANSFER_DATA_MIFARE_CARD_ENTRY_TYPE,                /* MiFare	            */
    MARSHALLAPI_TRANSFER_DATA_HID_CARD_ENTRY_TYPE,                   /* HID	                */
    MARSHALLAPI_TRANSFER_DATA_NFC_CARD_ENTRY_TYPE,                   /* NFC	                */
    MARSHALLAPI_TRANSFER_DATA_CNOUS_CARD_ENTRY_TYPE,                 /* CNOUS	            */
        
    MARSHALLAPI_TRANSFER_DATA_CARD_ENTRY_TYPE_MAX,

}E_MarshallApi_TDCardEntryType;


					/************************************************/
					/*		    		S T A T U S	  	 			*/
					/************************************************/

/**
* @name 	S_MarshallApi_Status
* @brief	Status message
*/
typedef P_STRUCT S_MarshallApi_Status_tag
{
	uint8_t			Type;				                        /** According to E_MarshallApi_StatusType   */
    uint16_t		DataSize;		                            /** Additional status data size             */
    uint8_t         DataFirstByte;                              /** Status additional data                  */
    
}S_MarshallApi_Status;

/**
* @name 	E_MarshallApi_StatusType
* @brief	Firmware info - type
*/
typedef enum E_MarshallApi_StatusType_tag
{
    MARSHALLAPI_STATUS_TYPE_UNEXPECTED_ERR          = 0,       		/** Unexpected error                                */
	MARSHALLAPI_STATUS_TYPE_TIMEOUT                 = 1,            /** Process time-out                                */
	MARSHALLAPI_STATUS_TYPE_OUT_OF_SEQUENCE         = 2,      		/** Received out of sequence command                */
	MARSHALLAPI_STATUS_TYPE_PEND_SERV_DATA          = 3,			/** Pending 3rd party communication data received   */
    MARSHALLAPI_STATUS_TYPE_CLOSE_SESSION_STATUS    = 4,			/** In multi session, a response for close session  */

	MARSHALLAPI_STATUS_TYPE_MAX,

} E_MarshallApi_StatusType;
    
/**
* @name 	E_MarshallApi_CloseSessionRetVal
* @brief	Firmware info - type
*/
typedef enum E_MarshallApi_CloseSessionRetVal_tag
{
    MARSHALLAPI_CLOSE_SESSION_OK_RET_VAL                = 0,       	/** Status OK                                       */
    MARSHALLAPI_CLOSE_SESSION_ITEM_NOT_FOUND_RET_VAL    = 1,	    /** Item not found                                  */
    MARSHALLAPI_CLOSE_SESSION_UNEXPECTED_ERR_RET_VAL    = 2,	    /** Unexpected error                                */

	MARSHALLAPI_CLOSE_SESSION_RET_VAL_MAX,

} E_MarshallApi_CloseSessionRetVal;    

					/************************************************/
					/*		  D I S P L A Y   M E S S A G E	  	    */
					/************************************************/
/**
 * @name 	S_MarshallApi_DisplaysMessage
 * @brief	A message to display on LCD
 */
typedef P_STRUCT S_MarshallApi_DisplaysMessage_tag
{
	uint8_t			FirstByteOfMsgToDisplay;

}S_MarshallApi_DisplaysMessage;

					/************************************************************/
					/*		  D I S P L A Y   M E S S A G E	  S T A T U S	    */
					/************************************************************/
/**
 * @name 	S_MarshallApi_DisplaysMsgStatus
 * @brief	A response to display message
 */
typedef P_STRUCT S_MarshallApi_DisplaysMsgStatus_tag
{
	uint8_t			DisplayStatus;

}S_MarshallApi_DisplaysMsgStatus;

/**
* @name 	E_MarshallApi_DisplaysMsgStatus
* @brief	The display message status 
*/
typedef enum E_MarshallApi_DisplaysMsgStatus_tag
{
	MARSHALLAPI_DISPLAYS_MSG_STATUS_OK      = 0,      	        /** Display message OK         */
	MARSHALLAPI_DISPLAYS_MSG_STATUS_ERR     = 1,		        /** Display message error      */

	MARSHALLAPI_DISPLAYS_MSG_STATUS_MAX,

} E_MarshallApi_DisplaysMsgStatus;

					/************************************************/
					/*		   M O D E M     S T A T U S	        */
					/************************************************/

/**
* @name 	E_MarshallApi_ModemRetVal
* @brief	The modem return value
*/
typedef enum E_MarshallApi_ModemRetVal_tag
{
	MARSHALLAPI_MODEM_RET_VAL_OK                            = 0,           
    MARSHALLAPI_MODEM_RET_VAL_FAIL,
    MARSHALLAPI_MODEM_RET_VAL_ESC,
    MARSHALLAPI_MODEM_RET_VAL_BUSY, 
    MARSHALLAPI_MODEM_RET_VAL_PARAM_IN,
    MARSHALLAPI_MODEM_RET_VAL_CMD,
    
	MARSHALLAPI_MODEM_RET_VAL_NO_AVAILABLE_RESOURCE         = 0x80, 
    MARSHALLAPI_MODEM_RET_VAL_SOCKET_CLOSE, 
    MARSHALLAPI_MODEM_RET_VAL_SOCKET_OPEN,
    MARSHALLAPI_MODEM_RET_VAL_NOT_SUPPORTED_SOCKET_TYPE,   
    
	MARSHALLAPI_MODEM_RET_VAL_MAX,

}E_MarshallApi_ModemRetVal;


/**
 * @name 	S_MarshallApi_ModemStatus
 * @brief	The modem status   
 */
typedef P_STRUCT S_MarshallApi_ModemStatus_tag
{
	uint8_t			Cmd;                                        /** The command the modem responding to (0 incase the status message isn't a response) */	
    uint8_t         ModemRetVal;                                /** E_MarshallApi_ModemRetVal                       */
    uint8_t         ModemExtraData;                             /** If the error isn't busy it will contain the modem err number
                                                                       In case of busy - it will contain an estimate busy time in seconds*/

}S_MarshallApi_ModemStatus;


					/************************************************/
					/*		       O P E N    S O C K E T	  		*/
					/************************************************/
/**
 * @name 	S_MarshallApi_OpenSocket
 * @brief	Open a socket for communication with a server.  
 */
typedef P_STRUCT S_MarshallApi_OpenSocket_tag
{
	uint8_t			ServerDestination;				            /** Server destination 			                            */
	uint8_t			Protocol;					                /** Protocol type 			                                */
	uint16_t		Port;						                /** Port number			 			                        */
	uint8_t		    ServerAddrFirstByte;	                    /** IP or DNS address (ASCII string with variable length    */

}S_MarshallApi_OpenSocket;

/**
* @name 	E_MarshallApi_OpenSocketDest
* @brief	Open socket  - Destination 
*/
typedef enum E_MarshallApi_OpenSocketDest_tag
{
	MARSHALLAPI_OPEN_SOCKET_DEST_NAYAX_SERVER           = 0,    /** Nayax’s Server          */
	MARSHALLAPI_OPEN_SOCKET_DEST_TRD_PARTIY_SERVER      = 1,    /** 3rd party's Server      */

	MARSHALLAPI_OPEN_SOCKET_DEST_MAX,

} E_MarshallApi_OpenSocketDest;

/**
* @name 	E_MarshallApi_OpenSocketProtocol
* @brief	Open socket  - Protocol type
*/
typedef enum E_MarshallApi_OpenSocketProtocol_tag
{
	MARSHALLAPI_OPEN_SOCKET_PROTOCOL_TCP        = 0,            /** TCP     */
	MARSHALLAPI_OPEN_SOCKET_PROTOCOL_UDP        = 1,            /** UDP     */
    MARSHALLAPI_OPEN_SOCKET_PROTOCOL_FTP        = 2,            /** FTP     */

	MARSHALLAPI_OPEN_SOCKET_PROTOCOL_MAX,

} E_MarshallApi_OpenSocketProtocol;


					/************************************************/
					/*		       S E N D    D A T A  		        */
					/************************************************/
/**
 * @name 	S_MarshallApi_SendData
 * @brief	Send data to the opened socket.
 */
typedef P_STRUCT S_MarshallApi_SendData_tag
{
	uint16_t			DataLength;				        /** The data length                     */
    uint8_t			    FirstByteOfData;		        /** The first byte of n data bytes      */

}S_MarshallApi_SendData;

					/************************************************/
					/*		    R E Q U E S T    D A T A  		    */
					/************************************************/
/**
 * @name 	S_MarshallApi_ReqData
 * @brief	Request data from server message.
 */
typedef P_STRUCT S_MarshallApi_ReqData_tag
{
	uint16_t			TimeOutSec;				       /** Time out in seconds for retrieving the data  */
    uint16_t			DataLength;				       /** The data length to retrieve                  */

}S_MarshallApi_ReqData;

					/************************************************/
					/*	     R E C E I V E D    D A T A    		    */
					/************************************************/

/**
 * @name 	S_MarshallApi_RcvData
 * @brief	The data receieved
 */
typedef P_STRUCT S_MarshallApi_RcvData_tag
{
	uint16_t			DataLength;				        /** The data length, 0 in case of time out and no data available    */
    uint8_t			    FirstByteOfData;		        /** The first byte of n data bytes                                  */

}S_MarshallApi_RcvData;

#define MARSHALLAPI_GET_DATA_RES_DATA_LENGTH_SIZE		2


					/************************************************/
					/*		           T R A C E  	 	            */
					/************************************************/
/**
 * @name 	S_MarshallApi_Trace
 * @brief	Trace command
 */
typedef P_STRUCT S_MarshallApi_Trace_tag
{

	uint16_t		TraceNumber;					    /** Peripheral trace number 			    */
	uint8_t			ParametersNum;					    /** Number of Parameters, values: 0, 1 or 2 */
	uint16_t		Parameter1 ;						/** Parameter 1			 			        */
	uint16_t		Parameter2 ;						/** Parameter 2			 			        */

}S_MarshallApi_Trace;

					/************************************************/
					/*		           A L E R T 	                */
					/************************************************/

#define MARSHALL_API_ALERT_NUMBER_SIZE                  2

/**
 * @name 	S_MarshallApi_Alert
 * @brief	Alert command
 */
typedef P_STRUCT S_MarshallApi_Alert_tag
{

	uint16_t		AlertID;					        /** Peripheral alert ID 			        */
	uint8_t			AlertStringFirstByte;				/** Alert string                            */

}S_MarshallApi_Alert;


                    /************************************************/
                    /*		     M D B    C O M A N D S   (VMC)     */
                    /************************************************/

#define MARSHALLAPI_MDB_CMD_OFFSET				        0
#define MARSHALLAPI_MDB_SUB_CMD_OFFSET			        1

/**
 * @name 	E_MarshallApi_MdbCommandsCode
 * @brief	The supported MDB commands code (peripheral -> amit)
 */
typedef enum    E_MarshallApi_MdbCommandsCode_tag
{
    MARSHALLAPI_MDB_CMD_CODE_VEND                       = 0x13,        
    MARSHALLAPI_MDB_CMD_CODE_READER                     = 0x14,
    MARSHALLAPI_MDB_CMD_CODE_EXPANSION                  = 0x17,
   
    MARSHALLAPI_MDB_CMD_CODE_MAX,

} E_MarshallApi_MdbCommandsCode;

/**
 * @name 	E_MarshallApi_MdbVendSubCommandsCode
 * @brief	The supported MDB vend sub commands code
 */
typedef enum    E_MarshallApi_MdbVendSubCommandsCode_tag
{
    MARSHALLAPI_MDB_CMD_CODE_VEND_REQEUST               = 0x0,        
    MARSHALLAPI_MDB_CMD_CODE_VEND_CANCEL                = 0x1,  
    MARSHALLAPI_MDB_CMD_CODE_VEND_SUCCESS               = 0x2,  
    MARSHALLAPI_MDB_CMD_CODE_VEND_FAILURE               = 0x3,  
    MARSHALLAPI_MDB_CMD_CODE_VEND_SESSION_COMPLETE      = 0x4, 
    MARSHALLAPI_MDB_CMD_CODE_CASH_SALE                  = 0x5,

    /** Extended MDB commands */
    MARSHALLAPI_MDB_CMD_CODE_VEND_CLOSE_SESSION         = 0x80,
    MARSHALLAPI_MDB_CMD_CODE_VEND_SESSION_TYPE          = 0x81,
   
    MARSHALLAPI_MDB_CMD_CODE_VEND_MAX,

} E_MarshallApi_MdbVendSubCommandsCode;    

/**
 * @name 	E_MarshallApi_MdbReaderSubCommandsCode
 * @brief	The supported MDB reader sub commands code
 */
typedef enum    E_MarshallApi_MdbReaderSubCommandsCode_tag
{
    MARSHALLAPI_MDB_CMD_CODE_READER_DISABLE             = 0x0,        
    MARSHALLAPI_MDB_CMD_CODE_READER_ENABLE              = 0x1,  
    MARSHALLAPI_MDB_CMD_CODE_READER_CANCEL              = 0x2,  
   
    MARSHALLAPI_MDB_CMD_CODE_READER_MAX,

} E_MarshallApi_MdbReaderSubCommandsCode;

/**
 * @name 	E_MarshallApi_MdbFtlSubCommandsCode_tag
 * @brief	The supported MDB ftl commands
 */
typedef enum    E_MarshallApi_MdbFtlSubCommandsCode_tag
{
    MARSHALLAPI_MDB_FTL_CMD_SEND_BLOCK                  = 0xFC,
   
    MARSHALLAPI_MDB_FTL_CMD_MAX,

} E_MarshallApi_MdbFtlSubCommandsCode; 

/**
* @name 	S_MarshallApi_MdbCmdHeader
* @brief	MDB command header
*/
typedef P_STRUCT S_MarshallApi_MdbCmdHeader_tag
{
    uint8_t     Cmd;
    uint8_t     SubCmd;
}S_MarshallApi_MdbCmdHeader;

/**
* @name 	S_MarshallApi_MdbVendRequest
* @brief	MDB vend request command
*/
typedef P_STRUCT S_MarshallApi_MdbVendRequest_tag
{
	uint16_t	ItemPrice;
	uint16_t	ItemNumber;

}S_MarshallApi_MdbVendRequest;

/**
* @name 	S_MarshallApi_MdbVendSuccess
* @brief	MDB vend success command
*/
typedef P_STRUCT S_MarshallApi_MdbVendSuccess_tag
{
	uint16_t	ItemNumber;

}S_MarshallApi_MdbVendSuccess;

/**
* @name 	S_MarshallApi_MdbVendCashSale
* @brief	MDB vend cash sale command
*/
typedef P_STRUCT S_MarshallApi_MdbVendCashSale_tag
{
	uint16_t	ItemPrice;
	uint16_t	ItemNumber;

}S_MarshallApi_MdbVendCashSale;

/**
* @name 	E_MarshallApi_SessionStatus
* @brief	MDB close session command - status definition
*/
typedef enum E_MarshallApi_SessionStatus_tag
{
	MARSHALLAPI_MDB_CLOSE_SESSION_CMD_STATUS_OK                     = 0x0, 
    MARSHALLAPI_MDB_CLOSE_SESSION_CMD_STATUS_USER_CANCEL            = 0x1, 
    MARSHALLAPI_MDB_CLOSE_SESSION_CMD_STATUS_FAIL_TO_DISPENCE       = 0x2,
    MARSHALLAPI_MDB_CLOSE_SESSION_CMD_STATUS_SESSION_TIMEOUT        = 0x3,
    
    MARSHALLAPI_MDB_CLOSE_SESSION_CMD_STATUS_MAX,

}E_MarshallApi_SessionStatus;

/**
* @name 	S_MarshallApi_MdbVendCloseSession
* @brief	(extended)MDB - close session command
*/
typedef P_STRUCT S_MarshallApi_MdbVendCloseSession_tag
{
	uint8_t	    SessionStatus;                      /** Acording to E_MarshallApi_SessionStatus */
	uint16_t	Price;
	uint16_t	ProductID;
    uint16_t    Quantity;

}S_MarshallApi_MdbVendCloseSession;

/**
* @name 	E_MarshallApi_MdbVendSessionType
* @brief	MDB session type command - type definition
*/
typedef enum E_MarshallApi_MdbVendSessionType
{
	MARSHALLAPI_MDB_SESSION_TYPE_VENDING        = 0x0, 
    MARSHALLAPI_MDB_SESSION_TYPE_INFO,
    
    MARSHALLAPI_MDB_SESSION_TYPE_MAX,

}E_MarshallApi_MdbVendSessionType;

/**
* @name 	S_MarshallApi_MdbVendSessionType
* @brief	(extended)MDB - session type command
*/
typedef P_STRUCT S_MarshallApi_MdbVendSessionType_tag
{
	uint8_t	    SessionType;                      /** Acording to E_MarshallApi_MdbVendSessionType */

}S_MarshallApi_MdbVendSessionType;

/**
 * @name 	U_MarshallApi_MdbFunctions
 * @brief	MDB functions
 */
typedef union U_MarshallApi_MdbFunctions_tag
{
	S_MarshallApi_MdbVendRequest	    MdbVendRequest;
	S_MarshallApi_MdbVendSuccess	    MdbVendSuccess;
    S_MarshallApi_MdbVendCloseSession   MdbVendCloseSession;
    S_MarshallApi_MdbVendSessionType    MdbVendSessionType;
    S_MarshallApi_MdbVendCashSale       MdbVendCashSale;
    
}U_MarshallApi_MdbFunctions;

/**
* @name 	S_MarshallApi_MdbCmd
* @brief	MDB command
*/
typedef P_STRUCT S_MarshallApi_MdbCmd_tag
{
    S_MarshallApi_MdbCmdHeader  CmdHeader;
    
    U_MarshallApi_MdbFunctions  MdbFunctions;
}S_MarshallApi_MdbCmd;


                    /************************************************/
                    /*	 M D B   C O M A N D S  (Cashless Device)   */
                    /************************************************/

/**
 * @name 	E_MarshallApi_MdbCashlessDevCode
 * @brief	The supported MDB CashlessDev code (amit -> peripheral)
 */
typedef enum    E_MarshallApi_MdbCashlessDevCode_tag
{
	MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_BEGIN_SESSION			= 0x03,
	MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_CANCEL_REQ	= 0x04,
	MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_VEND_APPROVE	= 0x05,
	MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_VEND_DENIED	= 0x06,
	MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_SESSION_END_SESSION	= 0x07,
	MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_CANCELED				= 0x08,
    
    MARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_FTL_SEND_BLOCK		= 0x1D,
   
    MMARSHALLAPI_MDB_CMD_CASHLESS_DEV_CODE_MAX,

} E_MarshallApi_MdbCashlessDevCode;

/**
* @name 	S_MarshallApi_MdbCashlessDevCmdHeader
* @brief	MDB cashless device command header
*/
typedef P_STRUCT S_MarshallApi_MdbCashlessDevCmdHeader_tag
{
    uint8_t     Cmd;
}S_MarshallApi_MdbCashlessDevCmdHeader;

/**
* @name 	S_MarshallApi_MdbCashlessDevBeginSession
* @brief	MDB cashless device begin session 
*               Allow a patron to make a selection, but do not dispense product until funds are approved.
*/
typedef P_STRUCT S_MarshallApi_MdbCashlessDevBeginSession_tag
{
    uint16_t	TransCredit;                    /** Transaction credit */

}S_MarshallApi_MdbCashlessDevBeginSession;

/**
* @name 	S_MarshallApi_MdbCashlessDevVendApprove
* @brief	MDB cashless device vend approve
*               Allow the selected product to be dispensed.
*/
typedef P_STRUCT S_MarshallApi_MdbCashlessDevVendApprove_tag
{
	uint16_t	VendAmount;                     /** This is the amount deducted from the user, it may be surcharged or discounted. */
    
}S_MarshallApi_MdbCashlessDevVendApprove;

/**
 * @name 	U_MarshallApi_MdbCashlessDevFunctions
 * @brief	MDB cashless device functions
 */
typedef union U_MarshallApi_MdbCashlessDevFunctions_tag
{
    S_MarshallApi_MdbCashlessDevBeginSession    MdbCashlessDevBeginSession;
	S_MarshallApi_MdbCashlessDevVendApprove	    MdbCashlessDevVendApprove;
    
}U_MarshallApi_MdbCashlessDevFunctions;

/**
* @name 	S_MarshallApi_MdbCashlessDevCmd
* @brief	MDB command
*/
typedef P_STRUCT S_MarshallApi_MdbCashlessDevCmd_tag
{
    S_MarshallApi_MdbCashlessDevCmdHeader  MdbCashlessDevCmdHeader;
    
    U_MarshallApi_MdbCashlessDevFunctions  MdbCashlessDevCmdFunctions;
    
}S_MarshallApi_MdbCashlessDevCmd;


					/*************************************************/
					/*	     	 P A C K E T    U N I O N			 */
					/*************************************************/
/**
 * @name 	U_MarshallApi_Functions
 * @brief	All functions parameters
 */
typedef union U_MarshallApi_Functions_tag
{
	S_MarshallApi_FirmInfoParams	    FirmInfoParams;
	S_MarshallApi_ConfigParams		    ConfigParams;
    S_MarshallApi_Status                Status;
    S_MarshallApi_DisplaysMessage       DisplaysMessage;
    S_MarshallApi_DisplaysMsgStatus     DisplaysMsgStatus;
    S_MarshallApi_TLV                   TransferData;
    S_MarshallApi_MdbCmd                MdbCmd;
    S_MarshallApi_MdbCashlessDevCmd     MdbCashlessDevCmd;
    S_MarshallApi_ModemStatus           ModemStatus;
    S_MarshallApi_OpenSocket			OpenSocket;    
    S_MarshallApi_SendData              SendData;
    S_MarshallApi_ReqData               ReqData;
    S_MarshallApi_RcvData               RcvData;
    S_MarshallApi_Trace                 Trace;
    S_MarshallApi_Alert                 Alert;
    
}U_MarshallApi_Functions;


/**
 * @name 	S_MarshallApi_Packet
 * @brief	The Marshall packet structure
 */
typedef P_STRUCT S_MarshallApi_Packet_tag
{
	S_MarshallApi_PacketHeader	Header;

	U_MarshallApi_Functions		FunctionsParams;
}S_MarshallApi_Packet;

#if !defined STM32F2XX && !defined VPOS_PORJ
	/** Return to default pack definition */
	# pragma pack ()
#endif

#endif  //__MARSHALL_API_H__


