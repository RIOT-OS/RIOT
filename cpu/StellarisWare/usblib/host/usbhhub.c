//*****************************************************************************
//
// usbhhub.c - This file contains the host HID driver.
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

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/usb.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rtos_bindings.h"
#include "usblib/usblib.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhhub.h"
#ifdef INCLUDE_DEBUG_OUTPUT
#include "utils/uartstdio.h"
#define DEBUG_OUTPUT UARTprintf
#else
#define DEBUG_OUTPUT while(0)((int (*)(char *, ...))0)
#endif

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Forward references to the hub class driver functions.
//
//*****************************************************************************
static void *HubDriverOpen(tUSBHostDevice *pDevice);
static void HubDriverClose(void *pvInstance);

//*****************************************************************************
//
//! This constant global structure defines the Hub Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_USBHubClassDriver =
{
    USB_CLASS_HUB,
    HubDriverOpen,
    HubDriverClose,
    0
};

//*****************************************************************************
//
// The instance data storage for attached hub.
//
//*****************************************************************************
static tHubInstance *g_pRootHub;

//*****************************************************************************
//
// Hub and port state change flags as reported via the hub's IN endpoint.
//
//*****************************************************************************
static volatile unsigned long g_ulChangeFlags;

//
// Note: The following assumes ROOT_HUB_MAX_PORTS is less than 32!
//
static unsigned long g_ulHubChanges;

//*****************************************************************************
//
// This function is called to send a request to the hub to set a feature on
// a given port.
//
// \param ulInstance is the hub device instance.
// \param ucPort is the port number for this request.
// \param usFeature is one of the HUB_FEATURE_PORT_* values.
//
// This function will send the set feature request to the hub indicated by the
// \e ulInstance parameter.  The \e ucPort value indicates which port number
// to send this request to and can range from 0 to the number of valid ports
// on the given hub.  A \e ucPort value of 0 is an access to the hub itself and
// not one of the hub ports.  The \e usFeature is the feature request to set
// on the given port.  For example, a \e usFeature value of
// \e HUB_FEATURE_PORT_RESET and \e ucPort value of 1 will cause reset
// signaling to hub port 1.
//
// \return None.
//
//*****************************************************************************
static void
HubSetPortFeature(unsigned long ulInstance, unsigned char ucPort,
                  unsigned short usFeature)
{
    tUSBRequest SetupPacket;
    tHubInstance *pHubInstance;
    tUSBHostDevice *pDevice;

    //
    // Retrieve the hub instance and device pointer.
    //
    pHubInstance = (tHubInstance *)ulInstance;
    pDevice = pHubInstance->pDevice;

    //
    // This is a standard OUT request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                USB_RTYPE_OTHER;

    //
    // Set the field to clear the requested port feature.
    //
    SetupPacket.bRequest = USBREQ_SET_FEATURE;
    SetupPacket.wValue = usFeature;
    SetupPacket.wIndex = ucPort;
    SetupPacket.wLength = 0;

    //
    // Send the request.
    //
    USBHCDControlTransfer(0,
                          &SetupPacket,
                          pDevice,
                          0,
                          0,
                          pDevice->DeviceDescriptor.bMaxPacketSize0);
}

//*****************************************************************************
//
// This function is called to send a request to the hub to clear a feature on
// a given port.
//
// \param ulInstance is the hub device instance.
// \param ucPort is the port number for this request.
// \param usFeature is one of the HUB_FEATURE_PORT_* values.
//
// This function will send the clear feature request to the hub indicated by
// the \e ulInstance parameter.  The \e ucPort value indicates which port
// number to send this request to and can range from 0 to the number of valid
// ports on the given hub.  A \e ucPort value of 0 is an access to the hub
// itself and not one of the hub ports.  The \e usFeature is the feature
// request to clear on the given port.  For example, a \e usFeature value of
// \e HUB_FEATURE_C_PORT_RESET and \e ucPort value of 1 will clear the reset
// complete signaling on hub port 1.  Values like the reset feature will
// remain set until actively cleared by this function.
//
// \return None.
//
//*****************************************************************************
static void
HubClearPortFeature(unsigned long ulInstance, unsigned char ucPort,
                    unsigned short usFeature)
{
    tUSBRequest SetupPacket;
    tHubInstance *pHubInstance;
    tUSBHostDevice *pDevice;

    //
    // Retrieve the hub instance and device pointer.
    //
    pHubInstance = (tHubInstance *)ulInstance;
    pDevice = pHubInstance->pDevice;

    //
    // This is a standard OUT request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                USB_RTYPE_OTHER;

    //
    // Set the field to clear the requested port feature.
    //
    SetupPacket.bRequest = USBREQ_CLEAR_FEATURE;
    SetupPacket.wValue = usFeature;
    SetupPacket.wIndex = ucPort;
    SetupPacket.wLength = 0;

    //
    // Send the request.
    //
    USBHCDControlTransfer(0,
                          &SetupPacket,
                          pDevice,
                          0,
                          0,
                          pDevice->DeviceDescriptor.bMaxPacketSize0);
}

