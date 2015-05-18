//*****************************************************************************
//
// usblibpriv.h - Private header file used to share internal variables and
//                function prototypes between the various modules in the USB
//                library.  This header MUST NOT be used by application code.
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

#ifndef __USBLIBPRIV_H__
#define __USBLIBPRIV_H__

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
// Internal interrupt handlers called from the main vectors in device and
// host mode.
//
//*****************************************************************************
extern void USBDeviceIntHandlerInternal(unsigned long ulIndex,
                                        unsigned long ulStatus);
extern void USBHostIntHandlerInternal(unsigned long ulIndex,
                                      unsigned long ulStatus);

//*****************************************************************************
//
// The maximum number of tick handlers that can be registered in a system.
//
//*****************************************************************************
#define MAX_USB_TICK_HANDLERS       6

//*****************************************************************************
//
// This value defines the number of SOF ticks that must pass before a call
// is made to InternalUSBStartOfFrameTick.  The value 5 ensures that the
// function is called every 5 milliseconds assuming that SOF interrupts are
// enabled and SOF is present.
//
//*****************************************************************************
#define USB_SOF_TICK_DIVIDE 5

//*****************************************************************************
//
// Tick handler function pointer type.
//
//*****************************************************************************
typedef void(* tUSBTickHandler)(void *pvInstance, unsigned long ulTicksmS);

//*****************************************************************************
//
// Internal functions use to initialize the tick handler and register tick
// callbacks.
//
//*****************************************************************************
extern void InternalUSBTickInit(void);
extern void InternalUSBTickReset(void);
extern long InternalUSBRegisterTickHandler(tUSBTickHandler pfHandler,
                                           void *pvInstance);
extern void InternalUSBStartOfFrameTick(unsigned long ulTicksmS);
extern void InternalUSBHCDSendEvent(unsigned long ulIndex,
                                    tEventInfo *psEvent,
                                    unsigned long ulEvFlag);

//*****************************************************************************
//
// g_ulCurrentUSBTick holds the elapsed time in milliseconds since the
// tick module was first initialized based on calls to the function
// InternalUSBStartOfFrameTick.  The granularity is USB_SOF_TICK_DIVIDE
// milliseconds.
//
//*****************************************************************************
extern unsigned long g_ulCurrentUSBTick;

//*****************************************************************************
//
// g_ulUSBSOFCount is a global counter for Start of Frame interrupts.  It is
// incremented by the low level device- or host-mode interrupt handlers.
//
//*****************************************************************************
extern unsigned long g_ulUSBSOFCount;

//*****************************************************************************
//
// InternalUSBGetTime is a macro which will return the system time in
// milliseconds as calculated based on calls to the function
// InternalUSBStartOfFrameTick.  The granularity is USB_SOF_TICK_DIVIDE
// milliseconds.
//
// Currently, this merely returns the value of a global variable.
//
//*****************************************************************************
#define InternalUSBGetTime() g_ulCurrentUSBTick

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBLIBPRIV_H__
