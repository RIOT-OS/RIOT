//*****************************************************************************
//
// usbhhid.c - This file contains the host HID driver.
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

#include "inc/hw_types.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhhid.h"

static void * HIDDriverOpen(tUSBHostDevice *pDevice);
static void HIDDriverClose(void *pvInstance);

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If the user has not explicitly stated the maximum number of HID devices to
// support, we assume that we need to support up to the maximum number of USB
// devices that the build is configured for.
//
//*****************************************************************************
#ifndef MAX_HID_DEVICES
#define MAX_HID_DEVICES MAX_USB_DEVICES
#endif

//*****************************************************************************
//
// This is the structure that holds all of the data for a given instance of
// a HID device.
//
//*****************************************************************************
typedef struct
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *pDevice;

    //
    // Used to save the callback.
    //
    tUSBCallback pfnCallback;

    //
    // Callback data provided by caller.
    //
    unsigned long ulCBData;

    //
    // Used to remember what type of device was registered.
    //
    tHIDSubClassProtocol eDeviceType;

    //
    // Interrupt IN pipe.
    //
    unsigned long ulIntInPipe;
}
tHIDInstance;

//*****************************************************************************
//
// The instance data storage for attached hid devices.
//
//*****************************************************************************
static tHIDInstance g_pHIDDevice[MAX_HID_DEVICES];

//*****************************************************************************
//
//! This constant global structure defines the HID Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_USBHIDClassDriver =
{
    USB_CLASS_HID,
    HIDDriverOpen,
    HIDDriverClose,
    0
};

//*****************************************************************************
//
//! This function is used to open an instance of a HID device.
//!
//! \param eDeviceType is the type of device that should be loaded for this
//! instance of the HID device.
//! \param pfnCallback is the function that will be called whenever changes
//! are detected for this device.
//! \param ulCBData is the data that will be returned in when the pfnCallback
//! function is called.
//!
//! This function creates an instance of an specific type of HID device.  The
//! \e eDeviceType parameter is one subclass/protocol values of the types
//! specified in enumerated types tHIDSubClassProtocol.  Only devices that
//! enumerate with this type will be called back via the \e pfnCallback
//! function.  The \e pfnCallback parameter is the callback function for any
//! events that occur for this device type.  The \e pfnCallback function must
//! point to a valid function of type \e tUSBCallback for this call to complete
//! successfully.  To release this device instance the caller of USBHHIDOpen()
//! should call USBHHIDClose() and pass in the value returned from the
//! USBHHIDOpen() call.
//!
//! \return This function returns and instance value that should be used with
//! any other APIs that require an instance value.  If a value of 0 is returned
//! then the device instance could not be created.
//
//*****************************************************************************
unsigned long
USBHHIDOpen(tHIDSubClassProtocol eDeviceType, tUSBCallback pfnCallback,
            unsigned long ulCBData)
{
    unsigned long ulLoop;

    //
    // Find a free device instance structure.
    //
    for(ulLoop = 0; ulLoop < MAX_HID_DEVICES; ulLoop++)
    {
        if(g_pHIDDevice[ulLoop].eDeviceType == USBH_HID_DEV_NONE)
        {
            //
            // Save the instance data for this device.
            //
            g_pHIDDevice[ulLoop].pfnCallback = pfnCallback;
            g_pHIDDevice[ulLoop].eDeviceType = eDeviceType;
            g_pHIDDevice[ulLoop].ulCBData = ulCBData;

            //
            // Return the device instance pointer.
            //
            return((unsigned long)&g_pHIDDevice[ulLoop]);
        }
    }

    //
    // If we get here, there are no space device slots so return NULL to
    // indicate a problem.
    //
    return(0);
}

//*****************************************************************************
//
//! This function is used to release an instance of a HID device.
//!
//! \param ulHIDInstance is the instance value for a HID device to release.
//!
//! This function releases an instance of a HID device that was created by a
//! call to USBHHIDOpen().  This call is required to allow other HID devices
//! to be enumerated after another HID device has been disconnected.  The
//! \e ulHIDInstance parameter should hold the value that was returned from the
//! previous call to USBHHIDOpen().
//!
//! \return None.
//
//*****************************************************************************
void
USBHHIDClose(unsigned long ulHIDInstance)
{
    tHIDInstance *pInst;

    //
    // Get our instance pointer.
    //
    pInst = (tHIDInstance *)ulHIDInstance;

    //
    // Disable any more notifications from the HID layer.
    //
    pInst->pfnCallback = 0;

    //
    // Mark this device slot as free.
    //
    pInst->eDeviceType = USBH_HID_DEV_NONE;
}

