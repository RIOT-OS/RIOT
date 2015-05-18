//*****************************************************************************
//
// usbhhidkeyboard.c - This file holds the application interfaces for USB
//                     keyboard devices.
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
#include "usblib/host/usbhhidkeyboard.h"

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
static unsigned long USBHKeyboardCallback(void *pvCBData,
                                          unsigned long ulEvent,
                                          unsigned long ulMsgParam,
                                          void *pvMsgData);

//*****************************************************************************
//
// The size of a USB keyboard report.
//
//*****************************************************************************
#define USBHKEYB_REPORT_SIZE    8

//*****************************************************************************
//
// These are the flags for the tUSBHKeyboard.ulHIDFlags member variable.
//
//*****************************************************************************
#define USBHKEYB_DEVICE_PRESENT 0x00000001

//*****************************************************************************
//
// This is the structure definition for a keyboard device instance.
//
//*****************************************************************************
typedef struct
{
    //
    // Global flags for an instance of a keyboard.
    //
    unsigned long ulHIDFlags;

    //
    // The applications registered callback.
    //
    tUSBCallback pfnCallback;

    //
    // The HID instance pointer for this keyboard instance.
    //
    unsigned long ulHIDInstance;

    //
    // NUM_LOCK, CAPS_LOCK, SCROLL_LOCK, COMPOSE or KANA keys.
    //
    unsigned char ucKeyModSticky;

    //
    // This is the current state of the keyboard modifier keys.
    //
    unsigned char ucKeyModState;

    //
    // This holds the keyboard usage codes for keys that are being held down.
    //
    unsigned char pucKeyState[6];

    //
    // This is a local buffer to hold the current HID report that comes up
    // from the HID driver layer.
    //
    unsigned char pucBuffer[USBHKEYB_REPORT_SIZE];
}
tUSBHKeyboard;

//*****************************************************************************
//
// This is the per instance information for a keyboard device.
//
//*****************************************************************************
static tUSBHKeyboard g_sUSBHKeyboard =
{
    0
};

//*****************************************************************************
//
//! This function is used open an instance of a keyboard.
//!
//! \param pfnCallback is the callback function to call when new events occur
//! with the keyboard returned.
//! \param pucBuffer is the memory used by the keyboard to interact with the
//! USB keyboard.
//! \param ulSize is the size of the buffer provided by \e pucBuffer.
//!
//! This function is used to open an instance of the keyboard.  The value
//! returned from this function should be used as the instance identifier for
//! all other USBHKeyboard calls.  The \e pucBuffer memory buffer is used to
//! access the keyboard.  The buffer size required is at least enough to hold
//! a normal report descriptor for the device.  If there is not enough space
//! only a partial report descriptor will be read out.
//!
//! \return Returns the instance identifier for the keyboard that is attached.
//! If there is no keyboard present this will return 0.
//
//*****************************************************************************
unsigned long
USBHKeyboardOpen(tUSBCallback pfnCallback, unsigned char *pucBuffer,
                 unsigned long ulSize)
{
    //
    // Save the callback and data pointers.
    //
    g_sUSBHKeyboard.pfnCallback = pfnCallback;

    //
    // Save the instance pointer for the HID device that was opened.
    //
    g_sUSBHKeyboard.ulHIDInstance =
        USBHHIDOpen(USBH_HID_DEV_KEYBOARD, USBHKeyboardCallback,
                    (unsigned long)&g_sUSBHKeyboard);

    return((unsigned long)&g_sUSBHKeyboard);
}

//*****************************************************************************
//
//! This function is used close an instance of a keyboard.
//!
//! \param ulInstance is the instance value for this keyboard.
//!
//! This function is used to close an instance of the keyboard that was opened
//! with a call to USBHKeyboardOpen().  The \e ulInstance value is the value
//! that was returned when the application called USBHKeyboardOpen().
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
unsigned long
USBHKeyboardClose(unsigned long ulInstance)
{
    tUSBHKeyboard *pUSBHKeyboard;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHKeyboard = (tUSBHKeyboard *)ulInstance;

    //
    // Reset the callback to null.
    //
    pUSBHKeyboard->pfnCallback = 0;

    //
    // Call the HID driver layer to close out this instance.
    //
    USBHHIDClose(pUSBHKeyboard->ulHIDInstance);

    return(0);
}

