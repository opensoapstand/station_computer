/***********************************************************************************
 * File         : Utils.c
 * Version      : 
 * Designer     : 
 * Date         : 
 * Description  : This library implements all kind of needed functions
 *
 ************************************************************************************/

#include "stdafx.h"
#include "Hal.h"
#include "Utils.h"

/**
* @brief  :This function Return CRC-CCITT calculation based on current CRC value and a given byte
* @param  :Crc  - Current CRC
*          Byte - Current byte to insert to the Crc
* @retval :New CRC
*/
static uint16_t Utils_CRC_CCITT ( uint16_t Crc, uint8_t Byte )
{
    uint8_t i;
    uint16_t Temp, ShortC, CurrentCRC = UTILS_SEED_CCITT;
    
    ShortC = (Byte & 0x00FF);
    Temp = ((Crc >> 8) ^ ShortC) << 8;
    for ( i = 0; i < 8; i++ )
    {
        if ( ((CurrentCRC ^ Temp) & 0x8000) )
            CurrentCRC = ((CurrentCRC << 1) ^ UTILS_POLYNOMIAL_CCITT); /* 1010.0000 0000.0001 = x^16 + x^15 + x^13 + 1 */
        else
            CurrentCRC <<= 1;
        Temp <<= 1;
    }
    
    return ( (Crc << 8) ^ CurrentCRC );
}

/**
* @brief  :This function Return CRC-CCITT calculation for an entire buffer
* @param  :*pData  - Pointer to a buffer
*          Len     - Buffer length
* @retval :New CRC
*/
uint16_t Utils_Calc_CRC_CCITT ( uint8_t *pData, uint32_t Len, uint16_t Seed )
{
    register uint32_t i;
    uint16_t Crc = Seed;
    
    for ( i = 0; i < Len; i++ )
        Crc = Utils_CRC_CCITT(Crc,pData[i]);
    
    return ( Crc );
}

/**
* @brief  :This function Convert a byte to an hex ascii format
* @param  :Byte          - Byte
*          pBuff         - Pointer to a buffer
* @retval :none
*/
void Utils_ByteToXAscii(uint8_t Byte, uint8_t *pBuff)
{
    uint8_t i;
    char NextChar;

    for (i = 2; i > 0; i--)
    {
        NextChar = (Byte & 0xf);
        if (NextChar <= 9)
        {
            NextChar += '0';
        }
        else
        {
            NextChar += ('A' - 10);
        }
        pBuff[i - 1] = NextChar;
        Byte >>= 4;
    }
}
