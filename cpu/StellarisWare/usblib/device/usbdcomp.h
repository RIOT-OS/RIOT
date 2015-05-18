//*****************************************************************************
//
// usbdcomp.h - USB composite device class driver.
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
// This is part of revision 9453 of the Stellaris USB Library.
//
//*****************************************************************************

#ifndef __USBDCOMP_H__
#define __USBDCOMP_H__

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
// Return to default packing when using the IAR Embedded Workbench compiler.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack()
#endif

//*****************************************************************************
//
//! \addtogroup composite_device_class_api
//! @{
//
//*****************************************************************************

//
// Defines a single entry in a table of device types supported by the composite
// device.
//
typedef struct
{
    //
    // This is set internally by the composite class so it can be left
    // uninitialized by the application.
    //
    const tDeviceInfo *pDeviceInfo;

    //
    // This should be the header to the configuration header for a class.
    //
    const tConfigHeader *psConfigHeader;

    //
    // The offset to this devices interface, filled in by the composite class.
    //
    unsigned char ucIfaceOffset;

} tUSBDCompositeEntry;

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for the
// composite device class.  The memory for this structure is pointed to by
// the psPrivateData field in the tUSBDCompositeDevice structure passed on
// USBDCompositeInit() and should not be modified by any code outside of the
// composite device code.
//
//*****************************************************************************
typedef struct
{
    //
    // Saves which USB controller is in use.
    //
    unsigned long ulUSBBase;

    //
    // The device information pointer.
    //
    tDeviceInfo *psDevInfo;

    //
    // This is the configuration descriptor for this instance.
    //
    tConfigDescriptor sConfigDescriptor;

    //
    // This is the device descriptor for this instance.
    //
    tDeviceDescriptor sDeviceDescriptor;

    //
    // The configuration header for this instance.
    //
    tConfigHeader sCompConfigHeader;

    //
    // These are the configuration sections that will be built from the
    // Configuration Descriptor header and the descriptors from the devices
    // that are part of this composite device.
    //
    tConfigSection psCompSections[2];
    tConfigSection *ppsCompSections[2];

    //
    // The size and pointer to the data used by the instance.
    //
    unsigned long ulDataSize;
    unsigned char *pucData;

    //
    // The current "owner" of endpoint 0.  This is used to track the device
    // class which is currently transferring data on EP0.
    //
    unsigned long ulEP0Owner;
}
tCompositeInstance;

//*****************************************************************************
//
//! This type is used by an application to describe and instance of a device
//! and an instance data pointer for that class.  The psDevice pointer should
//! be a pointer to a valid device class to include in the composite device.
//! The pvInstance pointer should be a pointer to an instance pointer for the
//! device in the psDevice pointer.
//!
//
//*****************************************************************************
typedef struct
{
    //
    //! This is the top level device information structure.
    //
    const tDeviceInfo *psDevice;

    //
    //! This is the instance data for the device structure.
    //
    void *pvInstance;
}
tCompositeEntry;

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the composite device class.
//
//*****************************************************************************
typedef struct
{
    //
    //! The vendor ID that this device is to present in the device descriptor.
    //
    unsigned short usVID;

    //
    //! The product ID that this device is to present in the device descriptor.
    //
    unsigned short usPID;

    //
    //! The maximum power consumption of the device, expressed in mA.
    //
    unsigned short usMaxPowermA;

    //
    //! Indicates whether the device is self or bus-powered and whether or not
    //! it supports remote wake up.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    //
    unsigned char ucPwrAttributes;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events relating to the operation of the composite
    //! device.
    //
    tUSBCallback pfnCallback;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1), Composite
    //! device interface description string (language 1), Configuration
    //! description string (language 1).
    //!
    //! If supporting more than 1 language, the descriptor block (except for
    //! string descriptor 0) must be repeated for each language defined in the
    //! language descriptor.
    //!
    //
    const unsigned char * const *ppStringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be 1 + ((5 + (number of strings)) *
    //!                           (number of languages)).
    //
    unsigned long ulNumStringDescriptors;

    //
    //! The number of devices in the psDevices array.
    //
    unsigned long ulNumDevices;

    //
    //! This application supplied array holds the the top level device class
    //! information as well as the Instance data for that class.
    //
    tCompositeEntry *psDevices;

    //
    //! A pointer to per-device workspace used by the composite device.  This
    //! buffer must be sized to contain at least ulNumDevices long integers
    //! (or 4 * ulNumDevices bytes).  It must remain accessible for as long as
    //! the composite device is in use and must not be modified by any code
    //! outside the composite class driver.
    //
    unsigned long *pulDeviceWorkspace;

    //
    //! A pointer to RAM work space for this device instance.  The client
    //! must fill in this field with a pointer to at least
    //! sizeof(tCompositeInstance) bytes of read/write storage that the
    //! library can use for driver work space.  This memory must remain
    //! accessible for as long as the composite device is in use and must not
    //! be modified by any code outside the composite class driver.
    //
    tCompositeInstance *psPrivateData;
}
tUSBDCompositeDevice;

//*****************************************************************************
//
// Return to default packing when using the IAR Embedded Workbench compiler.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack()
#endif

//*****************************************************************************
//
// Composite specific device class driver events
//
//*****************************************************************************

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDCompositeInit(unsigned long ulIndex,
                               tUSBDCompositeDevice *psCompDevice,
                               unsigned long ulSize,
                               unsigned char *pucData);
extern void USBDCompositeTerm(void *pvInstance);

//*****************************************************************************
//
// Close the Doxygen group.
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

