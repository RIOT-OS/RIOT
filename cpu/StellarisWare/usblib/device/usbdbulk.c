//*****************************************************************************
//
// usbdbulk.c - USB bulk device class driver.
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

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup bulk_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The subset of endpoint status flags that we consider to be reception
// errors.  These are passed to the client via USB_EVENT_ERROR if seen.
//
//*****************************************************************************
#define USB_RX_ERROR_FLAGS      (USBERR_DEV_RX_DATA_ERROR |                   \
                                 USBERR_DEV_RX_OVERRUN |                      \
                                 USBERR_DEV_RX_FIFO_FULL)

//*****************************************************************************
//
// Flags that may appear in usDeferredOpFlags to indicate some operation that
// has been requested but could not be processed at the time it was received.
// Each deferred operation is defined as the bit number that should be set in
// tBulkInstance->usDeferredOpFlags to indicate that the operation is pending.
//
//*****************************************************************************
#define BULK_DO_PACKET_RX           5

//*****************************************************************************
//
// Macros to convert between USB controller base address and an index.  These
// are currently trivial but are included to allow for the possibility of
// supporting more than one controller in the future.
//
//*****************************************************************************
#define USB_BASE_TO_INDEX(BaseAddr) (0)
#define USB_INDEX_TO_BASE(Index) (USB0_BASE)

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define DATA_IN_ENDPOINT        USB_EP_1
#define DATA_OUT_ENDPOINT       USB_EP_1

//*****************************************************************************
//
// Maximum packet size for the bulk endpoints used for serial data
// transmission and reception and the associated FIFO sizes to set aside
// for each endpoint.
//
//*****************************************************************************
#define DATA_IN_EP_FIFO_SIZE    USB_FIFO_SZ_64
#define DATA_OUT_EP_FIFO_SIZE   USB_FIFO_SZ_64

#define DATA_IN_EP_MAX_SIZE     USB_FIFO_SZ_TO_BYTES(DATA_IN_EP_FIFO_SIZE)
#define DATA_OUT_EP_MAX_SIZE    USB_FIFO_SZ_TO_BYTES(DATA_IN_EP_FIFO_SIZE)

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
unsigned char g_pBulkDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_VEND_SPECIFIC,    // USB Device Class
    0,                          // USB Device Sub-class
    0,                          // USB Device protocol
    64,                         // Maximum packet size for default pipe.
    USBShort(0),                // Vendor ID (VID).
    USBShort(0),                // Product ID (PID).
    USBShort(0x100),            // Device Version BCD.
    1,                          // Manufacturer string identifier.
    2,                          // Product string identifier.
    3,                          // Product serial number.
    1                           // Number of configurations.
};

//*****************************************************************************
//
// Bulk device configuration descriptor.
//
// It is vital that the configuration descriptor bConfigurationValue field
// (byte 6) is 1 for the first configuration and increments by 1 for each
// additional configuration defined here.  This relationship is assumed in the
// device stack for simplicity even though the USB 2.0 specification imposes
// no such restriction on the bConfigurationValue values.
//
// Note that this structure is deliberately located in RAM since we need to
// be able to patch some values in it based on client requirements.
//
//*****************************************************************************
unsigned char g_pBulkDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(32),               // The total size of this full structure.
    1,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    5,                          // The string identifier that describes this
                                // configuration.
    USB_CONF_ATTR_SELF_PWR,     // Bus Powered, Self Powered, remote wake up.
    250,                        // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// The remainder of the configuration descriptor is stored in flash since we
// don't need to modify anything in it at runtime.
//
//*****************************************************************************
const unsigned char g_pBulkInterface[] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,                          // The index for this interface.
    0,                          // The alternate setting for this interface.
    2,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_VEND_SPECIFIC,    // The interface class
    0,                          // The interface sub-class.
    0,                          // The interface protocol for the sub-class
                                // specified above.
    4,                          // The string index for this interface.

    //
    // Endpoint Descriptor
    //
    7,                               // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,              // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USB_EP_TO_INDEX(DATA_IN_ENDPOINT),
    USB_EP_ATTR_BULK,                // Endpoint is a bulk endpoint.
    USBShort(DATA_IN_EP_MAX_SIZE),   // The maximum packet size.
    0,                               // The polling interval for this endpoint.

    //
    // Endpoint Descriptor
    //
    7,                               // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,              // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | USB_EP_TO_INDEX(DATA_OUT_ENDPOINT),
    USB_EP_ATTR_BULK,                // Endpoint is a bulk endpoint.
    USBShort(DATA_OUT_EP_MAX_SIZE),  // The maximum packet size.
    0,                               // The polling interval for this endpoint.
};

