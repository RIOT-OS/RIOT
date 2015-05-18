//*****************************************************************************
//
// usbdhidkeyb.c - USB HID Keyboard device class driver.
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
#include "usblib/device/usbdhidkeyb.h"

//*****************************************************************************
//
//! \addtogroup hid_keyboard_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The following is the HID report structure definition that is passed back
// to the host.
//
//*****************************************************************************
static const unsigned char g_pucKeybReportDescriptor[]=
{
    UsagePage(USB_HID_GENERIC_DESKTOP),
    Usage(USB_HID_KEYBOARD),
    Collection(USB_HID_APPLICATION),

        //
        // Modifier keys.
        // 8 - 1 bit values indicating the modifier keys (ctrl, shift...)
        //
        ReportSize(1),
        ReportCount(8),
        UsagePage(USB_HID_USAGE_KEYCODES),
        UsageMinimum(224),
        UsageMaximum(231),
        LogicalMinimum(0),
        LogicalMaximum(1),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

        //
        // One byte of rsvd data required by HID spec.
        //
        ReportCount(1),
        ReportSize(8),
        Input(USB_HID_INPUT_CONSTANT),

        //
        // Keyboard LEDs.
        // 5 - 1 bit values.
        //
        ReportCount(5),
        ReportSize(1),
        UsagePage(USB_HID_USAGE_LEDS),
        UsageMinimum(1),
        UsageMaximum(5),
        Output(USB_HID_OUTPUT_DATA | USB_HID_OUTPUT_VARIABLE |
               USB_HID_OUTPUT_ABS),
        //
        // 1 - 3 bit value to pad out to a full byte.
        //
        ReportCount(1),
        ReportSize(3),
        Output(USB_HID_OUTPUT_CONSTANT), //LED report padding

        //
        // The Key buffer.
        // 6 - 8 bit values to store the current key state.
        //
        ReportCount(6),
        ReportSize(8),
        LogicalMinimum(0),
        LogicalMaximum(101),
        UsagePage(USB_HID_USAGE_KEYCODES),
        UsageMinimum (0),
        UsageMaximum (101),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_ARRAY),
    EndCollection
};

//*****************************************************************************
//
// The HID class descriptor table.  For the keyboard class, we have only a
// single report descriptor.
//
//*****************************************************************************
static const unsigned char * const g_pKeybClassDescriptors[] =
{
    g_pucKeybReportDescriptor
};

//*****************************************************************************
//
// The HID descriptor for the keyboard device.
//
//*****************************************************************************
static const tHIDDescriptor g_sKeybHIDDescriptor =
{
    9,                                     // bLength
    USB_HID_DTYPE_HID,                     // bDescriptorType
    0x111,                                 // bcdHID (version 1.11 compliant)
    0,                                     // bCountryCode (not localized)
    1,                                     // bNumDescriptors
    {
        {
            USB_HID_DTYPE_REPORT,                  // Report descriptor
            sizeof(g_pucKeybReportDescriptor)      // Size of report descriptor
        }
    }
};

//*****************************************************************************
//
// Forward references for keyboard device callback functions.
//
//*****************************************************************************
static unsigned long HIDKeyboardRxHandler(void *pvCBData,
                                          unsigned long ulEvent,
                                          unsigned long ulMsgData,
                                          void *pvMsgData);
static unsigned long HIDKeyboardTxHandler(void *pvCBData,
                                          unsigned long ulEvent,
                                          unsigned long ulMsgData,
                                          void *pvMsgData);

