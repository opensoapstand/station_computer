#include <iostream>
#include"Hal.h"
#include "Peripheral.h"
using namespace std;

void stringStatus(int status); //function prototype
static void UpdateLog(char * pStr, uint32_t Src);

/************************************************/
/*				  D E F I N E S					*/
/************************************************/
#define SIMULATOR_MAX_LOADSTRING                    100
#define SIMULATOR_LOG_MAX_SIZE                      10000
#define SIMULATOR_PERIPHERAL_TYPE				    MARSHALLAPI_INFO_TYPE_BEVERAGE_MACHINE
#define SIMULATOR_PERIPHERAL_SUB_TYPE				MARSHALLAPI_INFO_BEVERAGE_MACHINE_SUB_TYPE_1

static uint8_t  PeripheralComID = 0;
pthread_t    PeripheralThreadId;
uint16_t    PerCapabilities = 0x2020;

int main()
{
    uint8_t peripheralModel[20] = {0x44,0x52,0x49,0x4e,0x4b,0x46,0x49,0x4c,0x4c,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
    E_Status_Ret currentStatus;
    S_Marshall_PeripheralInfo ev2;
    ev2.Type = 5;
    ev2.SubType= 0;
    ev2.Capabilities = 0x2020;
    int i = 1;
    while(i<=20){
        ev2.Model[i-1] = peripheralModel[i-1];
        ev2.SerialNum[i-1]= peripheralModel[i-1];
        ev2.SwVer[i-1] = peripheralModel[i-1];
        i++;
    }

    PeripheralThreadId = Peripheral_Init(PeripheralComID, SIMULATOR_PERIPHERAL_TYPE, SIMULATOR_PERIPHERAL_SUB_TYPE, PerCapabilities);


    sleep(100);

    return 0;
}

void stringStatus(int status)
{
    switch (status){
        case 0: cout << "STATUS_RET_SUCCESS" << endl; break;
        case 1: cout << "STATUS_RET_FAIL" << endl; break;
        case 2: cout << "STATUS_RET_UNEXPECTED_ERROR" << endl; break;
        case 3: cout << "STATUS_RET_NOT_SUPPORTED_PARAM_IN" << endl; break;
        case 4: cout << "STATUS_RET_NO_AVAILABLE_RESOURCE" << endl; break;
        case 5: cout << "STATUS_RET_ERROR_PARAM_IN" << endl; break;
        case 6: cout << "STATUS_RET_TIMEOUT" << endl; break;
        case 7: cout << "STATUS_RET_MODULE_ALREADY_OPEN" << endl; break;
        case 8: cout << "STATUS_RET_MODULE_IS_CLOSE" << endl; break;
        case 9: cout << "STATUS_RET_BUFF_ALLOCATION_ERROR" << endl; break;
        case 10: cout << "STATUS_RET_BUFF_EMPTY" << endl; break;
        case 11: cout << "STATUS_RET_BUFF_FULL" << endl; break;
        case 12: cout << "STATUS_RET_RCV_PACKET_OK" << endl; break;
        case 13: cout << "STATUS_RET_NO_PACKET_AVAILABLE" << endl; break;
        case 14: cout << "STATUS_RET_PACKET_CRC_ERROR" << endl; break;
        case 15: cout << "STATUS_RET_COM_ERROR" << endl; break;
        case 16: cout << "STATUS_RET_COM_CHANNEL_IS_OPEN" << endl; break;
        case 17: cout << "STATUS_RET_COM_CHANNEL_IS_CLOSE" << endl; break;
        case 18: cout << "STATmarshallUS_RET_COM_CHANNEL_UNABLE_TO_OPEN_COM" << endl; break;
        case 19: cout << "STATUS_RET_COM_CHANNEL_UNABLE_TO_CONFIG_COM" << endl; break;
        case 20: cout << "STATUS_RET_COM_CHANNEL_NOT_CONNECTED" << endl; break;
        case 21: cout << "STATUS_RET_FAIL_BECAUSE_OF_RESET" << endl; break;
        default: cout << "NOT RECOGNIZED STATUS" << endl; break;
    }
}
