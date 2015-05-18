//*****************************************************************************
//
// usbhhidkeyboard.h - This file holds the application interfaces for USB
//                     keyboard devices.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 9453 of the Stellaris USB Library.
//
//*****************************************************************************

#ifndef __USBHHIDKEYBOARD_H__
#define __USBHHIDKEYBOARD_H__

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
//! \addtogroup usblib_host_device
//! @{
//
//*****************************************************************************

extern unsigned long USBHKeyboardOpen(tUSBCallback pfnCallback,
                                      unsigned char *pucBuffer,
                                      unsigned long ulBufferSize);
extern unsigned long USBHKeyboardClose(unsigned long ulInstance);
extern unsigned long USBHKeyboardInit(unsigned long ulInstance);
extern unsigned long USBHKeyboardModifierSet(unsigned long ulInstance,
                                             unsigned long ulModifiers);
extern unsigned long USBHKeyboardPollRateSet(unsigned long ulInstance,
                                             unsigned long ulPollRate);

extern unsigned long USBHKeyboardUsageToChar(
    unsigned long ulInstance,
    const tHIDKeyboardUsageTable *pTable,
    unsigned char ucUsageID);

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
