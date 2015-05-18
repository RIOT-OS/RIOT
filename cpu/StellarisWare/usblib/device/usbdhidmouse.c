//*****************************************************************************
//
// usbdhidmouse.c - USB HID Mouse device class driver.
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
#include "driverlib/debug.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidmouse.h"

//*****************************************************************************
//
//! \addtogroup hid_mouse_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The report descriptor for the mouse class device.
//
//*****************************************************************************
static const unsigned char g_pucMouseReportDescriptor[]=
{
    UsagePage(USB_HID_GENERIC_DESKTOP),
    Usage(USB_HID_MOUSE),
    Collection(USB_HID_APPLICATION),
        Usage(USB_HID_POINTER),
        Collection(USB_HID_PHYSICAL),

            //
            // The buttons.
            //
            UsagePage(USB_HID_BUTTONS),
            UsageMinimum(1),
            UsageMaximum(3),
            LogicalMinimum(0),
            LogicalMaximum(1),

            //
            // 3 - 1 bit values for the buttons.
            //
            ReportSize(1),
            ReportCount(3),
            Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE |
                  USB_HID_INPUT_ABS),

            //
            // 1 - 5 bit unused constant value to fill the 8 bits.
            //
            ReportSize(5),
            ReportCount(1),
            Input(USB_HID_INPUT_CONSTANT | USB_HID_INPUT_ARRAY |
                  USB_HID_INPUT_ABS),

            //
            // The X and Y axis.
            //
            UsagePage(USB_HID_GENERIC_DESKTOP),
            Usage(USB_HID_X),
            Usage(USB_HID_Y),
            LogicalMinimum(-127),
            LogicalMaximum(127),

            //
            // 2 - 8 bit Values for x and y.
            //
            ReportSize(8),
            ReportCount(2),
            Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE |
                  USB_HID_INPUT_RELATIVE),

            //
            // 2 - 8 bit Values for x and y.  // Padding
            //
            ReportSize(8),
            ReportCount(MOUSE_REPORT_SIZE - 3),
            Input(USB_HID_INPUT_CONSTANT | USB_HID_INPUT_ARRAY |
                  USB_HID_INPUT_ABS),

        EndCollection,
    EndCollection,
};

//*****************************************************************************
//
// The HID class descriptor table.  For the mouse class, we have only a single
// report descriptor.
//
//*****************************************************************************
static const unsigned char * const g_pMouseClassDescriptors[] =
{
    g_pucMouseReportDescriptor
};

//*****************************************************************************
//
// The HID descriptor for the mouse device.
//
//*****************************************************************************
static const tHIDDescriptor g_sMouseHIDDescriptor =
{
    9,                                 // bLength
    USB_HID_DTYPE_HID,                 // bDescriptorType
    0x111,                             // bcdHID (version 1.11 compliant)
    0,                                 // bCountryCode (not localized)
    1,                                 // bNumDescriptors
    {
        {
            USB_HID_DTYPE_REPORT,                  // Report descriptor
            sizeof(g_pucMouseReportDescriptor)     // Size of report descriptor
        }
    }
};

//*****************************************************************************
//
// Forward references for mouse device callback functions.
//
//*****************************************************************************
static unsigned long HIDMouseRxHandler(void *pvCBData,
                                          unsigned long ulEvent,
                                          unsigned long ulMsgData,
                                          void *pvMsgData);
static unsigned long HIDMouseTxHandler(void *pvCBData,
                                          unsigned long ulEvent,
                                          unsigned long ulMsgData,
                                          void *pvMsgData);

//*****************************************************************************
//
// The HID mouse report offsets for this mouse application.
//
//*****************************************************************************
#define HID_REPORT_BUTTONS      0
#define HID_REPORT_X            1
#define HID_REPORT_Y            2

