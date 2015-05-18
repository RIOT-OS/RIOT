//*****************************************************************************
//
// usbhostpriv.h - Internal header file for USB host functions.
//
// Copyright (c) 2011-2012 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __USBHOSTPRIV_H__
#define __USBHOSTPRIV_H__

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
// The states a hub port can be in during device connection.
//
//*****************************************************************************
typedef enum
{
    //
    // The port has no device connected.
    //
    PORT_IDLE,

    //
    // The port has a device present and is waiting for the enumeration
    // sequence to begin.
    //
    PORT_CONNECTED,

    //
    // A device connection notification has been received and we have initiated
    // a reset to the port.  We are waiting for the reset to complete.
    //
    PORT_RESET_ACTIVE,

    //
    // The Port reset has completed but now the hub is waiting the required
    // 10ms before accessing the device.
    //
    PORT_RESET_WAIT,

    //
    // A device is connected and the port has been reset.  Control has been
    // passed to the main host handling portion of USBLib to enumerate the
    // device.
    //
    PORT_ACTIVE,

    //
    // A device has completed enumeration.
    //
    PORT_ENUMERATED,

    //
    // A device is attached to the port but enumeration failed.
    //
    PORT_ERROR
}
tHubPortState;

//*****************************************************************************
//
// The list of valid event flags in the g_sUSBHCD.ulEventEnables member
// variable.
//
//*****************************************************************************
#define USBHCD_EVFLAG_SOF       0x00000001
#define USBHCD_EVFLAG_CONNECT   0x00000002
#define USBHCD_EVFLAG_UNKCNCT   0x00000004
#define USBHCD_EVFLAG_DISCNCT   0x00000008
#define USBHCD_EVFLAG_PWRFAULT  0x00000010
#define USBHCD_EVFLAG_PWRDIS    0x00000020
#define USBHCD_EVFLAG_PWREN     0x00000040

//*****************************************************************************
//
// This structure holds all data specific to a single hub port.
//
//*****************************************************************************
typedef struct
{
    //
    // A pointer to storage for the configuration descriptor of a device
    // attached to this port.
    //
    unsigned char *pucConfigDesc;

    //
    // The size of the storage pointed to by pucConfigDesc.
    //
    unsigned long ulConfigSize;

    //
    // The handle used by the HCD layer to identify this device.
    //
    unsigned long ulDevHandle;

    //
    // The current state of the port.
    //
    volatile tHubPortState sState;

    //
    // General counter used in various states.
    //
    volatile unsigned long ulCount;

    //
    // A flag used to indicate that the downstream device is a low speed
    // device.
    //
    tBoolean bLowSpeed;

    //
    // This flag is set if the hub reports that a change is pending on this
    // port.
    //
    volatile tBoolean bChanged;
}
tHubPort;

//*****************************************************************************
//
// This is the structure that holds all of the data for a given instance of
// a Hub device.
//
//*****************************************************************************
typedef struct
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *pDevice;

    //
    // Used to save the callback function pointer.
    //
    tUSBCallback pfnCallback;

    //
    // Callback data provided by caller.
    //
    unsigned long ulCBData;

    //
    // Interrupt IN pipe.
    //
    unsigned long ulIntInPipe;

    //
    // Hub characteristics as reported in the class-specific hub descriptor.
    //
    unsigned short usHubCharacteristics;

    //
    // The number of downstream-facing ports the hub supports.
    //
    unsigned char ucNumPorts;

    //
    // The number of ports on the hub that we can actually talk to.  This will
    // be the smaller of the number of ports on the hub and MAX_USB_DEVICES.
    //
    unsigned char ucNumPortsInUse;

    //
    // The size of a status change packet sent by the hub.  This is determined
    // from the number of ports supported by the hub.
    //
    unsigned char ucReportSize;

    //
    // Flag indicating whether the hub is connected.
    //
    tBoolean bHubActive;

    //
    // Flag indicating that a device is currently in process of being
    // enumerated.
    //
    volatile tBoolean bEnumerationBusy;

    //
    // This is valid if bEnumerationBusy is set and indicates the port
    // that is in the process of enumeration.
    //
    unsigned char ucEnumIdx;

    //
    // The state of each of the ports we support on the hub.
    //
    tHubPort psPorts[MAX_USB_DEVICES];
}
tHubInstance;

//*****************************************************************************
//
// Functions within the host controller that are called by the hub class driver
//
//*****************************************************************************
extern unsigned long USBHCDHubDeviceConnected(unsigned long ulIndex,
                                              unsigned char ucHub,
                                              unsigned char ucPort,
                                              tBoolean bLowSpeed,
                                              unsigned char *pucConfigPool,
                                              unsigned long ulConfigSize);
extern void USBHCDHubDeviceDisconnected(unsigned long ulIndex,
                                        unsigned long ulDevIndex);

//*****************************************************************************
//
// Functions in the hub class driver that are called by the host controller.
//
//*****************************************************************************
extern void USBHHubMain(void);
extern void USBHHubInit(void);
extern void USBHHubEnumerationComplete(unsigned char ucHub,
                                       unsigned char ucPort);
extern void USBHHubEnumerationError(unsigned char ucHub, unsigned char ucPort);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBHOSTPRIV_H__