//*****************************************************************************
//
//! This function is used to map a USB usage ID to a printable character.
//!
//! \param ulInstance is the instance value for this keyboard.
//! \param pTable is the table to use to map the usage ID to characters.
//! \param ucUsageID is the USB usage ID to map to a character.
//!
//! This function is used to map a USB usage ID to a character.  The provided
//! \e pTable is used to perform the mapping and is described by the
//! tHIDKeyboardUsageTable type defined structure.  See the documentation on
//! the tHIDKeyboardUsageTable structure for more details on the internals of
//! this structure.  This function uses the current state of the shift keys
//! and the Caps Lock key to modify the data returned by this function.  The
//! pTable structure has values indicating which keys are modified by Caps Lock
//! and alternate values for shifted cases.  The number of bytes returned from
//! this function depends on the \e pTable structure passed in as it holds the
//! number of bytes per character in the table.
//!
//! \return Returns the character value for the given usage id.
//
//*****************************************************************************
unsigned long
USBHKeyboardUsageToChar(unsigned long ulInstance,
                        const tHIDKeyboardUsageTable *pTable,
                        unsigned char ucUsageID)
{
    unsigned long ulValue;
    const unsigned char *pucKeyBoardMap;
    const unsigned short *pusKeyBoardMap;
    unsigned long ulOffset;
    unsigned long ulShift;
    tUSBHKeyboard *pUSBHKeyboard;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHKeyboard = (tUSBHKeyboard *)ulInstance;

    //
    // The added offset for the shifted character value.
    //
    ulShift = 0;

    //
    // Offset in the table for the character.
    //
    ulOffset = (ucUsageID * pTable->ucBytesPerChar * 2);

    //
    // Handle the case where CAPS lock has been set.
    //
    if(pUSBHKeyboard->ucKeyModSticky &= HID_KEYB_CAPS_LOCK)
    {
        //
        // See if this usage ID is modified by Caps Lock by checking the packed
        // bit array in the pulShiftState member of the pTable array.
        //
        if((pTable->pulCapsLock[ucUsageID >> 5]) >> (ucUsageID & 0x1f) & 1)
        {
            ulShift = pTable->ucBytesPerChar;
        }
    }

    //
    // Now handle if a shift key is being held.
    //
    if((pUSBHKeyboard->ucKeyModState & 0x22) != 0)
    {
        //
        // Not shifted yet so we need to shift.
        //
        if(ulShift == 0)
        {
            ulShift = pTable->ucBytesPerChar;
        }
        else
        {
            //
            // Unshift because CAPS LOCK and shift were presed.
            //
            ulShift = 0;
        }
    }

    //
    // One byte per character.
    //
    if(pTable->ucBytesPerChar == 1)
    {
        //
        // Get the base address of the table.
        //
        pucKeyBoardMap = pTable->pCharMapping;

        ulValue = pucKeyBoardMap[ulOffset + ulShift];
    }
    //
    // Two bytes per character.
    //
    else if(pTable->ucBytesPerChar == 2)
    {
        //
        // Get the base address of the table.
        //
        pusKeyBoardMap = (unsigned short *)pTable->pCharMapping;

        ulValue = pusKeyBoardMap[ulOffset + ulShift];
    }
    //
    // All other sizes are unsupported for now.
    //
    else
    {
        ulValue = 0;
    }

    return(ulValue);
}

//*****************************************************************************
//
//! This function is used to set one of the fixed modifier keys on a keyboard.
//!
//! \param ulInstance is the instance value for this keyboard.
//! \param ulModifiers is a bit mask of the modifiers to set on the keyboard.
//!
//! This function is used to set the modifier key states on a keyboard.  The
//! \e ulModifiers value is a bitmask of the following set of values:
//! - HID_KEYB_NUM_LOCK
//! - HID_KEYB_CAPS_LOCK
//! - HID_KEYB_SCROLL_LOCK
//! - HID_KEYB_COMPOSE
//! - HID_KEYB_KANA
//!
//! Not all of these will be supported on all keyboards however setting values
//! on a keyboard that does not have them should have no effect.  The
//! \e ulInstance value is the value that was returned when the application
//! called USBHKeyboardOpen().  If the value \b HID_KEYB_CAPS_LOCK is used it
//! will modify the values returned from the USBHKeyboardUsageToChar()
//! function.
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
unsigned long
USBHKeyboardModifierSet(unsigned long ulInstance, unsigned long ulModifiers)
{
    tUSBHKeyboard *pUSBHKeyboard;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHKeyboard = (tUSBHKeyboard *)ulInstance;

    //
    // Remeber the fact that this is set.
    //
    pUSBHKeyboard->ucKeyModSticky = (unsigned char)ulModifiers;

    //
    // Set the LEDs on the keyboard.
    //
    USBHHIDSetReport(pUSBHKeyboard->ulHIDInstance, 0,
                     (unsigned char *)&ulModifiers, 1);

    return(0);
}

