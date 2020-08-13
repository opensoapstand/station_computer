//***************************************
//
// enums.h
// payment device packet field Definition
//
// created: 03-Oct-2019
// by: Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef ENUMS_H
#define ENUMS_H
//packet message format that are constant
enum communicationPacketField{
    STX         = 0x02,
    parent_ID   = 0x02,
    ETX         = 0x03,

    /*class ID*/
    ppPos       = 0x0D, //POS direct connect API
                        //Performs a number of financial and administrative transactions
                        //according with the rules specified by the host interface
    ppDvc       = 0x01, //Device: Device information (SN, version), reset the device, get card hash, etc.
    ppRtc       = 0x09,  //RTC API (time and date)

    /*ACK & NAK*/
    ACK         = 0x06, //indicates that the previous command was completed successfully
    NAK         = 0x15  //indicates that the previous command could not be processed
};

//Sent only on an event completion response - most will be set by the terminal
enum ErrorCode
{                                   //PCIAPI Errors
    ERR_NONE = 0,                           //Command processed normally, without errors
    ERR_PARAMS = 61001,	                    //Incorrect parameters.  The parameters supplied to the function are incorrect.
    ERR_TIMEOUT = 61003,	                //Timeout.  The API function timed out waiting for the operation to complete.
    ERR_UNKNOWN_COMMAND = 61004,    	    //Unknown function.  The PINPad did not recognise the command.
    ERR_NO_MASTER_KEY = 61054,	            //Master key is not loaded.
    ERR_RSA_LEN = 61055,                    //RSA key length is wrong.
    ERR_RSA_KEY = 61056,                	//RSA key error.
    ERR_RSA_CALC = 61057,	                //RSA calculation error.
    ERR_NO_SESSION_KEY = 61058,	            //Session key is not loaded.
    ERR_DUKPT = 61059,	                    //DUKPT’s IPPK and KSN are not loaded.
    ERR_PPAD_BUSY = 61060,	                //PINPad busy. The PINPad is busy processing another host request.
    ERR_CARD_READ = 61061,	                //Card Read Error. An error occurred reading the MCR.
    ERR_KEK = 61064,	                    //KEK Key not loaded.
    ERR_LCD_FONT = 61065,               	//LCD font is not selected.
    ERR_PPAD_NOT_INIT = 61066,          	//PINPad not online.  Factory-initialisation has not been completed.
    ERR_USER_ABORT = 61067,	                //Operation aborted by user.
    ERR_PIN_SHORT  = 61068,	                //The entered PIN is too short.
    ERR_LCD_POS = 61069,	                //LCD position is out of range for the current LCD type.
    ERR_NO_SC = 61070,	                    //No card.  There is no smartcard in the SCT.
    ERR_SC_READ = 61071,                	//Bad read from the smart card.
    ERR_SC_READ2 = 61072,               	//Card Read error.  An EEPROM read error occurred on the smartcard or SctReset has not been executed.
    ERR_SC_WRITE = 61073,	                //Card Write error.  An EEPROM write error occurred on the smartcard or SctReset has not been executed.
    ERR_SC_SYNC = 61074,	                //Sync card access error.
    ERR_PIN_ENTRY = 61075,                 	//PIN or data entry terminated by pressing CORR key with no PIN data entered
    ERR_LED = 61101,                    	//LED colour is not supported.
    ERR_CMD_TOO_LARGE = 61102,	            //Bad parameters in command (exceeded data packet size of 1000 bytes).
    ERR_MEMORY = 61103,	                    //Insufficient memory.
    ERR_MEMORY_PROMPT = 61104,          	//Not enough memory to load prompt.
    ERR_LOW_LEVEL = 61105,              	//Low level error, possibly bad parameters.
    ERR_DEV_INVALID = 61106,            	//Device not supported.
    ERR_SIG_INVALID = 61107,            	//Signature of secure prompt or secure hash of RSA incorrect.
    ERR_ACCESS = 61108,	                    //Access to specified record or key not allowed.
    ERR_SEC_PROMPT = 61109,	                //Secure prompt has to be displayed for this command.
    ERR_FONT_MISSING = 61110,	            //Non-system font is required.
    ERR_KEY_SCHEME = 61111,	                //Key management scheme is not selected.
    ERR_EMV_BUSY =  61201,                  //EMV kernel busy
    ERR_EMV_NOT_INIT = 61202,	            //EMV library has not been initialized at power-up.
    ERR_INV_ARG = 61203,	                //The function has been passed invalid arguments like NULL pointers.
    ERR_INV_EMV_OP = 61204,	                //EMV invalid operation.
    ERR_INIT_WARN = 61205,	                //Initialization has already been performed, so no action has been taken by the function.  This error is non-critical and provided for information only.
    ERR_EMV_GEN = 61206,	                //EMV general error
    ERR_EMV_MODE = 61207,	                //The requested function or mode of operation is not supported by this version of the EMV library.
    ERR_KERNEL_NOT_READY = 61208,	        //EMV kernel not ready
    ERR_INV_TAG = 61209,	                //The requested tag ID is not found in the current tag definition list.
    ERR_INV_EMV_CMD = 61210,	            //Invalid command received by the EMV library.
    ERR_HANDLE = 61211,	                    //Function called with an invalid handle.
    ERR_EMV_CONFIG_MISSING = 61212,	        //EMV library is not fully configured, or the requested configuration data is missing.
    ERR_NO_RESOURCE = 61213,	            //No resource (usually memory) to execute the function.
    ERR_EMV_CONFIG_INVALID = 61214,	        //Invalid EMV configuration data.
    ERR_CARD_DURING_TXN = 61215,	        //Card error occurred during a transaction.
    ERR_CA_INVALID = 61216,	                //Invalid CA  Key
    ERR_CA_EXPIRED = 61217,	                //Expired CA  Key
    ERR_APP_BLOCKED = 61221,	            //Card application blocked.
    ERR_CARD_BLOCKED = 61222,           	//Card blocked.
    ERR_CARD_REMOVED = 61223,	            //Card removed.
    ERR_CARD_FAULTY = 61224,	            //Card faulty.
    ERR_WRONG_CARD = 61225,	                //Wrong card.
    ERR_BAD_CRD_DATA = 61226,	            //Bad card data.
    ERR_MISSING_CARD_DATA = 61227,	        //Missing card data.
    ERR_BAD_TERM_DATA = 61228,  	        //Bad terminal data.
    ERR_BAD_RESPONSE = 61229,	            //Incorrect response.
    ERR_INV_USAGE = 61230,	                //Invalid usage of application.
    ERR_CARD_DRIVER = 61231,            	//Card driver error.
    ERR_INV_DATE_TIME= 61300,       	    //Invalid date or time passed in PpRtcSetDateTime.
    ERR_ACCT_BALANCE = 61301, 	            //Account balance did not verify.
    ERR_CARD_RDR_AUTH = 61302,	            //Card reader authentication errors
    ERR_IP_CONNECT = 61304,	                //IP connectivity error
    ERR_IP_XMIT = 61305,        	        //IP transmission error
    ERR_INIT_REQD = 61306,              	//Initialization required
    ERR_NO_SAF_TO_SEND = 61307,	            //SAF is empty
    ERR_SAF_NOT_ENABLED = 61308,	        //SAF is not enabled
    //DLL Errors
    ERR_RS232_TIMEOUT = 90000,              //A timeout condition has occurred
    ERR_TERMINAL = 90001,                   //Terminal status error (check TerminalStatus value)
    ERR_DLL_BUSY = 90002,                   //Invalid Mac value
    ERR_DLL_SYSTEM = 90003,                //This is used to flag a programatic failure
    ERR_RS232_COMM = 90004,                 //Serial communications error
    ERR_DLL_PARAM= 90005,                   //This DLL encountered an invalid parameter
    ERR_DLL_CANCEL = 90006,                 //This DLL received a Cancel request
};

