/*******************************************************************************
 * Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */

/* **** Includes **** */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "DS2485.h"
#include "one_wire.h"

/* **** Definitions **** */
#define I2C_MASTER	"/dev/i2c-1"
#define I2C_MASTER_IDX	0
#define DS2485Q_ADDRESS  0b1000000

typedef unsigned char u_char;
typedef unsigned char uint8_t;

/* **** Prototypes **** */
int GetI2CPortHandle (void);

/* **** Functions **** */
int I2C_MasterWrite (int i2c_port,
		     unsigned char packet[],
		     int packetsize)
{
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
    
    messages[0].addr = DS2485Q_ADDRESS;
    messages[0].flags = 0;
    messages[0].len = packetsize;
    messages[0].buf = packet;
    
    packets.msgs  = messages;
    packets.nmsgs = 1;
    if (ioctl(i2c_port, I2C_RDWR, &packets) < 0) 
    {
	fprintf (stderr, "I2C_MasterWrite: %s\n", strerror(errno));
	return -1;
    }

    return packetsize;

}  // End of I2C_MasterWrite()


int I2C_MasterRead (int i2c_port,
		    unsigned char packet[],
		    int packetsize)
{
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    messages[0].addr = DS2485Q_ADDRESS;
    messages[0].flags = I2C_M_RD;
    messages[0].len = packetsize;
    messages[0].buf = packet;
    
    packets.msgs  = messages;
    packets.nmsgs = 1;
    if (ioctl(i2c_port, I2C_RDWR, &packets) < 0) 
    {
	fprintf (stderr, "I2C_MasterRead: %s\n", strerror(errno));
	return -1;
    }

    return packetsize;
    
}  // End of I2C_MasterRead()


int ExecuteCommand(u_char *packet, int packetSize, int delay, u_char *response, int responseSize)
{
    int error = 0;
    int i2c_port;

    // Get the port handle
    i2c_port = GetI2CPortHandle();
    if (i2c_port == 0)
    {
	return -1;
    }

    // Send the packet
    if((error = I2C_MasterWrite(i2c_port, packet, packetSize)) != packetSize)
    {
        return error;
    }

    // Wait the required delay time
    usleep (delay);

    //Read out Length Byte
    if((error = I2C_MasterRead(i2c_port, response, responseSize)) != responseSize)
    {
        return error;
    }

    return 0;
}

