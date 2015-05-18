//*****************************************************************************
//
// usbhhidmouse.c - This file holds the application interfaces for USB
//                  mouse devices.
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
#include "usblib/usblib.h"
#include "usblib/host/usbhost.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhhid.h"
#include "usblib/host/usbhhidmouse.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_device
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for local functions.
//
//*****************************************************************************
static unsigned long USBHMouseCallback(void *pvCBData,
                                       unsigned long ulEvent,
                                       unsigned long ulMsgParam,
                                       void *pvMsgData);

//*****************************************************************************
//
// The size of a USB mouse report.
//
//*****************************************************************************
#define USBHMS_REPORT_SIZE      4

//*****************************************************************************
//
// These are the flags for the tUSBHMouse.ulHIDFlags member variable.
//
//*****************************************************************************
#define USBHMS_DEVICE_PRESENT   0x00000001

//*****************************************************************************
//
// This is the structure definition for a mouse device instance.
//
//*****************************************************************************
typedef struct
{
    //
    // Global flags for an instance of a mouse.
    //
    unsigned long ulHIDFlags;

    //
    // The applications registered callback.
    //
    tUSBCallback pfnCallback;

    //
    // The current state of the buttons.
    //
    unsigned char ucButtons;

    //
    // This is a local buffer to hold the current HID report that comes up
    // from the HID driver layer.
    //
    unsigned char pucBuffer[USBHMS_REPORT_SIZE];

    //
    // Heap data for the mouse currently used to read the HID Report
    // Descriptor.
    //
    unsigned char *pucHeap;

    //
    // Size of the heap in bytes.
    //
    unsigned long ulHeapSize;

    //
    // This is the instance value for the HID device that will be used for the
    // mouse.
    //
    unsigned long ulMouseInstance;
}
tUSBHMouse;

//*****************************************************************************
//
// This is the per instance information for a mouse device.
//
//*****************************************************************************
static tUSBHMouse g_sUSBHMouse =
{
    0
};

//*****************************************************************************
//
//! This function is used open an instance of a mouse.
//!
//! \param pfnCallback is the callback function to call when new events occur
//! with the mouse returned.
//! \param pucBuffer is the memory used by the driver to interact with the
//! USB mouse.
//! \param ulSize is the size of the buffer provided by \e pucBuffer.
//!
//! This function is used to open an instance of the mouse.  The value
//! returned from this function should be used as the instance identifier for
//! all other USBHMouse calls.  The \e pucBuffer memory buffer is used to
//! access the mouse.  The buffer size required is at least enough to hold
//! a normal report descriptor for the device.
//!
//! \return Returns the instance identifier for the mouse that is attached.
//! If there is no mouse present this will return 0.
//
//*****************************************************************************
unsigned long
USBHMouseOpen(tUSBCallback pfnCallback, unsigned char *pucBuffer,
              unsigned long ulSize)
{
    //
    // Save the callback and data pointers.
    //
    g_sUSBHMouse.pfnCallback = pfnCallback;

    //
    // Save the instance pointer for the HID device that was opened.
    //
    g_sUSBHMouse.ulMouseInstance = USBHHIDOpen(USBH_HID_DEV_MOUSE,
                                               USBHMouseCallback,
                                               (unsigned long)&g_sUSBHMouse);

    //
    // Save the heap buffer and size.
    //
    g_sUSBHMouse.pucHeap = pucBuffer;
    g_sUSBHMouse.ulHeapSize = ulSize;

    return((unsigned long)&g_sUSBHMouse);
}

//*****************************************************************************
//
//! This function is used close an instance of a mouse.
//!
//! \param ulInstance is the instance value for this mouse.
//!
//! This function is used to close an instance of the mouse that was opened
//! with a call to USBHMouseOpen().  The \e ulInstance value is the value
//! that was returned when the application called USBHMouseOpen().
//!
//! \return Returns 0.
//
//*****************************************************************************
unsigned long
USBHMouseClose(unsigned long ulInstance)
{
    tUSBHMouse *pUSBHMouse;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHMouse = (tUSBHMouse *)ulInstance;

    //
    // Reset the callback to null.
    //
    pUSBHMouse->pfnCallback = 0;

    //
    // Call the HID driver layer to close out this instance.
    //
    USBHHIDClose(pUSBHMouse->ulMouseInstance);

    return(0);
}