//*****************************************************************************
//
// The serial config descriptor is defined as two sections, one containing
// just the 9 byte USB configuration descriptor and the other containing
// everything else that is sent to the host along with it.
//
//*****************************************************************************
const tConfigSection g_sBulkConfigSection =
{
    sizeof(g_pBulkDescriptor),
    g_pBulkDescriptor
};

const tConfigSection g_sBulkInterfaceSection =
{
    sizeof(g_pBulkInterface),
    g_pBulkInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete bulk device configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psBulkSections[] =
{
    &g_sBulkConfigSection,
    &g_sBulkInterfaceSection
};

#define NUM_BULK_SECTIONS (sizeof(g_psBulkSections) /                         \
                           sizeof(tConfigSection *))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sBulkConfigHeader =
{
    NUM_BULK_SECTIONS,
    g_psBulkSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_pBulkConfigDescriptors[] =
{
    &g_sBulkConfigHeader
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleConfigChange(void *pvInstance, unsigned long ulInfo);
static void HandleDisconnect(void *pvInstance);
static void HandleEndpoints(void *pvInstance, unsigned long ulStatus);
static void HandleSuspend(void *pvInstance);
static void HandleResume(void *pvInstance);
static void HandleDevice(void *pvInstance, unsigned long ulRequest,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB serial device.
//
//*****************************************************************************
tDeviceInfo g_sBulkDeviceInfo =
{
    //
    // Device event handler callbacks.
    //
    {
        0,                     // GetDescriptor
        0,                     // RequestHandler
        0,                     // InterfaceChange
        HandleConfigChange,    // ConfigChange
        0,                     // DataReceived
        0,                     // DataSentCallback
        0,                     // ResetHandler
        HandleSuspend,         // SuspendHandler
        HandleResume,          // ResumeHandler
        HandleDisconnect,      // DisconnectHandler
        HandleEndpoints,       // EndpointHandler
        HandleDevice           // Device handler.
    },
    g_pBulkDeviceDescriptor,
    g_pBulkConfigDescriptors,
    0,                         // Will be completed during USBDBulkInit().
    0,                         // Will be completed during USBDBulkInit().
    &g_sUSBDefaultFIFOConfig
};

//*****************************************************************************
//
// Set or clear deferred operation flags in an "atomic" manner.
//
// \param pusDeferredOp points to the flags variable which is to be modified.
// \param usBit indicates which bit number is to be set or cleared.
// \param bSet indicates the state that the flag must be set to.  If \b true,
// the flag is set, if \b false, the flag is cleared.
//
// This function safely sets or clears a bit in a flag variable.  The operation
// makes use of bitbanding to ensure that the operation is atomic (no read-
// modify-write is required).
//
// \return None.
//
//*****************************************************************************
static void
SetDeferredOpFlag(volatile unsigned short *pusDeferredOp, unsigned short usBit,
                  tBoolean bSet)
{
    //
    // Set the flag bit to 1 or 0 using a bitband access.
    //
    HWREGBITH(pusDeferredOp, usBit) = bSet ? 1 : 0;
}

//*****************************************************************************
//
// Receives notifications related to data received from the host.
//
// \param psDevice is the device instance whose endpoint is to be processed.
// \param ulStatus is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts signaling
// the arrival of data on the bulk OUT endpoint (in other words, whenever the
// host has sent us a packet of data).  We inform the client that a packet
// is available and, on return, check to see if the packet has been read.  If
// not, we schedule another notification to the client for a later time.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static tBoolean
ProcessDataFromHost(const tUSBDBulkDevice *psDevice, unsigned long ulStatus)
{
    unsigned long ulEPStatus;
    unsigned long ulSize;
    tBulkInstance *psInst;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateBulkData;

    //
    // Get the endpoint status to see why we were called.
    //
    ulEPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ucOUTEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(USB0_BASE, psInst->ucOUTEndpoint, ulEPStatus);

    //
    // Has a packet been received?
    //
    if(ulEPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Set the flag we use to indicate that a packet read is pending.  This
        // will be cleared if the packet is read.  If the client doesn't read
        // the packet in the context of the USB_EVENT_RX_AVAILABLE callback,
        // the event will be signaled later during tick processing.
        //
        SetDeferredOpFlag(&psInst->usDeferredOpFlags, BULK_DO_PACKET_RX, true);

        //
        // How big is the packet we've just been sent?
        //
        ulSize = MAP_USBEndpointDataAvail(psInst->ulUSBBase,
                                          psInst->ucOUTEndpoint);

        //
        // The receive channel is not blocked so let the caller know
        // that a packet is waiting.  The parameters are set to indicate
        // that the packet has not been read from the hardware FIFO yet.
        //
        psDevice->pfnRxCallback(psDevice->pvRxCBData,
                                USB_EVENT_RX_AVAILABLE, ulSize,
                                (void *)0);
    }
    else
    {
        //
        // No packet was received.  Some error must have been reported.  Check
        // and pass this on to the client if necessary.
        //
        if(ulEPStatus & USB_RX_ERROR_FLAGS)
        {
            //
            // This is an error we report to the client so...
            //
            psDevice->pfnRxCallback(psDevice->pvRxCBData,
                                    USB_EVENT_ERROR,
                                    (ulEPStatus & USB_RX_ERROR_FLAGS),
                                    (void *)0);
        }
        return(false);
    }

    return(true);
}

//*****************************************************************************
//
// Receives notifications related to data sent to the host.
//
// \param psDevice is the device instance whose endpoint is to be processed.
// \param ulStatus is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts originating
// from the bulk IN endpoint (in other words, whenever data has been
// transmitted to the USB host).  We examine the cause of the interrupt and,
// if due to completion of a transmission, notify the client.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static tBoolean
ProcessDataToHost(const tUSBDBulkDevice *psDevice, unsigned long ulStatus)
{
    tBulkInstance *psInst;
    unsigned long ulEPStatus;
    unsigned long ulSize;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateBulkData;

    //
    // Get the endpoint status to see why we were called.
    //
    ulEPStatus = MAP_USBEndpointStatus(psInst->ulUSBBase, psInst->ucINEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(psInst->ulUSBBase, psInst->ucINEndpoint,
                                  ulEPStatus);

    //
    // Our last transmission completed.  Clear our state back to idle and
    // see if we need to send any more data.
    //
    psInst->eBulkTxState = BULK_STATE_IDLE;

    //
    // Notify the client that the last transmission completed.
    //
    ulSize = psInst->usLastTxSize;
    psInst->usLastTxSize = 0;
    psDevice->pfnTxCallback(psDevice->pvTxCBData, USB_EVENT_TX_COMPLETE,
                            ulSize, (void *)0);

    return(true);
}

//*****************************************************************************
//
// Called by the USB stack for any activity involving one of our endpoints
// other than EP0.  This function is a fan out that merely directs the call to
// the correct handler depending upon the endpoint and transaction direction
// signaled in ulStatus.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvInstance, unsigned long ulStatus)
{
    const tUSBDBulkDevice *psBulkInst;
    tBulkInstance *psInst;

    ASSERT(pvInstance != 0);

    //
    // Determine if the serial device is in single or composite mode because
    // the meaning of ulIndex is different in both cases.
    //
    psBulkInst = (const tUSBDBulkDevice *)pvInstance;
    psInst = psBulkInst->psPrivateBulkData;

    //
    // Handler for the bulk OUT data endpoint.
    //
    if(ulStatus & (0x10000 << USB_EP_TO_INDEX(psInst->ucOUTEndpoint)))
    {
        //
        // Data is being sent to us from the host.
        //
        ProcessDataFromHost(pvInstance, ulStatus);
    }

    //
    // Handler for the bulk IN data endpoint.
    //
    if(ulStatus & (1 << USB_EP_TO_INDEX(psInst->ucINEndpoint)))
    {
        ProcessDataToHost(pvInstance, ulStatus);
    }
}

//*****************************************************************************
//
// Called by the USB stack whenever a configuration change occurs.
//
//*****************************************************************************
static void
HandleConfigChange(void *pvInstance, unsigned long ulInfo)
{
    tBulkInstance *psInst;
    const tUSBDBulkDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create a device instance pointer.
    //
    psDevice = (const tUSBDBulkDevice *)pvInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateBulkData;

    //
    // Set all our endpoints to idle state.
    //
    psInst->eBulkRxState = BULK_STATE_IDLE;
    psInst->eBulkTxState = BULK_STATE_IDLE;

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psDevice->pfnRxCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psDevice->pfnRxCallback(psDevice->pvRxCBData, USB_EVENT_CONNECTED, 0,
                                (void *)0);
    }

    //
    // Remember that we are connected.
    //
    psInst->bConnected = true;
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvInstance, unsigned long ulRequest, void *pvRequestData)
{
    tBulkInstance *psInst;
    unsigned char *pucData;

    //
    // Create the serial instance data.
    //
    psInst = ((tUSBDBulkDevice *)pvInstance)->psPrivateBulkData;

    //
    // Create the char array used by the events supported by the USB CDC
    // serial class.
    //
    pucData = (unsigned char *)pvRequestData;

    switch(ulRequest)
    {
        //
        // This was an interface change event.
        //
        case USB_EVENT_COMP_IFACE_CHANGE:
        {
            psInst->ucInterface = pucData[1];
            break;
        }

        //
        // This was an endpoint change event.
        //
        case USB_EVENT_COMP_EP_CHANGE:
        {
            //
            // Determine if this is an IN or OUT endpoint that has changed.
            //
            if(pucData[0] & USB_EP_DESC_IN)
            {
                psInst->ucINEndpoint = INDEX_TO_USB_EP((pucData[1] & 0x7f));
            }
            else
            {
                //
                // Extract the new endpoint number.
                //
                psInst->ucOUTEndpoint = INDEX_TO_USB_EP(pucData[1] & 0x7f);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device is
// disconnected from the host.
//
//*****************************************************************************
static void
HandleDisconnect(void *pvInstance)
{
    const tUSBDBulkDevice *psBulkDevice;
    tBulkInstance *psInst;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psBulkDevice = (const tUSBDBulkDevice *)pvInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = psBulkDevice->psPrivateBulkData;

    //
    // If we are not currently connected so let the client know we are open
    // for business.
    //
    if(psInst->bConnected)
    {
        //
        // Pass the disconnected event to the client.
        //
        psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData,
                                    USB_EVENT_DISCONNECTED, 0, (void *)0);
    }

    //
    // Remember that we are no longer connected.
    //
    psInst->bConnected = false;
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is put into
// suspend state.
//
//*****************************************************************************
static void
HandleSuspend(void *pvInstance)
{
    const tUSBDBulkDevice *psBulkDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psBulkDevice = (const tUSBDBulkDevice *)pvInstance;

    //
    // Pass the event on to the client.
    //
    psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData, USB_EVENT_SUSPEND, 0,
                                (void *)0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is taken
// out of suspend state.
//
//*****************************************************************************
static void
HandleResume(void *pvInstance)
{
    const tUSBDBulkDevice *psBulkDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psBulkDevice = (const tUSBDBulkDevice *)pvInstance;

    //
    // Pass the event on to the client.
    //
    psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData, USB_EVENT_RESUME, 0,
                                (void *)0);
}

//*****************************************************************************
//
// This function is called periodically and provides us with a time reference
// and method of implementing delayed or time-dependent operations.
//
// \param ulIndex is the index of the USB controller for which this tick
// is being generated.
// \param ulTimemS is the elapsed time in milliseconds since the last call
// to this function.
//
// \return None.
//
//*****************************************************************************
static void
BulkTickHandler(void *pvInstance, unsigned long ulTimemS)
{
    tBulkInstance *psInst;
    unsigned long ulSize;
    const tUSBDBulkDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDBulkDevice *)pvInstance;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateBulkData;

    //
    // Do we have a deferred receive waiting
    //
    if(psInst->usDeferredOpFlags & (1 << BULK_DO_PACKET_RX))
    {
        //
        // Yes - how big is the waiting packet?
        //
        ulSize = MAP_USBEndpointDataAvail(psInst->ulUSBBase,
                                          psInst->ucOUTEndpoint);

        //
        // Tell the client that there is a packet waiting for it.
        //
        psDevice->pfnRxCallback(psDevice->pvRxCBData, USB_EVENT_RX_AVAILABLE,
                                ulSize, (void *)0);
    }

    return;
}

//*****************************************************************************
//
//! Initializes bulk device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for bulk device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the bulk device.
//!
//! An application wishing to make use of a USB bulk communication channel
//! must call this function to initialize the USB controller and attach the
//! device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psDevice pointer
//! passed to it.  This must be passed on all future calls to the device driver
//! related to this device.
//!
//! The USBDBulk interface offers packet-based transmit and receive operation.
//! If the application would rather use block based communication with
//! transmit and receive buffers, USB buffers may be used above the bulk
//! transmit and receive channels to offer this functionality.
//!
//! Transmit Operation:
//!
//! Calls to USBDBulkPacketWrite must send no more than 64 bytes of data at a
//! time and may only be made when no other transmission is currently
//! outstanding.
//!
//! Once a packet of data has been acknowledged by the USB host, a
//! USB_EVENT_TX_COMPLETE event is sent to the application callback to inform
//! it that another packet may be transmitted.
//!
//! Receive Operation:
//!
//! An incoming USB data packet will result in a call to the application
//! callback with event USBD_EVENT_RX_AVAILABLE.  The application must then
//! call USBDBulkPacketRead(), passing a buffer capable of holding 64 bytes, to
//! retrieve the data and acknowledge reception to the USB host.
//!
//! \note The application must not make any calls to the low level USB Device
//! API if interacting with USB via the USB bulk device class API.  Doing so
//! will cause unpredictable (though almost certainly unpleasant) behavior.
//!
//! \return Returns NULL on failure or the psDevice pointer on success.
//
//*****************************************************************************
void *
USBDBulkInit(unsigned long ulIndex, const tUSBDBulkDevice *psDevice)
{
    void *pvInstance;

    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);

    pvInstance = USBDBulkCompositeInit(ulIndex, psDevice);

    if(pvInstance)
    {
        //
        // All is well so now pass the descriptors to the lower layer and put
        // the bulk device on the bus.
        //
        USBDCDInit(ulIndex, psDevice->psPrivateBulkData->psDevInfo);
    }

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return(pvInstance);
}

//*****************************************************************************
//
//! Initializes bulk device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for bulk device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the bulk device.
//!
//! This call is very similar to USBDBulkInit() except that it is used for
//! initializing an instance of the bulk device for use in a composite device.
//!
//! \return Returns zero on failure or a non-zero value that should be
//! used with the remaining USB HID Bulk APIs.
//
//*****************************************************************************
void *
USBDBulkCompositeInit(unsigned long ulIndex, const tUSBDBulkDevice *psDevice)
{
    tBulkInstance *psInst;
    tDeviceDescriptor *psDevDesc;

    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateBulkData);
    ASSERT(psDevice->pfnRxCallback);
    ASSERT(psDevice->pfnTxCallback);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = psDevice->psPrivateBulkData;
    psInst->psConfDescriptor = (tConfigDescriptor *)g_pBulkDescriptor;
    psInst->psDevInfo = &g_sBulkDeviceInfo;
    psInst->ulUSBBase = USB0_BASE;
    psInst->eBulkRxState = BULK_STATE_UNCONFIGURED;
    psInst->eBulkTxState = BULK_STATE_UNCONFIGURED;
    psInst->usDeferredOpFlags = 0;
    psInst->bConnected = false;

    //
    // Set the default endpoint and interface assignments.
    //
    psInst->ucINEndpoint = DATA_IN_ENDPOINT;
    psInst->ucOUTEndpoint = DATA_OUT_ENDPOINT;
    psInst->ucInterface = 0;

    //
    // Fix up the device descriptor with the client-supplied values.
    //
    psDevDesc = (tDeviceDescriptor *)psInst->psDevInfo->pDeviceDescriptor;
    psDevDesc->idVendor = psDevice->usVID;
    psDevDesc->idProduct = psDevice->usPID;

    //
    // Fix up the configuration descriptor with client-supplied values.
    //
    psInst->psConfDescriptor->bmAttributes = psDevice->ucPwrAttributes;
    psInst->psConfDescriptor->bMaxPower =
                        (unsigned char)(psDevice->usMaxPowermA / 2);

    //
    // Plug in the client's string stable to the device information
    // structure.
    //
    psInst->psDevInfo->ppStringDescriptors = psDevice->ppStringDescriptors;
    psInst->psDevInfo->ulNumStringDescriptors
            = psDevice->ulNumStringDescriptors;

    //
    // Set the device instance.
    //
    psInst->psDevInfo->pvInstance = (void *)psDevice;

    //
    // Initialize the USB tick module, this will prevent it from being
    // initialized later in the call to USBDCDInit();
    //
    InternalUSBTickInit();

    //
    // Register our tick handler (this must be done after USBDCDInit).
    //
    InternalUSBRegisterTickHandler(BulkTickHandler,
                                   (void *)psDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! Shut down the bulk device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//!
//! This function terminates device operation for the instance supplied and
//! removes the device from the USB bus.  This function should not be called
//! if the bulk device is part of a composite device and instead the
//! USBDCompositeTerm() function should be called for the full composite
//! device.
//!
//! Following this call, the \e pvInstance instance should not me used in any
//! other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDBulkTerm(void *pvInstance)
{
    tBulkInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get a pointer to our instance data.
    //
    psInst = ((tUSBDBulkDevice *)pvInstance)->psPrivateBulkData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(USB_BASE_TO_INDEX(psInst->ulUSBBase));

    psInst->ulUSBBase = 0;
    psInst->psDevInfo = (tDeviceInfo *)0;
    psInst->psConfDescriptor = (tConfigDescriptor *)0;

    return;
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the receive channel
//! callback.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the receive channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnRxCallback function
//! passed on USBDBulkInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the \e pvInstance structure passed to USBDBulkInit() resides in
//! RAM.  If this structure is in flash, callback pointer changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's receive callback.
//
//*****************************************************************************
void *
USBDBulkSetRxCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvInstance);

    //
    // Set the callback data for the receive channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDBulkDevice *)pvInstance)->pvRxCBData;
    ((tUSBDBulkDevice *)pvInstance)->pvRxCBData = pvCBData;

    //
    // Return the previous callback pointer.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the transmit callback.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the transmit channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnTxCallback function
//! passed on USBDBulkInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the \e pvInstance structure passed to USBDBulkInit() resides in
//! RAM.  If this structure is in flash, callback pointer changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's transmit callback.
//
//*****************************************************************************
void *
USBDBulkSetTxCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvInstance);

    //
    // Set the callback pointer for the transmit channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDBulkDevice *)pvInstance)->pvTxCBData;
    ((tUSBDBulkDevice *)pvInstance)->pvTxCBData = pvCBData;

    //
    // Return the previous callback pointer value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Transmits a packet of data to the USB host via the bulk data interface.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pcData points to the first byte of data which is to be transmitted.
//! \param ulLength is the number of bytes of data to transmit.
//! \param bLast indicates whether more data is to be written before a packet
//! should be scheduled for transmission.  If \b true, the client will make
//! a further call to this function.  If \b false, no further call will be
//! made and the driver should schedule transmission of a short packet.
//!
//! This function schedules the supplied data for transmission to the USB
//! host in a single USB packet.  If no transmission is currently ongoing,
//! the data is immediately copied to the relevant USB endpoint FIFO for
//! transmission.  Whenever a USB packet is acknowledged by the host, a
//! USB_EVENT_TX_COMPLETE event will be sent to the transmit channel callback
//! indicating that more data can now be transmitted.
//!
//! The maximum value for \e ulLength is 64 bytes (the maximum USB packet size
//! for the bulk endpoints in use by the device).  Attempts to send more data
//! than this will result in a return code of 0 indicating that the data cannot
//! be sent.
//!
//! The \e bLast parameter allows a client to make multiple calls to this
//! function before scheduling transmission of the packet to the host.  This
//! can be helpful if, for example, constructing a packet on the fly or
//! writing a packet which spans the wrap point in a ring buffer.
//!
//! \return Returns the number of bytes actually sent.  At this level, this
//! will either be the number of bytes passed (if less than or equal to the
//! maximum packet size for the USB endpoint in use and no outstanding
//! transmission ongoing) or 0 to indicate a failure.
//
//*****************************************************************************
unsigned long
USBDBulkPacketWrite(void *pvInstance, unsigned char *pcData,
                    unsigned long ulLength, tBoolean bLast)
{
    tBulkInstance *psInst;
    long lRetcode;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDBulkDevice *)pvInstance)->psPrivateBulkData;

    //
    // Can we send the data provided?
    //
    if((ulLength > DATA_IN_EP_MAX_SIZE) ||
       (psInst->eBulkTxState != BULK_STATE_IDLE))
    {
        //
        // Either the packet was too big or we are in the middle of sending
        // another packet.  Return 0 to indicate that we can't send this data.
        //
        return(0);
    }

    //
    // Copy the data into the USB endpoint FIFO.
    //
    lRetcode = MAP_USBEndpointDataPut(psInst->ulUSBBase, psInst->ucINEndpoint,
                                      pcData, ulLength);

    //
    // Did we copy the data successfully?
    //
    if(lRetcode != -1)
    {
        //
        // Remember how many bytes we sent.
        //
        psInst->usLastTxSize += (unsigned short)ulLength;

        //
        // If this is the last call for this packet, schedule transmission.
        //
        if(bLast)
        {
            //
            // Send the packet to the host if we have received all the data we
            // can expect for this packet.
            //
            psInst->eBulkTxState = BULK_STATE_WAIT_DATA;
            lRetcode = MAP_USBEndpointDataSend(psInst->ulUSBBase,
                                               psInst->ucINEndpoint,
                                               USB_TRANS_IN);
        }
    }

    //
    // Did an error occur while trying to send the data?
    //
    if(lRetcode != -1)
    {
        //
        // No - tell the caller we sent all the bytes provided.
        //
        return(ulLength);
    }
    else
    {
        //
        // Yes - tell the caller we couldn't send the data.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Reads a packet of data received from the USB host via the bulk data
//! interface.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pcData points to a buffer into which the received data will be
//! written.
//! \param ulLength is the size of the buffer pointed to by pcData.
//! \param bLast indicates whether the client will make a further call to
//! read additional data from the packet.
//!
//! This function reads up to \e ulLength bytes of data received from the USB
//! host into the supplied application buffer.  If the driver detects that the
//! entire packet has been read, it is acknowledged to the host.
//!
//! The \e bLast parameter is ignored in this implementation since the end of
//! a packet can be determined without relying upon the client to provide
//! this information.
//!
//! \return Returns the number of bytes of data read.
//
//*****************************************************************************
unsigned long
USBDBulkPacketRead(void *pvInstance, unsigned char *pcData,
                   unsigned long ulLength, tBoolean bLast)
{
    unsigned long ulEPStatus, ulCount, ulPkt;
    tBulkInstance *psInst;
    long lRetcode;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDBulkDevice *)pvInstance)->psPrivateBulkData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ulEPStatus = MAP_USBEndpointStatus(psInst->ulUSBBase,
                                       psInst->ucOUTEndpoint);

    if(ulEPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // How many bytes are available for us to receive?
        //
        ulPkt = MAP_USBEndpointDataAvail(psInst->ulUSBBase,
                                         psInst->ucOUTEndpoint);

        //
        // Get as much data as we can.
        //
        ulCount = ulLength;
        lRetcode = MAP_USBEndpointDataGet(psInst->ulUSBBase,
                                          psInst->ucOUTEndpoint,
                                          pcData, &ulCount);

        //
        // Did we read the last of the packet data?
        //
        if(ulCount == ulPkt)
        {
            //
            // Clear the endpoint status so that we know no packet is
            // waiting.
            //
            MAP_USBDevEndpointStatusClear(psInst->ulUSBBase,
                                          psInst->ucOUTEndpoint,
                                          ulEPStatus);

            //
            // Acknowledge the data, thus freeing the host to send the
            // next packet.
            //
            MAP_USBDevEndpointDataAck(psInst->ulUSBBase, psInst->ucOUTEndpoint,
                                      true);

            //
            // Clear the flag we set to indicate that a packet read is
            // pending.
            //
            SetDeferredOpFlag(&psInst->usDeferredOpFlags, BULK_DO_PACKET_RX,
                              false);
        }

        //
        // If all went well, tell the caller how many bytes they got.
        //
        if(lRetcode != -1)
        {
            return(ulCount);
        }
    }

    //
    // No packet was available or an error occurred while reading so tell
    // the caller no bytes were returned.
    //
    return(0);
}

//*****************************************************************************
//
//! Returns the number of free bytes in the transmit buffer.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//!
//! This function returns the maximum number of bytes that can be passed on a
//! call to USBDBulkPacketWrite and accepted for transmission.  The value
//! returned will be the maximum USB packet size (64) if no transmission is
//! currently outstanding or 0 if a transmission is in progress.
//!
//! \return Returns the number of bytes available in the transmit buffer.
//
//*****************************************************************************
unsigned long
USBDBulkTxPacketAvailable(void *pvInstance)
{
    tBulkInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer.
    //
    psInst = ((tUSBDBulkDevice *)pvInstance)->psPrivateBulkData;

    //
    // Do we have a packet transmission currently ongoing?
    //
    if(psInst->eBulkTxState != BULK_STATE_IDLE)
    {
        //
        // We are not ready to receive a new packet so return 0.
        //
        return(0);
    }
    else
    {
        //
        // We can receive a packet so return the max packet size for the
        // relevant endpoint.
        //
        return(DATA_IN_EP_MAX_SIZE);
    }
}

//*****************************************************************************
//
//! Determines whether a packet is available and, if so, the size of the
//! buffer required to read it.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//!
//! This function may be used to determine if a received packet remains to be
//! read and allows the application to determine the buffer size needed to
//! read the data.
//!
//! \return Returns 0 if no received packet remains unprocessed or the
//! size of the packet if a packet is waiting to be read.
//
//*****************************************************************************
unsigned long
USBDBulkRxPacketAvailable(void *pvInstance)
{
    unsigned long ulEPStatus;
    unsigned long ulSize;
    tBulkInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDBulkDevice *)pvInstance)->psPrivateBulkData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ulEPStatus = MAP_USBEndpointStatus(psInst->ulUSBBase,
                                       psInst->ucOUTEndpoint);

    if(ulEPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Yes - a packet is waiting.  How big is it?
        //
        ulSize = MAP_USBEndpointDataAvail(psInst->ulUSBBase,
                                          psInst->ucOUTEndpoint);

        return(ulSize);
    }
    else
    {
        //
        // There is no packet waiting to be received.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the USB library.
//!
//! \param pvInstance is the pointer to the bulk device instance structure.
//! \param ucPower indicates the current power status, either \b
//! USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//!
//! Applications which support switching between bus- or self-powered
//! operation should call this function whenever the power source changes
//! to indicate the current power status to the USB library.  This information
//! is required by the USB library to allow correct responses to be provided
//! when the host requests status from the device.
//!
//! \return None.
//
//*****************************************************************************
void
USBDBulkPowerStatusSet(void *pvInstance, unsigned char ucPower)
{
    ASSERT(pvInstance);

    //
    // Pass the request through to the lower layer.
    //
    USBDCDPowerStatusSet(0, ucPower);
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param pvInstance is the pointer to the bulk device instance structure.
//!
//! When the bus is suspended, an application which supports remote wake up
//! (advertised to the host via the configuration descriptor) may call this function
//! to initiate remote wake up signaling to the host.  If the remote wake up
//! feature has not been disabled by the host, this will cause the bus to
//! resume operation within 20mS.  If the host has disabled remote wake up,
//! \b false will be returned to indicate that the wake up request was not
//! successful.
//!
//! \return Returns \b true if the remote wake up is not disabled and the
//! signaling was started or \b false if remote wake up is disabled or if
//! signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
tBoolean
USBDBulkRemoteWakeupRequest(void *pvInstance)
{
    ASSERT(pvInstance);

    //
    // Pass the request through to the lower layer.
    //
    return(USBDCDRemoteWakeupRequest(0));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