//*****************************************************************************
//
//! This function is used to initialize a keyboard interface after a keyboard
//! has been detected.
//!
//! \param ulInstance is the instance value for this keyboard.
//!
//! This function should be called after receiving a \b USB_EVENT_CONNECTED
//! event in the callback function provided by USBHKeyboardOpen(), however this
//! function should only be called outside the callback function.  This will
//! initialize the keyboard interface and determine the keyboard's
//! layout and how it reports keys to the USB host controller.  The
//! \e ulInstance value is the value that was returned when the application
//! called USBHKeyboardOpen().  This function only needs to be called once
//! per connection event but it should be called every time a
//! \b USB_EVENT_CONNECTED event occurs.
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
unsigned long
USBHKeyboardInit(unsigned long ulInstance)
{
    unsigned char ucModData;
    tUSBHKeyboard *pUSBHKeyboard;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHKeyboard = (tUSBHKeyboard *)ulInstance;

    //
    // Set the initial rate to only update on keyboard state changes.
    //
    USBHHIDSetIdle(pUSBHKeyboard->ulHIDInstance, 0, 0);

    //
    // Read out the Report Descriptor from the keyboard and parse it for
    // the format of the reports coming back from the keyboard.
    //
    USBHHIDGetReportDescriptor(pUSBHKeyboard->ulHIDInstance,
                               pUSBHKeyboard->pucBuffer,
                               USBHKEYB_REPORT_SIZE);

    //
    // Set the keyboard to boot protocol.
    //
    USBHHIDSetProtocol(pUSBHKeyboard->ulHIDInstance, 1);

    //
    // Used to clear the initial state of all on keyboard modifiers.
    //
    ucModData = 0;

    //
    // Update the keyboard LED state.
    //
    USBHHIDSetReport(pUSBHKeyboard->ulHIDInstance, 0, &ucModData, 1);

    return(0);
}

//*****************************************************************************
//
//! This function is used to set the automatic poll rate of the keyboard.
//!
//! \param ulInstance is the instance value for this keyboard.
//! \param ulPollRate is the rate in ms to cause the keyboard to update the
//! host regardless of no change in key state.
//!
//! This function will allow an application to tell the keyboard how often it
//! should send updates to the USB host controller regardless of any changes
//! in keyboard state.  The \e ulInstance value is the value that was returned
//! when the application called USBHKeyboardOpen().  The \e ulPollRate is the
//! new value in ms for the update rate on the keyboard.  This value is
//! initially set to 0 which indicates that the keyboard should only to update
//! when the keyboard state changes.  Any value other than 0 can be used to
//! force the keyboard to generate auto-repeat sequences for the application.
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
unsigned long
USBHKeyboardPollRateSet(unsigned long ulInstance, unsigned long ulPollRate)
{
    tUSBHKeyboard *pUSBHKeyboard;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHKeyboard = (tUSBHKeyboard *)ulInstance;

    //
    // Send the Set Idle command to the USB keyboard.
    //
    USBHHIDSetIdle(pUSBHKeyboard->ulHIDInstance, ulPollRate, 0);

    return(0);
}

