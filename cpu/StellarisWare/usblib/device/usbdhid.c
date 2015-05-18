//*****************************************************************************
//
// usbdhid.c - USB HID device class driver.
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
#include "driverlib/usb.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup hid_device_class_api
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
// Marker used to indicate that a given HID descriptor cannot be found in the
// client-supplied list.
//
//*****************************************************************************
#define HID_NOT_FOUND           0xFFFFFFFF

//*****************************************************************************
//
// Flags that may appear in usDeferredOpFlags to indicate some operation that
// has been requested but could not be processed at the time it was received.
// Each deferred operation is defined as the bit number that should be set in
// tHIDInstance->usDeferredOpFlags to indicate that the operation is pending.
//
//*****************************************************************************
#define HID_DO_PACKET_RX        5
#define HID_DO_SEND_IDLE_REPORT 6

//*****************************************************************************
//
// Macros to convert between USB controller base address and an index.  These
// are currently trivial but are included to allow for the possibility of
// supporting more than one controller in the future.
//
//*****************************************************************************
#define USB_BASE_TO_INDEX(BaseAddr) (0)
#define USB_INDEX_TO_BASE(Index)    (USB0_BASE)

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define INT_IN_ENDPOINT         USB_EP_3
#define INT_OUT_ENDPOINT        USB_EP_3

//*****************************************************************************
//
// Maximum packet size for the interrupt endpoints used for report transmission
// and reception and the associated FIFO sizes to set aside for each endpoint.
//
//*****************************************************************************
#define INT_IN_EP_FIFO_SIZE     USB_FIFO_SZ_64
#define INT_OUT_EP_FIFO_SIZE    USB_FIFO_SZ_64

#define INT_IN_EP_MAX_SIZE      USB_FIFO_SZ_TO_BYTES(INT_IN_EP_FIFO_SIZE)
#define INT_OUT_EP_MAX_SIZE     USB_FIFO_SZ_TO_BYTES(INT_IN_EP_FIFO_SIZE)

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
unsigned char g_pHIDDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_DEVICE,           // USB Device Class
    0,                          // USB Device Sub-class
    USB_HID_PROTOCOL_NONE,      // USB Device protocol
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
// HID device configuration descriptor.
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
unsigned char g_pHIDDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(34),               // The total size of this full structure.
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
unsigned char g_pHIDInterface[] =
{
    //
    // HID Device Class Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,                          // The index for this interface.
    0,                          // The alternate setting for this interface.
    2,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_HID,              // The interface class
    0,                          // The interface sub-class.
    0,                          // The interface protocol for the sub-class
                                // specified above.
    4,                          // The string index for this interface.
};

const unsigned char g_pHIDInEndpoint[] =
{
    //
    // Interrupt IN endpoint descriptor
    //
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USB_EP_TO_INDEX(INT_IN_ENDPOINT),
    USB_EP_ATTR_INT,            // Endpoint is an interrupt endpoint.
    USBShort(INT_IN_EP_MAX_SIZE), // The maximum packet size.
    16,                         // The polling interval for this endpoint.
};

const unsigned char g_pHIDOutEndpoint[] =
{
    //
    // Interrupt OUT endpoint descriptor
    //
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | USB_EP_TO_INDEX(INT_OUT_ENDPOINT),
    USB_EP_ATTR_INT,            // Endpoint is an interrupt endpoint.
    USBShort(INT_OUT_EP_MAX_SIZE), // The maximum packet size.
    16,                         // The polling interval for this endpoint.
};

//*****************************************************************************
//
// The HID configuration descriptor is defined as four or five sections
// depending upon the client's configuration choice.  These sections are:
//
// 1.  The 9 byte configuration descriptor (RAM).
// 2.  The interface descriptor (RAM).
// 3.  The HID report and physical descriptors (provided by the client)
//     (FLASH).
// 4.  The mandatory interrupt IN endpoint descriptor (FLASH).
// 5.  The optional interrupt OUT endpoint descriptor (FLASH).
//
//*****************************************************************************
const tConfigSection g_sHIDConfigSection =
{
    sizeof(g_pHIDDescriptor),
    g_pHIDDescriptor
};

const tConfigSection g_sHIDInterfaceSection =
{
    sizeof(g_pHIDInterface),
    g_pHIDInterface
};

const tConfigSection g_sHIDInEndpointSection =
{
    sizeof(g_pHIDInEndpoint),
    g_pHIDInEndpoint
};

const tConfigSection g_sHIDOutEndpointSection =
{
    sizeof(g_pHIDOutEndpoint),
    g_pHIDOutEndpoint
};

//*****************************************************************************
//
// Place holder for the user's HID descriptor block.
//
//*****************************************************************************
tConfigSection g_sHIDDescriptorSection =
{
   0, (void *)0
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete HID configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psHIDSections[] =
{
    &g_sHIDConfigSection,
    &g_sHIDInterfaceSection,
    &g_sHIDDescriptorSection,
    &g_sHIDInEndpointSection,
    &g_sHIDOutEndpointSection
};

#define NUM_HID_SECTIONS (sizeof(g_psHIDSections) /                         \
                          sizeof(tConfigSection *))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.  Note that this must be