enum TXN_FIDs                    // FIDs that can be returned on a transaction response
{
    SPDH_TXN_TYPE =             01,
    DATE_TIME =                 02,
    AMOUNT =                    03,
    CASH_BACK =                 04,
    PAN =                       05,
    ISO_CODE =                  06,
    HOST_CODE =                 07,
    APPROVAL_CODE =             8,
    SEQ_NUM =                   9,
    CUSTOMER_LANG =             10,
    MOP =                       11,
    ACCOUNT_TYPE =              12,
    CARD_NAME =                 13,
    PIN_FLAG =                  14,
    SAF_APPROVED =              15,
    AID =                       16,
    CHIP_LABEL =                17,
    CHIP_PREF_NAME =            18,
    ARQC =                      19,
    ARPC =                      20,
    TVR_AFTER_ARQC =            21,
    TVR_AFTER_ARPC =            22,
    FINAL_TSI =                 23,
    TERM_TOTALS =               24,
    ALL_HOST_TOTALS =           25,
    SAF_COUNT =                 26,
    SAF_AMOUNT =                27,
    TID =                       28,
    PED_ID =                    29,
    MID =                       30,
    PURCH_HOST_TOTALS =         31,
    TXN_FINAL_DISP =            32,
    INIT_RQSTD =                33,
    TXN_TRACE_NR =              34,
    COMPLETION_REQUIRED =       35,
    TOKEN_VALUE =               36,
    INVOICE_NR =                37,
    BIN =                       38,
    CLOCK_PAC =                 39,
    CTLS_FF =                   40,
    CARD_NAME2 =                41,
};