//*****************************************************************************
//
// Main HID device class event handler function.
//
// \param pvCBData is the event callback pointer provided during USBDHIDInit().
// This is a pointer to our HID device structure (&g_sHIDMouseDevice).
// \param ulEvent identifies the event we are being called back for.
// \param ulMsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to operation of the
// mouse HID device.
//
// \return Returns a value which is event-specific.
//
//*****************************************************************************
static unsigned long
HIDMouseRxHandler(void *pvCBData, unsigned long ulEvent,
                  unsigned long ulMsgData, void *pvMsgData)
{
    tHIDMouseInstance *psInst;
    tUSBDHIDMouseDevice *psDevice;

    //
    // Make sure we didn't get a NULL pointer.
    //
    ASSERT(pvCBData);

    //
    // Get a pointer to our instance data
    //
    psDevice = (tUSBDHIDMouseDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDMouseData;

    //
    // Which event were we sent?
    //
    switch (ulEvent)
    {
        //
        // The host has connected to us and configured the device.
        //
        case USB_EVENT_CONNECTED:
        {
            psInst->ucUSBConfigured = true;

            //
            // Pass the information on to the client.
            //
            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_CONNECTED,
                                  0, (void *)0);

            break;
        }

        //
        // The host has disconnected from us.
        //
        case USB_EVENT_DISCONNECTED:
        {
            psInst->ucUSBConfigured = false;

            //
            // Pass the information on to the client.
            //
            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_DISCONNECTED,
                                  0, (void *)0);

            break;
        }

        //
        // The host is polling us for a particular report and the HID driver
        // is asking for the latest version to transmit.
        //
        case USBD_HID_EVENT_IDLE_TIMEOUT:
        case USBD_HID_EVENT_GET_REPORT:
        {
            //
            // We only support a single input report so we don't need to check
            // the ulMsgValue parameter in this case.  Set the report pointer
            // in *pvMsgData and return the length of the report in bytes.
            //
            *(unsigned char **)pvMsgData = psInst->pucReport;
            return(8);
        }

        //
        // The device class driver has completed sending a report to the
        // host in response to a Get_Report request.
        //
        case USBD_HID_EVENT_REPORT_SENT:
        {
            //
            // We have nothing to do here.
            //
            break;
        }

        //
        // This event is sent in response to a host Set_Report request.  The
        // mouse device has no output reports so we return a NULL pointer and
        // zero length to cause this request to be stalled.
        //
        case USBD_HID_EVENT_GET_REPORT_BUFFER:
        {
            //
            // We are being asked for a report that does not exist for
            // this device.  Return 0 to indicate that we are not providing
            // a buffer.
            //
            return(0);
        }

        //
        // The host is asking us to set either boot or report protocol (not
        // that it makes any difference to this particular mouse).
        //
        case USBD_HID_EVENT_SET_PROTOCOL:
        {
            psInst->ucProtocol = ulMsgData;
            break;
        }

        //
        // The host is asking us to tell it which protocol we are currently
        // using, boot or request.
        //
        case USBD_HID_EVENT_GET_PROTOCOL:
        {
            return(psInst->ucProtocol);
        }

        //
        // Pass ERROR, SUSPEND and RESUME to the client unchanged.
        //
        case USB_EVENT_ERROR:
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
        {
            return(psDevice->pfnCallback(psDevice->pvCBData, ulEvent,
                                         ulMsgData, pvMsgData));
        }

        //
        // We ignore all other events.
        //
        default:
        {
            break;
        }
    }
    return(0);
}

//*****************************************************************************
//
// HID device class transmit channel event handler function.
//
// \param pvCBData is the event callback pointer provided during USBDHIDInit().
// This is a pointer to our HID device structure (&g_sHIDMouseDevice).
// \param ulEvent identifies the event we are being called back for.
// \param ulMsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to report
// transmissions made using the interrupt IN endpoint.
//
// \return Returns a value which is event-specific.
//
//*****************************************************************************
static unsigned long
HIDMouseTxHandler(void *pvCBData, unsigned long ulEvent,
                  unsigned long ulMsgData, void *pvMsgData)
{
    tHIDMouseInstance *psInst;
    tUSBDHIDMouseDevice *psDevice;

    //
    // Make sure we didn't get a NULL pointer.
    //
    ASSERT(pvCBData);

    //
    // Get a pointer to our instance data
    //
    psDevice = (tUSBDHIDMouseDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDMouseData;

    //
    // Which event were we sent?
    //
    switch (ulEvent)
    {
        //
        // A report transmitted via the interrupt IN endpoint was acknowledged
        // by the host.
        //
        case USB_EVENT_TX_COMPLETE:
        {
            //
            // Our last transmission is complete.
            //
            psInst->eMouseState = HID_MOUSE_STATE_IDLE;

            //
            // Pass the event on to the client.
            //
            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_TX_COMPLETE,
                                  ulMsgData, (void *)0);

            break;
        }

        //
        // We ignore all other events related to transmission of reports via
        // the interrupt IN endpoint.
        //
        default:
        {
            break;
        }
    }

    return(0);
}

