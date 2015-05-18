//*****************************************************************************
//
// usbhhid.h - This hold the host driver for hid class.
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

#ifndef __USBHHID_H__
#define __USBHHID_H__

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
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These defines are the the events that will be passed in the ulEvent
// parameter of the callback from the driver.
//
//*****************************************************************************
#define USBH_EVENT_HID_SETRPT   USBH_HID_EVENT_BASE + 0
#define USBH_EVENT_HID_REPORT   USBH_HID_EVENT_BASE + 1

//
//! The HID keyboard detected a key being pressed.
//
#define USBH_EVENT_HID_KB_PRESS USBH_HID_EVENT_BASE + 16

//
//! The HID keyboard detected a key being released.
//
#define USBH_EVENT_HID_KB_REL   USBH_HID_EVENT_BASE + 17

//
//! The HID keyboard detected one of the keyboard modifiers being pressed.
//
#define USBH_EVENT_HID_KB_MOD   USBH_HID_EVENT_BASE + 18

//
//! A button was pressed on a HID mouse.
//
#define USBH_EVENT_HID_MS_PRESS USBH_HID_EVENT_BASE + 32

//
//! A button was released on a HID mouse.
//
#define USBH_EVENT_HID_MS_REL   USBH_HID_EVENT_BASE + 33

//
//! The HID mouse detected movement in the X direction.
//
#define USBH_EVENT_HID_MS_X     USBH_HID_EVENT_BASE + 34

//
//! The HID mouse detected movement in the Y direction.
//
#define USBH_EVENT_HID_MS_Y     USBH_HID_EVENT_BASE + 35

//*****************************************************************************
//
//! The following values are used to register callbacks to the USB HOST HID
//! device class layer.
//
//*****************************************************************************
typedef enum
{
    //
    //! No device should be used.  This value should not be used by
    //! applications.
    //
    USBH_HID_DEV_NONE =         0,

    //
    //! This is a keyboard device.
    //
    USBH_HID_DEV_KEYBOARD,

    //
    //! This is a mouse device.
    //
    USBH_HID_DEV_MOUSE,

    //
    //! This is a vendor specific device.
    //
    USBH_HID_DEV_VENDOR
}
tHIDSubClassProtocol;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************
extern unsigned long USBHHIDOpen(tHIDSubClassProtocol eDeviceType,
                                 tUSBCallback pfnCallback,
                                 unsigned long ulCBData);
extern void USBHHIDClose(unsigned long ulInstance);
extern unsigned long USBHHIDGetReportDescriptor(unsigned long ulInstance,
                                                unsigned char *pucBuffer,
                                                unsigned long ulSize);
extern unsigned long USBHHIDSetIdle(unsigned long ulInstance,
                                    unsigned char ucDuration,
                                    unsigned char ucReportID);
extern unsigned long USBHHIDSetProtocol(unsigned long ulInstance,
                                        unsigned long ulBootProtocol);
extern unsigned long USBHHIDSetReport(unsigned long ulInstance,
                                      unsigned long ulInterface,
                                      unsigned char *pucData,
                                      unsigned long ulSize);
extern unsigned long USBHHIDGetReport(unsigned long ulInstance,
                                      unsigned long ulInterface,
                                      unsigned char *pucData,
                                      unsigned long ulSize);
extern const tUSBHostClassDriver g_USBHIDClassDriver;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBHHID_H__