//*****************************************************************************
//
// This function is used to retrieve the current status of a port on the
// hub.
//
// \param ulInstance is the hub device instance.
// \param ucPort is the port number for this request.
// \param pusPortStatus is a pointer to the memory to store the current status
// of the port.
// \param pusPortChange is a pointer to the memory to store the current change
// status of the ports.
//
// This function is used to retrieve the current overall status and change
// status for the port given in the \e ucPort parameter.  The \e ucPort value
// indicates which port number to send this request to and can range from 0 to
// the number of valid ports on the given hub.  A \e ucPort value of 0 is an
// access to the hub itself and not one of the hub ports.
//
// \return None.
//
//*****************************************************************************
static tBoolean
HubGetPortStatus(unsigned long ulInstance, unsigned char ucPort,
                 unsigned short *pusPortStatus, unsigned short *pusPortChange)
{
    unsigned long ulData, ulRead;
    tUSBRequest SetupPacket;
    tHubInstance *pHubInstance;
    tUSBHostDevice *pDevice;

    //
    // Retrieve the hub instance and device pointer.
    //
    pHubInstance = (tHubInstance *)ulInstance;
    pDevice = pHubInstance->pDevice;

    //
    // This is a standard OUT request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                USB_RTYPE_OTHER;

    //
    // Set the fields to get the hub status.
    //
    SetupPacket.bRequest = USBREQ_GET_STATUS;
    SetupPacket.wValue = 0;
    SetupPacket.wIndex = (unsigned short)ucPort;
    SetupPacket.wLength = 4;

    //
    // Send the request.
    //
    ulRead = USBHCDControlTransfer(0, &SetupPacket,
                                   pDevice,
                                   (unsigned char *)&ulData, 4,
                                   pDevice->DeviceDescriptor.bMaxPacketSize0);

    //
    // Check that we received the correct number of bytes.
    //
    if(ulRead != 4)
    {
        return(false);
    }
    else
    {
        //
        // We got 4 bytes from the device. Now translate these into the 2
        // unsigned shorts we pass back to the caller.
        //
        *pusPortStatus = (unsigned short)(ulData & 0xFFFF);
        *pusPortChange = (unsigned short)(ulData >> 16);

        DEBUG_OUTPUT("Port %d, status 0x%04x, change 0x%04x\n", ucPort,
                     *pusPortStatus, *pusPortChange);
    }

    //
    // All is well.
    //
    return(true);
}