/* **** Device Function Commands **** */
int DS2485_WriteMemory(memory_page pgNumber, u_char *pgData)
{
    int error = 0;

    //Command specific variables
    const int txLength = 35;
    const int delay = tWM;
    const int rxLength = 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_WRITE_MEMORY; 				 // Command
    packet[1] = sizeof(packet) - 2;  			 		 // Command length byte
    packet[2] = pgNumber; 						 // Parameter
    memcpy(&packet[3], &pgData[0], 32);          // Data

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x55:
	error = RB_WRITE_PROTECTED;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_ReadMemory(memory_page pgNumber, u_char *pgData)
{
    int error = 0;

    //Command specific variables
    const int txLength = 3;
    const int delay = tRM;
    const int rxLength = 34;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_READ_MEMORY; 				 // Command
    packet[1] = sizeof(packet) - 2;  			 		 // Command length byte
    packet[2] = pgNumber; 						 // Parameter

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch page data from response
    memcpy(&pgData[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_ReadStatus(status_outputs output, u_char *status)
{
    int error = 0;

    //Command specific variables
    const int txLength = 3;
    const int delay = tRM;
    int rxLength;

    switch (output){
    case PAGE_PROTECTIONS:
	rxLength = 8;
	break;
    default:
	rxLength = 4;
	break;
    }

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_READ_STATUS; 				 // Command
    packet[1] = sizeof(packet) - 2;  			 		 // Command length byte
    packet[2] = output; 						 // Parameter

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch status data from response
    memcpy(&status[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_SetI2cAddress(u_char newAddress)
{
    int error = 0;

    //Command specific variables
    const int txLength = 3;
    const int delay = tWS;
    const int rxLength = 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_SET_I2C_ADDRESS; 			 // Command
    packet[1] = sizeof(packet) - 2;  			 		 // Command length byte
    packet[2] = newAddress << 1; 				 // Parameter

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x55:
	error = RB_SET_ADDRESS_FAIL;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_SetPageProtection(memory_page pgNumber, page_protection protection)
{
    int error = 0;

    //Command specific variables
    const int txLength = 4;
    const int delay = tWS;
    const int rxLength = 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_SET_PAGE_PROTECTION; 	     // Command
    packet[1] = sizeof(packet) - 2;  			 		 // Command length byte
    packet[2] = pgNumber; 				         // Parameter
    packet[3] = protection; 				 	 // Parameter

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x55:
	error = RB_ALREADY_PROTECTED;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_ReadOneWirePortConfig(configuration_register reg, u_char *regData)
{
    int error = 0;

    //Command specific variables
    const int txLength = 3;
    const int delay = tOP;
    int rxLength;

    if (reg > 0x13)
    {
	rxLength = 42;
    }
    else
    {
	rxLength = 4;
    }

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_READ_ONE_WIRE_PORT_CONFIG; 	 // Command
    packet[1] = sizeof(packet) - 2;  			 // Command length byte
    packet[2] = reg; 						     // Parameter

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch status data from response
    memcpy(&regData[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_WriteOneWirePortConfig(configuration_register reg, u_char *regData)
{
    int error = 0;

    //Command specific variables
    const int txLength = 5;
    const int delay = tOP + 1000;
    const int rxLength = 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_WRITE_ONE_WIRE_PORT_CONFIG; 	// Command
    packet[1] = sizeof(packet) - 2;  			 	// Command length byte
    packet[2] = reg; 				 				// Parameter
    packet[3] = regData[0]; 				 		// Data
    packet[4] = regData[1]; 				 		// Data


    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_MasterReset(void)
{
    int error = 0;

    //Command specific variables
    const int txLength = 1;
    const int delay = tOP;
    const int rxLength = 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_MASTER_RESET; 			 // Command

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x22:
	error = RB_MASTER_RESET_FAIL;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_OneWireScript(u_char *script, u_char script_length, double accumulativeOneWireTime, u_char commandsCount, u_char *scriptResponse, u_char scriptResponse_length)
{
    int error = 0;

    //Command specific variables
    const int txLength = script_length + 2;
    const int delay = tOP + (tSEQ*(commandsCount)) + accumulativeOneWireTime + 1000;
    const int rxLength = scriptResponse_length + 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_ONE_WIRE_SCRIPT; 			 					// Command
    packet[1] = sizeof(packet) - 2;  			 					// Command length byte
    memcpy(&packet[2], &script[0], script_length);        			// Primitive commands + data + parameters = script

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch page CRC16 from response
    memcpy(&scriptResponse[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    case 0x22:
	error = RB_COMMS_FAIL;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}


int DS2485_OneWireBlock(u_char *blockData, int blockData_Length, u_char *ow_data, bool ow_reset, bool ignore, bool spu, bool pe)
{
    int error = 0;
    one_wire_speeds master_speed;

    // Delay variables
    double one_wire_time;
    double ow_rst_time;
    double t_slot;

    // Timings
    double t_rstl;
    double t_rsth;
    double t_w0l;
    double t_rec;

    /***** Fetch timings *****/
    if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
    {
	return error;
    }

    if(master_speed != STANDARD)
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
	{
	    return error;
	}
    }
    else
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
	{
	    return error;
	}
    }

    //'1-Wire time'
    t_slot = t_w0l + t_rec;					//Time it takes to complete a 1-Wire Write/Read bit time slot
    ow_rst_time = t_rstl + t_rsth;			//Time it takes to complete a 1-Wire Reset slot
    one_wire_time = ((t_slot * 8) * (blockData_Length));
    if(ow_reset)
    {
	one_wire_time += ow_rst_time;
    }

    //Command specific variables
    const int txLength = blockData_Length + 3;
    const int delay = tOP + (tSEQ*(blockData_Length + ow_reset)) + one_wire_time;
    const int rxLength = blockData_Length + 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_ONE_WIRE_BLOCK; 			 							// Command
    packet[1] = sizeof(packet) - 2; 			 							// Command length byte
    packet[2] = (pe << 3) | (spu << 2) | (ignore << 1) | (ow_reset << 0);   // Parameter byte
    memcpy(&packet[3], &blockData[0], blockData_Length);        			// Data

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch page CRC16 from response
    memcpy(&ow_data[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    case 0x22:
	error = RB_COMMS_FAIL;
	break;

    case 0x33:
	error = RB_NOT_DETECTED;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_OneWireWriteBlock(u_char *writeData, int writeData_Length, bool ow_reset, bool ignore, bool spu)
{
    int error = 0;
    one_wire_speeds master_speed;

    // Delay variables
    double one_wire_time;
    double ow_rst_time;
    double t_slot;

    // Timings
    double t_rstl;
    double t_rsth;
    double t_w0l;
    double t_rec;

    /***** Fetch timings *****/
    if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
    {
	return error;
    }

    if(master_speed != STANDARD)
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
	{
	    return error;
	}
    }
    else
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
	{
	    return error;
	}
    }

    //'1-Wire time'
    t_slot = t_w0l + t_rec;					//Time it takes to complete a 1-Wire Write/Read bit time slot
    ow_rst_time = t_rstl + t_rsth; 			//Time it takes to complete a 1-Wire Reset slot
    one_wire_time = ((t_slot * 8) * (writeData_Length));
    if(ow_reset)
    {
	one_wire_time += ow_rst_time;
    }


    //Command specific variables
    const int txLength = writeData_Length + 3;
    const int delay = tOP + (tSEQ*(writeData_Length + ow_reset)) + one_wire_time;
    const int rxLength = 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_ONE_WIRE_WRITE_BLOCK; 			 		 	// Command
    packet[1] = sizeof(packet) - 2;		 					 	// Command length byte
    packet[2] = (spu << 2) | (ignore << 1) | (ow_reset << 0);   // Parameter byte
    memcpy(&packet[3], &writeData[0], writeData_Length);     	// Data

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x22:
	error = RB_COMMS_FAIL;
	break;

    case 0x33:
	error = RB_NO_PRESENCE;
	break;

    case 0x00:
	error = RB_NO_MATCH_WRITES;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_OneWireReadBlock(u_char *readData, u_char bytes)
{
    int error = 0;
    one_wire_speeds master_speed;

    // Delay variables
    double one_wire_time;
    double t_slot;

    // Timings
    double t_rstl;
    double t_rsth;
    double t_w0l;
    double t_rec;

    /***** Fetch timings *****/
    if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
    {
	return error;
    }

    if(master_speed != STANDARD)
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
	{
	    return error;
	}
    }
    else
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
	{
	    return error;
	}
    }

    //'1-Wire time'
    t_slot = t_w0l + t_rec;					//Time it takes to complete a 1-Wire Write/Read bit time slot
    one_wire_time = ((t_slot * 8) * (bytes));


    //Command specific variables
    const int txLength = 3;
    const int delay = tOP + (tSEQ*(bytes)) + one_wire_time;
    const int rxLength = bytes + 2;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_ONE_WIRE_READ_BLOCK; 			 		// Command
    packet[1] = sizeof(packet) - 2; 			            // Command length byte
    packet[2] = bytes;  			 				        // Parameter Byte

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch read data from response
    memcpy(&readData[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x22:
	error = RB_COMMS_FAIL;
	break;

    case 0x77:
	error = RB_INVALID_LENGTH;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_OneWireSearch(u_char *romId, u_char code, bool ow_reset, bool ignore, bool search_rst, bool *flag)
{
    int error = 0;
    one_wire_speeds master_speed;

    // Delay variables
    double one_wire_time;
    double ow_rst_time;
    double t_slot;

    // Timings
    double t_rstl;
    double t_rsth;
    double t_w0l;
    double t_rec;

    /***** Fetch timings *****/
    if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
    {
	return error;
    }

    if(master_speed != STANDARD)
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
	{
	    return error;
	}
    }
    else
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
	{
	    return error;
	}
    }

    //'1-Wire time'
    t_slot = t_w0l + t_rec;					//Time it takes to complete a 1-Wire Write/Read bit time slot
    ow_rst_time = t_rstl + t_rsth;			//Time it takes to complete a 1-Wire Reset slot
    one_wire_time = ((t_slot * 8) * (64));
    if(ow_reset)
    {
	one_wire_time += ow_rst_time;
    }


    //Command specific variables
    const int txLength = 4;
    const int delay = tOP + (tSEQ*(64 + ow_reset)) + one_wire_time;
    const int rxLength = 11;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_ONE_WIRE_SEARCH; 			 		 				// Command
    packet[1] = sizeof(packet) - 2;  			 						// Command length byte
    packet[2] = (search_rst << 2) | (ignore << 1) | (ow_reset << 0);   	// Parameter byte
    packet[3] = code;													// Search command code

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch ROM ID + last device flag from response
    memcpy(&romId[0], &response[2], 8);
    *flag = response[10];

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x00:
	error = RB_NOT_DETECTED;
	break;

    case 0x33:
	error = RB_NO_PRESENCE;
	break;

    case 0x77:
	error = RB_INVALID_PARAMETER;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

int DS2485_FullCommandSequence(u_char *owData, int owData_Length, u_char *rom_id, full_command_sequence_delays ow_delay, u_char *ow_rslt_data, u_char ow_rslt_len)
{
    int error = 0;
    one_wire_speeds master_speed;

    // Delay variables
    double one_wire_time;
    double ow_rst_time;
    double t_slot;

    // Timings
    double t_rstl;
    double t_rsth;
    double t_w0l;
    double t_rec;

    /***** Fetch timings *****/
    if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
    {
	return error;
    }

    if(master_speed != STANDARD)
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
	{
	    return error;
	}
    }
    else
    {
	if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
	{
	    return error;
	}
	if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
	{
	    return error;
	}
    }

    //'1-Wire time'
    t_slot = t_w0l + t_rec;					//Time it takes to complete a 1-Wire Write/Read bit time slot
    ow_rst_time = t_rstl + t_rsth;          //Time it takes to complete a 1-Wire Reset slot
    one_wire_time = ow_rst_time + ((t_slot * 8) * (18 + owData_Length + ow_rslt_len));

    //Command specific variables
    const int txLength = owData_Length + 11;
    const int delay = tOP + (tSEQ*(19 + owData_Length + ow_rslt_len)) + one_wire_time + (ow_delay * 2000);
    const int rxLength = ow_rslt_len + 3;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_FULL_COMMAND_SEQUENCE; 			 		 			// Command
    packet[1] = sizeof(packet) - 2;  			 						// Command length byte
    packet[2] = ow_delay;   											// Parameter byte
    memcpy(&packet[3], &rom_id[0], 8);									// ROM ID for Match ROM
    memcpy(&packet[11], &owData[0], owData_Length);						// 1-Wire Data

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch OW_RSLT_DATA from response
    memcpy(&ow_rslt_data[0], &response[3], sizeof(response) - 3);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x00:
	error = RB_INCORRECT_CRC;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}


int DS2485_ComputeCrc16(u_char *crcData, int crcData_Length, u_char *crc16)
{
    int error = 0;

    //Command specific variables
    const int txLength = crcData_Length + 2;
    const int delay = tOP;
    const int rxLength = 4;

    uint8_t packet[txLength];
    uint8_t response[rxLength];

    //Build command packet
    packet[0] = DFC_COMPUTE_CRC16; 			 				// Command
    packet[1] = sizeof(packet) - 2;  			 				// Command length byte
    memcpy(&packet[2], &crcData[0], crcData_Length);        // Data

    //Execute Command
    if ((error = ExecuteCommand(packet, sizeof(packet), delay, response, sizeof(response))) != 0)
    {
	return error;
    }

    //Fetch CRC16 from response
    memcpy(&crc16[0], &response[2], sizeof(response) - 2);

    switch (response[1]) {
    case 0xAA:
	error = RB_SUCCESS;
	break;

    case 0x77:
	error = RB_INVALID_LENGTH;
	break;

    case 0xFF:
	error = RB_LENGTH_MISMATCH;
	break;

    default:
	error = RB_UNKNOWN;
	break;
    }

    return error;
}

///* **** Primitive Commands Functions **** */
//int OneWire_Script_Execute(void)
//{
//	int error = 0;
//
//	if((error = DS2485_OneWireScript(oneWireScript, oneWireScript_length, oneWireScript_accumulativeOneWireTime, oneWireScript_commandsCount, oneWireScriptResponse, oneWireScriptResponse_length)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//
//}
//int OneWire_Script_Add_OW_RESET(u_char *response_index, one_wire_speeds spd, bool ignore)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double ow_rst_time;
//	double t_rstl;
//	double t_rsth;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_RESET;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = ((spd ^ 1) << 7) | (spd << 3) | (ignore << 1);
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count *****/
//	oneWireScript_commandsCount++;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	// Fetch timings
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//	//Calculate 1-Wire time
//	ow_rst_time = t_rstl + t_rsth;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += ow_rst_time;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_WRITE_BIT(u_char *response_index, bool bit_value)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_WRITE_BIT;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = bit_value;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
////	oneWireScript_commandsCount++;			// omitted
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += t_slot;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_READ_BIT(u_char *response_index)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_READ_BIT;
//
//	/***** Command parameter *****/
//	//No parameter
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
////	oneWireScript_commandsCount++;			// omitted
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += t_slot;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_WRITE_BYTE(u_char *response_index, u_char txByte)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_WRITE_BYTE;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = txByte;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount++;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += byte_time;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_READ_BYTE(u_char *response_index)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_READ_BYTE;
//
//	/***** Command parameter *****/
//	//no parameter
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount++;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += byte_time;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_TRIPLET(u_char *response_index, bool t_value)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_TRIPLET;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = t_value;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
////	oneWireScript_commandsCount++;			// omitted
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += t_slot;
//
//	return error;
//}
//
//int OneWire_Script_Add_OV_SKIP(u_char *response_index)
//{
//	int error = 0;
//
//	// Delay variables
//	double standard_ow_rst_time;
//	double overdrive_ow_rst_time;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double standard_t_w0l;
//	double standard_t_rec;
//	double standard_t_rstl;
//	double standard_t_rsth;
//	double overdrive_t_rstl;
//	double overdrive_t_rsth;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_OV_SKIP;
//
//	/***** Command parameter *****/
//	//No parameter
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount += 3; //1-Wire STD reset + Overdrive Skip Command + 1-Wire OV reset
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_tRSTH(&overdrive_t_rsth, OVERDRIVE)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tRSTL(&overdrive_t_rstl, OVERDRIVE)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tW0L(&standard_t_w0l, STANDARD)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tREC(&standard_t_rec, STANDARD)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tRSTH(&standard_t_rsth, STANDARD)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tRSTL(&standard_t_rstl, STANDARD)) != 0)
//	{
//		return error;
//	}
//
//	//Calculate 1-Wire time
//	t_slot = standard_t_w0l + standard_t_rec;
//	byte_time = 8 * t_slot;
//	standard_ow_rst_time = standard_t_rsth + standard_t_rstl;
//	overdrive_ow_rst_time = overdrive_t_rsth + overdrive_t_rstl;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += standard_ow_rst_time + byte_time + overdrive_ow_rst_time + 2000;
//
//	return error;
//}
//
//int OneWire_Script_Add_SKIP(u_char *response_index)
//{
//	int error = 0;
//
//	// Delay variables
//	double ow_rst_time;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//	double t_rstl;
//	double t_rsth;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_SKIP;
//
//	/***** Command parameter *****/
//	//No parameter
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount += 2; //1-Wire STD reset + Skip ROM Command
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
//	{
//		return error;
//	}
//	if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
//	{
//		return error;
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//	ow_rst_time = t_rsth + t_rstl;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += ow_rst_time + byte_time;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_READ_BLOCK(u_char *response_index, u_char rxBytes)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_READ_BLOCK;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = rxBytes;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += rxBytes + 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount += rxBytes;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += byte_time * rxBytes;
//
//	return error;
//}
//
//int OneWire_Script_Add_OW_WRITE_BLOCK(u_char *response_index, u_char *txData, u_char txData_length)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_OW_WRITE_BLOCK;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = txData_length;
//	for(int i = 0; i < txData_length; i++)
//	{
//		oneWireScript[oneWireScript_length++] = txData[i];
//	}
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount += txData_length;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += byte_time * txData_length;
//
//	return error;
//}
//
//void OneWire_Script_Add_DELAY(u_char ms)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_DELAY;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = ms;
//
//	/***** Add expected response size to total response length *****/
////	*response_index = oneWireScriptResponse_length; //ommited
////	oneWireScriptResponse_length += 2; //ommited
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	oneWireScript_accumulativeOneWireTime += ms * 1000;
//}
//
//void OneWire_Script_Add_PRIME_SPU(void)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_PRIME_SPU;
//
//	/***** Command parameter *****/
//	//ommited
//
//	/***** Add expected response size to total response length *****/
//	//ommited
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//void OneWire_Script_Add_SPU_OFF(void)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_SPU_OFF;
//
//	/***** Command parameter *****/
//	//ommited
//
//	/***** Add expected response size to total response length *****/
//	//ommited
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//int OneWire_Script_Add_SPEED(one_wire_speeds spd, bool ignore)
//{
//	int error = 0;
//
//	// Delay variables
//	double ow_rst_time;
//
//	// Timings
//	double t_rstl;
//	double t_rsth;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_SPEED;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = ((spd ^ 1) << 7) | (spd << 3) | (ignore << 1);
//
//	/***** Add expected response size to total response length *****/
//	//ommited
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount++;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if(spd != STANDARD)
//	{
//		if ((error = OneWire_Get_tRSTH(&t_rsth, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tRSTL(&t_rstl, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tRSTH(&t_rsth, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tRSTL(&t_rstl, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	ow_rst_time = t_rstl + t_rsth;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += ow_rst_time;
//
//	return error;
//}
//
//int OneWire_Script_Add_VERIFY_TOGGLE(u_char *response_index)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_VERIFY_TOGGLE;
//
//	/***** Command parameter *****/
//	//ommited
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount++;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += byte_time;
//
//	return error;
//}
//
//int OneWire_Script_Add_VERIFY_BYTE(u_char *response_index, u_char byte)
//{
//	int error = 0;
//
//	// Delay variables
//	one_wire_speeds master_speed;
//	double t_slot;
//	double byte_time;
//
//	// Timings
//	double t_w0l;
//	double t_rec;
//
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_VERIFY_BYTE;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = byte;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//	oneWireScript_commandsCount++;
//
//	/***** Add accumulative 1-Wire time in us *****/
//	/***** Fetch timings *****/
//	if ((error = OneWire_Get_OneWireMasterSpeed(&master_speed)) != 0)
//	{
//		return error;
//	}
//
//	if(master_speed != STANDARD)
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, OVERDRIVE)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if ((error = OneWire_Get_tW0L(&t_w0l, STANDARD)) != 0)
//		{
//			return error;
//		}
//		if ((error = OneWire_Get_tREC(&t_rec, STANDARD)) != 0)
//		{
//			return error;
//		}
//	}
//
//	//Calculate 1-Wire time
//	t_slot = t_w0l + t_rec;
//	byte_time = 8 * t_slot;
//
//	//Add to total 1-Wire time
//	oneWireScript_accumulativeOneWireTime += byte_time;
//
//	return error;
//}
//
//void OneWire_Script_Add_CRC16_START(void)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_CRC16_START;
//
//	/***** Command parameter *****/
//	//ommited
//
//	/***** Add expected response size to total response length *****/
//	//ommited
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//void OneWire_Script_Add_VERIFY_CRC16(u_char *response_index, u_short hex_value)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_VERIFY_CRC16;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = (u_char)hex_value; //LSB
//	oneWireScript[oneWireScript_length++] = (u_char)(hex_value >> 8); //MSB
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//void OneWire_Script_Add_SET_GPIO(u_char *response_index, gpio_settings pioac)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_SET_GPIO;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = pioac;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//void OneWire_Script_Add_READ_GPIO(u_char *response_index)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_READ_GPIO;
//
//	/***** Command parameter *****/
//	//ommited
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//void OneWire_Script_Add_VERIFY_GPIO(u_char *response_index, gpio_verify_level_detection pioal)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_VERIFY_GPIO;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = pioal;
//
//	/***** Add expected response size to total response length *****/
//	*response_index = oneWireScriptResponse_length;
//	oneWireScriptResponse_length += 2;
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
//void OneWire_Script_Add_CONFIG_RPUP_BUF(u_short hex_value)
//{
//	/***** Command code *****/
//	oneWireScript[oneWireScript_length++] = PC_CONFIG_RPUP_BUF;
//
//	/***** Command parameter *****/
//	oneWireScript[oneWireScript_length++] = (u_char)hex_value; //LSB
//	oneWireScript[oneWireScript_length++] = (u_char)(hex_value >> 8); //MSB
//
//	/***** Add expected response size to total response length *****/
//	//ommited
//
//	/***** Add 1-Wire actions to total command count (only applies to 1-Wire reset, and 1-Wire bytes) *****/
//
//	/***** Add accumulative 1-Wire time in us *****/
//	//ommited
//}
//
///* **** High Level Functions **** */
//int OneWire_Enable_APU(bool apu)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (apu)
//	{
//		if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		reg_data[0] = 0x00;
//		reg_data[1] |= 0x10;
//
//		if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//	}
//	else
//	{
//		if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		reg_data[0] = 0x00;
//		reg_data[1] &= ~(0x10);
//
//		if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//	}
//	return error;
//}
//
//int OneWire_Enable_SPU(bool spu)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (spu)
//	{
//		if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		reg_data[0] = 0x00;
//		reg_data[1] |= 0x20;
//
//		if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//	}
//	else
//	{
//		if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		reg_data[0] = 0x00;
//		reg_data[1] &= ~(0x20);
//
//		if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//	}
//	return error;
//}
//
//int OneWire_Enable_OneWirePowerDown(bool pdn)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (pdn)
//	{
//		if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		reg_data[0] = 0x00;
//		reg_data[1] |= 0x40;
//
//		if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//	}
//	else
//	{
//		if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		reg_data[0] = 0x00;
//		reg_data[1] &= ~(0x40);
//
//		if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//		{
//			return error;
//		}
//	}
//	return error;
//}
//
//int OneWire_Set_OneWireMasterSpeed(one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	reg_data[0] = 0x00;
//	reg_data[1] = ((reg_data[1] & ~0x80) | (spd << 7));
//
//	if((error = DS2485_WriteOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//
//int OneWire_Get_OneWireMasterSpeed(one_wire_speeds *spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if((error = DS2485_ReadOneWirePortConfig(MASTER_CONFIGURATION, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	if((reg_data[1] >> 7) != STANDARD)
//	{
//		*spd = OVERDRIVE;
//	}
//	else
//	{
//		*spd = STANDARD;
//	}
//
//    return error;
//}
//
//int OneWire_Set_tRSTL_Standard_Predefined(one_wire_timing_presets trstl)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = trstl;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tRSTL, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tRSTL_Overdrive_Predefined(one_wire_timing_presets trstl)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = trstl;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tRSTL, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tRSTL_Standard_Custom(double trstl)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (trstl > 1020)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	trstl *= 1000; //us -> ns
//	trstl /= (double)62.5;
//	trstl = ((int)trstl | 0x8000);
//	reg_data[0] = (u_char)trstl;
//	reg_data[1] = ((int)trstl >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tRSTL, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tRSTL_Overdrive_Custom(double trstl)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (trstl > 126)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	trstl *= 1000; //us -> ns
//	trstl /= (double)62.5;
//	trstl = ((int)trstl | 0x8000);
//	reg_data[0] = (u_char)trstl;
//	reg_data[1] = ((int)trstl >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tRSTL, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tRSTL(double *trstl, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tRSTL, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*trstl = (double)value;
//			*trstl *= (double)62.5;
//			*trstl /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*trstl = tRSTL_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*trstl = tRSTL_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*trstl = tRSTL_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*trstl = tRSTL_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*trstl = tRSTL_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*trstl = tRSTL_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*trstl = tRSTL_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*trstl = tRSTL_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*trstl = tRSTL_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*trstl = tRSTL_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*trstl = tRSTL_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*trstl = tRSTL_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*trstl = tRSTL_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*trstl = tRSTL_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*trstl = tRSTL_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*trstl = tRSTL_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*trstl = tRSTL_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tRSTL, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*trstl = (double)value;
//			*trstl *= (double)62.5;
//			*trstl /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*trstl = tRSTL_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*trstl = tRSTL_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*trstl = tRSTL_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*trstl = tRSTL_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*trstl = tRSTL_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*trstl = tRSTL_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*trstl = tRSTL_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*trstl = tRSTL_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*trstl = tRSTL_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*trstl = tRSTL_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*trstl = tRSTL_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*trstl = tRSTL_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*trstl = tRSTL_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*trstl = tRSTL_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*trstl = tRSTL_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*trstl = tRSTL_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*trstl = tRSTL_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//int OneWire_Set_tRSTH_Standard_Predefined(one_wire_timing_presets trsth)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = trsth;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tRSTH, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tRSTH_Overdrive_Predefined(one_wire_timing_presets trsth)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = trsth;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tRSTH, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tRSTH_Standard_Custom(double trsth)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (trsth > 1020)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	trsth *= 1000; //us -> ns
//	trsth /= (double)62.5;
//	trsth = ((int)trsth | 0x8000);
//	reg_data[0] = (u_char)trsth;
//	reg_data[1] = ((int)trsth >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tRSTH, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tRSTH_Overdrive_Custom(double trsth)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (trsth > 126)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	trsth *= 1000; //us -> ns
//	trsth /= (double)62.5;
//	trsth = ((int)trsth | 0x8000);
//	reg_data[0] = (u_char)trsth;
//	reg_data[1] = ((int)trsth >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tRSTH, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tRSTH(double *trsth, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tRSTH, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*trsth = (double)value;
//			*trsth *= (double)62.5;
//			*trsth /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*trsth = tRSTH_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*trsth = tRSTH_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*trsth = tRSTH_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*trsth = tRSTH_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*trsth = tRSTH_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*trsth = tRSTH_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*trsth = tRSTH_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*trsth = tRSTH_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*trsth = tRSTH_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*trsth = tRSTH_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*trsth = tRSTH_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*trsth = tRSTH_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*trsth = tRSTH_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*trsth = tRSTH_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*trsth = tRSTH_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*trsth = tRSTH_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*trsth = tRSTH_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tRSTH, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*trsth = (double)value;
//			*trsth *= (double)62.5;
//			*trsth /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*trsth = tRSTH_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*trsth = tRSTH_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*trsth = tRSTH_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*trsth = tRSTH_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*trsth = tRSTH_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*trsth = tRSTH_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*trsth = tRSTH_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*trsth = tRSTH_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*trsth = tRSTH_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*trsth = tRSTH_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*trsth = tRSTH_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*trsth = tRSTH_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*trsth = tRSTH_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*trsth = tRSTH_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*trsth = tRSTH_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*trsth = tRSTH_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*trsth = tRSTH_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//int OneWire_Set_tW0L_Standard_Predefined(one_wire_timing_presets tw0l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tw0l;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tW0L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tW0L_Overdrive_Predefined(one_wire_timing_presets tw0l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tw0l;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tW0L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tW0L_Standard_Custom(double tw0l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tw0l > 126)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tw0l *= 1000; //us -> ns
//	tw0l /= (double)62.5;
//	tw0l = ((int)tw0l | 0x8000);
//	reg_data[0] = (u_char)tw0l;
//	reg_data[1] = ((int)tw0l >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tW0L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tW0L_Overdrive_Custom(double tw0l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tw0l > (double)31.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tw0l *= 1000; //us -> ns
//	tw0l /= (double)62.5;
//	tw0l = ((int)tw0l | 0x8000);
//	reg_data[0] = (u_char)tw0l;
//	reg_data[1] = ((int)tw0l >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tW0L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tW0L(double *tw0l, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tW0L, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tw0l = (double)value;
//			*tw0l *= (double)62.5;
//			*tw0l /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tw0l = tW0L_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tw0l = tW0L_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tw0l = tW0L_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tw0l = tW0L_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tw0l = tW0L_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tw0l = tW0L_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tw0l = tW0L_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tw0l = tW0L_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tw0l = tW0L_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tw0l = tW0L_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tw0l = tW0L_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tw0l = tW0L_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tw0l = tW0L_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tw0l = tW0L_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tw0l = tW0L_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tw0l = tW0L_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*tw0l = tW0L_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tW0L, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tw0l = (double)value;
//			*tw0l *= (double)62.5;
//			*tw0l /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tw0l = tW0L_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tw0l = tW0L_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tw0l = tW0L_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tw0l = tW0L_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tw0l = tW0L_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tw0l = tW0L_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tw0l = tW0L_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tw0l = tW0L_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tw0l = tW0L_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tw0l = tW0L_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tw0l = tW0L_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tw0l = tW0L_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tw0l = tW0L_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tw0l = tW0L_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tw0l = tW0L_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tw0l = tW0L_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*tw0l = tW0L_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//int OneWire_Set_tREC_Standard_Predefined(one_wire_timing_presets trec)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = trec;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tREC, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tREC_Overdrive_Predefined(one_wire_timing_presets trec)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = trec;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tREC, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tREC_Standard_Custom(double trec)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (trec > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	trec *= 1000; //us -> ns
//	trec /= (double)62.5;
//	trec = ((int)trec | 0x8000);
//	reg_data[0] = (u_char)trec;
//	reg_data[1] = ((int)trec >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tREC, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tREC_Overdrive_Custom(double trec)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (trec > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	trec *= 1000; //us -> ns
//	trec /= (double)62.5;
//	trec = ((int)trec | 0x8000);
//	reg_data[0] = (u_char)trec;
//	reg_data[1] = ((int)trec >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tREC, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tREC(double *trec, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tREC, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*trec = (double)value;
//			*trec *= (double)62.5;
//			*trec /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*trec = tREC_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*trec = tREC_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*trec = tREC_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*trec = tREC_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*trec = tREC_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*trec = tREC_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*trec = tREC_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*trec = tREC_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*trec = tREC_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*trec = tREC_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*trec = tREC_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*trec = tREC_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*trec = tREC_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*trec = tREC_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*trec = tREC_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*trec = tREC_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*trec = tREC_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tREC, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*trec = (double)value;
//			*trec *= (double)62.5;
//			*trec /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*trec = tREC_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*trec = tREC_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*trec = tREC_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*trec = tREC_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*trec = tREC_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*trec = tREC_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*trec = tREC_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*trec = tREC_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*trec = tREC_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*trec = tREC_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*trec = tREC_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*trec = tREC_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*trec = tREC_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*trec = tREC_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*trec = tREC_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*trec = tREC_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*trec = tREC_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//
//int OneWire_Set_tMSI_Standard_Predefined(one_wire_timing_presets tmsi)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tmsi;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tMSI, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tMSI_Overdrive_Predefined(one_wire_timing_presets tmsi)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tmsi;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tMSI, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tMSI_Standard_Custom(double tmsi)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tmsi > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tmsi *= 1000; //us -> ns
//	tmsi /= (double)62.5;
//	tmsi = ((int)tmsi | 0x8000);
//	reg_data[0] = (u_char)tmsi;
//	reg_data[1] = ((int)tmsi >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tMSI, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tMSI_Overdrive_Custom(double tmsi)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tmsi > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tmsi *= 1000; //us -> ns
//	tmsi /= (double)62.5;
//	tmsi = ((int)tmsi | 0x8000);
//	reg_data[0] = (u_char)tmsi;
//	reg_data[1] = ((int)tmsi >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tMSI, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tMSI(double *tmsi, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tMSI, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tmsi = (double)value;
//			*tmsi *= (double)62.5;
//			*tmsi /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tmsi = tMSI_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tmsi = tMSI_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tmsi = tMSI_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tmsi = tMSI_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tmsi = tMSI_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tmsi = tMSI_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tmsi = tMSI_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tmsi = tMSI_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tmsi = tMSI_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tmsi = tMSI_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tmsi = tMSI_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tmsi = tMSI_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tmsi = tMSI_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tmsi = tMSI_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tmsi = tMSI_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tmsi = tMSI_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*tmsi = tMSI_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tMSI, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tmsi = (double)value;
//			*tmsi *= (double)62.5;
//			*tmsi /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tmsi = tMSI_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tmsi = tMSI_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tmsi = tMSI_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tmsi = tMSI_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tmsi = tMSI_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tmsi = tMSI_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tmsi = tMSI_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tmsi = tMSI_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tmsi = tMSI_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tmsi = tMSI_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tmsi = tMSI_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tmsi = tMSI_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tmsi = tMSI_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tmsi = tMSI_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tmsi = tMSI_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tmsi = tMSI_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*tmsi = tMSI_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//
//int OneWire_Set_tMSP_Standard_Predefined(one_wire_timing_presets tmsp)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tmsp;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tMSP, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tMSP_Overdrive_Predefined(one_wire_timing_presets tmsp)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tmsp;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tMSP, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tMSP_Standard_Custom(double tmsp)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tmsp > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tmsp *= 1000; //us -> ns
//	tmsp /= (double)62.5;
//	tmsp = ((int)tmsp | 0x8000);
//	reg_data[0] = (u_char)tmsp;
//	reg_data[1] = ((int)tmsp >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tMSP, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tMSP_Overdrive_Custom(double tmsp)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tmsp > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tmsp *= 1000; //us -> ns
//	tmsp /= (double)62.5;
//	tmsp = ((int)tmsp | 0x8000);
//	reg_data[0] = (u_char)tmsp;
//	reg_data[1] = ((int)tmsp >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tMSP, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tMSP(double *tmsp, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tMSP, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tmsp = (double)value;
//			*tmsp *= (double)62.5;
//			*tmsp /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tmsp = tMSP_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tmsp = tMSP_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tmsp = tMSP_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tmsp = tMSP_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tmsp = tMSP_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tmsp = tMSP_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tmsp = tMSP_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tmsp = tMSP_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tmsp = tMSP_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tmsp = tMSP_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tmsp = tMSP_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tmsp = tMSP_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tmsp = tMSP_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tmsp = tMSP_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tmsp = tMSP_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tmsp = tMSP_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*tmsp = tMSP_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tMSP, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tmsp = (double)value;
//			*tmsp *= (double)62.5;
//			*tmsp /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tmsp = tMSP_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tmsp = tMSP_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tmsp = tMSP_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tmsp = tMSP_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tmsp = tMSP_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tmsp = tMSP_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tmsp = tMSP_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tmsp = tMSP_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tmsp = tMSP_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tmsp = tMSP_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tmsp = tMSP_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tmsp = tMSP_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tmsp = tMSP_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tmsp = tMSP_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tmsp = tMSP_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tmsp = tMSP_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*tmsp = tMSP_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//
//int OneWire_Set_tW1L_Standard_Predefined(one_wire_timing_presets tw1l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tw1l;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tW1L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tW1L_Overdrive_Predefined(one_wire_timing_presets tw1l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tw1l;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tW1L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tW1L_Standard_Custom(double tw1l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tw1l > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tw1l *= 1000; //us -> ns
//	tw1l /= (double)62.5;
//	tw1l = ((int)tw1l | 0x8000);
//	reg_data[0] = (u_char)tw1l;
//	reg_data[1] = ((int)tw1l >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tW1L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tW1L_Overdrive_Custom(double tw1l)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tw1l > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tw1l *= 1000; //us -> ns
//	tw1l /= (double)62.5;
//	tw1l = ((int)tw1l | 0x8000);
//	reg_data[0] = (u_char)tw1l;
//	reg_data[1] = ((int)tw1l >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tW1L, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tW1L(double *tw1l, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tW1L, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tw1l = (double)value;
//			*tw1l *= (double)62.5;
//			*tw1l /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tw1l = tW1L_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tw1l = tW1L_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tw1l = tW1L_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tw1l = tW1L_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tw1l = tW1L_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tw1l = tW1L_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tw1l = tW1L_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tw1l = tW1L_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tw1l = tW1L_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tw1l = tW1L_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tw1l = tW1L_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tw1l = tW1L_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tw1l = tW1L_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tw1l = tW1L_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tw1l = tW1L_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tw1l = tW1L_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*tw1l = tW1L_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tW1L, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tw1l = (double)value;
//			*tw1l *= (double)62.5;
//			*tw1l /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tw1l = tW1L_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tw1l = tW1L_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tw1l = tW1L_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tw1l = tW1L_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tw1l = tW1L_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tw1l = tW1L_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tw1l = tW1L_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tw1l = tW1L_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tw1l = tW1L_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tw1l = tW1L_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tw1l = tW1L_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tw1l = tW1L_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tw1l = tW1L_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tw1l = tW1L_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tw1l = tW1L_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tw1l = tW1L_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*tw1l = tW1L_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}
//
//int OneWire_Set_tMSR_Standard_Predefined(one_wire_timing_presets tmsr)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tmsr;
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tMSR, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tMSR_Overdrive_Predefined(one_wire_timing_presets tmsr)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	reg_data[1] = 0x00;
//	reg_data[0] = tmsr;
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tMSR, reg_data)) != 0)
//	{
//		return error;
//	}
//
//    return error;
//}
//int OneWire_Set_tMSR_Standard_Custom(double tmsr)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tmsr > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tmsr *= 1000; //us -> ns
//	tmsr /= (double)62.5;
//	tmsr = ((int)tmsr | 0x8000);
//	reg_data[0] = (u_char)tmsr;
//	reg_data[1] = ((int)tmsr >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(STANDARD_SPEED_tMSR, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Set_tMSR_Overdrive_Custom(double tmsr)
//{
//	int error = 0;
//	u_char reg_data[2];
//
//	if (tmsr > (double)255.5)
//	{
//		return error = RB_INVALID_PARAMETER;
//	}
//
//	tmsr *= 1000; //us -> ns
//	tmsr /= (double)62.5;
//	tmsr = ((int)tmsr | 0x8000);
//	reg_data[0] = (u_char)tmsr;
//	reg_data[1] = ((int)tmsr >> 8);
//
//	if((error = DS2485_WriteOneWirePortConfig(OVERDRIVE_SPEED_tMSR, reg_data)) != 0)
//	{
//		return error;
//	}
//
//	return error;
//}
//int OneWire_Get_tMSR(double *tmsr, one_wire_speeds spd)
//{
//	int error = 0;
//	u_char reg_data[2];
//	u_short value;
//
//	if(spd != STANDARD)  //Overdrive
//	{
//		if((error = DS2485_ReadOneWirePortConfig(OVERDRIVE_SPEED_tMSR, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //custom value
//		{
//
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tmsr = (double)value;
//			*tmsr *= (double)62.5;
//			*tmsr /= 1000;
//		}
//		else //predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tmsr = tMSR_OVERDRIVE_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tmsr = tMSR_OVERDRIVE_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tmsr = tMSR_OVERDRIVE_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tmsr = tMSR_OVERDRIVE_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tmsr = tMSR_OVERDRIVE_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tmsr = tMSR_OVERDRIVE_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tmsr = tMSR_OVERDRIVE_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tmsr = tMSR_OVERDRIVE_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tmsr = tMSR_OVERDRIVE_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tmsr = tMSR_OVERDRIVE_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tmsr = tMSR_OVERDRIVE_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tmsr = tMSR_OVERDRIVE_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tmsr = tMSR_OVERDRIVE_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tmsr = tMSR_OVERDRIVE_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tmsr = tMSR_OVERDRIVE_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tmsr = tMSR_OVERDRIVE_PRESET_F;
//				break;
//
//			default:
//				*tmsr = tMSR_OVERDRIVE_PRESET_6;
//				break;
//			}
//		}
//	}
//	else //Standard
//	{
//		if((error = DS2485_ReadOneWirePortConfig(STANDARD_SPEED_tMSR, reg_data)) != 0)
//		{
//			return error;
//		}
//
//		if((reg_data[1] >> 7) != 0) //Custom Value
//		{
//			value = ((reg_data[1] & ~(0x80)) << 8) | reg_data[0];
//			*tmsr = (double)value;
//			*tmsr *= (double)62.5;
//			*tmsr /= 1000;
//		}
//		else //Predefined value
//		{
//			switch(reg_data[0])
//			{
//			case PRESET_0:
//				*tmsr = tMSR_STANDARD_PRESET_0;
//				break;
//
//			case PRESET_1:
//				*tmsr = tMSR_STANDARD_PRESET_1;
//				break;
//
//			case PRESET_2:
//				*tmsr = tMSR_STANDARD_PRESET_2;
//				break;
//
//			case PRESET_3:
//				*tmsr = tMSR_STANDARD_PRESET_3;
//				break;
//
//			case PRESET_4:
//				*tmsr = tMSR_STANDARD_PRESET_4;
//				break;
//
//			case PRESET_5:
//				*tmsr = tMSR_STANDARD_PRESET_5;
//				break;
//
//			case PRESET_6:
//				*tmsr = tMSR_STANDARD_PRESET_6;
//				break;
//
//			case PRESET_7:
//				*tmsr = tMSR_STANDARD_PRESET_7;
//				break;
//
//			case PRESET_8:
//				*tmsr = tMSR_STANDARD_PRESET_8;
//				break;
//
//			case PRESET_9:
//				*tmsr = tMSR_STANDARD_PRESET_9;
//				break;
//
//			case PRESET_A:
//				*tmsr = tMSR_STANDARD_PRESET_A;
//				break;
//
//			case PRESET_B:
//				*tmsr = tMSR_STANDARD_PRESET_B;
//				break;
//
//			case PRESET_C:
//				*tmsr = tMSR_STANDARD_PRESET_C;
//				break;
//
//			case PRESET_D:
//				*tmsr = tMSR_STANDARD_PRESET_D;
//				break;
//
//			case PRESET_E:
//				*tmsr = tMSR_STANDARD_PRESET_E;
//				break;
//
//			case PRESET_F:
//				*tmsr = tMSR_STANDARD_PRESET_F;
//				break;
//
//			default:
//				*tmsr = tMSR_STANDARD_PRESET_6;
//				break;
//			}
//		}
//	}
//
//    return error;
//}



