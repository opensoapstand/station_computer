/***********************************************************************************
 * File         : Utils.h
 * Version      : 
 * Designer     : Ilan Graidy
 * Date         : 2013-06-05
 * Description  : This library implements all kind of needed functions
 *
 ************************************************************************************/

#ifndef __UTILS_H_
#define __UTILS_H_

#include<stdint.h>
/************************************** Defines ************************************/
#define UTILS_POLYNOMIAL_CCITT         0x1021   /* x^16 + x^12 + x^5 + 1 */
#define UTILS_SEED_CCITT               0





/************************************** Prototypes *********************************/
/**
* @brief  :This function Return CRC-CCITT calculation for an entire buffer
* @param  :*pData  - Pointer to a buffer
*          Len     - Buffer length
* @retval :New CRC
*/
uint16_t Utils_Calc_CRC_CCITT(uint8_t *pData, uint32_t Len, uint16_t Seed);

/**
* @brief  :This function Convert a byte to an hex ascii format
* @param  :Byte          - Byte
*          pBuff         - Pointer to a buffer
* @retval :none
*/
void Utils_ByteToXAscii(uint8_t Byte, uint8_t *pBuff);

#endif