//*****************************************************************************
//
//! Initializes HID mouse device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for HID mouse device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the HID mouse device.
//!
//! An application wishing to offer a USB HID mouse interface to a USB host
//! must call this function to initialize the USB controller and attach the
//! mouse device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psDevice pointer
//! passed to it.  This must be passed on all future calls to the HID mouse
//! device driver.
//!
//! When a host connects and configures the device, the application callback
//! will receive \b USB_EVENT_CONNECTED after which calls can be made to
//! USBDHIDMouseStateChange() to report pointer movement and button presses
//! to the host.
//!
//! \note The application must not make any calls to the lower level USB device
//! interfaces if interacting with USB via the USB HID mouse device API.
//! Doing so will cause unpredictable (though almost certainly unpleasant)
//! behavior.
//!
//! \return Returns NULL on failure or the psDevice pointer on success.
//
//*****************************************************************************
void *
USBDHIDMouseInit(unsigned long ulIndex, const tUSBDHIDMouseDevice *psDevice)
{
    void *pvRetcode;
    tUSBDHIDDevice *psHIDDevice;

    //
    // Check parameter validity.
    //
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDMouseData);
    ASSERT(psDevice->pfnCallback);

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDMouseData->sHIDDevice;

    //
    // Call the common initialization routine.
    //
    pvRetcode = USBDHIDMouseCompositeInit(ulIndex, psDevice);

    //
    // If we initialized the HID layer successfully, pass our device pointer
    // back as the return code, otherwise return NULL to indicate an error.
    //
    if(pvRetcode)
    {
        //
        // Initialize the lower layer HID driver and pass it the various
        // structures and descriptors necessary to declare that we are a
        // keyboard.
        //
        pvRetcode = USBDHIDInit(ulIndex, psHIDDevice);

        return((void *)psDevice);
    }
    else
    {
        return((void *)0);
    }
}

//*****************************************************************************
//
//! Initializes HID mouse device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for HID mouse device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the HID mouse device.
//!
//! This call is very similar to USBDHIDMouseInit() except that it is used for
//! initializing an instance of the HID mouse device for use in a composite
//! device.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB HID Mouse APIs.
//
//*****************************************************************************
void *
USBDHIDMouseCompositeInit(unsigned long ulIndex,
                          const tUSBDHIDMouseDevice *psDevice)
{
    tHIDMouseInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;


    //
    // Check parameter validity.
    //
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDMouseData);
    ASSERT(psDevice->pfnCallback);

    //
    // Get a pointer to our instance data
    //
    psInst = psDevice->psPrivateHIDMouseData;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDMouseData->sHIDDevice;

    //
    // Initialize the various fields in our instance structure.
    //
    psInst->ucUSBConfigured = 0;
    psInst->ucProtocol = USB_HID_PROTOCOL_REPORT;
    psInst->sReportIdle.ucDuration4mS = 0;
    psInst->sReportIdle.ucReportID = 0;
    psInst->sReportIdle.ulTimeSinceReportmS = 0;
    psInst->sReportIdle.usTimeTillNextmS = 0;
    psInst->eMouseState = HID_MOUSE_STATE_UNCONFIGURED;

    //
    // Initialize the HID device class instance structure based on input from
    // the caller.
    //
    psHIDDevice->usPID = psDevice->usPID;
    psHIDDevice->usVID = psDevice->usVID;
    psHIDDevice->usMaxPowermA = psDevice->usMaxPowermA;
    psHIDDevice->ucPwrAttributes = psDevice->ucPwrAttributes;
    psHIDDevice->ucSubclass = USB_HID_SCLASS_BOOT;
    psHIDDevice->ucProtocol = USB_HID_PROTOCOL_MOUSE;
    psHIDDevice->ucNumInputReports = 1;
    psHIDDevice->psReportIdle = &psInst->sReportIdle;
    psHIDDevice->pfnRxCallback = HIDMouseRxHandler;
    psHIDDevice->pvRxCBData = (void *)psDevice;
    psHIDDevice->pfnTxCallback = HIDMouseTxHandler;
    psHIDDevice->pvTxCBData = (void *)psDevice;
    psHIDDevice->bUseOutEndpoint = false;
    psHIDDevice->psHIDDescriptor = &g_sMouseHIDDescriptor;
    psHIDDevice->ppClassDescriptors= g_pMouseClassDescriptors;
    psHIDDevice->ppStringDescriptors = psDevice->ppStringDescriptors;
    psHIDDevice->ulNumStringDescriptors = psDevice->ulNumStringDescriptors;
    psHIDDevice->psPrivateHIDData = &psInst->sHIDInstance;

    //
    // Initialize the lower layer HID driver and pass it the various structures
    // and descriptors necessary to declare that we are a keyboard.
    //
    return(USBDHIDCompositeInit(ulIndex, psHIDDevice));
}

