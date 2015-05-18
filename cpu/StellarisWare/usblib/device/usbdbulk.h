//*****************************************************************************
//
// usbdcdc.h - USBLib support for a generic bulk device.
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

#ifndef __USBDBULK_H__
#define __USBDBULK_H__

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
//! \addtogroup bulk_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB Bulk example code and are here only to help with the application
// allocating the correct amount of memory for the Bulk example device code.
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// This enumeration holds the various states that the device can be in during
// normal operation.
//
//*****************************************************************************
typedef enum
{
    //
    // Unconfigured.
    //
    BULK_STATE_UNCONFIGURED,

    //
    // No outstanding transaction remains to be completed.
    //
    BULK_STATE_IDLE,

    //
    // Waiting on completion of a send or receive transaction.
    //
    BULK_STATE_WAIT_DATA,

    //
    // Waiting for client to process data.
    //
    BULK_STATE_WAIT_CLIENT
} tBulkState;

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for the
// Bulk only example device.  The memory for this structure is pointed to by
// the psPrivateBulkData field in the tUSBDBulkDevice structure passed on
// USBDBulkInit().
//
//*****************************************************************************
typedef struct
{
    unsigned long ulUSBBase;
    tDeviceInfo *psDevInfo;
    tConfigDescriptor *psConfDescriptor;
    volatile tBulkState eBulkRxState;
    volatile tBulkState eBulkTxState;
    volatile unsigned short usDeferredOpFlags;
    unsigned short usLastTxSize;
    volatile tBoolean bConnected;
    unsigned char ucINEndpoint;
    unsigned char ucOUTEndpoint;
    unsigned char ucInterface;
}
tBulkInstance;

#ifndef DEPRECATED
//*****************************************************************************
//
// The number of bytes of workspace required by the bulk device class driver.
// The client must provide a block of RAM of at least this size in the
// tBulkInstance field of the tUSBBulkDevice structure passed on USBDBulkInit.
//
// This value is deprecated and should not be used, any new code should just
// pass in a tBulkInstance structure in the psPrivateBulkData field.
//
//*****************************************************************************
#define USB_BULK_WORKSPACE_SIZE (sizeof(tBulkInstance))
#endif

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the USB Bulk Device.
//! This does not include the configuration descriptor which is automatically
//! ignored by the composite device class.
//
// For reference this is sizeof(g_sCDCSerIfaceHeaderSectionNOINT) +
// sizeof(g_sCDCSerInterfaceSection) + sizeof(g_sCDCSerIfaceEndpointsNOINT)
//
//*****************************************************************************
#define COMPOSITE_DBULK_SIZE     (23)

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the bulk device.
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
    //! The maximum power consumption of the device, expressed in milliamps.
    //
    unsigned short usMaxPowermA;

    //
    //! Indicates whether the device is self- or bus-powered and whether or not
    //! it supports remote wakeup.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    //
    unsigned char ucPwrAttributes;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events related to the device's data receive channel.
    //
    tUSBCallback pfnRxCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the receive channel callback,
    //! pfnRxCallback.
    //
    void *pvRxCBData;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events related to the device's data transmit
    //! channel.
    //
    tUSBCallback pfnTxCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the transmit channel callback,
    //! pfnTxCallback.
    //
    void *pvTxCBData;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain pointers to the following string descriptors in this
    //! order.  Language descriptor, Manufacturer name string (language 1),
    //! Product name string (language 1), Serial number string (language 1),
    //! Interface description string (language 1) and Configuration description
    //! string (language 1).
    //!
    //! If supporting more than 1 language, the strings for indices 1 through 5
    //! must be repeated for each of the other languages defined in the
    //! language descriptor.
    //
    const unsigned char * const *ppStringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors array.
    //! This must be 1 + (5 * number of supported languages).
    //
    unsigned long ulNumStringDescriptors;

    //
    //! A pointer to private instance data for this device.  This memory must
    //! remain accessible for as long as the bulk device is in use and must not
    //! be modified by any code outside the bulk class driver.
    //
    tBulkInstance *psPrivateBulkData;
}
tUSBDBulkDevice;

extern tDeviceInfo g_sBulkDeviceInfo;

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDBulkInit(unsigned long ulIndex,
                         const tUSBDBulkDevice *psDevice);
extern void *USBDBulkCompositeInit(unsigned long ulIndex,
                                   const tUSBDBulkDevice *psDevice);
extern void USBDBulkTerm(void *pvInstance);
extern void *USBDBulkSetRxCBData(void *pvInstance, void *pvCBData);
extern void *USBDBulkSetTxCBData(void *pvInstance, void *pvCBData);
extern unsigned long USBDBulkPacketWrite(void *pvInstance,
                                         unsigned char *pcData,
                                         unsigned long ulLength,
                                         tBoolean bLast);
extern unsigned long USBDBulkPacketRead(void *pvInstance,
                                        unsigned char *pcData,
                                        unsigned long ulLength,
                                        tBoolean bLast);
extern unsigned long USBDBulkTxPacketAvailable(void *pvInstance);
extern unsigned long USBDBulkRxPacketAvailable(void *pvInstance);
extern void USBDBulkPowerStatusSet(void *pvInstance, unsigned char ucPower);
extern tBoolean USBDBulkRemoteWakeupRequest(void *pvInstance);

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

#endif // __USBDBULK_H__