enum TXN_CODES                    // Host approve decline codes
{
    MAX_APPROVAL_CODE = 49
};

enum TXN_STATUS_CODE             // Codes denoting the success or failure of a host transaction
{
    SUCCCESS =                  '1',
    NOT_COMPLETED =             '2',
    TIMEOUT =                   '5',
    HOST_COMM_ERROR =           '9'
};

enum ACCT_TYPE               // Account Type
{
    Uknown = -1,
    Credit      = 0,
    Debit       = 01,
    Savings     = 03,
    Chequing   = 05
};

enum CONFIG_ID               // ID Codes denoting the configuration paramters
{
    INVALID = -1,
    CON_TID = 0,                        //Terminal ID. Value assigned by the acquirer
    CON_PED_ID = 1,	                    //PED ID. Value assigned by the acquirer
    CON_MID = 2,	                    //Merchant ID. Value assigned by the acquirer
    INIT_FLAG = 3,	                //Terminal initialization status (‘Y’ when all host parameters and EMV records were downloaded, ‘N’ otherwise). Writing is restricted only to ‘N’ to force terminal initialization
    TERM_LANG = 4,	                //Terminal Language (0x01 = English /  0x02 = French)
    MERCH_NAME = 5,	                //Merchant’s name as configured in the acquirer host.
    MERCH_ADDR = 6,	                //Merchant’s address as configured in the acquirer host. May be downloaded to the terminal during initialization.
    MERCH_ADD_INFO = 7,             //Merchant’s additional information as configured in the acquirer host. May be downloaded to the terminal during initialization.
    MERCH_NUM = 8,	                //Merchant’s business number as configured in the host. Downloaded to the terminal during initialization.
    GLBL_TOTALS = 9,	            //Terminal global totals over multiple batches. PpPosSetConfig can only be used to reset the global counters and totals.
    SAF_FLAG = 10,	                //SAF allowed flag  (‘Y’ / ‘N’)
    PROX_READER = 11,	            //Contactless reader enabled flag (‘Y’/’N’)
    VISA_DEBIT_FLAG = 12,	        //Visa Debit opt-out flag (‘Y’ / ‘N’). Flag downloaded during initialization and indicates whether or not the terminal supports Visa Debit application
    HOST_CONNECT_TIMEOUT = 13,      //Host connectivity timeout (in seconds). The maximum time to establish connectivity with the host. This parameter may be downloaded from the host part of the initialization process.
    HOST_RESP_TIMEOUT = 14,	        //Host response timeout (in seconds). Maximum time allowed for the host to respond. This parameter is downloaded from the host part of the initialization process.
    URL1 = 15,	                    //The first URL definition, format <host_address:port>. The address may be in IPv4 format or alphanumeric URL
    URL2 = 16,	                    //The second URL definition, format <host_address:port>. The address may be in IPv4 format or alphanumeric URL
    PRE_DIAL_FLAG = 17,	            //Pre-dial flag. Indicates if the POS application should initiate connection to the host at a stage prior to the final payment definition sequence (0x01 = pre-dial enabled)
    MAX_HOST_CONNECT_ATTEMPTS = 18,	//Maximum number of attempts to connect to the host.
    LAN_IP = 19,	                //LAN IP address. If this parameter is null then the LAN controller will use DHCP
    LAN_SUBNET_MASK = 20,	        //LAN subnet mask (static IP)
    LAN_GATEWAY_IP = 21,	        //LAN gateway IP address (static IP)
    LAN_DNS_IP = 22,	            //LAN DNS IP address (static IP)
    APP_EMV_VERSIONS = 23           //Terminal application & EMV kernel version
};


