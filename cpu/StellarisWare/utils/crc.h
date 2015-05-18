//*****************************************************************************
//
// crc.h - Prototypes for the CRC functions.
//
// Copyright (c) 2010-2012 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __CRC_H__
#define __CRC_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Prototypes for the functions.
//
//*****************************************************************************
extern unsigned char Crc8CCITT(unsigned char ucCrc,
                               const unsigned char *pucData,
                               unsigned long ulCount);
extern unsigned short Crc16(unsigned short usCrc, const unsigned char *pucData,
                            unsigned long ulCount);
extern unsigned short Crc16Array(unsigned long ulWordLen,
                                 const unsigned long *pulData);
extern void Crc16Array3(unsigned long ulWordLen, const unsigned long *pulData,
                        unsigned short *pusCrc3);
extern unsigned long Crc32(unsigned long ulCrc, const unsigned char *pucData,
                           unsigned long ulCount);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __CRC_H__