// in RAM since we need to include or exclude the final section based on
// client supplied initialization parameters.
//
//*****************************************************************************
tConfigHeader g_sHIDConfigHeader =
{
    NUM_HID_SECTIONS,
    g_psHIDSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_pHIDConfigDescriptors[] =
{
    &g_sHIDConfigHeader
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest);
static void HandleRequest(void *pvInstance, tUSBRequest *pUSBRequest);
static void HandleConfigChange(void *pvInstance, unsigned long ulInfo);
static void HandleEP0DataReceived(void *pvInstance, unsigned long ulInfo);
static void HandleEP0DataSent(void *pvInstance, unsigned long ulInfo);
static void HandleReset(void *pvInstance);
static void HandleSuspend(void *pvInstance);
static void HandleResume(void *pvInstance);
static void HandleDisconnect(void *pvInstance);
static void HandleEndpoints(void *pvInstance, unsigned long ulStatus);
static void HandleDevice(void *pvInstance, unsigned long ulRequest,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB HID devices.
//
//*****************************************************************************
tDeviceInfo g_sHIDDeviceInfo =
{
    //
    // Device event handler callbacks.
    //
    {
        HandleGetDescriptor,   // GetDescriptor
        HandleRequest,         // RequestHandler
        0,                     // InterfaceChange
        HandleConfigChange,    // ConfigChange
        HandleEP0DataReceived, // DataReceived
        HandleEP0DataSent,     // DataSentCallback
        HandleReset,           // ResetHandler
        HandleSuspend,         // SuspendHandler
        HandleResume,          // ResumeHandler
        HandleDisconnect,      // DisconnectHandler
        HandleEndpoints,       // EndpointHandler
        HandleDevice           // Device handler.
    },
    g_pHIDDeviceDescriptor,
    g_pHIDConfigDescriptors,
    0,                         // Will be completed during USBDHIDInit().
    0,                         // Will be completed during USBDHIDInit().
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
SetDeferredOpFlag(volatile unsigned short *pusDeferredOp,
                  unsigned short usBit, tBoolean bSet)
{
    //
    // Set the flag bit to 1 or 0 using a bitband access.
    //
    HWREGBITH(pusDeferredOp, usBit) = bSet ? 1 : 0;
}

//*****************************************************************************
//
// This function is called to clear the counter used to keep track of the time
// elapsed since a given report was last sent.
//
// \param psDevice points to the HID device structure whose report timer is to
// be cleared.
// \param ucReportID is the first byte of the report to be sent.  If this
// device offers more than one input report, this value is used to find the
// relevant report timer structure in the psDevice structure.
//
// \return None.
//
//*****************************************************************************
static void
ClearReportTimer(const tUSBDHIDDevice *psDevice, unsigned char ucReportID)
{
    unsigned long ulLoop;

    if(psDevice->ucNumInputReports > 1)
    {
        //
        // We have more than 1 input report so the report must begin with a
        // byte containing the report ID.  Scan the table we were provided
        // when the device was initialized to find the entry for this report.
        //
        for(ulLoop = 0; ulLoop < psDevice->ucNumInputReports; ulLoop++)
        {
            if(psDevice->psReportIdle[ulLoop].ucReportID == ucReportID)
            {
                break;
            }
        }
    }
    else
    {
        ulLoop = 0;
    }

    //
    // If we drop out of the loop with an index less than ucNumInputReports,
    // we found the relevant report so clear its timer.
    //
    if(ulLoop < psDevice->ucNumInputReports)
    {
        psDevice->psReportIdle[ulLoop].ulTimeSinceReportmS = 0;
    }
}

//*****************************************************************************
//
// This function is called to clear the idle period timers for each input
// report supported by the device.
//
// \param psDevice points to the HID device structure whose timers are to be
// cleared.
// \param ulTimemS is the elapsed time in milliseconds since the last call
// to this function.
//
// \return None.
//
//*****************************************************************************
static void
ClearIdleTimers(const tUSBDHIDDevice *psDevice)
{
    unsigned long ulLoop;

    //
    // Clear the "time till next report" counters for each input report.
    //
    for(ulLoop = 0; ulLoop < psDevice->ucNumInputReports; ulLoop++)
    {
        psDevice->psReportIdle[ulLoop].usTimeTillNextmS =
            psDevice->psReportIdle[ulLoop].ucDuration4mS * 4;
    }
}

//*****************************************************************************
//
// This function is called periodically to allow us to process the report idle
// timers.
//
// \param psDevice points to the HID device structure whose timers are to be
// updated.
// \param ulElapsedmS indicates the number of milliseconds that have elapsed
// since the last call to this function.
//
// \return None.
//
//*****************************************************************************
static void
ProcessIdleTimers(const tUSBDHIDDevice *psDevice, unsigned long ulElapsedmS)
{
    unsigned long ulLoop;
    unsigned long ulSizeReport;
    void *pvReport;
    tHIDInstance *psInst;
    tBoolean bDeferred;

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDHIDDevice *)psDevice)->psPrivateHIDData;

    //
    // We have not had to defer any report transmissions yet.
    //
    bDeferred = false;

    //
    // Look at each of the input report idle timers in turn.
    //
    for(ulLoop = 0; ulLoop < psDevice->ucNumInputReports; ulLoop++)
    {
        //
        // Update the time since the last report was sent.
        //
        psDevice->psReportIdle[ulLoop].ulTimeSinceReportmS += ulElapsedmS;

        //
        // Is this timer running?
        //
        if(psDevice->psReportIdle[ulLoop].ucDuration4mS)
        {
            //
            // Yes - is it about to expire?
            //
            if(psDevice->psReportIdle[ulLoop].usTimeTillNextmS <= ulElapsedmS)
            {
                //
                // The timer is about to expire.  Can we send a report right
                // now?
                //
                if((psInst->eHIDTxState == HID_STATE_IDLE) &&
                   (psInst->bSendInProgress == false))
                {
                    //
                    // We can send a report so send a message to the
                    // application to retrieve its latest report for
                    // transmission to the host.
                    //
                    ulSizeReport = psDevice->pfnRxCallback(
                               psDevice->pvRxCBData,
                               USBD_HID_EVENT_IDLE_TIMEOUT,
                               psDevice->psReportIdle[ulLoop].ucReportID,
                               &pvReport);

                    //
                    // Schedule the report for transmission.
                    //
                    USBDHIDReportWrite((void *)psDevice, pvReport,
                                       ulSizeReport, true);

                    //
                    // Reload the timer for the next period.
                    //
                    psDevice->psReportIdle[ulLoop].usTimeTillNextmS =
                        psDevice->psReportIdle[ulLoop].ucDuration4mS * 4;
                }
                else
                {
                    //
                    // We can't send the report straight away so flag it for
                    // transmission as soon as the previous transmission ends.
                    //
                    psDevice->psReportIdle[ulLoop].usTimeTillNextmS = 0;
                    bDeferred = true;
                }
            }
            else
            {
                //
                // The timer is not about to expire.  Update the time till the
                // next report transmission.
                //
                psDevice->psReportIdle[ulLoop].usTimeTillNextmS -= ulElapsedmS;
            }
        }
    }

    //
    // If we had to defer transmission of any report, remember this so that we
    // will process it as soon as possible.
    //
    SetDeferredOpFlag(&psInst->usDeferredOpFlags,
                      HID_DO_SEND_IDLE_REPORT, bDeferred);
}

static void
SetIdleTimeout(const tUSBDHIDDevice *psDevice, unsigned char ucReportID,
               unsigned char ucTimeout4mS)
{
    unsigned long ulLoop;
    tBoolean bReportNeeded;
    tHIDReportIdle *psIdle;

    //
    // Remember that we have not found any report that needs to be sent
    // immediately.
    //
    bReportNeeded = false;

    //
    // Search through all the input reports looking for ones that fit the
    // requirements.
    //
    for(ulLoop = 0; ulLoop < psDevice->ucNumInputReports; ulLoop++)
    {
        psIdle = &psDevice->psReportIdle[ulLoop];

        //
        // If the report ID passed matches the report ID in the idle timer
        // control structure or we were passed a report ID of zero, which
        // indicates that all timers are to be set...
        //
        if(!ucReportID || (ucReportID == psIdle->ucReportID))
        {
            //
            // Save the new duration for the idle timer.
            //
            psIdle->ucDuration4mS = ucTimeout4mS;

            //
            // Are we enabling the idle timer?  If so, fix up the time until it
            // needs to fire.
            //
            if(ucTimeout4mS)
            {
                //
                // Determine what the timeout is for this report given the time
                // since the last report of this type was sent.
                //
                if(psIdle->ulTimeSinceReportmS >=
                   ((unsigned long)ucTimeout4mS * 4))
                {
                    psIdle->usTimeTillNextmS = 0;
                    bReportNeeded = true;
                }
                else
                {
                    psIdle->usTimeTillNextmS =
                                        (((unsigned short)ucTimeout4mS * 4) -
                                         psIdle->ulTimeSinceReportmS);
                }
            }
        }
    }

    //
    // If we get to here and bReportNeeded is true, this means we need to
    // send back at least one of the input reports as soon as possible.  Try
    // to do this immediately.
    //
    if(bReportNeeded)
    {
        ProcessIdleTimers(psDevice, 0);
    }
}

//*****************************************************************************
//
// Find the idle timeout for a given HID input report.
//
// \param psDevice points to the HID device whose report idle timeout is to be
// found.
// \param ucReportID identifies the report whose timeout is requested.  If 0,
// the timeout for the first report is returns, regardless of its ID (or
// whether it has one).
//
// This function returns the current idle timeout for a given HID input report.
// The value returned is expressed in terms of 4mS intervals.  Convert to
// milliseconds by multiplying by 4.  If the return value is 0, this indicates
// that an infinite timeout is currently set and the device will not send the
// report unless a state change occurs.
//
// \return Returns the current idle timeout for the given report.
//
//*****************************************************************************
static unsigned long
GetIdleTimeout(const tUSBDHIDDevice *psDevice, unsigned char ucReportID)
{
    unsigned long ulLoop;
    tHIDReportIdle *psIdle;

    //
    // Search through all the input reports looking for ones that fit the
    // requirements.
    //
    for(ulLoop = 0; ulLoop < psDevice->ucNumInputReports; ulLoop++)
    {
        psIdle = &psDevice->psReportIdle[ulLoop];

        //
        // If the report ID passed matches the report ID in the idle timer
        // control structure or we were passed a report ID of zero, which
        // indicates that all timers are to be set...
        //
        if(!ucReportID || (ucReportID == psIdle->ucReportID))
        {
            //
            // We found a report matching the required ID or we were not passed
            // an ID and we are looking at the first report information.
            //
            return((unsigned long)psIdle->ucDuration4mS);
        }
    }

    //
    // If we drop out, the report couldn't be found so we need to indicate
    // an error.
    //
    return(HID_NOT_FOUND);
}

//*****************************************************************************
//
// Find the n-th HID class descriptor of a given type in the client-provided
// descriptor table.
//
// \param psDevice points to the HID device which is to be searched for the
// required class descriptor.
// \param ucType is the type of class descriptor being requested.  This will
// be either USB_HID_DTYPE_REPORT or USB_HID_DTYPE_PHYSICAL.
// \param ulIndex is the zero-based index of the descriptor that is being
// requested.
//
// This function parses the supplied HID descriptor to find the index into the
// sClassDescriptor array that corresponds to the requested descriptor.  If
// a descriptor with the requested index does not exist, HID_NOT_FOUND will be
// returned unless the request is for a physical descriptor and at least one
// such descriptor exists.  In this case, the index returned will be for the
// last physical descriptor (as required by the HID spec 7.1.1).
//
// \return Returns the index of the descriptor within the sClassDescriptor
// of the tHIDDevice structure if found or HID_NOT_FOUND otherwise.
//
//*****************************************************************************
static unsigned long
FindHIDDescriptor(const tUSBDHIDDevice *psDevice, unsigned char ucType,
                  unsigned long ulIndex, unsigned long *pulLen)
{
    tBoolean bFoundType;
    unsigned long ulLoop;
    unsigned long ulCount;
    unsigned long ulLastFound;
    const tHIDClassDescriptorInfo *psDesc;

    //
    // Remember that we have not found any descriptor with a matching type yet.
    //
    bFoundType = false;
    ulCount = 0;
    ulLastFound = 0;

    //
    // Walk through all the class descriptors looking for the one which
    // matches the requested index and type.
    //
    for(ulLoop = 0; ulLoop < psDevice->psHIDDescriptor->bNumDescriptors;
        ulLoop++)
    {
        psDesc = &(psDevice->psHIDDescriptor->sClassDescriptor[ulLoop]);
        if(psDesc->bDescriptorType == ucType)
        {
            //
            // We found a descriptor of the correct type.  Is this the
            // correct index?
            //
            bFoundType = true;

            //
            // Is this the descriptor we are looking for?
            //
            if(ulCount == ulIndex)
            {
                //
                // Yes - we found it so return the index and size to the
                // caller.
                //
                *pulLen = (unsigned long)psDesc->wDescriptorLength;
                return(ulLoop);
            }
            else
            {
                //
                // Update our count and keep looking.  Remember where we were
                // when we found this descriptor in case we need to return the
                // last physical descriptor.
                //
                ulCount++;
                ulLastFound = ulLoop;
            }
        }
    }

    //
    // If we drop out, we didn't find the requested descriptor.  Now handle
    // the special case of a physical descriptor - if we found any physical
    // descriptors, return the last one.
    //
    if((ucType == USB_HID_DTYPE_PHYSICAL) && bFoundType)
    {
        //
        // Get the length of the last descriptor we found.
        //
        psDesc = &(psDevice->psHIDDescriptor->sClassDescriptor[ulLastFound]);
        *pulLen = (unsigned long)psDesc->wDescriptorLength;

        //
        // Return the index to the caller.
        //
        return(ulLastFound);
    }
    else
    {
        //
        // We couldn't find the descriptor so return an appropriate error.
        //
        return(HID_NOT_FOUND);
    }
}

//*****************************************************************************
//
// Schedule transmission of the next packet forming part of an input report.
//
// \param psInst points to the device instance whose input report is to be
// sent.
//
// This function is called to transmit the next packet of an input report
// passed to the driver via a call to USBDHIDReportWrite.  If any data remains
// to be sent, a USB packet is written to the FIFO and scheduled for
// transmission to the host.  The function ensures that reports are sent as
// a sequence of full packets followed by either a single short packet or a
// packet with no data to indicate the end of the transaction.
//
//*****************************************************************************
static long
ScheduleReportTransmission(tHIDInstance *psInst)
{
    unsigned long ulNumBytes;
    unsigned char *pucData;
    long lRetcode;

    //
    // Set the number of bytes to send this iteration.
    //
    ulNumBytes = (unsigned long)(psInst->usInReportSize -
                                 psInst->usInReportIndex);

    //
    // Limit individual transfers to the maximum packet size for the endpoint.
    //
    if(ulNumBytes > INT_IN_EP_MAX_SIZE)
    {
        ulNumBytes = INT_IN_EP_MAX_SIZE;
    }

    //
    // Where are we sending this data from?
    //
    pucData = psInst->pucInReportData + psInst->usInReportIndex;

    //
    // Put the data in the correct FIFO.
    //
    lRetcode = MAP_USBEndpointDataPut(psInst->ulUSBBase, psInst->ucINEndpoint,
                                      pucData, ulNumBytes);

    if(lRetcode != -1)
    {
        //
        // Update the count and index ready for the next time round.
        //
        psInst->usInReportIndex += ulNumBytes;

        //
        // Send out the current data.
        //
        lRetcode = MAP_USBEndpointDataSend(psInst->ulUSBBase,
                                           psInst->ucINEndpoint,
                                           USB_TRANS_IN);
    }

    //
    // Tell the caller how we got on.
    //
    return(lRetcode);
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
// the arrival of data on the interrupt OUT endpoint (in other words, whenever
// the host has sent us a packet of data).  We inform the client that a packet
// is available and, on return, check to see if the packet has been read.  If
// not, we schedule another notification to the client for a later time.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static tBoolean
ProcessDataFromHost(const tUSBDHIDDevice *psDevice, unsigned long ulStatus)
{
    unsigned long ulEPStatus;
    unsigned long ulSize;
    tHIDInstance *psInst;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateHIDData;

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
        SetDeferredOpFlag(&psInst->usDeferredOpFlags, HID_DO_PACKET_RX, true);

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
// from the interrupt IN endpoint (in other words, whenever data has been
// transmitted to the USB host).  We examine the cause of the interrupt and,
// if due to completion of a transmission, notify the client.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static tBoolean
ProcessDataToHost(const tUSBDHIDDevice *psDevice, unsigned long ulStatus)
{
    tHIDInstance *psInst;
    unsigned long ulEPStatus;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateHIDData;

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
    // Our last packet was transmitted successfully.  Is there any more data to
    // send or have we finished sending the whole report?  We know we finished
    // if the usInReportIndex has reached the usInReportSize value.
    //
    if(psInst->usInReportSize == psInst->usInReportIndex)
    {
        //
        // We finished sending the last report so are idle once again.
        //
        psInst->eHIDTxState = HID_STATE_IDLE;

        //
        // Notify the client that the report transmission completed.
        //
        psDevice->pfnTxCallback(psDevice->pvTxCBData, USB_EVENT_TX_COMPLETE,
                                psInst->usInReportSize, (void *)0);

        //
        // Do we have any reports to send as a result of idle timer timeouts?
        //
        if(psInst->usDeferredOpFlags & (1 << HID_DO_SEND_IDLE_REPORT))
        {
            //
            // Yes - send reports for any timers that expired recently.
            //
            ProcessIdleTimers(psDevice, 0);
        }
    }
    else
    {
        //
        // There must be more data or a zero length packet waiting to be sent
        // so go ahead and do this.
        //
        ScheduleReportTransmission(psInst);
    }

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
    const tUSBDHIDDevice *psHIDInst;
    tHIDInstance *psInst;

    ASSERT(pvInstance != 0);

    //
    // Determine if the serial device is in single or composite mode because
    // the meaning of ulIndex is different in both cases.
    //
    psHIDInst = (const tUSBDHIDDevice *)pvInstance;
    psInst = psHIDInst->psPrivateHIDData;

    //
    // Handler for the interrupt OUT data endpoint.
    //
    if(ulStatus & (0x10000 << USB_EP_TO_INDEX(psInst->ucOUTEndpoint)))
    {
        //
        // Data is being sent to us from the host.
        //
        ProcessDataFromHost(pvInstance, ulStatus);
    }

    //
    // Handler for the interrupt IN data endpoint.
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
    tHIDInstance *psInst;
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = pvInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateHIDData;

    //
    // Set all our endpoints to idle state.
    //
    psInst->eHIDRxState = HID_STATE_IDLE;
    psInst->eHIDTxState = HID_STATE_IDLE;

    //
    // If we are not currently connected let the client know we are open for
    // business.
    //
    if(!psInst->bConnected)
    {
        //
        // Pass the connected event to the client.
        //
        psDevice->pfnRxCallback(psDevice->pvRxCBData, USB_EVENT_CONNECTED, 0,
                                (void *)0);
    }

    //
    // Clear the idle timers for each input report.
    //
    ClearIdleTimers(psDevice);

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
    tHIDInstance *psInst;
    unsigned char *pucData;

    //
    // Create the serial instance data.
    //
    psInst = ((tUSBDHIDDevice *)pvInstance)->psPrivateHIDData;

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
                psInst->ucINEndpoint =
                    INDEX_TO_USB_EP((pucData[1] & 0x7f));
            }
            else
            {
                //
                // Extract the new endpoint number.
                //
                psInst->ucOUTEndpoint =
                    INDEX_TO_USB_EP(pucData[1] & 0x7f);
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
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDHIDDevice *)pvInstance;

    //
    // If we are not currently connected so let the client know we are open
    // for business.
    //
    if(psDevice->psPrivateHIDData->bConnected)
    {
        //
        // Pass the disconnected event to the client.
        //
        psDevice->pfnRxCallback(psDevice->pvRxCBData, USB_EVENT_DISCONNECTED,
                                0, (void *)0);
    }

    //
    // Remember that we are no longer connected.
    //
    psDevice->psPrivateHIDData->bConnected = false;
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a request for a
// non-standard descriptor is received.
//
// \param pvInstance is the instance data for this request.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure and determines which
// descriptor is being requested.  Assuming the descriptor can be found, it is
// scheduled for transmission via endpoint zero.  If the descriptor cannot be
// found, the endpoint is stalled to indicate an error to the host.
//
//*****************************************************************************
static void
HandleGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned long ulSize;
    unsigned long ulDesc;
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Which device are we dealing with?
    //
    psDevice = pvInstance;

    //
    // Which type of class descriptor are we being asked for?
    //
    switch(pUSBRequest->wValue >> 8)
    {
        //
        // This is a request for a HID report or physical descriptor.
        //
        case USB_HID_DTYPE_REPORT:
        case USB_HID_DTYPE_PHYSICAL:
        {
            //
            // Find the index to the descriptor that is being queried.
            //
            ulSize = 0;
            ulDesc = FindHIDDescriptor(psDevice, pUSBRequest->wValue >> 8,
                                       pUSBRequest->wValue & 0xFF,
                                       &ulSize);

            //
            // Did we find the descriptor?
            //
            if(ulDesc == HID_NOT_FOUND)
            {
                //
                // No - stall the endpoint and return.
                //
                USBDCDStallEP0(0);
                return;
            }

            //
            // If there is more data to send than the host requested then just
            // send the requested amount of data.
            //
            if(ulSize > pUSBRequest->wLength)
            {
                ulSize = pUSBRequest->wLength;
            }

            //
            // Send the data via endpoint 0.
            //
            USBDCDSendDataEP0(0,
                (unsigned char *)psDevice->ppClassDescriptors[ulDesc], ulSize);

            break;
        }

        //
        // This is a request for the HID descriptor (as found in the
        // configuration descriptor following the relevant interface).
        //
        case USB_HID_DTYPE_HID:
        {
            //
            // How big is the HID descriptor?
            //
            ulSize = (unsigned long)psDevice->psHIDDescriptor->bLength;

            //
            // If there is more data to send than the host requested then just
            // send the requested amount of data.
            //
            if(ulSize > pUSBRequest->wLength)
            {
                ulSize = pUSBRequest->wLength;
            }

            //
            // Send the data via endpoint 0.
            //
            USBDCDSendDataEP0(0, (unsigned char *)psDevice->psHIDDescriptor,
                              ulSize);
            break;
        }

        //
        // This was an unknown request so stall.
        //
        default:
        {
            USBDCDStallEP0(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvInstance is the instance data for this HID device.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure.  Assuming the request is
// understood, it is handled and any required response generated.  If the
// request cannot be handled by this device class, endpoint zero is stalled to
// indicate an error to the host.
//
//*****************************************************************************
static void
HandleRequest(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tHIDInstance *psInst;
    unsigned char ucProtocol;
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Which device are we dealing with?
    //
    psDevice = pvInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateHIDData;

    //
    // Make sure the request was for this interface.
    //
    if(pUSBRequest->wIndex != psInst->ucInterface)
    {
        return;
    }

    //
    // Determine the type of request.
    //
    switch(pUSBRequest->bRequest)
    {
        //
        // A Set Report request is received from the host when it sends an
        // Output report via endpoint 0.
        //
        case USBREQ_SET_REPORT:
        {
            //
            // Ask the application for a buffer large enough to hold the
            // report we are to be sent.
            //
            psInst->usOutReportSize = pUSBRequest->wLength;
            psInst->pucOutReportData =
                (unsigned char *)psDevice->pfnRxCallback(
                               psDevice->pvRxCBData,
                               USBD_HID_EVENT_GET_REPORT_BUFFER,
                               pUSBRequest->wValue,
                               (void *)(unsigned long)(pUSBRequest->wLength));

            //
            // Did the client provide us a buffer?
            //
            if(!psInst->pucOutReportData)
            {
                //
                // The application couldn't provide us a buffer so stall the
                // request.
                //
                USBDCDStallEP0(0);
            }
            else
            {
                //
                // The client provided us a buffer to read the report into
                // so request the data from the host.
                //

                //
                // Set the state to indicate we are waiting for data.
                //
                psInst->eHIDRxState = HID_STATE_WAIT_DATA;

                //
                // Now read the payload of the request.  We handle the actual
                // operation in the data callback once this data is received.
                //
                USBDCDRequestDataEP0(0, psInst->pucOutReportData,
                                     (unsigned long)pUSBRequest->wLength);

                //
                // Need to ACK the data on end point 0 in this case.  Do this
                // after requesting the data to prevent race conditions that
                // occur if you acknowledge before setting up to receive the
                // request data.
                //
                MAP_USBDevEndpointDataAck(psInst->ulUSBBase, USB_EP_0, false);
            }

            break;
        }

        //
        // A Get Report request is used by the host to poll a device for its
        // current state.
        //
        case USBREQ_GET_REPORT:
        {
            unsigned long ulSize;
            unsigned char *pucReport;

            //
            // Get the latest report from the application.
            //
            ulSize = psDevice->pfnRxCallback(psDevice->pvRxCBData,
                                             USBD_HID_EVENT_GET_REPORT,
                                             pUSBRequest->wValue, &pucReport);

            //
            // Need to ACK the data on end point 0 in this case.
            //
            MAP_USBDevEndpointDataAck(psInst->ulUSBBase, USB_EP_0, true);

            //
            // ..then send back the requested report.
            //
            psInst->bGetRequestPending = true;
            USBDCDSendDataEP0(0, pucReport, ulSize);

            break;
        }

        //
        // A set IDLE request has been made.  This indicates to us how often a
        // given report should be sent back to the host in the absence of any
        // change in state of the device.
        //
        case USBREQ_SET_IDLE:
        {
            //
            // Set the idle timeout for the requested report(s).
            //
            SetIdleTimeout(psDevice, pUSBRequest->wValue & 0xFF,
                           (pUSBRequest->wValue >> 8) & 0xFF);

            //
            // Need to ACK the data on end point 0 in this case.
            //
            MAP_USBDevEndpointDataAck(psInst->ulUSBBase, USB_EP_0, true);

            break;
        }

        //
        // A get IDLE request has been made.  This request queries the current
        // idle timeout for a given report.
        //
        case USBREQ_GET_IDLE:
        {
            unsigned long ulTimeout;

            //
            // Determine the timeout for the requested report.
            //
            ulTimeout = GetIdleTimeout(psDevice, pUSBRequest->wValue);

            if(ulTimeout != HID_NOT_FOUND)
            {
                //
                // Need to ACK the data on end point 0 in this case.
                //
                MAP_USBDevEndpointDataAck(psInst->ulUSBBase, USB_EP_0, true);

                //
                // Send our response to the host.
                //
                USBDCDSendDataEP0(0, (unsigned char *)&ulTimeout, 1);
            }
            else
            {
                //
                // The report ID was not found so stall the endpoint.
                //
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // Set either boot or report protocol for reports sent from the device.
        // This is only supported by devices in the boot subclass.
        //
        case USBREQ_SET_PROTOCOL:
        {
            if(psDevice->ucSubclass == USB_HID_SCLASS_BOOT)
            {
                //
                // We need to ACK the data on end point 0 in this case.
                //
                MAP_USBDevEndpointDataAck(psInst->ulUSBBase, USB_EP_0, true);

                //
                // We are a boot subclass device so pass this on to the
                // application.
                //
                psDevice->pfnRxCallback(psDevice->pvRxCBData,
                                        USBD_HID_EVENT_SET_PROTOCOL,
                                        pUSBRequest->wValue,
                                        (void *)0);
            }
            else
            {
                //
                // This is not a boot subclass device so stall the endpoint to
                // show that we don't support this request.
                //
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // Inform the host of the protocol, boot or report, that is currently
        // in use.  This is only supported by devices in the boot subclass.
        //
        case USBREQ_GET_PROTOCOL:
        {
            if(psDevice->ucSubclass == USB_HID_SCLASS_BOOT)
            {
                //
                // We need to ACK the data on end point 0 in this case.
                //
                MAP_USBDevEndpointDataAck(psInst->ulUSBBase, USB_EP_0, true);

                //
                // We are a boot subclass device so pass this on to the
                // application callback to get the answer.
                //
                ucProtocol = (unsigned char)psDevice->pfnRxCallback(
                    psDevice->pvRxCBData, USBD_HID_EVENT_GET_PROTOCOL, 0,
                    (void *)0);

                //
                // Send our response to the host.
                //
                USBDCDSendDataEP0(0, (unsigned char *)&ucProtocol, 1);
            }
            else
            {
                //
                // This is not a boot subclass device so stall the endpoint to
                // show that we don't support this request.
                //
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // This request was not recognized so stall.
        //
        default:
        {
            USBDCDStallEP0(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the data requested
// on endpoint zero is received.
//
//*****************************************************************************
static void
HandleEP0DataReceived(void *pvInstance, unsigned long ulDataSize)
{
    tHIDInstance *psInst;
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Which device are we dealing with?
    //
    psDevice = pvInstance;

    //
    // If we were not passed any data, just return.
    //
    if(ulDataSize == 0)
    {
        return;
    }

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateHIDData;

    //
    // Make sure we are actually expecting something.
    //
    if(psInst->eHIDRxState != HID_STATE_WAIT_DATA)
    {
        return;
    }

    //
    // Change the endpoint state back to idle now that we have been passed
    // the data we were waiting for.
    //
    psInst->eHIDRxState = HID_STATE_IDLE;

    //
    // The only things we ever request via endpoint zero are reports sent to
    // us via a Set_Report request.  Pass the newly received report on to
    // the client.
    //
    psDevice->pfnRxCallback(psDevice->pvRxCBData, USBD_HID_EVENT_SET_REPORT,
                            psInst->usOutReportSize,
                            psInst->pucOutReportData);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the data sent on
// endpoint zero is received and acknowledged by the host.
//
//*****************************************************************************
static void
HandleEP0DataSent(void *pvInstance, unsigned long ulInfo)
{
    tHIDInstance *psInst;
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Which device are we dealing with?
    //
    psDevice = pvInstance;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateHIDData;

    //
    // If we just sent a report in response to a Get_Report request, send an
    // event to the application telling it that the transmission completed.
    //
    if(psInst->bGetRequestPending)
    {
        //
        // Clear the flag now that we are sending the application callback.
        //
        psInst->bGetRequestPending = false;

        psDevice->pfnRxCallback(psDevice->pvRxCBData,
                                USBD_HID_EVENT_REPORT_SENT, 0, (void *)0);
    }

    return;
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device is
// reset.  If we are currently connected, send a disconnect event at this
// point.
//
//*****************************************************************************
static void
HandleReset(void *pvInstance)
{
    ASSERT(pvInstance != 0);

    //
    // Merely call the disconnect handler.  This causes a disconnect message to
    // be sent to the client if we think we are currently connected.
    //
    HandleDisconnect(pvInstance);
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
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDHIDDevice *)pvInstance;

    //
    // Pass the event on to the client.
    //
    psDevice->pfnRxCallback(psDevice->pvRxCBData, USB_EVENT_SUSPEND, 0,
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
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDHIDDevice *)pvInstance;

    //
    // Pass the event on to the client.
    //
    psDevice->pfnRxCallback(psDevice->pvRxCBData,
                            USB_EVENT_RESUME, 0, (void *)0);
}

//*****************************************************************************
//
// This function is called periodically and provides us with a time reference
// and method of implementing delayed or time-dependent operations.
//
// \param pvInstance is the instance data for this request.
// \param ulTimemS is the elapsed time in milliseconds since the last call
// to this function.
//
// \return None.
//
//*****************************************************************************
static void
HIDTickHandler(void *pvInstance, unsigned long ulTimemS)
{
    tHIDInstance *psInst;
    unsigned long ulSize;
    const tUSBDHIDDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDHIDDevice *)pvInstance;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateHIDData;

    //
    // If we are connected, process our idle timers.
    //
    if(psInst->bConnected)
    {
        ProcessIdleTimers(psDevice, ulTimemS);
    }

    //
    // Do we have a deferred receive waiting
    //
    if(psInst->usDeferredOpFlags & (1 << HID_DO_PACKET_RX))
    {
        //
        // Yes - how big is the waiting packet?
        //
        ulSize = MAP_USBEndpointDataAvail(USB0_BASE, psInst->ucOUTEndpoint);

        //
        // Tell the client that there is a packet waiting for it.
        //
        psDevice->pfnRxCallback(psDevice->pvRxCBData,
                                USB_EVENT_RX_AVAILABLE, ulSize, (void *)0);
    }

    return;
}

//*****************************************************************************
//
//! Initializes HID device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for HID device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the HID device.
//!
//! An application wishing to offer a USB HID interface to a host system
//! must call this function to initialize the USB controller and attach the
//! device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psDevice pointer
//! passed to it.  This must be passed on all future calls from the application
//! to the HID device class driver.
//!
//! The USB HID device class API offers the application a report-based transmit
//! interface for Input reports.  Output reports may be received via the
//! control endpoint or via a dedicated Interrupt OUT endpoint.  If using the
//! dedicated endpoint, report data is delivered to the application packet-by-
//! packet.  If the application uses reports longer than 64 bytes and would
//! rather receive full reports, it may use a USB buffer above the receive
//! channel to allow full reports to be read.
//!
//! Transmit Operation:
//!
//! Calls to USBDHIDReportWrite() pass complete reports to the driver for
//! transmission.  These will be transmitted to the host using as many USB
//! packets as are necessary to complete the transmission.
//!
//! Once a full Input report has been acknowledged by the USB host, a
//! USB_EVENT_TX_COMPLETE event is sent to the application transmit callback to
//! inform it that another report may be transmitted.
//!
//! Receive Operation (when using a dedicated interrupt OUT endpoint):
//!
//! An incoming USB data packet will result in a call to the application
//! callback with event USB_EVENT_RX_AVAILABLE.  The application must then
//! call USBDHIDPacketRead(), passing a buffer capable of holding the received
//! packet.  The size of the packet may be determined by calling function
//! USBDHIDRxPacketAvailable() prior to reading the packet.
//!
//! Receive Operation (when not using a dedicated OUT endpoint):
//!
//! If no dedicated OUT endpoint is used, Output and Feature reports are sent
//! from the host using the control endpoint, endpoint zero.  When such a
//! report is received, USBD_HID_EVENT_GET_REPORT_BUFFER is sent to the
//! application which must respond with a buffer large enough to hold the
//! report.  The device class driver will then copy the received report into
//! the supplied buffer before sending USBD_HID_EVENT_SET_REPORT to indicate
//! that the report is now available.
//!
//! \note The application must not make any calls to the low level USB device
//! interface if interacting with USB via the USB HID device class API.  Doing
//! so will cause unpredictable (though almost certainly unpleasant) behavior.
//!
//! \return Returns NULL on failure or the \e psDevice pointer on success.
//
//*****************************************************************************
void *
USBDHIDInit(unsigned long ulIndex, const tUSBDHIDDevice *psDevice)
{
    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDData);
    ASSERT(psDevice->pfnRxCallback);
    ASSERT(psDevice->pfnTxCallback);
    ASSERT(psDevice->ppClassDescriptors);
    ASSERT(psDevice->psHIDDescriptor);
    ASSERT((psDevice->ucNumInputReports == 0) || psDevice->psReportIdle);

    USBDHIDCompositeInit(ulIndex, psDevice);

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the HID device on the bus.
    //
    USBDCDInit(ulIndex, psDevice->psPrivateHIDData->psDevInfo);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! Initializes HID device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for HID device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the HID device.
//!
//!
//! \return Returns NULL on failure or the \e psDevice pointer on success.
//
//*****************************************************************************
void *
USBDHIDCompositeInit(unsigned long ulIndex, const tUSBDHIDDevice *psDevice)
{
    tHIDInstance *psInst;
    tDeviceDescriptor *psDevDesc;
    tInterfaceDescriptor *psDevIf;

    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDData);
    ASSERT(psDevice->pfnRxCallback);
    ASSERT(psDevice->pfnTxCallback);
    ASSERT(psDevice->ppClassDescriptors);
    ASSERT(psDevice->psHIDDescriptor);
    ASSERT((psDevice->ucNumInputReports == 0) || psDevice->psReportIdle);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = psDevice->psPrivateHIDData;
    psInst->psConfDescriptor = (tConfigDescriptor *)g_pHIDDescriptor;
    psInst->psDevInfo = &g_sHIDDeviceInfo;
    psInst->ulUSBBase = USB0_BASE;
    psInst->eHIDRxState = HID_STATE_UNCONFIGURED;
    psInst->eHIDTxState = HID_STATE_UNCONFIGURED;
    psInst->usDeferredOpFlags = 0;
    psInst->bConnected = false;
    psInst->bGetRequestPending = false;
    psInst->bSendInProgress = false;
    psInst->usInReportIndex = 0;
    psInst->usInReportSize = 0;
    psInst->pucInReportData = (unsigned char *)0;
    psInst->usOutReportSize = 0;
    psInst->pucOutReportData = (unsigned char *)0;

    //
    // Set the default endpoint and interface assignments.
    //
    psInst->ucINEndpoint = INT_IN_ENDPOINT;
    psInst->ucOUTEndpoint = INT_OUT_ENDPOINT;
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
    // Slot the client's HID descriptor into our standard configuration
    // descriptor.
    //
    g_sHIDDescriptorSection.usSize = psDevice->psHIDDescriptor->bLength;
    g_sHIDDescriptorSection.pucData =
                                (unsigned char *)psDevice->psHIDDescriptor;

    //
    // Fix up the interface and endpoint descriptors depending upon client
    // choices.
    //
    psDevIf = (tInterfaceDescriptor *)g_pHIDInterface;
    psDevIf->bNumEndpoints = psDevice->bUseOutEndpoint ? 2 : 1;
    psDevIf->bInterfaceSubClass = psDevice->ucSubclass;
    psDevIf->bInterfaceProtocol = psDevice->ucProtocol;

    //
    // If necessary, remove the interrupt OUT endpoint from the configuration
    // descriptor.
    //
    if(psDevice->bUseOutEndpoint == false)
    {
        g_sHIDConfigHeader.ucNumSections = (NUM_HID_SECTIONS - 1);
    }
    else
    {
        g_sHIDConfigHeader.ucNumSections = NUM_HID_SECTIONS;
    }

    //
    // Plug in the client's string table to the device information
    // structure.
    //
    psInst->psDevInfo->ppStringDescriptors = psDevice->ppStringDescriptors;
    psInst->psDevInfo->ulNumStringDescriptors
            = psDevice->ulNumStringDescriptors;
    psInst->psDevInfo->pvInstance = (void *)psDevice;

    //
    // Initialize the input report idle timers if any input reports exist.
    //
    ClearIdleTimers(psDevice);

    //
    // Initialize the USB tick module, this will prevent it from being
    // initialized later in the call to USBDCDInit();
    //
    InternalUSBTickInit();

    //
    // Register our tick handler (this must be done after USBDCDInit).
    //
    InternalUSBRegisterTickHandler(HIDTickHandler,
                                   (void *)psDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! Shuts down the HID device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//!
//! This function terminates HID operation for the instance supplied and
//! removes the device from the USB bus.  This function should not be called
//! if the HID device is part of a composite device and instead the
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
USBDHIDTerm(void *pvInstance)
{
    tHIDInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get a pointer to our instance data.
    //
    psInst = ((tUSBDHIDDevice *)pvInstance)->psPrivateHIDData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(USB_BASE_TO_INDEX(psInst->ulUSBBase));

    psInst->ulUSBBase = 0;
    psInst->psDevInfo = (tDeviceInfo *)0;
    psInst->psConfDescriptor = (tConfigDescriptor *)0;
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the receive channel
//! callback.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the receive channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnRxCallback function
//! passed on USBDHIDInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the pvInstance structure passed to USBDHIDInit() resides in
//! RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's receive callback.
//
//*****************************************************************************
void *
USBDHIDSetRxCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvInstance);

    //
    // Set the callback data for the receive channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDHIDDevice *)pvInstance)->pvRxCBData;
    ((tUSBDHIDDevice *)pvInstance)->pvRxCBData = pvCBData;

    //
    // Return the previous callback data value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Sets the client-specific data pointer for the transmit callback.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the transmit channel callback function.
//!
//! The client uses this function to change the callback data pointer passed in
//! the first parameter on all callbacks to the \e pfnTxCallback function
//! passed on USBDHIDInit().
//!
//! If a client wants to make runtime changes in the callback data, it must
//! ensure that the pvInstance structure passed to USBDHIDInit() resides in
//! RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback data pointer that was being used for
//! this instance's transmit callback.
//
//*****************************************************************************
void *
USBDHIDSetTxCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvInstance);

    //
    // Set the callback data for the transmit channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDHIDDevice *)pvInstance)->pvTxCBData;
    ((tUSBDHIDDevice *)pvInstance)->pvTxCBData = pvCBData;

    //
    // Return the previous callback data value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Transmits a HID device report to the USB host via the HID interrupt IN
//! endpoint.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pcData points to the first byte of data which is to be transmitted.
//! \param ulLength is the number of bytes of data to transmit.
//! \param bLast is ignored in this implementation.  This parameter is required
//! to ensure compatibility with other device class drivers and USB buffers.
//!
//! This function schedules the supplied data for transmission to the USB
//! host in a single USB transaction using as many packets as it takes to send
//! all the data in the report.  If no transmission is currently ongoing,
//! the first packet of data is immediately copied to the relevant USB endpoint
//! FIFO for transmission.  Whenever all the report data has been acknowledged
//! by the host, a \b USB_EVENT_TX_COMPLETE event will be sent to the
//! application transmit callback indicating that another report can now be
//! transmitted.
//!
//! The caller must ensure that the data pointed to by pucData remains
//! accessible and unaltered until the \b USB_EVENT_TX_COMPLETE is received.
//!
//! \return Returns the number of bytes actually scheduled for transmission.
//! At this level, this will either be the number of bytes passed or 0 to
//! indicate a failure.
//
//*****************************************************************************
unsigned long
USBDHIDReportWrite(void *pvInstance, unsigned char *pcData,
                   unsigned long ulLength, tBoolean bLast)
{
    tHIDInstance *psInst;
    long lRetcode;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDHIDDevice *)pvInstance)->psPrivateHIDData;

    //
    // Set a flag indicating that we are currently in the process of sending
    // a packet.
    //
    psInst->bSendInProgress = true;

    //
    // Can we send the data provided?
    //
    if(psInst->eHIDTxState != HID_STATE_IDLE)
    {
        //
        // We are in the middle of sending another report.  Return 0 to
        // indicate that we can't send this report until the previous one
        // finishes.
        //
        psInst->bSendInProgress = false;
        return(0);
    }

    //
    // Clear the elapsed time since this report was last sent.
    //
    if(ulLength)
    {
        ClearReportTimer(pvInstance, *pcData);
    }

    //
    // Keep track of the whereabouts of the report so that we can send it in
    // multiple packets if necessary.
    //
    psInst->pucInReportData = pcData;
    psInst->usInReportIndex = 0;
    psInst->usInReportSize = ulLength;

    //
    // Schedule transmission of the first packet of the report.
    //
    psInst->eHIDTxState = HID_STATE_WAIT_DATA;
    lRetcode = ScheduleReportTransmission(psInst);

    //
    // Clear the flag we use to indicate that we are in the midst of sending
    // a packet.
    //
    psInst->bSendInProgress = false;

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
//! Reads a packet of data received from the USB host via the interrupt OUT
//! endpoint (if in use).
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pcData points to a buffer into which the received data will be
//! written.
//! \param ulLength is the size of the buffer pointed to by pcData.
//! \param bLast indicates whether the client will make a further call to
//! read additional data from the packet.
//!
//! This function reads up to ulLength bytes of data received from the USB
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
USBDHIDPacketRead(void *pvInstance, unsigned char *pcData,
                  unsigned long ulLength, tBoolean bLast)
{
    unsigned long ulEPStatus, ulCount, ulPkt;
    tHIDInstance *psInst;
    long lRetcode;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDHIDDevice *)pvInstance)->psPrivateHIDData;

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
            SetDeferredOpFlag(&psInst->usDeferredOpFlags,
                              HID_DO_PACKET_RX, false);
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
//! returned by USBDHIDInit().
//!
//! This function indicates to the caller whether or not it is safe to send a
//! new report using a call to USBDHIDReportWrite().  The value returned will
//! be the maximum USB packet size (64) if no transmission is currently
//! outstanding or 0 if a transmission is in progress.  Since the function
//! USBDHIDReportWrite() can accept full reports longer than a single USB
//! packet, the caller should be aware that the returned value from this
//! class driver, unlike others, does not indicate the maximum size of report
//! that can be written but is merely an indication that another report can be
//! written.
//!
//! \return Returns 0 if an outgoing report is still being transmitted or 64
//! if no transmission is currently in progress.
//
//*****************************************************************************
unsigned long
USBDHIDTxPacketAvailable(void *pvInstance)
{
    tHIDInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer.
    //
    psInst = ((tUSBDHIDDevice *)pvInstance)->psPrivateHIDData;

    //
    // Do we have a packet transmission currently ongoing?
    //
    if(psInst->eHIDTxState != HID_STATE_IDLE)
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
        return(INT_IN_EP_MAX_SIZE);
    }
}

//*****************************************************************************
//
//! Determines whether a packet is available and, if so, the size of the
//! buffer required to read it.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
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
USBDHIDRxPacketAvailable(void *pvInstance)
{
    unsigned long ulEPStatus;
    unsigned long ulSize;
    tHIDInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get our instance data pointer
    //
    psInst = ((tUSBDHIDDevice *)pvInstance)->psPrivateHIDData;

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
//! \param pvInstance is the pointer to the HID device instance structure.
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
USBDHIDPowerStatusSet(void *pvInstance, unsigned char ucPower)
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
//! \param pvInstance is the pointer to the HID device instance structure.
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
USBDHIDRemoteWakeupRequest(void *pvInstance)
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
