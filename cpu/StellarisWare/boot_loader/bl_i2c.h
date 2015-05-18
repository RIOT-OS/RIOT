//*****************************************************************************
//
// bl_i2c.h - Definitions for the I2C transport functions.
//
// Copyright (c) 2006-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#ifndef __BL_I2C_H__
#define __BL_I2C_H__

//*****************************************************************************
//
// This defines the I2C clock pin that is being used by the boot loader.
//
//*****************************************************************************
#define I2C_CLK                 (1 << 2)

//*****************************************************************************
//
// This defines the I2C data pin that is being used by the boot loader.
//
//*****************************************************************************
#define I2C_DATA                (1 << 3)

//*****************************************************************************
//
// This defines the combination of pins used to implement the I2C port used by
// the boot loader.
//
//*****************************************************************************
#define I2C_PINS                (I2C_CLK | I2C_DATA)

//*****************************************************************************
//
// I2C Transport APIs
//
//*****************************************************************************
extern void I2CSend(unsigned char const *pucData, unsigned long ulSize);
extern void I2CReceive(unsigned char *pucData, unsigned long ulSize);
extern void I2CFlush(void);

//*****************************************************************************
//
// Define the transport functions if the I2C port is being used.
//
//*****************************************************************************
#ifdef I2C_ENABLE_UPDATE
#define SendData                I2CSend
#define FlushData               I2CFlush
#define ReceiveData             I2CReceive
#endif

#endif // __BL_I2C_H__