//Bit-wise flags sent on an immediate command response
//Note that these values can change over multiple events
enum TerminalStatus          //Bit flags
{
    UnknownStatus = 0,              //Status when the there is no serial communications
    ErrorStatus = 1,                //An error condition was detected
    ConnectedStatus = 2,            //There is an active serial connection with the PINPad
    BusyStatus = 4,                 //A command is in progress
    LogOnStatus = 8,                //The PINPad is logged onto the client
    SessionKeysLoaded = 16,         //Session Keys have been load - the PINPad is ready for other commands
    CardInsertedStatus = 32,        //A chip card has been inserted
    Card_Changed = 64,              //The card in use has changed
    Inactivity_Timeout = 128        //An inactivity (Idle) timeout change has occurred
};

//enum CommandAckNak
//{
//    NOT_SET = 0,
//    ACK = 1,
//    NAK = 2
//};

//API IDs for the PpPOsStatus command (Class 0x0D)
enum API_ID
{
    PpPosDirectTxn       = 0x0000,
    PpPosCancel          = 0x0001,
    PpPosStatus          = 0x0002,
    PpPosSetConfig       = 0x0003,
    PpPosGetConfig       = 0x0004,
    PpPosSafInitialize   = 0x0005,
    PpPosSetComPort      = 0x0006,
    PpPosGetMerchantData = 0x0007
};

//Status Type for the PpPOsStatus command
enum TxnType
{
    PreAuth            = 'A',
    BatchClose         = 'B',
    Completion         = 'C',
    Initialize         = 'I',
    SAF                = 'F',
    Purchase           = 'P',
    BatchTotals        = 'T',
    CompletionToken    = 'c',

    Logon              = 'L'
};

//Status Type for the PpPOsStatus command
enum StatusType
{
    GetLanStatus = 0x00,
    GetLanInfo = 0x01,
    HostConnectivityTest = 0x02,
    None = 0xff
};

enum TCommandId
{
    NO_COMMAND,
    SET_COM_PORT =                      'C',
    DIRECT_TXN =                        'D',
    SAF_INIT =                          'E',
    GET_CONFIG =                        'G',
    SET_CONFIG =                        'H',
    INDEPENDENT_EVENT =                 'I',
    MERCHANT_DATA =                     'M',
    STATUS_TXN =                        'S',
    CANCEL_COMMAND =                    'X'
};

enum Lang_ID
{
    PURCH               = 0,
    APP                 = 1,
    THANKS              = 2,
    AMT                 = 3,
    CLIENTCOPY          = 4,
    CARD                = 5,
    TIME                = 6,
    REF                 = 7,
    AUTH                = 8,
    NOT_COMPLETE        = 9,
    TRANS               = 10,
    ACCT                = 11,
    AGREE1              = 12,
    AGREE2              = 13,
    AGREE3              = 14,
    RETAIN1             = 15,
    RETAIN2             = 16,
    SAV                 = 17,
    CHQ                 = 18,
    NOT_APP             = 19,
    PIN_CVM             = 20,
    USER_CAN            = 21,
    CHIP_ERROR          = 22,
    DECL_BY_CARD        = 23,
    CHIP_SWIPED         = 24,
    CARD_REMOVED        = 25,
    ENTRY_METHOD        = 26,
    CHIP_MAG            = 27,
    SWIPED              = 28,
    CHIP                = 29,
    PIN_AGREE1          = 30,
    PIN_AGREE2          = 31,
    PIN_AGREE3          = 32,
    DBT                 = 33,
} ;


#endif // ENUMS_H