//*****************************************************************************
//
// This function handles callbacks for the interrupt IN endpoint.
//
//*****************************************************************************
static void
HIDIntINCallback(unsigned long ulPipe, unsigned long ulEvent)
{
    long lDev;

    switch (ulEvent)
    {
        //
        // Handles a request to schedule a new request on the interrupt IN
        // pipe.
        //
        case USB_EVENT_SCHEDULER:
        {
            USBHCDPipeSchedule(ulPipe, 0, 1);
            break;
        }
        //
        // Called when new data is available on the interrupt IN pipe.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // Determine which device this notification is intended for.
            //
            for(lDev = 0; lDev < MAX_HID_DEVICES; lDev++)
            {
                //
                // Does this device own the pipe we've been passed?
                //
                if(g_pHIDDevice[lDev].ulIntInPipe == ulPipe)
                {
                    //
                    // Yes - send the report data to the USB host HID device
                    // class driver.
                    //
                    g_pHIDDevice[lDev].pfnCallback(
                                          (void *)g_pHIDDevice[lDev].ulCBData,
                                          USB_EVENT_RX_AVAILABLE,
                                          ulPipe,
                                          0);
                }
            }

            break;
        }
    }
}

//*****************************************************************************
//
//! This function is used to open an instance of the HID driver.
//!
//! \param pDevice is a pointer to the device information structure.
//!
//! This function will attempt to open an instance of the HID driver based on
//! the information contained in the pDevice structure.  This call can fail if
//! there are not sufficient resources to open the device.  The function will
//! return a value that should be passed back into USBHIDClose() when the
//! driver is no longer needed.
//!
//! \return The function will return a pointer to a HID driver instance.
//
//*****************************************************************************
static void *
HIDDriverOpen(tUSBHostDevice *pDevice)
{
    long lIdx, lDev;
    tEndpointDescriptor *pEndpointDescriptor;
    tInterfaceDescriptor *pInterface;

    //
    // Get the interface descriptor.
    //
    pInterface = USBDescGetInterface(pDevice->pConfigDescriptor, 0, 0);

    //
    // Search the currently open instances for one that supports the protocol
    // of this device.
    //
    for(lDev = 0; lDev < MAX_HID_DEVICES; lDev++)
    {
        if(g_pHIDDevice[lDev].eDeviceType == pInterface->bInterfaceProtocol)
        {
            //
            // Save the device pointer.
            //
            g_pHIDDevice[lDev].pDevice = pDevice;

            for(lIdx = 0; lIdx < 3; lIdx++)
            {
                //
                // Get the first endpoint descriptor.
                //
                pEndpointDescriptor = USBDescGetInterfaceEndpoint(pInterface,
                                                                  lIdx, 256);

                //
                // If no more endpoints then break out.
                //
                if(pEndpointDescriptor == 0)
                {
                    break;
                }

                //
                // Interrupt
                //
                if((pEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
                   USB_EP_ATTR_INT)
                {
                    //
                    // Interrupt IN.
                    //
                    if(pEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
                    {
                        g_pHIDDevice[lDev].ulIntInPipe = USBHCDPipeAlloc(0,
                                                          USBHCD_PIPE_INTR_IN,
                                                          pDevice,
                                                          HIDIntINCallback);
                        USBHCDPipeConfig(g_pHIDDevice[lDev].ulIntInPipe,
                                         pEndpointDescriptor->wMaxPacketSize,
                                         pEndpointDescriptor->bInterval,
                                         (pEndpointDescriptor->bEndpointAddress &
                                          USB_EP_DESC_NUM_M));
                    }
                }
            }

            //
            // If there is a callback function call it to inform the application that
            // the device has been enumerated.
            //
            if(g_pHIDDevice[lDev].pfnCallback != 0)
            {
                g_pHIDDevice[lDev].pfnCallback(
                                        (void *)g_pHIDDevice[lDev].ulCBData,
                                        USB_EVENT_CONNECTED,
                                        (unsigned long)&g_pHIDDevice[lDev], 0);
            }

            //
            // Save the device pointer.
            //
            g_pHIDDevice[lDev].pDevice = pDevice;

            return (&g_pHIDDevice[lDev]);
        }
    }

    //
    // If we get here, no user has registered an interest in this particular
    // HID device so we return an error.
    //
    return(0);
}

//*****************************************************************************
//
//! This function is used to release an instance of the HID driver.
//!
//! \param pvInstance is an instance pointer that needs to be released.
//!
//! This function will free up any resources in use by the HID driver instance
//! that is passed in.  The \e pvInstance pointer should be a valid value that
//! was returned from a call to USBHIDOpen().
//!
//! \return None.
//
//*****************************************************************************
static void
HIDDriverClose(void *pvInstance)
{
    tHIDInstance *pInst;

    //
    // Get our instance pointer.
    //
    pInst = (tHIDInstance *)pvInstance;

    //
    // Reset the device pointer.
    //
    pInst->pDevice = 0;

    //
    // Free the Interrupt IN pipe.
    //
    if(pInst->ulIntInPipe != 0)
    {
        USBHCDPipeFree(pInst->ulIntInPipe);
    }

    //
    // If the callback exists, call it with a DISCONNECTED event.
    //
    if(pInst->pfnCallback != 0)
    {
        pInst->pfnCallback((void *)pInst->ulCBData,
                           USB_EVENT_DISCONNECTED,
                           (unsigned long)pvInstance, 0);
    }
}

//*****************************************************************************
//
//! This function is used to set the idle timeout for a HID device.
//!
//! \param ulInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ucDuration is the duration of the timeout in milliseconds.
//! \param ucReportID is the report identifier to set the timeout on.
//!
//! This function will send the Set Idle command to a HID device to set the
//! idle timeout for a given report.  The length of the timeout is specified
//! by the \e ucDuration parameter and the report the timeout for is in the
//! \e ucReportID value.
//!
//! \return Always returns 0.
//
//*****************************************************************************
unsigned long
USBHHIDSetIdle(unsigned long ulInstance, unsigned char ucDuration,
               unsigned char ucReportID)
{
    tUSBRequest SetupPacket;
    tHIDInstance *pHIDInstance;

    pHIDInstance = (tHIDInstance *)ulInstance;

    //
    // This is a Class specific interface OUT request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS
                    | USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_SET_IDLE;
    SetupPacket.wValue = (ucDuration << 8) | ucReportID;

    //
    // Set this on interface 1.
    //
    SetupPacket.wIndex = 0;

    //
    // This is always 0 for this request.
    //
    SetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    return(USBHCDControlTransfer(0, &SetupPacket, pHIDInstance->pDevice,
                                 0, 0, MAX_PACKET_SIZE_EP0));
}

//*****************************************************************************
//
//! This function can be used to retrieve the report descriptor for a given
//! device instance.
//!
//! \param ulInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param pucBuffer is the memory buffer to use to store the report
//! descriptor.
//! \param ulSize is the size in bytes of the buffer pointed to by
//! \e pucBuffer.
//!
//! This function is used to return a report descriptor from a HID device
//! instance so that it can determine how to interpret reports that are
//! returned from the device indicated by the \e ulInstance parameter.
//! This call is blocking and will return the number of bytes read into the
//! \e pucBuffer.
//!
//! \return Returns the number of bytes read into the \e pucBuffer.
//
//*****************************************************************************
unsigned long
USBHHIDGetReportDescriptor(unsigned long ulInstance, unsigned char *pucBuffer,
                           unsigned long ulSize)
{
    tUSBRequest SetupPacket;
    unsigned long ulBytes;
    tHIDInstance *pHIDInstance;

    pHIDInstance = (tHIDInstance *)ulInstance;

    //
    // This is a Standard Device IN request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_STANDARD
                    | USB_RTYPE_INTERFACE;

    //
    // Request a Report Descriptor.
    //
    SetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    SetupPacket.wValue = USB_HID_DTYPE_REPORT << 8;

    //
    // Index is always 0 for device requests.
    //
    SetupPacket.wIndex = 0;

    //
    // All devices must have at least an 8 byte max packet size so just ask
    // for 8 bytes to start with.
    //
    SetupPacket.wLength = ulSize;

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    ulBytes = USBHCDControlTransfer(
                0,
                &SetupPacket,
                pHIDInstance->pDevice,
                pucBuffer,
                ulSize,
                pHIDInstance->pDevice->DeviceDescriptor.bMaxPacketSize0);

    return(ulBytes);
}

//*****************************************************************************
//
//! This function is used to set or clear the boot protocol state of a device.
//!
//! \param ulInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ulBootProtocol is either zero or non-zero to indicate which protocol
//! to use for the device.
//!
//! A USB host device can use this function to set the protocol for a connected
//! HID device.  This is commonly used to set keyboards and mice into their
//! simplified boot protocol modes to fix the report structure to a know
//! state.
//!
//! \return This function returns 0.
//
//*****************************************************************************
unsigned long
USBHHIDSetProtocol(unsigned long ulInstance, unsigned long ulBootProtocol)
{
    tUSBRequest SetupPacket;
    tHIDInstance *pHIDInstance;

    pHIDInstance = (tHIDInstance *)ulInstance;

    //
    // This is a Standard Device IN request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS
                    | USB_RTYPE_INTERFACE;

    //
    // Request a Report Descriptor.
    //
    SetupPacket.bRequest = USBREQ_SET_PROTOCOL;

    if(ulBootProtocol)
    {
        //
        // Boot Protocol.
        //
        SetupPacket.wValue = 0;
    }
    else
    {
        //
        // Report Protocol.
        //
        SetupPacket.wValue = 1;
    }

    //
    // Index is always 0 for device requests.
    //
    SetupPacket.wIndex = 0;

    //
    // Always 0.
    //
    SetupPacket.wLength = 0;

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    USBHCDControlTransfer(
        0,
        &SetupPacket,
        pHIDInstance->pDevice,
        0,
        0,
        pHIDInstance->pDevice->DeviceDescriptor.bMaxPacketSize0);

    return(0);
}

//*****************************************************************************
//
//! This function is used to retrieve a report from a HID device.
//!
//! \param ulInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ulInterface is the interface to retrieve the report from.
//! \param pucData is the memory buffer to use to store the report.
//! \param ulSize is the size in bytes of the buffer pointed to by
//! \e pucBuffer.
//!
//! This function is used to retrieve a report from a USB pipe.  It is usually
//! called when the USB HID layer has detected a new data available in a USB
//! pipe.  The USB HID host device code will receive a
//! \b USB_EVENT_RX_AVAILABLE event when data is available, allowing the
//! callback function to retrieve the data.
//!
//! \return Returns the number of bytes read from report.
//
//*****************************************************************************
unsigned long
USBHHIDGetReport(unsigned long ulInstance,
                 unsigned long ulInterface,
                 unsigned char *pucData,
                 unsigned long ulSize)
{
    tHIDInstance *pHIDInstance;

    //
    // Cast the instance pointer to the correct type for ease of use.
    //
    pHIDInstance = (tHIDInstance *)ulInstance;

    //
    // Read the Data out.
    //
    ulSize = USBHCDPipeReadNonBlocking(pHIDInstance->ulIntInPipe, pucData,
                                       ulSize);

    //
    // Return the number of bytes read from the interrupt in pipe.
    //
    return(ulSize);
}

//*****************************************************************************
//
//! This function is used to send a report to a HID device.
//!
//! \param ulInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ulInterface is the interface to send the report to.
//! \param pucData is the memory buffer to use to store the report.
//! \param ulSize is the size in bytes of the buffer pointed to by
//! \e pucBuffer.
//!
//! This function is used to send a report to a USB HID device.  It can be
//! only be called from outside the callback context as this function will not
//! return from the call until the data has been sent successfully.
//!
//! \return Returns the number of bytes sent to the device.
//
//*****************************************************************************
unsigned long
USBHHIDSetReport(unsigned long ulInstance, unsigned long ulInterface,
                 unsigned char *pucData, unsigned long ulSize)
{
    tUSBRequest SetupPacket;
    tHIDInstance *pHIDInstance;

    pHIDInstance = (tHIDInstance *)ulInstance;

    //
    // This is a Standard Device IN request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS
                    | USB_RTYPE_INTERFACE;

    //
    // Request a Report Descriptor.
    //
    SetupPacket.bRequest = USBREQ_SET_REPORT;
    SetupPacket.wValue = USB_HID_REPORT_OUTPUT << 8;

    //
    // Index is always 0 for device requests.
    //
    SetupPacket.wIndex = (unsigned short)ulInterface;

    //
    // Always 0.
    //
    SetupPacket.wLength = ulSize;

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    USBHCDControlTransfer(0, &SetupPacket, pHIDInstance->pDevice,
            pucData, ulSize,
            pHIDInstance->pDevice->DeviceDescriptor.bMaxPacketSize0);

    return(ulSize);
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