//*****************************************************************************
//
// Main HID device class event handler function.
//
// \param pvCBData is the event callback pointer provided during USBDHIDInit().
// This is a pointer to our HID device structure (&g_sHIDKeybDevice).
// \param ulEvent identifies the event we are being called back for.
// \param ulMsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to operation of the
// keyboard HID device.
//
// \return Returns a value which is event-specific.
//
//*****************************************************************************
static unsigned long
HIDKeyboardRxHandler(void *pvCBData, unsigned long ulEvent,
                     unsigned long ulMsgData, void *pvMsgData)
{
    tHIDKeyboardInstance *psInst;
    tUSBDHIDKeyboardDevice *psDevice;

    //
    // Make sure we didn't get a NULL pointer.
    //
    ASSERT(pvCBData);

    //
    // Get a pointer to our instance data
    //
    psDevice = (tUSBDHIDKeyboardDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDKbdData;

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
            return(KEYB_IN_REPORT_SIZE);
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
        // This event is sent in response to a host Set_Report request.  We
        // must return a pointer to a buffer large enough to receive the
        // report into.
        //
        case USBD_HID_EVENT_GET_REPORT_BUFFER:
        {
            //
            // Are we being asked for a report that is shorter than the storage
            // we have set aside for this?  The only output report we define is
            // 8 bits long so we really expect to see a length of 1 passed.
            //
            if((unsigned long)pvMsgData == KEYB_OUT_REPORT_SIZE )
            {
                //
                // Yes - return our pointer.
                //
                return((unsigned long)psInst->pucDataBuffer);
            }
            else
            {
                //
                // We are being passed a report that is longer than the
                // only report we expect so return NULL.  This causes the
                // device class driver to stall the request.
                //
                return(0);
            }
        }

        //
        // This event indicates that the host has sent us an Output or
        // Feature report and that the report is now in the buffer we provided
        // on the previous USBD_HID_EVENT_GET_REPORT_BUFFER callback.
        //
        case USBD_HID_EVENT_SET_REPORT:
        {
            //
            // Inform the application if the keyboard LEDs have changed.
            //
            if(psInst->ucLEDStates != psInst->pucDataBuffer[0])
            {
                //
                // Note the new LED states.
                //
                psInst->ucLEDStates = psInst->pucDataBuffer[0];

                //
                // Pass the information on to the client.
                //
                psDevice->pfnCallback(psDevice->pvCBData,
                                      USBD_HID_KEYB_EVENT_SET_LEDS,
                                      psInst->pucDataBuffer[0], (void *)0);
            }
            break;
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
// This is a pointer to our HID device structure (&g_sHIDKeybDevice).
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
HIDKeyboardTxHandler(void *pvCBData, unsigned long ulEvent,
                     unsigned long ulMsgData, void *pvMsgData)
{
    tHIDKeyboardInstance *psInst;
    tUSBDHIDKeyboardDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;
    unsigned long ulCount;

    //
    // Make sure we didn't get a NULL pointer.
    //
    ASSERT(pvCBData);

    //
    // Get a pointer to our instance data
    //
    psDevice = (tUSBDHIDKeyboardDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDKbdData;
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

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
            // Do we have any pending changes needing transmitted?
            //
            if(psInst->bChangeMade)
            {
                //
                // Yes - go ahead and send another report immediately.
                //
                ulCount = USBDHIDReportWrite((void *)psHIDDevice,
                                             psInst->pucReport,
                                             KEYB_IN_REPORT_SIZE, true);

                //
                // If we scheduled the report for transmission, clear the
                // change flag.
                //
                if(ulCount != 0)
                {
                    psInst->bChangeMade = false;
                }
            }
            else
            {
                //
                // Our last transmission is complete and we have nothing more
                // to send.
                //
                psInst->eKeyboardState = HID_KEYBOARD_STATE_IDLE;
            }

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
// Add the supplied usage code to the list of keys currently in the pressed
// state.
//
// \param ucUsageCode is the HID usage code of the newly pressed key.
//
// This function adds the supplied usage code to the global list of keys which
// are currently pressed (assuming it is not already noted as pressed and that
// there is space in the list to hold the new information).  The return code
// indicates success if the list did not overflow and failure if the list
// already contains as many pressed keys as can be reported.
//
// \return Returns \b true if the usage code was successfully added to the
// list or \b false if there was insufficient space to hold the new key
// press (in which case the caller should report a roll over error to the host).
//
//*****************************************************************************
static tBoolean
AddKeyToPressedList(tHIDKeyboardInstance *psInst, unsigned char ucUsageCode)
{
    unsigned long ulLoop;
    tBoolean bRetcode;

    //
    // Assume all is well until we determine otherwise.
    //
    bRetcode = true;

    //
    // Look through the list of existing pressed keys to see if the new one
    // is already there.
    //
    for(ulLoop = 0; ulLoop < (unsigned long)psInst->ucKeyCount; ulLoop++)
    {
        //
        // Is this key already included in the list of keys in the pressed
        // state?
        //
        if(ucUsageCode == psInst->pucKeysPressed[ulLoop])
        {
            //
            // Yes - drop out.
            //
            break;
        }
    }

    //
    // If we exited the loop at the end of the existing key presses, this
    // key does not exist already so add it if space exists.
    //
    if(ulLoop >= psInst->ucKeyCount)
    {
        if(psInst->ucKeyCount < KEYB_MAX_CHARS_PER_REPORT)
        {
            //
            // We have room so store the new key press in the list.
            //
            psInst->pucKeysPressed[psInst->ucKeyCount] = ucUsageCode;
            psInst->ucKeyCount++;
            bRetcode = true;
        }
        else
        {
            //
            // We have no room for the new key - declare a rollover error.
            //
            bRetcode = false;
        }
    }

    return(bRetcode);
}

//*****************************************************************************
//
// Remove the supplied usage code from the list of keys currently in the
// pressed state.
//
// \param ucUsageCode is the HID usage code of the newly released key.
//
// This function removes the supplied usage code from the global list of keys
// which are currently pressed.  The return code indicates whether the key was
// found in the list.  On exit, the list will have been cleaned up to ensure
// that all key presses are contiguous starting at the first entry.
//
// \return Returns \b true if the usage code was found and removed from the
// list or \b false if the code was not found.  The caller need not pass a new
// report to the host if \b false is returned since the key list will not have
// changed.
//
//*****************************************************************************
static tBoolean
RemoveKeyFromPressedList(tHIDKeyboardInstance *psInst,
                         unsigned char ucUsageCode)
{
    unsigned long ulLoop;
    unsigned long ulPos;

    //
    // Keep the compiler happy by setting ulPos to something.
    //
    ulPos = 0;

    //
    // Find the usage code in the current list.
    //
    for(ulLoop = 0; ulLoop < KEYB_MAX_CHARS_PER_REPORT; ulLoop++)
    {
        if(psInst->pucKeysPressed[ulLoop] == ucUsageCode)
        {
            ulPos = ulLoop;
            break;
        }
    }

    //
    // If we dropped out at the end of the loop, we couldn't find the code so
    // just return false.
    //
    if(ulLoop == KEYB_MAX_CHARS_PER_REPORT)
    {
        return(false);
    }

    //
    // Now shuffle all the values to the right of the usage code we found
    // down one position to fill the gap left by removing it.
    //
    for(ulLoop = (ulPos + 1); ulLoop < KEYB_MAX_CHARS_PER_REPORT; ulLoop++)
    {
        psInst->pucKeysPressed[ulLoop - 1] = psInst->pucKeysPressed[ulLoop];
    }

    //
    // Clear the last entry in the array and adjust the number of keys in the
    // array.
    //
    psInst->pucKeysPressed[KEYB_MAX_CHARS_PER_REPORT - 1] =
        HID_KEYB_USAGE_RESERVED;
    psInst->ucKeyCount--;

    //
    // Tell the caller we were successful.
    //
    return(true);
}

//*****************************************************************************
//
//! Initializes HID keyboard device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for HID keyboard device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the HID keyboard device.
//!
//! An application wishing to offer a USB HID keyboard interface to a USB host
//! must call this function to initialize the USB controller and attach the
//! keyboard device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psDevice pointer
//! passed to it.  This must be passed on all future calls to the HID keyboard
//! device driver.
//!
//! When a host connects and configures the device, the application callback
//! will receive \b USB_EVENT_CONNECTED after which calls can be made to
//! USBDHIDKeyboardKeyStateChange() to report key presses and releases to the
//! USB host.
//!
//! \note The application must not make any calls to the lower level USB device
//! interfaces if interacting with USB via the USB HID keyboard device class
//! API.  Doing so will cause unpredictable (though almost certainly
//! unpleasant) behavior.
//!
//! \return Returns NULL on failure or the psDevice pointer on success.
//
//*****************************************************************************
void *
USBDHIDKeyboardInit(unsigned long ulIndex,
                    const tUSBDHIDKeyboardDevice *psDevice)
{
    void *pvRetcode;
    tUSBDHIDDevice *psHIDDevice;

    //
    // Check parameter validity.
    //
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDKbdData);
    ASSERT(psDevice->pfnCallback);

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

    //
    // Call the common initialization routine.
    //
    pvRetcode = USBDHIDKeyboardCompositeInit(ulIndex, psDevice);

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
//! Initializes HID keyboard device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for HID keyboard device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the HID keyboard device.
//!
//! This call is very similar to USBDKeyboardInit() except that it is used for
//! initializing an instance of the HID keyboard device for use in a composite
//! device.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB HID Keyboard APIs.
//
//*****************************************************************************
void *
USBDHIDKeyboardCompositeInit(unsigned long ulIndex,
                             const tUSBDHIDKeyboardDevice *psDevice)
{
    tHIDKeyboardInstance *psInst;
    unsigned long ulLoop;
    tUSBDHIDDevice *psHIDDevice;

    //
    // Check parameter validity.
    //
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDKbdData);
    ASSERT(psDevice->pfnCallback);

    //
    // Get a pointer to our instance data
    //
    psInst = psDevice->psPrivateHIDKbdData;

    //
    // Initialize the various fields in our instance structure.
    //
    psInst->ucUSBConfigured = 0;
    psInst->ucProtocol = USB_HID_PROTOCOL_REPORT;
    psInst->sReportIdle.ucDuration4mS = 125;
    psInst->sReportIdle.ucReportID = 0;
    psInst->sReportIdle.ulTimeSinceReportmS = 0;
    psInst->sReportIdle.usTimeTillNextmS = 0;
    psInst->ucLEDStates = 0;
    psInst->ucKeyCount = 0;
    for(ulLoop = 0; ulLoop < KEYB_MAX_CHARS_PER_REPORT; ulLoop++)
    {
        psInst->pucKeysPressed[ulLoop] = HID_KEYB_USAGE_RESERVED;
    }

    psInst->eKeyboardState = HID_KEYBOARD_STATE_UNCONFIGURED;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

    //
    // Initialize the HID device class instance structure based on input from
    // the caller.
    //
    psHIDDevice->usPID = psDevice->usPID;
    psHIDDevice->usVID = psDevice->usVID;
    psHIDDevice->usMaxPowermA = psDevice->usMaxPowermA;
    psHIDDevice->ucPwrAttributes = psDevice->ucPwrAttributes;
    psHIDDevice->ucSubclass = USB_HID_SCLASS_BOOT;
    psHIDDevice->ucProtocol = USB_HID_PROTOCOL_KEYB;
    psHIDDevice->ucNumInputReports = 1;
    psHIDDevice->psReportIdle = 0;
    psHIDDevice->pfnRxCallback = HIDKeyboardRxHandler;
    psHIDDevice->pvRxCBData = (void *)psDevice;
    psHIDDevice->pfnTxCallback = HIDKeyboardTxHandler;
    psHIDDevice->pvTxCBData = (void *)psDevice;
    psHIDDevice->bUseOutEndpoint = false,
    psHIDDevice->psHIDDescriptor = &g_sKeybHIDDescriptor;
    psHIDDevice->ppClassDescriptors = g_pKeybClassDescriptors;
    psHIDDevice->ppStringDescriptors = psDevice->ppStringDescriptors;
    psHIDDevice->ulNumStringDescriptors = psDevice->ulNumStringDescriptors;
    psHIDDevice->psPrivateHIDData = &psInst->sHIDInstance;
    psHIDDevice->psReportIdle = &psInst->sReportIdle;

    //
    // Initialize the lower layer HID driver and pass it the various structures
    // and descriptors necessary to declare that we are a keyboard.
    //
    return(USBDHIDCompositeInit(ulIndex, psHIDDevice));
}

//*****************************************************************************
//
//! Shuts down the HID keyboard device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDKeyboardInit().
//!
//! This function terminates HID keyboard operation for the instance supplied
//! and removes the device from the USB bus.  Following this call, the \e
//! pvInstance instance may not me used in any other call to the HID keyboard
//! device other than USBDHIDKeyboardInit().
//!
//! \return None.
//
//*****************************************************************************
void
USBDHIDKeyboardTerm(void *pvInstance)
{
    tUSBDHIDKeyboardDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    //
    // Get a pointer to the device.
    //
    psDevice = (tUSBDHIDKeyboardDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

    //
    // Mark the device as no longer configured.
    //
    psDevice->psPrivateHIDKbdData->ucUSBConfigured = 0;

    //
    // Terminate the low level HID driver.
    //
    USBDHIDTerm(psHIDDevice);
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the keyboard callback.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDKeyboardInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the keyboard callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnCallback function
//! passed on USBDHIDKeyboardInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the pvInstance structure passed to USBDHIDKeyboardInit() resides
//! in RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was set for this
//! instance.
//
//*****************************************************************************
void *
USBDHIDKeyboardSetCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldCBData;
    tUSBDHIDKeyboardDevice *psKeyboard;

    //
    // Check for a NULL pointer in the device parameter.
    //
    ASSERT(pvInstance);

    //
    // Get a pointer to our keyboard device.
    //
    psKeyboard = (tUSBDHIDKeyboardDevice *)pvInstance;

    //
    // Save the old callback pointer and replace it with the new value.
    //
    pvOldCBData = psKeyboard->pvCBData;
    psKeyboard->pvCBData = pvCBData;

    //
    // Pass the old callback pointer back to the caller.
    //
    return(pvOldCBData);
}

//*****************************************************************************
//
//! Reports a key state change to the USB host.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDKeyboardInit().
//! \param ucModifiers contains the states of each of the keyboard modifiers
//! (left/right shift, ctrl, alt or GUI keys).  Valid values are logical OR
//! combinations of the labels \b HID_KEYB_LEFT_CTRL, \b HID_KEYB_LEFT_SHIFT,
//! \b HID_KEYB_LEFT_ALT, \b HID_KEYB_LEFT_GUI, \b HID_KEYB_RIGHT_CTRL, \b
//! HID_KEYB_RIGHT_SHIFT, \b HID_KEYB_RIGHT_ALT and \b HID_KEYB_RIGHT_GUI.
//! Presence of one of these bit flags indicates that the relevant modifier
//! key is pressed and absence indicates that it is released.
//! \param ucUsageCode is the usage code of the key whose state has changed.
//! If only modifier keys have changed, \b HID_KEYB_USAGE_RESERVED should be
//! passed in this parameter.
//! \param bPress is \b true if the key has been pressed or \b false if it has
//! been released.  If only modifier keys have changed state, this parameter is
//! ignored.
//!
//! This function adds or removes a key usage code from the list of keys
//! currently pressed and schedules a report transmission to the host to
//! inform it of the new keyboard state.  If the maximum number of simultaneous
//! key presses are already recorded, the report to the host will contain the
//! rollover error code, HID_KEYB_USAGE_ROLLOVER instead of key usage codes
//! and the caller will receive return code KEYB_ERR_TOO_MANY_KEYS.
//!
//! \return Returns \b KEYB_SUCCESS if the key usage code was added to or
//! removed from the current list successfully.  \b KEYB_ERR_TOO_MANY_KEYS is
//! returned if an attempt is made to press a 7th key (the BIOS keyboard
//! protocol can report no more than 6 simultaneously pressed keys).  If called
//! before the USB host has configured the device, \b KEYB_ERR_NOT_CONFIGURED
//! is returned and, if an error is reported while attempting to transmit the
//! report, \b KEYB_ERR_TX_ERROR is returned.  If an attempt is made to remove
//! a key from the pressed list (by setting parameter \e bPressed to \b false)
//! but the key usage code is not found, \b KEYB_ERR_NOT_FOUND is returned.
//
//*****************************************************************************
unsigned long
USBDHIDKeyboardKeyStateChange(void *pvInstance, unsigned char ucModifiers,
                              unsigned char ucUsageCode, tBoolean bPress)
{
    tBoolean bRetcode;
    unsigned long ulLoop;
    unsigned long ulCount;
    tHIDKeyboardInstance *psInst;
    tUSBDHIDKeyboardDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    psDevice = (tUSBDHIDKeyboardDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

    //
    // Assume all is well until we determine otherwise.
    //
    bRetcode = true;

    //
    // Get a pointer to our instance data
    //
    psInst = psDevice->psPrivateHIDKbdData;

    //
    // Update the global keyboard report with the information passed.
    //
    psInst->pucReport[0] = ucModifiers;
    psInst->pucReport[1] = 0;

    //
    // Were we passed a usage code for a new key press or release or was
    // this call just telling us about a modifier change?
    //
    if(ucUsageCode != HID_KEYB_USAGE_RESERVED)
    {
        //
        // Has a key been pressed or released?
        //
        if(bPress)
        {
            //
            // A key has been pressed - add it to the list if there is space an
            // and the key is not already in the list.
            //
            bRetcode = AddKeyToPressedList(psInst, ucUsageCode);
        }
        else
        {
            //
            // A key has been released - remove it from the list.
            //
            bRetcode = RemoveKeyFromPressedList(psInst, ucUsageCode);

            //
            // The return code here indicates whether the key was found.  If it
            // wasn't, the list has not changes so merely exit at this point
            // without sending anything to the host.
            //
            if(!bRetcode)
            {
                return(KEYB_ERR_NOT_FOUND);
            }
        }

        //
        // Build the report from the current list of keys.  If we added a key
        // and got a bad return code indicating a roll over error, we need to
        // send a roll over report
        //
        for(ulLoop = 0; ulLoop < KEYB_MAX_CHARS_PER_REPORT; ulLoop++)
        {
            psInst->pucReport[2 + ulLoop] = (bRetcode ?
                  psInst->pucKeysPressed[ulLoop] : HID_KEYB_USAGE_ROLLOVER);
        }
    }

    //
    // If we are not configured, return an error here before trying to send
    // anything.
    //
    if(!psInst->ucUSBConfigured)
    {
        return(KEYB_ERR_NOT_CONFIGURED);
    }

    //
    // Only send a report if the transmitter is currently free.
    //
    if(USBDHIDTxPacketAvailable((void *)psHIDDevice))
    {
        //
        // Send the report to the host.
        //
        psInst->eKeyboardState = HID_KEYBOARD_STATE_SEND;
        ulCount = USBDHIDReportWrite((void *)psHIDDevice,
                                     psInst->pucReport, KEYB_IN_REPORT_SIZE,
                                     true);

        //
        // Did we schedule a packet for transmission correctly?
        //
        if(!ulCount)
        {
            //
            // No - report the error to the caller.
            //
            return(KEYB_ERR_TX_ERROR);
        }
    }
    else
    {
        //
        // We can't send the report immediately so mark the instance so that
        // it is sent next time the transmitter is free.
        //
        psInst->bChangeMade = true;
    }

    //
    // If we get this far, the key information was sent successfully.  Are
    // too many keys currently pressed, though?
    //
    return(bRetcode ? KEYB_SUCCESS : KEYB_ERR_TOO_MANY_KEYS);
}

//*****************************************************************************
//
//! Reports the device power status (bus or self powered) to the USB library.
//!
//! \param pvInstance is the pointer to the keyboard device instance structure.
//! \param ucPower indicates the current power status, either \b
//! USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//!
//! Applications which support switching between bus or self powered
//! operation should call this function whenever the power source changes
//! to indicate the current power status to the USB library.  This information
//! is required by the USB library to allow correct responses to be provided
//! when the host requests status from the device.
//!
//! \return None.
//
//*****************************************************************************
void
USBDHIDKeyboardPowerStatusSet(void *pvInstance, unsigned char ucPower)
{
    tUSBDHIDKeyboardDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    //
    // Get the keyboard device pointer.
    //
    psDevice = (tUSBDHIDKeyboardDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

    //
    // Pass the request through to the lower layer.
    //
    USBDHIDPowerStatusSet((void *)psHIDDevice, ucPower);
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param pvInstance is the pointer to the keyboard device instance structure.
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
USBDHIDKeyboardRemoteWakeupRequest(void *pvInstance)
{
    tUSBDHIDKeyboardDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    //
    // Get the keyboard device pointer.
    //
    psDevice = (tUSBDHIDKeyboardDevice *)pvInstance;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psDevice->psPrivateHIDKbdData->sHIDDevice;

    //
    // Pass the request through to the lower layer.
    //
    return(USBDHIDRemoteWakeupRequest((void *)psHIDDevice));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
