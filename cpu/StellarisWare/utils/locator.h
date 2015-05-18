//*****************************************************************************
//
// locator.h - Prototypes for the device locator server.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __LOCATOR_H__
#define __LOCATOR_H__

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
// Function prototypes.
//
//*****************************************************************************
extern void LocatorInit(void);
extern void LocatorBoardTypeSet(unsigned long ulType);
extern void LocatorBoardIDSet(unsigned long ulID);
extern void LocatorClientIPSet(unsigned long ulIP);
extern void LocatorMACAddrSet(unsigned char *pucMACArray);
extern void LocatorVersionSet(unsigned long ulVersion);
extern void LocatorAppTitleSet(const char *pcAppTitle);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __LOCATOR_H__