//*****************************************************************************
//
//! Shuts down the HID mouse device.
//!
//! \param pvInstance is the pointer to the device instance structure.
//!
//! This function terminates HID mouse operation for the instance supplied
//! and removes the device from the USB bus.  Following this call, the \e
//! pvInstance instance may not me used in any other call to the HID mouse
//! device other than USBDHIDMouseInit().
//!
//! \return None.
//
//*****************************************************************************
void
USBDHIDMouseTerm(void *pvInstance)
{
    tUSBDHIDMouseDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    //
    // Get a pointer to the device.
    //
    psDevice = (tUSBDHIDMouseDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDMouseData->sHIDDevice;

    //
    // Mark our device as no longer configured.
    //
    psDevice->psPrivateHIDMouseData->ucUSBConfigured = 0;

    //
    // Terminate the low level HID driver.
    //
    USBDHIDTerm(psHIDDevice);
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the mouse callback.
//!
//! \param pvInstance is the pointer to the mouse device instance structure.
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the mouse callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnCallback function
//! passed on USBDHIDMouseInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the pvInstance structure passed to USBDHIDMouseInit() resides
//! in RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was set for this
//! instance.
//
//*****************************************************************************
void *
USBDHIDMouseSetCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldCBData;
    tUSBDHIDMouseDevice *psMouse;

    //
    // Check for a NULL pointer in the device parameter.
    //
    ASSERT(pvInstance);

    //
    // Get a pointer to our mouse device.
    //
    psMouse = (tUSBDHIDMouseDevice *)pvInstance;

    //
    // Save the old callback pointer and replace it with the new value.
    //
    pvOldCBData = psMouse->pvCBData;
    psMouse->pvCBData = pvCBData;

    //
    // Pass the old callback pointer back to the caller.
    //
    return(pvOldCBData);
}

//*****************************************************************************
//
//! Reports a mouse state change, pointer movement or button press, to the USB
//! host.
//!
//! \param pvInstance is the pointer to the mouse device instance structure.
//! \param cDeltaX is the relative horizontal pointer movement that the
//! application wishes to report.  Valid values are in the range [-127, 127]
//! with positive values indicating movement to the right.
//! \param cDeltaY is the relative vertical pointer movement that the
//! application wishes to report.  Valid values are in the range [-127, 127]
//! with positive values indicating downward movement.
//! \param ucButtons is a bit mask indicating which (if any) of the three
//! mouse buttons is pressed.  Valid values are logical OR combinations of
//! \e MOUSE_REPORT_BUTTON_1, \e MOUSE_REPORT_BUTTON_2 and \e
//! MOUSE_REPORT_BUTTON_3.
//!
//! This function is called to report changes in the mouse state to the USB
//! host.  These changes can be movement of the pointer, reported relative to
//! its previous position, or changes in the states of up to 3 buttons that
//! the mouse may support.  The return code indicates whether or not the
//! mouse report could be sent to the host.  In cases where a previous
//! report is still being transmitted, \b MOUSE_ERR_TX_ERROR will be returned
//! and the state change will be ignored.
//!
//! \return Returns \b MOUSE_SUCCESS on success, \b MOUSE_ERR_TX_ERROR if an
//! error occurred while attempting to schedule transmission of the mouse
//! report to the host (typically due to a previous report which has not yet
//! completed transmission or due to disconnection of the host) or \b
//! MOUSE_ERR_NOT_CONFIGURED if called before a host has connected to and
//! configured the device.
//
//*****************************************************************************
unsigned long
USBDHIDMouseStateChange(void *pvInstance, char cDeltaX, char cDeltaY,
                        unsigned char ucButtons)
{
    unsigned long ulRetcode;
    unsigned long ulCount;
    tHIDMouseInstance *psInst;
    tUSBDHIDMouseDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    //
    // Get a pointer to the device.
    //
    psDevice = (tUSBDHIDMouseDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDMouseData->sHIDDevice;

    //
    // Get a pointer to our instance data
    //
    psInst = psDevice->psPrivateHIDMouseData;

    //
    // Update the global mouse report with the information passed.
    //
    psInst->pucReport[HID_REPORT_BUTTONS] = ucButtons;
    psInst->pucReport[HID_REPORT_X] = (unsigned char)cDeltaX;
    psInst->pucReport[HID_REPORT_Y] = (unsigned char)cDeltaY;

    //
    // If we are not configured, return an error here before trying to send
    // anything.
    //
    if(!psInst->ucUSBConfigured)
    {
        return(MOUSE_ERR_NOT_CONFIGURED);
    }

    //
    // Only send a report if the transmitter is currently free.
    //
    if(USBDHIDTxPacketAvailable((void *)psHIDDevice))
    {
        //
        // Send the report to the host.
        //
        psInst->eMouseState = HID_MOUSE_STATE_SEND;
        ulCount = USBDHIDReportWrite((void *)psHIDDevice,
                                     psInst->pucReport, MOUSE_REPORT_SIZE,
                                     true);

        //
        // Did we schedule a packet for transmission correctly?
        //
        if(!ulCount)
        {
            //
            // No - report the error to the caller.
            //
            ulRetcode = MOUSE_ERR_TX_ERROR;
        }
        else
        {
            ulRetcode = MOUSE_SUCCESS;
        }
    }
    else
    {
        ulRetcode = MOUSE_ERR_TX_ERROR;
    }
    //
    // Return the relevant error code to the caller.
    //
    return(ulRetcode);
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the USB library.
//!
//! \param pvInstance is the pointer to the mouse device instance structure.
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
USBDHIDMousePowerStatusSet(void *pvInstance, unsigned char ucPower)
{
    tUSBDHIDMouseDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    //
    // Get the keyboard device pointer.
    //
    psDevice = (tUSBDHIDMouseDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDMouseData->sHIDDevice;

    //
    // Pass the request through to the lower layer.
    //
    USBDHIDPowerStatusSet((void *)psHIDDevice, ucPower);
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param pvInstance is the pointer to the mouse device instance structure.
//!
//! When the bus is suspended, an application which supports remote wake up
//! (advertised to the host via the configuration descriptor) may call this
//! function to initiate remote wake up signaling to the host.  If the remote
//! wake up feature has not been disabled by the host, this will cause the bus
//! to resume operation within 20mS.  If the host has disabled remote wake up,
//! \b false will be returned to indicate that the wake up request was not
//! successful.
//!
//! \return Returns \b true if the remote wake up is not disabled and the
//! signaling was started or \b false if remote wake up is disabled or if
//! signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
tBoolean
USBDHIDMouseRemoteWakeupRequest(void *pvInstance)
{
    tUSBDHIDMouseDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    //
    // Get the keyboard device pointer.
    //
    psDevice = (tUSBDHIDMouseDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDMouseData->sHIDDevice;

    //
    // Pass the request through to the lower layer.
    //
    return(USBDHIDRemoteWakeupRequest((void *)&psHIDDevice));
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