//*****************************************************************************
//
//! This function is used to initialize a mouse interface after a mouse has
//! been detected.
//!
//! \param ulInstance is the instance value for this mouse.
//!
//! This function should be called after receiving a \b USB_EVENT_CONNECTED
//! event in the callback function provided by USBHMouseOpen(), however it
//! should only be called outside of the callback function.  This will
//! initialize the mouse interface and determine how it reports events to the
//! USB host controller.  The \e ulInstance value is the value that was
//! returned when the application called USBHMouseOpen().  This function only
//! needs to be called once per connection event but it should be called every
//! time a \b USB_EVENT_CONNECTED event occurs.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
unsigned long
USBHMouseInit(unsigned long ulInstance)
{
    tUSBHMouse *pUSBHMouse;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHMouse = (tUSBHMouse *)ulInstance;

    //
    // Set the initial rate to only update on mouse state changes.
    //
    USBHHIDSetIdle(pUSBHMouse->ulMouseInstance, 0, 0);

    //
    // Read out the Report Descriptor from the mouse and parse it for
    // the format of the reports coming back from the mouse.
    //
    USBHHIDGetReportDescriptor(pUSBHMouse->ulMouseInstance,
                               pUSBHMouse->pucHeap,
                               pUSBHMouse->ulHeapSize);

    //
    // Set the mouse to boot protocol.
    //
    USBHHIDSetProtocol(pUSBHMouse->ulMouseInstance, 1);

    return(0);
}

//*****************************************************************************
//
// This function handles updating the state of the mouse buttons and axis.
//
// \param pUSBHMouse is the pointer to an instance of the mouse data.
//
// This function will check for updates to buttons or X/Y movements and send
// callbacks to the mouse callback function.
//
// \return None.
//
//*****************************************************************************
static void
UpdateMouseState(tUSBHMouse *pUSBHMouse)
{
    unsigned long ulButton;

    if(pUSBHMouse->pucBuffer[0] != pUSBHMouse->ucButtons)
    {
        for(ulButton = 1; ulButton <= 0x4; ulButton <<= 1)
        {
            if(((pUSBHMouse->pucBuffer[0] & ulButton) != 0) &&
               ((pUSBHMouse->ucButtons & ulButton) == 0))
            {
                //
                // Send the mouse button press notification to the application.
                //
                pUSBHMouse->pfnCallback(0,
                                        USBH_EVENT_HID_MS_PRESS,
                                        ulButton,
                                        0);
            }
            if(((pUSBHMouse->pucBuffer[0] & ulButton) == 0) &&
               ((pUSBHMouse->ucButtons & ulButton) != 0))
            {
                //
                // Send the mouse button release notification to the
                // application.
                //
                pUSBHMouse->pfnCallback(0,
                                        USBH_EVENT_HID_MS_REL,
                                        ulButton,
                                        0);
            }
        }

        //
        // Save the new state.
        //
        pUSBHMouse->ucButtons = pUSBHMouse->pucBuffer[0];
    }
    if(pUSBHMouse->pucBuffer[1] != 0)
    {
        //
        // Send the mouse button release notification to the
        // application.
        //
        pUSBHMouse->pfnCallback(0,
                                USBH_EVENT_HID_MS_X,
                                (unsigned long)pUSBHMouse->pucBuffer[1],
                                0);
    }
    if(pUSBHMouse->pucBuffer[2] != 0)
    {
        //
        // Send the mouse button release notification to the
        // application.
        //
        pUSBHMouse->pfnCallback(0,
                                USBH_EVENT_HID_MS_Y,
                                (unsigned long)pUSBHMouse->pucBuffer[2],
                                0);
    }
}

//*****************************************************************************
//
//! This function handles event callbacks from the USB HID driver layer.
//!
//! \param pvCBData is the pointer that was passed in to the USBHHIDOpen()
//! call.
//! \param ulEvent is the event that has been passed up from the HID driver.
//! \param ulMsgParam has meaning related to the \e ulEvent that occurred.
//! \param pvMsgData has meaning related to the \e ulEvent that occurred.
//!
//! This function will receive all event updates from the HID driver layer.
//! The mouse driver itself will mostly be concerned with report callbacks
//! from the HID driver layer and parsing them into keystrokes for the
//! application that has registered for callbacks with the USBHMouseOpen()
//! call.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
unsigned long
USBHMouseCallback(void *pvCBData, unsigned long ulEvent,
                  unsigned long ulMsgParam, void *pvMsgData)
{
    tUSBHMouse *pUSBHMouse;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHMouse = (tUSBHMouse *)pvCBData;

    switch(ulEvent)
    {
        //
        // New mouse has been connected so notify the application.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Remember that a mouse is present.
            //
            pUSBHMouse->ulHIDFlags |= USBHMS_DEVICE_PRESENT;

            //
            // Notify the application that a new mouse was connected.
            //
            pUSBHMouse->pfnCallback(0, ulEvent, ulMsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            //
            // No mouse is present.
            //
            pUSBHMouse->ulHIDFlags &= ~USBHMS_DEVICE_PRESENT;

            //
            // Notify the application that the mouse was disconnected.
            //
            pUSBHMouse->pfnCallback(0, ulEvent, ulMsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // New mouse report structure was received.
            //
            USBHHIDGetReport(pUSBHMouse->ulMouseInstance, 0,
                             pUSBHMouse->pucBuffer,
                             USBHMS_REPORT_SIZE);

            //
            // Update the current state of the mouse and notify the application
            // of any changes.
            //
            UpdateMouseState(pUSBHMouse);

            break;
        }
    }
    return(0);
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