//*****************************************************************************
//
// This function handles callbacks for the interrupt IN endpoint for the hub
// device.
//
//*****************************************************************************
static void
HubIntINCallback(unsigned long ulPipe, unsigned long ulEvent)
{
    switch (ulEvent)
    {
        //
        // Handles a request to schedule a new request on the interrupt IN
        // pipe.
        //
        case USB_EVENT_SCHEDULER:
        {
            //
            // Set things up to read the next change indication from the hub.
            //
            USBHCDPipeSchedule(ulPipe, (unsigned char *)&g_ulHubChanges,
                               (unsigned long)g_pRootHub->ucReportSize);
            break;
        }

        //
        // Called when new data is available on the interrupt IN pipe.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // For data transfers on INT IN endpoints, we need to acknowledge
            // the data from this callback.
            //
            USBHCDPipeDataAck(ulPipe);

            //
            // Update our global "ports needing service" flags with the latest
            // information we've just received.
            //
            g_ulChangeFlags |= g_ulHubChanges;

            //
            // Send the report data to the USB host hub device class driver if
            // we have been given a callback function.
            //
            if(g_pRootHub->pfnCallback)
            {
                g_pRootHub->pfnCallback((void *)g_pRootHub->ulCBData,
                                        USB_EVENT_RX_AVAILABLE,
                                        ulPipe,
                                        &g_ulHubChanges);
            }

            break;
        }
        case USB_EVENT_ERROR:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// Query the class-specific hub descriptor.
//
//*****************************************************************************
static tBoolean
GetHubDescriptor(tUsbHubDescriptor *psDesc)
{
    unsigned long ulRead;
    tUSBRequest SetupPacket;
    tUSBHostDevice *pDevice;

    //
    // Retrieve the device pointer.
    //
    pDevice = g_pRootHub->pDevice;

    //
    // This is a standard OUT request.
    //
    SetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                USB_RTYPE_DEVICE;

    //
    // Set the fields to get the hub descriptor.  Initially, we request only
    // the first 4 bytes of the descriptor.  This will give us the size which
    // we use to determine how many bytes to read to get the full descriptor.
    // This is necessary since we don't know how many ports the hub can support
    // and we only support up to MAX_USB_DEVICES.
    //
    SetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    SetupPacket.wValue = (USB_DTYPE_HUB << 8);
    SetupPacket.wIndex = 0;
    SetupPacket.wLength = sizeof(tUsbHubDescriptor);

    //
    // Send the request.
    //
    ulRead = USBHCDControlTransfer(0, &SetupPacket,
                                   pDevice,
                                   (void *)psDesc, sizeof(tUsbHubDescriptor),
                                   pDevice->DeviceDescriptor.bMaxPacketSize0);

     //
     // Make sure we got at least some data.
     //
     if(ulRead == 0)
     {
         return(false);
     }

    //
    // All is well.
    //
    return(true);
}

//*****************************************************************************
//
// Open an instance of the hub driver.  This is called when the USB host
// has enumerated a new hub device.
//
//*****************************************************************************
static void *
HubDriverOpen(tUSBHostDevice *pDevice)
{
    tEndpointDescriptor *pEndpointDescriptor;
    tInterfaceDescriptor *pInterface;
    tUsbHubDescriptor  sHubDesc;
    tBoolean bRetcode;
    unsigned long ulLoop;

    //
    // If we are already talking to a hub, fail the call.  We only support
    // a single hub.
    //
    if(g_pRootHub->bHubActive)
    {
        return(0);
    }

    //
    // Get pointers to the device descriptors we need to look at.
    //
    pInterface = USBDescGetInterface(pDevice->pConfigDescriptor, 0, 0);
    pEndpointDescriptor = USBDescGetInterfaceEndpoint(pInterface, 0,
                                      pDevice->ulConfigDescriptorSize);

    //
    // If there are no endpoints, something is wrong since a hub must have
    // a single INT endpoint for signaling.
    //
    if(pEndpointDescriptor == 0)
    {
        return 0;
    }

    //
    // Make sure we really are talking to a hub.
    //
    if((pInterface->bInterfaceClass != USB_CLASS_HUB) ||
       pInterface -> bInterfaceSubClass || pInterface -> bInterfaceProtocol)
    {
        //
        // Something is wrong - this isn't a hub or, if it is, we don't
        // understand the protocol it is using.
        //
        return(0);
    }

    //
    // Remember the device information for later.
    //
    g_pRootHub->pDevice = pDevice;

    //
    // A hub must support an interrupt endpoint so check this.
    //
    if((pEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
       USB_EP_ATTR_INT)
    {
        //
        // The endpoint is the correct type. Is it an IN endpoint?
        //
        if(pEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
        {
            //
            // Yes - all is well with the hub endpoint so allocate a pipe to
            // handle traffic from the hub.
            //
            g_pRootHub->ulIntInPipe = USBHCDPipeAlloc(0,USBHCD_PIPE_INTR_IN,
                                                      pDevice,
                                                      HubIntINCallback);
            USBHCDPipeConfig(g_pRootHub->ulIntInPipe,
                             pEndpointDescriptor->wMaxPacketSize,
                             pEndpointDescriptor->bInterval,
                             pEndpointDescriptor->bEndpointAddress &
                             USB_EP_DESC_NUM_M);
        }
    }

    //
    // Did we allocate the endpoint successfully?
    //
    if(!g_pRootHub->ulIntInPipe)
    {
        //
        // No - return an error.
        //
        return 0;
    }

    //
    // Assuming we have a callback, call it to tell the owner that a hub is
    // now connected.
    //
    if(g_pRootHub->pfnCallback != 0)
    {
        g_pRootHub->pfnCallback((void *)g_pRootHub->ulCBData,
                                USB_EVENT_CONNECTED,
                                (unsigned long)g_pRootHub, 0);
    }

    //
    // Get the hub descriptor and store information we'll need for later.
    //
    bRetcode = GetHubDescriptor(&sHubDesc);
    if(bRetcode)
    {

        //
        // We read the descriptor successfully so extract the parts we need.
        //
        g_pRootHub->ucNumPorts = sHubDesc.bNbrPorts;
        g_pRootHub->usHubCharacteristics = sHubDesc.wHubCharacteristics;
        g_pRootHub->ucNumPortsInUse = (sHubDesc.bNbrPorts > MAX_USB_DEVICES) ?
                                      MAX_USB_DEVICES : sHubDesc.bNbrPorts;

        //
        // The size of the status change report that the hub sends is dependent
        // upon the number of ports that the hub supports.  Calculate this by
        // adding 1 to the number of ports (bit 0 of the report is the hub
        // status, higher bits are one per port) then dividing by 8 (bits per
        // byte) and rounding up.
        //
        g_pRootHub->ucReportSize = ((sHubDesc.bNbrPorts + 1) + 7) / 8;

        //
        // Enable power to all ports on the hub.
        //
        for(ulLoop = 1; ulLoop <= sHubDesc.bNbrPorts; ulLoop++)
        {
            //
            // Turn on power to this port.
            //
            HubSetPortFeature((unsigned long )g_pRootHub, ulLoop,
                              HUB_FEATURE_PORT_POWER);
        }

        //
        // Clear out our port state structures.
        //
        for(ulLoop = 0; ulLoop < MAX_USB_DEVICES; ulLoop++)
        {
            g_pRootHub->psPorts[ulLoop].bChanged = false;
            g_pRootHub->psPorts[ulLoop].sState = PORT_IDLE;
        }
    }
    else
    {
        //
        // Oops - we can't read the hub descriptor!  Tidy up and return
        // an error.
        //
        USBHCDPipeFree(g_pRootHub->ulIntInPipe);
        g_pRootHub->pfnCallback = 0;
        g_pRootHub->bHubActive = false;
        return(0);
    }

    //
    // If we get here, all is well so remember that the hub is connected and
    // active.
    //
    g_pRootHub->bHubActive = true;

    //
    // Return our instance data pointer to the caller to use as a handle.
    //
    return((void *)g_pRootHub);
}

//*****************************************************************************
//
// Close an instance of the hub driver.
//
//*****************************************************************************
static void
HubDriverClose(void *pvInstance)
{
    unsigned long ulLoop;

    //
    // No device so just exit.
    //
    if(g_pRootHub->pDevice == 0)
    {
        return;
    }

    //
    // Disconnect any devices that are currently connected to the hub.
    //
    for(ulLoop = 0; ulLoop < MAX_USB_DEVICES; ulLoop++)
    {
        //
        // Does this port have a device connected to it that we have previously
        // reported to the host control layer?
        //
        if((g_pRootHub->psPorts[ulLoop].sState == PORT_ACTIVE) ||
           (g_pRootHub->psPorts[ulLoop].sState == PORT_RESET_WAIT) ||
           (g_pRootHub->psPorts[ulLoop].sState == PORT_ENUMERATED) ||
           (g_pRootHub->psPorts[ulLoop].sState == PORT_ERROR))
        {
            //
            // Yes - tell the host controller to disconnect the device.
            //
            USBHCDHubDeviceDisconnected(0,
                g_pRootHub->psPorts[ulLoop].ulDevHandle);

        }

        //
        // Make sure that the state returns to idle.
        //
        g_pRootHub->psPorts[ulLoop].sState = PORT_IDLE;

    }

    //
    // Reset the device pointer.
    //
    g_pRootHub->pDevice = 0;

    //
    // Mark the hub as absent.
    //
    g_pRootHub->bHubActive = false;

    //
    // Note that we are not in the middle of enumerating anything.
    //
    g_pRootHub->bEnumerationBusy = false;

    //
    // Free the Interrupt IN pipe.
    //
    if(g_pRootHub->ulIntInPipe != 0)
    {
        USBHCDPipeFree(g_pRootHub->ulIntInPipe);
    }

    //
    // If the callback exists, call it with a DISCONNECTED event.
    //
    if(g_pRootHub->pfnCallback != 0)
    {
        g_pRootHub->pfnCallback((void *)g_pRootHub->ulCBData,
                                USB_EVENT_DISCONNECTED,
                                (unsigned long)g_pRootHub, 0);
    }
}

//*****************************************************************************
//
// Perform any processing required as a result of a change in the reset
// signaling for a given port.
//
//*****************************************************************************
static void
HubDriverReset(unsigned char ucPort, tBoolean bResetActive)
{
    //
    // Did the reset sequence end or begin?
    //
    if(!bResetActive)
    {
        //
        // The reset ended.  Now wait for at least 10ms before signaling
        // USB enumeration code that a new device is waiting to be enumerated.
        //
        g_pRootHub->psPorts[ucPort].sState = PORT_RESET_WAIT;

        //
        // Set the wait to 10ms (10 frames) from now.
        //
        g_pRootHub->psPorts[ucPort].ulCount = 10;
    }
    else
    {
        //
        // Was this device previously active?
        //
        if(g_pRootHub->psPorts[ucPort].sState == PORT_ACTIVE)
        {
            USBHCDHubDeviceDisconnected(0,
                g_pRootHub->psPorts[ucPort].ulDevHandle);
        }

        //
        // The reset is active so mark our port as in reset.
        //
        g_pRootHub->psPorts[ucPort].sState = PORT_RESET_ACTIVE;
    }
}

//*****************************************************************************
//
// Start the process of enumerating a new device by issuing a reset to the
// appropriate downstream port.
//
//*****************************************************************************
static void
HubDriverDeviceReset(unsigned char ucPort)
{
    DEBUG_OUTPUT("Starting enumeration for port %d\n", ucPort);

    //
    // Record the fact that we are in the process of enumerating a device.
    //
    g_pRootHub->bEnumerationBusy = true;

    //
    // Save the port that is being enumerated.
    //
    g_pRootHub->ucEnumIdx = ucPort;

    //
    // Mark the port as being reset.
    //
    g_pRootHub->psPorts[ucPort].sState = PORT_RESET_ACTIVE;

    //
    // Initiate a reset on the relevant port to start the enumeration process.
    //
    HubSetPortFeature((unsigned long)g_pRootHub,
                      ucPort,
                      HUB_FEATURE_PORT_RESET);
}

//*****************************************************************************
//
// A new device has been connected to the hub.  Allocate resources to manage
// it and pass details back to the main USB host enumeration code to have the
// device enumerated.
//
//*****************************************************************************
static void
HubDriverDeviceConnect(unsigned char ucPort, tBoolean bLowSpeed)
{
    DEBUG_OUTPUT("HubDriverDeviceConnect\n");

    //
    // We've allocated a port table entry so fill it in then initiate a reset
    // on the device.
    //
    g_pRootHub->psPorts[ucPort].bChanged = false;
    g_pRootHub->psPorts[ucPort].bLowSpeed = bLowSpeed;

    //
    // Mark the port as having a device present but not enumerated.
    //
    DEBUG_OUTPUT("Deferring enumeration for port %d\n", ucPort);
    g_pRootHub->psPorts[ucPort].sState = PORT_CONNECTED;

    //
    // Wait 100ms to reset the device.
    //
    g_pRootHub->psPorts[ucPort].ulCount = 100;
}

//*****************************************************************************
//
// An existing device has been removed from the hub.  Tidy up and let the main
// USB host code know so that it can free device resources.
//
//*****************************************************************************
static void
HubDriverDeviceDisconnect(unsigned char ucPort)
{
    //
    // This is a device we are currently managing.  Have we already informed
    // the host controller that it is present?
    //
    if((g_pRootHub->psPorts[ucPort].sState == PORT_ACTIVE) ||
       (g_pRootHub->psPorts[ucPort].sState == PORT_RESET_WAIT) ||
       (g_pRootHub->psPorts[ucPort].sState == PORT_ENUMERATED) ||
       (g_pRootHub->psPorts[ucPort].sState == PORT_ERROR))
    {
        //
        // Yes - tell the host controller that the device is not longer
        // connected.
        //
        USBHCDHubDeviceDisconnected(0, g_pRootHub->psPorts[ucPort].ulDevHandle);
    }

    //
    // If the device was being enumerated, make sure we clear the flag
    // indicating that an enumeration is still ongoing.
    //
    if((g_pRootHub->psPorts[ucPort].sState == PORT_RESET_ACTIVE) ||
       (g_pRootHub->psPorts[ucPort].sState == PORT_RESET_WAIT) ||
       (g_pRootHub->psPorts[ucPort].sState == PORT_ACTIVE))
    {
        g_pRootHub->bEnumerationBusy = false;
    }

    //
    // Free up the port state structure.
    //
    g_pRootHub->psPorts[ucPort].sState = PORT_IDLE;
}

//*****************************************************************************
//
// This function is called periodically by USBHCDMain().  We use it to handle
// the hub port state machine.
//
//*****************************************************************************
void
USBHHubMain(void)
{
    unsigned short usStatus, usChanged;
    unsigned char ucPort;
    tBoolean bRetcode;

    //
    // If the hub isn't present, just return.
    //
    if((g_pRootHub == 0) || (!g_pRootHub->bHubActive))
    {
        return;
    }

    //
    // Initialize the status variables.
    //
    usStatus = 0;
    usChanged = 0;

    //
    // The hub is active and something changed. Check to see which port changed
    // state and handle as necessary.
    //
    for(ucPort = 0; ucPort <= g_pRootHub->ucNumPortsInUse; ucPort++)
    {
        //
        // Decrement any wait counter if there is one present.
        //
        if(g_pRootHub->psPorts[ucPort].ulCount != 0)
        {
            g_pRootHub->psPorts[ucPort].ulCount--;
        }

        //
        // Is this port waiting to be enumerated and is the last device
        // enumeration finished?
        //
        if((g_pRootHub->psPorts[ucPort].sState == PORT_CONNECTED) &&
           (!g_pRootHub->bEnumerationBusy) &&
           (g_pRootHub->psPorts[ucPort].ulCount == 0))
        {
            //
            // Yes - start the enumeration processing for this device.
            //
            HubDriverDeviceReset(ucPort);
        }

        //
        // If the state is PORT_RESET_WAIT then the hub is waiting before
        // accessing device as the USB 2.0 specification requires.
        //
        if((g_pRootHub->psPorts[ucPort].sState == PORT_RESET_WAIT) &&
           (g_pRootHub->psPorts[ucPort].ulCount == 0))
        {
            //
            // Start the enumeration process if the timeout has passed and
            // the hub is waiting to start enumerating the device.
            //
            g_pRootHub->psPorts[ucPort].sState = PORT_ACTIVE;

            //
            // Call the main host controller layer to have it enumerate the newly
            // connected device.
            //
            g_pRootHub->psPorts[ucPort].ulDevHandle =
                        USBHCDHubDeviceConnected(0, 1, ucPort,
                                     g_pRootHub->psPorts[ucPort].bLowSpeed,
                                     g_pRootHub->psPorts[ucPort].pucConfigDesc,
                                     g_pRootHub->psPorts[ucPort].ulConfigSize);
        }

        //
        // If an enumeration is in progress and the loop is not on the port
        // being enumerated then skip the port.
        //
        if(g_pRootHub->bEnumerationBusy && (g_pRootHub->ucEnumIdx != ucPort))
        {
            continue;
        }

        //
        // Did something change for this particular port?
        //
        if(g_ulChangeFlags & (1 << ucPort))
        {
            //
            // Yes - query the port status.
            //
            bRetcode = HubGetPortStatus((unsigned long)g_pRootHub, ucPort,
                                        &usStatus, &usChanged);

            //
            // Clear this change with the USB interrupt temporarily disabled to
            // ensure that we do not clear a flag that the interrupt routine
            // has just set.
            //
            OS_INT_DISABLE(INT_USB0);
            g_ulChangeFlags &= ~(1 << ucPort);
            OS_INT_ENABLE(INT_USB0);

            //
            // If there was an error, go on and look at the next bit.
            //
            if(!bRetcode)
            {
                continue;
            }

            //
            // Now consider what changed and handle it as necessary.
            //

            //
            // Was a device connected to or disconnected from the port?
            //
            if(usChanged & HUB_PORT_CHANGE_DEVICE_PRESENT)
            {
                DEBUG_OUTPUT("Connection change on port %d\n", ucPort);

                //
                // Clear the condition.
                //
                HubClearPortFeature((unsigned long)g_pRootHub, ucPort,
                                    HUB_FEATURE_C_PORT_CONNECTION);

                //
                // Was a device connected or disconnected?
                //
                if(usStatus & HUB_PORT_STATUS_DEVICE_PRESENT)
                {
                    DEBUG_OUTPUT("Connected\n");

                    //
                    // A device was connected.
                    //
                    HubDriverDeviceConnect(ucPort,
                            ((usStatus & HUB_PORT_STATUS_LOW_SPEED) ?
                             true : false));
                }
                else
                {
                    DEBUG_OUTPUT("Disconnected\n");

                    //
                    // A device was disconnected.
                    //
                    HubDriverDeviceDisconnect(ucPort);
                }
            }

            //
            // Did a reset on the port complete?
            //
            if(usChanged & HUB_PORT_CHANGE_RESET)
            {
                //
                // Clear the condition.
                //
                HubClearPortFeature((unsigned long)g_pRootHub, ucPort,
                                    HUB_FEATURE_C_PORT_RESET);

                //
                // Yes - query the port status.
                //
                bRetcode = HubGetPortStatus((unsigned long)g_pRootHub, ucPort,
                                            &usStatus, &usChanged);

                DEBUG_OUTPUT("Reset %s for port %d\n",
                        ((usStatus & HUB_PORT_STATUS_RESET) ? "asserted" :
                        "deasserted"), ucPort);

                //
                // Handle the reset case.
                //
                HubDriverReset(ucPort, (usStatus & HUB_PORT_STATUS_RESET) ?
                               true : false);
            }

            //
            // Did an over-current reset on the port complete?
            //
            if(usChanged & HUB_PORT_CHANGE_OVER_CURRENT)
            {
                DEBUG_OUTPUT("Port %d over current.\n", ucPort);

                //
                // Currently we ignore this and just clear the condition.
                //
                HubClearPortFeature((unsigned long)g_pRootHub, ucPort,
                                    HUB_FEATURE_C_PORT_OVER_CURRENT);
            }

            //
            // Has the port been enabled or disabled?
            //
            if(usChanged & HUB_PORT_CHANGE_ENABLED)
            {
                DEBUG_OUTPUT("Enable change for port %d.\n", ucPort);

                //
                // Currently we ignore this and just clear the condition.
                //
                HubClearPortFeature((unsigned long)g_pRootHub, ucPort,
                                    HUB_FEATURE_C_PORT_ENABLE);
            }

            //
            // Has the port been suspended or resumed?
            //
            if(usChanged & HUB_PORT_CHANGE_SUSPENDED)
            {
                DEBUG_OUTPUT("Suspend change for port %d.\n", ucPort);

                //
                // Currently we ignore this and just clear the condition.
                //
                HubClearPortFeature((unsigned long)g_pRootHub, ucPort,
                                    HUB_FEATURE_C_PORT_SUSPEND);
            }
        }
    }
}

//*****************************************************************************
//
//! Informs the hub class driver that a downstream device has been enumerated.
//!
//! \param ucHub is the address of the hub to which the downstream device
//! is attached.
//! \param ucPort is the port on the hub to which the downstream device is
//! attached.
//!
//! This function is called by the host controller driver to inform the hub
//! class driver that a downstream device has been enumerated successfully.
//! The hub driver then moves on and continues enumeration of any other newly
//! connected devices.
//!
//! \return None.
//
//*****************************************************************************
void
USBHHubEnumerationComplete(unsigned char ucHub, unsigned char ucPort)
{
    DEBUG_OUTPUT("Enumeration complete for hub %d, port %d\n", ucHub, ucPort);

    //
    // Record the fact that the device is up and running.
    //
    g_pRootHub->psPorts[ucPort].sState = PORT_ENUMERATED;

    //
    // Clear the flag we use to defer further enumerations.  This will cause
    // the next connected device (if any) to start enumeration on the next
    // call to USBHHubMain().
    //
    g_pRootHub->bEnumerationBusy = false;
}

//*****************************************************************************
//
//! Informs the hub class driver that a downstream device failed to enumerate.
//!
//! \param ucHub is the address of the hub to which the downstream device
//! is attached.
//! \param ucPort is the port on the hub to which the downstream device is
//! attached.
//!
//! This function is called by the host controller driver to inform the hub
//! class driver that an attempt to enumerate a downstream device has failed.
//! The hub driver then cleans up and continues enumeration of any other newly
//! connected devices.
//!
//! \return None.
//
//*****************************************************************************
void
USBHHubEnumerationError(unsigned char ucHub, unsigned char ucPort)
{
    DEBUG_OUTPUT("Enumeration error for hub %d, port %d\n", ucHub, ucPort);

    //
    // Record the fact that the device is not working correctly.
    //
    g_pRootHub->psPorts[ucPort].sState = PORT_ERROR;

    //
    // Clear the flag we use to defer further enumerations.  This will cause
    // the next connected device (if any) to start enumeration on the next
    // call to USBHHubMain().
    //
    g_pRootHub->bEnumerationBusy = false;
}

//*****************************************************************************
//
//! This function is used to enable the host hub class driver before any
//! devices are present.
//!
//! \param pfnCallback is the driver call back for host hub events.
//! \param pucHubPool is the memory pool allocated to the USB hub class.
//! \param ulPoolSize is the size in bytes of the memory pool provided by the
//! \e pucHubPool parameter.
//! \param psHubInstance is a pointer to an instance of the private hub data.
//! \param ulNumHubs is the number of hubs to support.
//!
//! This function is called to open an instance of a host hub device and
//! provides a valid callback function for host hub events in the
//! \e pfnCallback parameter.  This function must be called before the USB
//! host code can successfully enumerate a hub device or any devices attached
//! to the hub.  The \e pucHubPool is memory provided to the hub class to
//! manage the devices that are connected to the hub.  The \e ulPoolSize is
//! the number of bytes and should be at least 32 bytes per device including
//! the hub device itself.  A simple formula for providing memory to the hub
//! class is \b MAX_USB_DEVICES * 32 bytes of data to allow for proper
//! enumeration of connected devices.  The value for \b MAX_USB_DEVICES is
//! defined in the usblib.h file and controls the number of devices
//! supported by the USB library.  The \e ulNumHubs parameter
//! defaults to one and only one buffer of size tHubInstance is required to
//! be passed in the \e psHubInstance parameter.
//!
//! \note Changing the value of \b MAX_USB_DEVICES requires a rebuild of the
//! USB library to have an effect on the library.
//!
//! \return This function returns the driver instance to use for the other
//! host hub functions.  If there is no instance available at the time of
//! this call, this function returns zero.
//
//*****************************************************************************
unsigned long
USBHHubOpen(tUSBCallback pfnCallback, unsigned char *pucHubPool,
            unsigned long ulPoolSize, tHubInstance *psHubInstance,
            unsigned long ulNumHubs)
{
    unsigned long ulLoop, ulBlockSize;

    //
    // Only one hub is supported.
    //
    if(g_pRootHub)
    {
        DEBUG_OUTPUT("USBHHubOpen failed - already connected.\n");
        return(0);
    }

    //
    // Save this instance.
    //
    g_pRootHub = psHubInstance;

    //
    // Save the instance data for this device.
    //
    g_pRootHub->pfnCallback = pfnCallback;

    //
    // Divide the pool up into blocks, one for each supported port.  We make
    // sure that each block is a multiple of 4 bytes.
    //
    ulBlockSize = (ulPoolSize / MAX_USB_DEVICES) & ~3;
    for(ulLoop = 0; ulLoop < MAX_USB_DEVICES; ulLoop++)
    {
        g_pRootHub->psPorts[ulLoop].pucConfigDesc = (pucHubPool +
                                                  (ulLoop * ulBlockSize));
        g_pRootHub->psPorts[ulLoop].ulConfigSize = ulBlockSize;
    }

    DEBUG_OUTPUT("USBHHubOpen completed.\n");

    //
    // Return the device instance pointer.
    //
    return((unsigned long)g_pRootHub);
}

//*****************************************************************************
//
//! This function is used to release a hub device instance.
//!
//! \param ulInstance is the hub device instance that is to be released.
//!
//! This function is called when an instance of the hub device must be
//! released.  This function is typically made in preparation for shutdown or a switch
//! to function as a USB device when in OTG mode.  Following this call, the hub device is
//! no longer available, but it can be opened again using a call to
//! USBHHubOpen().  After calling USBHHubClose(), the host hub driver no
//! longer provides any callbacks or accepts calls to other hub driver APIs.
//!
//! \return None.
//
//*****************************************************************************
void
USBHHubClose(unsigned long ulInstance)
{
    //
    // Forget the instance pointer.
    //
    g_pRootHub = 0;

    DEBUG_OUTPUT("USBHHubClose completed.\n");
}

//*****************************************************************************
//
// This function is used to initialize the Hub driver.  This is an internal
// function that should not be called by the application.
//
//*****************************************************************************
void
USBHHubInit(void)
{
    //
    // Initialize Hub state.
    //
    g_pRootHub = 0;
    g_ulChangeFlags = 0;
    g_ulHubChanges = 0;
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