//*****************************************************************************
//
// This is an internal function used to modify the current keyboard state.
//
// This function checks for changes in the keyboard state due to a new report
// being received from the device.  It first checks if this is a "roll-over"
// case by seeing if 0x01 is in the first position of the new keyboard report.
// This indicates that too many keys were pressed to handle and to ignore this
// report.  Next the keyboard modifier state is stored and if any changes are
// detected a \b USBH_EVENT_HID_KB_MOD event is sent back to the application.
// Then this function will check for any keys that have been released and send
// a \b USBH_EVENT_HID_KB_REL even for each of these keys.  The last check is
// for any new keys that are pressed and a \b USBH_EVENT_HID_KB_PRESS event
// will be sent for each new key pressed.
//
// \return None.
//
//*****************************************************************************
static void
UpdateKeyboardState(tUSBHKeyboard *pUSBHKeyboard)
{
    long lNewKey, lOldKey;

    //
    // rollover code so ignore this buffer.
    //
    if(pUSBHKeyboard->pucBuffer[2] == 0x01)
    {
        return;
    }

    //
    // Handle the keyboard modifier states.
    //
    if(pUSBHKeyboard->ucKeyModState != pUSBHKeyboard->pucBuffer[0])
    {
        //
        // Notify the application of the event.
        //
        pUSBHKeyboard->pfnCallback(0, USBH_EVENT_HID_KB_MOD,
                                   pUSBHKeyboard->pucBuffer[0], 0);

        //
        // Save the new state of the modifier keys.
        //
        pUSBHKeyboard->ucKeyModState = pUSBHKeyboard->pucBuffer[0];
    }

    //
    // This loop checks for keys that have been released to make room for new
    // ones that may have been pressed.
    //
    for(lOldKey = 2; lOldKey < 8; lOldKey++)
    {
        //
        // If there is no old key pressed in this entry go to the next one.
        //
        if(pUSBHKeyboard->pucKeyState[lOldKey] == 0)
        {
            continue;
        }

        //
        // Check if this old key is still in the list of currently pressed
        // keys.
        //
        for(lNewKey = 2; lNewKey < 8; lNewKey++)
        {
            //
            // Break out if the key is still present.
            //
            if(pUSBHKeyboard->pucBuffer[lNewKey]
                            == pUSBHKeyboard->pucKeyState[lOldKey])
            {
                break;
            }
        }
        //
        // If the old key was no longer in the list of pressed keys then
        // notify the application of the key release.
        //
        if(lNewKey == 8)
        {
            //
            // Send the key release notification to the application.
            //
            pUSBHKeyboard->pfnCallback(0,
                                       USBH_EVENT_HID_KB_REL,
                                       pUSBHKeyboard->pucKeyState[lOldKey],
                                       0);
            //
            // Remove the old key from the currently held key list.
            //
            pUSBHKeyboard->pucKeyState[lOldKey] = 0;

        }
    }

    //
    // This loop checks for new keys that have been pressed.
    //
    for(lNewKey = 2; lNewKey < 8; lNewKey++)
    {
        //
        // The new list is empty so no new keys are pressed.
        //
        if(pUSBHKeyboard->pucBuffer[lNewKey] == 0)
        {
            break;
        }

        //
        // This loop checks if the current key was already pressed.
        //
        for(lOldKey = 2; lOldKey < 8; lOldKey++)
        {
            //
            // If it is in both lists then it was already pressed so ignore it.
            //
            if(pUSBHKeyboard->pucBuffer[lNewKey]
                            == pUSBHKeyboard->pucKeyState[lOldKey])
            {
                break;
            }
        }
        //
        // The key in the new list was not found so it is new.
        //
        if(lOldKey == 8)
        {
            //
            // Look for a free location to store this key usage code.
            //
            for(lOldKey = 2; lOldKey < 8; lOldKey++)
            {
                //
                // If an empty location is found, store it and notify the
                // application.
                //
                if(pUSBHKeyboard->pucKeyState[lOldKey] == 0)
                {
                    //
                    // Save the newly pressed key.
                    //
                    pUSBHKeyboard->pucKeyState[lOldKey]
                                    = pUSBHKeyboard->pucBuffer[lNewKey];

                    //
                    // Notify the application of the new key that has been
                    // pressed.
                    //
                    pUSBHKeyboard->pfnCallback(
                        0,
                        USBH_EVENT_HID_KB_PRESS,
                        pUSBHKeyboard->pucBuffer[lNewKey],
                        0);

                    break;
                }
            }
        }
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
//! The keyboard driver itself will mostly be concerned with report callbacks
//! from the HID driver layer and parsing them into keystrokes for the
//! application that has registered for callbacks with the USBHKeyboardOpen()
//! call.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
unsigned long
USBHKeyboardCallback(void *pvCBData, unsigned long ulEvent,
                     unsigned long ulMsgParam, void *pvMsgData)
{
    tUSBHKeyboard *pUSBHKeyboard;

    //
    // Recover the pointer to the instance data.
    //
    pUSBHKeyboard = (tUSBHKeyboard *)pvCBData;

    switch (ulEvent)
    {
        //
        // New keyboard has been connected so notify the application.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Remember that a keyboard is present.
            //
            pUSBHKeyboard->ulHIDFlags |= USBHKEYB_DEVICE_PRESENT;

            //
            // Notify the application that a new keyboard was connected.
            //
            pUSBHKeyboard->pfnCallback(0, ulEvent, ulMsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            //
            // No keyboard is present.
            //
            pUSBHKeyboard->ulHIDFlags &= ~USBHKEYB_DEVICE_PRESENT;

            //
            // Notify the application that the keyboard was disconnected.
            //
            pUSBHKeyboard->pfnCallback(0, ulEvent, ulMsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // New keyboard report structure was received.
            //
            USBHHIDGetReport(pUSBHKeyboard->ulHIDInstance, 0,
                             pUSBHKeyboard->pucBuffer,
                             USBHKEYB_REPORT_SIZE);

            //
            // Update the application on the changes in the keyboard state.
            //
            UpdateKeyboardState(pUSBHKeyboard);

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
