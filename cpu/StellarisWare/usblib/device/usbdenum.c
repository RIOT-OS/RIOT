//*****************************************************************************
//
// usbenum.c - Enumeration code to handle all endpoint zero traffic.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
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

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "driverlib/rtos_bindings.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdevicepriv.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
// External prototypes.
//
//*****************************************************************************
extern tUSBMode g_eUSBMode;

//*****************************************************************************
//
// Local functions prototypes.
//
//*****************************************************************************
static void USBDGetStatus(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDClearFeature(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDSetFeature(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDSetAddress(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDSetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDGetConfiguration(void *pvInstance,
                                 tUSBRequest *pUSBRequest);
static void USBDSetConfiguration(void *pvInstance,
                                 tUSBRequest *pUSBRequest);
static void USBDGetInterface(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDSetInterface(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDSyncFrame(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDEP0StateTx(unsigned long ulIndex);
static void USBDEP0StateTxConfig(unsigned long ulIndex);
static long USBDStringIndexFromRequest(unsigned short usLang,
                                       unsigned short usIndex);

//*****************************************************************************
//
//! \addtogroup device_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! The default USB endpoint FIFO configuration structure.  This structure
//! contains definitions to set all USB FIFOs into single buffered mode with
//! no DMA use.  Each endpoint's FIFO is sized to hold the largest maximum
//! packet size for any interface alternate setting in the current
//! configuration descriptor.  A pointer to this structure may be passed in the
//! psFIFOConfig field of the tDeviceInfo structure passed to USBCDCInit if the
//! application does not require any special handling of the USB controller
//! FIFO.
//
//*****************************************************************************
const tFIFOConfig g_sUSBDefaultFIFOConfig =
{
    {
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 }
    },
    {
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 },
        { false, 0 }
    },
};

//*****************************************************************************
//
// Indices into the ucHalt array to select the IN or OUT endpoint group.
//
//*****************************************************************************
#define HALT_EP_IN              0
#define HALT_EP_OUT             1

//*****************************************************************************
//
// Define the max packet size for endpoint zero.
//
//*****************************************************************************
#define EP0_MAX_PACKET_SIZE     64

//*****************************************************************************
//
// This is a flag used with g_sUSBDeviceState.ulDevAddress to indicate that a
// device address change is pending.
//
//*****************************************************************************
#define DEV_ADDR_PENDING        0x80000000

//*****************************************************************************
//
// This label defines the default configuration number to use after a bus
// reset.  This may be overridden by calling USBDCDSetDefaultConfiguration()
// during processing of the device reset handler if required.
//
//*****************************************************************************
#define DEFAULT_CONFIG_ID       1

//*****************************************************************************
//
// This label defines the number of milliseconds that the remote wake up signal
// must remain asserted before removing it. Section 7.1.7.7 of the USB 2.0 spec
// states that "the remote wake up device must hold the resume signaling for at
// least 1ms but for no more than 15ms" so 10mS seems a reasonable choice.
//
//*****************************************************************************
#define REMOTE_WAKEUP_PULSE_MS 10

//*****************************************************************************
//
// This label defines the number of milliseconds between the point where we
// assert the remote wake up signal and calling the client back to tell it that
// bus operation has been resumed.  This value is based on the timings provided
// in section 7.1.7.7 of the USB 2.0 specification which indicates that the host
// (which takes over resume signaling when the device's initial signal is
// detected) must hold the resume signaling for at least 20mS.
//
//*****************************************************************************
#define REMOTE_WAKEUP_READY_MS 20

//*****************************************************************************
//
// The buffer for reading data coming into EP0
//
//*****************************************************************************
static unsigned char g_pucDataBufferIn[EP0_MAX_PACKET_SIZE];

tDeviceInstance g_psUSBDevice[1];

//*****************************************************************************
//
// Function table to handle standard requests.
//
//*****************************************************************************
static const tStdRequest g_psUSBDStdRequests[] =
{
    USBDGetStatus,
    USBDClearFeature,
    0,
    USBDSetFeature,
    0,
    USBDSetAddress,
    USBDGetDescriptor,
    USBDSetDescriptor,
    USBDGetConfiguration,
    USBDSetConfiguration,
    USBDGetInterface,
    USBDSetInterface,
    USBDSyncFrame
};

//*****************************************************************************
//
// Functions accessible by USBLIB clients.
//
//*****************************************************************************

//*****************************************************************************
//
//! Initialize the USB library device control driver for a given hardware
//! controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized.
//! \param psDevice is a pointer to a structure containing information that
//! the USB library requires to support operation of this application's
//! device.  The structure contains event handler callbacks and pointers to the
//! various standard descriptors that the device wishes to publish to the
//! host.
//!
//! This function must be called by any application which wishes to operate
//! as a USB device.  It initializes the USB device control driver for the
//! given controller and saves the device information for future use.  Prior to
//! returning from this function, the device is connected to the USB bus.
//! Following return, the caller can expect to receive a callback to the
//! supplied <tt>pfnResetHandler</tt> function when a host connects to the
//! device.
//!
//! The device information structure passed in \e psDevice must remain
//! unchanged between this call and any matching call to USBDCDTerm() since
//! it is not copied by the USB library.
//!
//! The USBStackModeSet() function can be called with USB_MODE_FORCE_DEVICE in
//! order to cause the USB library to force the USB operating mode to a device
//! controller.  This allows the application to used the USBVBUS and USBID pins
//! as GPIOs on devices that support forcing OTG to operate as a device only
//! controller.  By default the USB library will assume that the USBVBUS and
//! USBID pins are configured as USB pins and not GPIOs.
//!
//! \note Forcing of the USB controller mode feature is not available on all
//! Stellaris microcontrollers.  Consult the data sheet for the microcontroller
//! that the application is using to determine if this feature is available.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDInit(unsigned long ulIndex, tDeviceInfo *psDevice)
{
    const tConfigHeader *psHdr;
    const tConfigDescriptor *psDesc;

    //
    // Check the arguments.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice != 0);

    //
    // Should not call this if the stack is in host mode.
    //
    ASSERT(g_eUSBMode != USB_MODE_HOST);
    ASSERT(g_eUSBMode != USB_MODE_FORCE_HOST);

    //
    // Initialize a couple of fields in the device state structure.
    //
    g_psUSBDevice[0].ulConfiguration = DEFAULT_CONFIG_ID;
    g_psUSBDevice[0].ulDefaultConfiguration = DEFAULT_CONFIG_ID;

    //
    // Remember the device information pointer.
    //
    g_psUSBDevice[0].psInfo = psDevice;
    g_psUSBDevice[0].pvInstance = psDevice->pvInstance;
    g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;

    //
    // Default to device mode if no mode was set.
    //
    if(g_eUSBMode == USB_MODE_NONE)
    {
        g_eUSBMode = USB_MODE_DEVICE;
    }

    //
    // Only do hardware update if the stack is in not in OTG mode.
    //
    if(g_eUSBMode != USB_MODE_OTG)
    {
        //
        // Reset the USB controller.
        //
        MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);

        //
        // Enable Clocking to the USB controller.
        //
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

        //
        // Turn on USB Phy clock.
        //
        MAP_SysCtlUSBPLLEnable();

        //
        // If the application is requesting OTG or DEVICE mode then set the mode
        // detection to OTG.  If the mode was device then the rest of the library
        // should make sure that no OTG signaling actually occurs.
        //
        if((g_eUSBMode == USB_MODE_DEVICE) || (g_eUSBMode == USB_MODE_OTG))
        {
            //
            // Switch to OTG mode to detect VBUS changes.
            //
            MAP_USBOTGMode(USB0_BASE);
        }
        else
        {
            //
            // Force device mode on devices that support forcing mode.
            //
            MAP_USBDevMode(USB0_BASE);
        }

        //
        // In all other cases, set the mode to device this function should not
        // be called in OTG mode.
        //
        g_eUSBMode = USB_MODE_DEVICE;
    }

    //
    // Initialize the USB tick module.
    //
    InternalUSBTickInit();

    //
    // Get a pointer to the default configuration descriptor.
    //
    psHdr = psDevice->ppConfigDescriptors[
                g_psUSBDevice[0].ulDefaultConfiguration - 1];
    psDesc = (const tConfigDescriptor *)(psHdr->psSections[0]->pucData);

    //
    // Default to the state where remote wake up is disabled.
    //
    g_psUSBDevice[0].ucStatus = 0;
    g_psUSBDevice[0].bRemoteWakeup = false;

    //
    // Determine the self- or bus-powered state based on the flags the
    // user provided.
    //
    g_psUSBDevice[0].bPwrSrcSet = false;

    if((psDesc->bmAttributes & USB_CONF_ATTR_PWR_M) == USB_CONF_ATTR_SELF_PWR)
    {
        g_psUSBDevice[0].ucStatus |= USB_STATUS_SELF_PWR;
    }
    else
    {
        g_psUSBDevice[0].ucStatus &= ~USB_STATUS_SELF_PWR;
    }

    //
    // Only do hardware update if the stack is not in OTG mode.
    //
    if(g_eUSBMode != USB_MODE_OTG)
    {
        //
        // Get the current interrupt status.to clear all pending USB interrupts.
        //
        MAP_USBIntStatusControl(USB0_BASE);
        MAP_USBIntStatusEndpoint(USB0_BASE);

        //
        // Enable USB Interrupts.
        //
        MAP_USBIntEnableControl(USB0_BASE, USB_INTCTRL_RESET |
                                           USB_INTCTRL_DISCONNECT |
                                           USB_INTCTRL_RESUME |
                                           USB_INTCTRL_SUSPEND |
                                           USB_INTCTRL_SOF);
        MAP_USBIntEnableEndpoint(USB0_BASE, USB_INTEP_ALL);

        //
        // Make sure we disconnect from the host for a while.  This ensures
        // that the host will enumerate us even if we were previously
        // connected to the bus.
        //
        MAP_USBDevDisconnect(USB0_BASE);

        //
        // Wait about 100mS.
        //
        OS_DELAY(MAP_SysCtlClockGet() / 30);

        //
        // Attach the device using the soft connect.
        //
        MAP_USBDevConnect(USB0_BASE);

        //
        // Enable the USB interrupt.
        //
        OS_INT_ENABLE(INT_USB0);
    }
}

//*****************************************************************************
//
//! Free the USB library device control driver for a given hardware controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! freed.
//!
//! This function should be called by an application if it no longer requires
//! the use of a given USB controller to support its operation as a USB device.
//! It frees the controller for use by another client.
//!
//! It is the caller's responsibility to remove its device from the USB bus
//! prior to calling this function.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDTerm(unsigned long ulIndex)
{
    //
    // Check the arguments.
    //
    ASSERT(ulIndex == 0);

    //
    // Disable the USB interrupts.
    //
    OS_INT_DISABLE(INT_USB0);

    //
    // Reset the tick handlers so that they can be reconfigured when and if
    // USBDCDInit() is called.
    //
    InternalUSBTickReset();

    g_psUSBDevice[0].psInfo = (tDeviceInfo *)0;
    g_psUSBDevice[0].pvInstance = 0;

    MAP_USBIntDisableControl(USB0_BASE, USB_INTCTRL_ALL);
    MAP_USBIntDisableEndpoint(USB0_BASE, USB_INTEP_ALL);

    //
    // Detach the device using the soft connect.
    //
    MAP_USBDevDisconnect(USB0_BASE);

    //
    // Clear any pending interrupts.
    //
    MAP_USBIntStatusControl(USB0_BASE);
    MAP_USBIntStatusEndpoint(USB0_BASE);

    //
    // Turn off USB Phy clock.
    //
    MAP_SysCtlUSBPLLDisable();

    //
    // Disable the USB peripheral
    //
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_USB0);
}

//*****************************************************************************
//
//! This function starts the request for data from the host on endpoint zero.
//!
//! \param ulIndex is the index of the USB controller from which the data
//! is being requested.
//! \param pucData is a pointer to the buffer to fill with data from the USB
//! host.
//! \param ulSize is the size of the buffer or data to return from the USB
//! host.
//!
//! This function handles retrieving data from the host when a custom command
//! has been issued on endpoint zero.  If the application needs notification
//! when the data has been received,
//! <tt>tDeviceInfo.sCallbacks.pfnDataReceived</tt> should contain valid
//! function pointer.  In nearly all cases this is necessary because the caller
//! of this function would likely need to know that the data requested was
//! received.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDRequestDataEP0(unsigned long ulIndex, unsigned char *pucData,
                     unsigned long ulSize)
{
    ASSERT(ulIndex == 0);

    //
    // Enter the RX state on end point 0.
    //
    g_psUSBDevice[0].eEP0State = USB_STATE_RX;

    //
    // Save the pointer to the data.
    //
    g_psUSBDevice[0].pEP0Data = pucData;

    //
    // Location to save the current number of bytes received.
    //
    g_psUSBDevice[0].ulOUTDataSize = ulSize;

    //
    // Bytes remaining to be received.
    //
    g_psUSBDevice[0].ulEP0DataRemain = ulSize;
}

//*****************************************************************************
//
//! This function requests transfer of data to the host on endpoint zero.
//!
//! \param ulIndex is the index of the USB controller which is to be used to
//! send the data.
//! \param pucData is a pointer to the buffer to send via endpoint zero.
//! \param ulSize is the amount of data to send in bytes.
//!
//! This function handles sending data to the host when a custom command is
//! issued or non-standard descriptor has been requested on endpoint zero.  If
//! the application needs notification when this is complete,
//! <tt>tDeviceInfo.sCallbacks.pfnDataSent</tt> should contain a valid function
//! pointer.  This callback could be used to free up the buffer passed into
//! this function in the \e pucData parameter.  The contents of the \e pucData
//! buffer must remain unchanged until the <tt>pfnDataSent</tt> callback is
//! received.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDSendDataEP0(unsigned long ulIndex, unsigned char *pucData,
                  unsigned long ulSize)
{
    ASSERT(ulIndex == 0);

    //
    // Return the externally provided device descriptor.
    //
    g_psUSBDevice[0].pEP0Data = pucData;

    //
    // The size of the device descriptor is in the first byte.
    //
    g_psUSBDevice[0].ulEP0DataRemain = ulSize;

    //
    // Save the total size of the data sent.
    //
    g_psUSBDevice[0].ulOUTDataSize = ulSize;

    //
    // Now in the transmit data state.
    //
    USBDEP0StateTx(0);
}

//*****************************************************************************
//
//! This function sets the default configuration for the device.
//!
//! \param ulIndex is the index of the USB controller whose default
//! configuration is to be set.
//! \param ulDefaultConfig is the configuration identifier (byte 6 of the
//! standard configuration descriptor) which is to be presented to the host
//! as the default configuration in cases where the configuration descriptor is
//! queried prior to any specific configuration being set.
//!
//! This function allows a device to override the default configuration
//! descriptor that will be returned to a host whenever it is queried prior
//! to a specific configuration having been set.  The parameter passed must
//! equal one of the configuration identifiers found in the
//! <tt>ppConfigDescriptors</tt> array for the device.
//!
//! If this function is not called, the USB library will return the first
//! configuration in the <tt>ppConfigDescriptors</tt> array as the default
//! configuration.
//!
//! \note The USB device stack assumes that the configuration IDs (byte 6 of
//! the configuration descriptor, <tt>bConfigurationValue</tt>) stored within
//! the configuration descriptor array, <tt>ppConfigDescriptors</tt>,
//! are equal to the array index + 1.  In other words, the first entry in the
//! array must contain a descriptor with <tt>bConfigurationValue</tt> 1, the
//! second must have <tt>bConfigurationValue</tt> 2 and so on.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDSetDefaultConfiguration(unsigned long ulIndex,
                              unsigned long ulDefaultConfig)
{
    ASSERT(ulIndex == 0);

    g_psUSBDevice[0].ulDefaultConfiguration = ulDefaultConfig;
}

//*****************************************************************************
//
//! This function generates a stall condition on endpoint zero.
//!
//! \param ulIndex is the index of the USB controller whose endpoint zero is to
//! be stalled.
//!
//! This function is typically called to signal an error condition to the host
//! when an unsupported request is received by the device.  It should be
//! called from within the callback itself (in interrupt context) and not
//! deferred until later since it affects the operation of the endpoint zero
//! state machine in the USB library.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDStallEP0(unsigned long ulIndex)
{
    ASSERT(ulIndex == 0);

    //
    // Stall the endpoint in question.
    //
    MAP_USBDevEndpointStall(USB0_BASE, USB_EP_0, USB_EP_DEV_OUT);

    //
    // Enter the stalled state.
    //
    g_psUSBDevice[0].eEP0State = USB_STATE_STALL;
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the library.
//!
//! \param ulIndex is the index of the USB controller whose device power
//! status is being reported.
//! \param ucPower indicates the current power status, either \b
//! USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//!
//! Applications which support switching between bus- or self-powered
//! operation should call this function whenever the power source changes
//! to indicate the current power status to the USB library.  This information
//! is required by the library to allow correct responses to be provided when
//! the host requests status from the device.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDPowerStatusSet(unsigned long ulIndex, unsigned char ucPower)
{
    //
    // Check for valid parameters.
    //
    ASSERT((ucPower == USB_STATUS_BUS_PWR) ||
           (ucPower == USB_STATUS_SELF_PWR));
    ASSERT(ulIndex == 0);

    //
    // Update the device status with the new power status flag.
    //
    g_psUSBDevice[0].bPwrSrcSet = true;
    g_psUSBDevice[0].ucStatus &= ~USB_STATUS_PWR_M;
    g_psUSBDevice[0].ucStatus |= ucPower;
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param ulIndex is the index of the USB controller that will request
//! a bus wake up.
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
USBDCDRemoteWakeupRequest(unsigned long ulIndex)
{
    //
    // Check for parameter validity.
    //
    ASSERT(ulIndex == 0);

    //
    // Is remote wake up signaling currently enabled?
    //
    if(g_psUSBDevice[0].ucStatus & USB_STATUS_REMOTE_WAKE)
    {
        //
        // The host has not disabled remote wake up. Are we still in the
        // middle of a previous wake up sequence?
        //
        if(!g_psUSBDevice[0].bRemoteWakeup)
        {
            //
            // No - we are not in the middle of a wake up sequence so start
            // one here.
            //
            g_psUSBDevice[0].ucRemoteWakeupCount = 0;
            g_psUSBDevice[0].bRemoteWakeup = true;
            MAP_USBHostResume(USB0_BASE, true);
            return(true);
        }
    }

    //
    // If we drop through to here, signaling was not initiated so return
    // false.
    return(false);
}

//*****************************************************************************
//
// Internal Functions, not to be called by applications
//
//*****************************************************************************

//*****************************************************************************
//
// This internal function is called on the SOF interrupt to process any
// outstanding remote wake up requests.
//
// \return None.
//
//*****************************************************************************
void
USBDeviceResumeTickHandler(tDeviceInstance *psDevInst)
{
    if(g_psUSBDevice[0].bRemoteWakeup)
    {
        //
        // Increment the millisecond counter we use to time the resume
        // signaling.
        //
        g_psUSBDevice[0].ucRemoteWakeupCount++;

        //
        // Have we reached the 10mS mark? If so, we need to turn the signaling
        // off again.
        //
        if(g_psUSBDevice[0].ucRemoteWakeupCount == REMOTE_WAKEUP_PULSE_MS)
        {
            MAP_USBHostResume(USB0_BASE, false);
        }

        //
        // Have we reached the point at which we can tell the client that the
        // bus has resumed? The controller doesn't give us an interrupt if we
        // initiated the wake up signaling so we just wait until 20mS have
        // passed then tell the client all is well.
        //
        if(g_psUSBDevice[0].ucRemoteWakeupCount == REMOTE_WAKEUP_READY_MS)
        {
            //
            // We are now finished with the remote wake up signaling.
            //
            g_psUSBDevice[0].bRemoteWakeup = false;

            //
            // If the client has registered a resume callback, call it.  In the
            // case of a remote wake up request, we do not get a resume
            // interrupt from the controller so we need to fake it here.
            //
            if(g_psUSBDevice[0].psInfo->sCallbacks.pfnResumeHandler)
            {
                g_psUSBDevice[0].psInfo->sCallbacks.pfnResumeHandler(
                    g_psUSBDevice[0].pvInstance);
            }
        }
    }
}

//*****************************************************************************
//
// This internal function reads a request data packet and dispatches it to
// either a standard request handler or the registered device request
// callback depending upon the request type.
//
// \return None.
//
//*****************************************************************************
static void
USBDReadAndDispatchRequest(unsigned long ulIndex)
{
    unsigned long ulSize;
    tUSBRequest *pRequest;

    //
    // Cast the buffer to a request structure.
    //
    pRequest = (tUSBRequest *)g_pucDataBufferIn;

    //
    // Set the buffer size.
    //
    ulSize = EP0_MAX_PACKET_SIZE;

    //
    // Get the data from the USB controller end point 0.
    //
    MAP_USBEndpointDataGet(USB0_BASE,
                           USB_EP_0,
                           g_pucDataBufferIn,
                           &ulSize);

    //
    // If there was a null setup packet then just return.
    //
    if(!ulSize)
    {
        return;
    }

    //
    // See if this is a standard request or not.
    //
    if((pRequest->bmRequestType & USB_RTYPE_TYPE_M) != USB_RTYPE_STANDARD)
    {
        //
        // Since this is not a standard request, see if there is
        // an external handler present.
        //
        if(g_psUSBDevice[0].psInfo->sCallbacks.pfnRequestHandler)
        {
            g_psUSBDevice[0].psInfo->sCallbacks.pfnRequestHandler(
                    g_psUSBDevice[0].pvInstance, pRequest);
        }
        else
        {
            //
            // If there is no handler then stall this request.
            //
            USBDCDStallEP0(0);
        }
    }
    else
    {
        //
        // Assure that the jump table is not out of bounds.
        //
        if((pRequest->bRequest <
           (sizeof(g_psUSBDStdRequests) / sizeof(tStdRequest))) &&
           (g_psUSBDStdRequests[pRequest->bRequest] != 0))
        {
            //
            // Jump table to the appropriate handler.
            //
            g_psUSBDStdRequests[pRequest->bRequest](&g_psUSBDevice[0],
                                                    pRequest);
        }
        else
        {
            //
            // If there is no handler then stall this request.
            //
            USBDCDStallEP0(0);
        }
    }
}

//*****************************************************************************
//
// This is interrupt handler for endpoint zero.
//
// This function handles all interrupts on endpoint zero in order to maintain
// the state needed for the control endpoint on endpoint zero.  In order to
// successfully enumerate and handle all USB standard requests, all requests
// on endpoint zero must pass through this function.  The endpoint has the
// following states: \b USB_STATE_IDLE, \b USB_STATE_TX, \b USB_STATE_RX,
// \b USB_STATE_STALL, and \b USB_STATE_STATUS.  In the \b USB_STATE_IDLE
// state the USB controller has not received the start of a request, and once
// it does receive the data for the request it will either enter the
// \b USB_STATE_TX, \b USB_STATE_RX, or \b USB_STATE_STALL depending on the
// command.  If the controller enters the \b USB_STATE_TX or \b USB_STATE_RX
// then once all data has been sent or received, it must pass through the
// \b USB_STATE_STATUS state to allow the host to acknowledge completion of
// the request.  The \b USB_STATE_STALL is entered from \b USB_STATE_IDLE in
// the event that the USB request was not valid.  Both the \b USB_STATE_STALL
// and \b USB_STATE_STATUS are transitional states that return to the
// \b USB_STATE_IDLE state.
//
// \return None.
//
// USB_STATE_IDLE -*--> USB_STATE_TX -*-> USB_STATE_STATUS -*->USB_STATE_IDLE
//                 |                  |                     |
//                 |--> USB_STATE_RX -                      |
//                 |                                        |
//                 |--> USB_STATE_STALL ---------->---------
//
//  ----------------------------------------------------------------
// | Current State       | State 0           | State 1              |
// | --------------------|-------------------|----------------------
// | USB_STATE_IDLE      | USB_STATE_TX/RX   | USB_STATE_STALL      |
// | USB_STATE_TX        | USB_STATE_STATUS  |                      |
// | USB_STATE_RX        | USB_STATE_STATUS  |                      |
// | USB_STATE_STATUS    | USB_STATE_IDLE    |                      |
// | USB_STATE_STALL     | USB_STATE_IDLE    |                      |
//  ----------------------------------------------------------------
//
//*****************************************************************************
void
USBDeviceEnumHandler(tDeviceInstance *pDevInstance)
{
    unsigned long ulEPStatus;

    //
    // Get the end point 0 status.
    //
    ulEPStatus = MAP_USBEndpointStatus(USB0_BASE, USB_EP_0);

    switch(pDevInstance->eEP0State)
    {
        //
        // Handle the status state, this is a transitory state from
        // USB_STATE_TX or USB_STATE_RX back to USB_STATE_IDLE.
        //
        case USB_STATE_STATUS:
        {
            //
            // Just go back to the idle state.
            //
            pDevInstance->eEP0State = USB_STATE_IDLE;

            //
            // If there is a pending address change then set the address.
            //
            if(pDevInstance->ulDevAddress & DEV_ADDR_PENDING)
            {
                //
                // Clear the pending address change and set the address.
                //
                pDevInstance->ulDevAddress &= ~DEV_ADDR_PENDING;
                MAP_USBDevAddrSet(USB0_BASE, pDevInstance->ulDevAddress);
            }

            //
            // If a new packet is already pending, we need to read it
            // and handle whatever request it contains.
            //
            if(ulEPStatus & USB_DEV_EP0_OUT_PKTRDY)
            {
                //
                // Process the newly arrived packet.
                //
                USBDReadAndDispatchRequest(0);
            }
            break;
        }

        //
        // In the IDLE state the code is waiting to receive data from the host.
        //
        case USB_STATE_IDLE:
        {
            //
            // Is there a packet waiting for us?
            //
            if(ulEPStatus & USB_DEV_EP0_OUT_PKTRDY)
            {
                //
                // Yes - process it.
                //
                USBDReadAndDispatchRequest(0);
            }
            break;
        }

        //
        // Data is still being sent to the host so handle this in the
        // EP0StateTx() function.
        //
        case USB_STATE_TX:
        {
            USBDEP0StateTx(0);
            break;
        }

        //
        // We are still in the middle of sending the configuration descriptor
        // so handle this in the EP0StateTxConfig() function.
        //
        case USB_STATE_TX_CONFIG:
        {
            USBDEP0StateTxConfig(0);
            break;
        }

        //
        // Handle the receive state for commands that are receiving data on
        // endpoint zero.
        //
        case USB_STATE_RX:
        {
            unsigned long ulDataSize;

            //
            // Set the number of bytes to get out of this next packet.
            //
            if(pDevInstance->ulEP0DataRemain > EP0_MAX_PACKET_SIZE)
            {
                //
                // Don't send more than EP0_MAX_PACKET_SIZE bytes.
                //
                ulDataSize = EP0_MAX_PACKET_SIZE;
            }
            else
            {
                //
                // There was space so send the remaining bytes.
                //
                ulDataSize = pDevInstance->ulEP0DataRemain;
            }

            //
            // Get the data from the USB controller end point 0.
            //
            MAP_USBEndpointDataGet(USB0_BASE, USB_EP_0, pDevInstance->pEP0Data,
                                   &ulDataSize);

            //
            // If there we not more that EP0_MAX_PACKET_SIZE or more bytes
            // remaining then this transfer is complete.  If there were exactly
            // EP0_MAX_PACKET_SIZE remaining then there still needs to be
            // null packet sent before this is complete.
            //
            if(pDevInstance->ulEP0DataRemain < EP0_MAX_PACKET_SIZE)
            {
                //
                // Need to ACK the data on end point 0 in this case and set the
                // data end as this is the last of the data.
                //
                MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

                //
                // Return to the idle state.
                //
                pDevInstance->eEP0State =  USB_STATE_STATUS;

                //
                // If there is a receive callback then call it.
                //
                if((pDevInstance->psInfo->sCallbacks.pfnDataReceived) &&
                   (pDevInstance->ulOUTDataSize != 0))
                {
                    //
                    // Call the custom receive handler to handle the data
                    // that was received.
                    //
                    pDevInstance->psInfo->sCallbacks.pfnDataReceived(
                        pDevInstance->pvInstance,
                        pDevInstance->ulOUTDataSize);

                    //
                    // Indicate that there is no longer any data being waited
                    // on.
                    //
                    pDevInstance->ulOUTDataSize = 0;
                }
            }
            else
            {
                //
                // Need to ACK the data on end point 0 in this case
                // without setting data end because more data is coming.
                //
                MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);
            }

            //
            // Advance the pointer.
            //
            pDevInstance->pEP0Data += ulDataSize;

            //
            // Decrement the number of bytes that are being waited on.
            //
            pDevInstance->ulEP0DataRemain -= ulDataSize;

            break;
        }
        //
        // The device stalled endpoint zero so check if the stall needs to be
        // cleared once it has been successfully sent.
        //
        case USB_STATE_STALL:
        {
            //
            // If we sent a stall then acknowledge this interrupt.
            //
            if(ulEPStatus & USB_DEV_EP0_SENT_STALL)
            {
                //
                // Clear the Setup End condition.
                //
                MAP_USBDevEndpointStatusClear(USB0_BASE, USB_EP_0,
                                              USB_DEV_EP0_SENT_STALL);

                //
                // Reset the global end point 0 state to IDLE.
                //
                pDevInstance->eEP0State = USB_STATE_IDLE;

            }
            break;
        }
        //
        // Halt on an unknown state, but only in DEBUG mode builds.
        //
        default:
        {
            ASSERT(0);
        }
    }
}

//*****************************************************************************
//
// This function handles bus reset notifications.
//
// This function is called from the low level USB interrupt handler whenever
// a bus reset is detected.  It performs tidy-up as required and resets the
// configuration back to defaults in preparation for descriptor queries from
// the host.
//
// \return None.
//
//*****************************************************************************
void
USBDeviceEnumResetHandler(tDeviceInstance *pDevInstance)
{
    unsigned long ulLoop;

    //
    // Disable remote wake up signaling (as per USB 2.0 spec 9.1.1.6).
    //
    pDevInstance->ucStatus &= ~USB_STATUS_REMOTE_WAKE;
    pDevInstance->bRemoteWakeup = false;

    //
    // Call the device dependent code to indicate a bus reset has occurred.
    //
    if(pDevInstance->psInfo->sCallbacks.pfnResetHandler)
    {
        pDevInstance->psInfo->sCallbacks.pfnResetHandler(
            pDevInstance->pvInstance);
    }

    //
    // Reset the default configuration identifier and alternate function
    // selections.
    //
    pDevInstance->ulConfiguration = pDevInstance->ulDefaultConfiguration;

    for(ulLoop = 0; ulLoop < USB_MAX_INTERFACES_PER_DEVICE; ulLoop++)
    {
        pDevInstance->pucAltSetting[ulLoop] = (unsigned char)0;
    }
}

//*****************************************************************************
//
// This function handles the GET_STATUS standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the request type and endpoint number if endpoint
// status is requested.
//
// This function handles responses to a Get Status request from the host
// controller.  A status request can be for the device, an interface or an
// endpoint.  If any other type of request is made this function will cause
// a stall condition to indicate that the command is not supported.  The
// \e pUSBRequest structure holds the type of the request in the
// bmRequestType field.  If the type indicates that this is a request for an
// endpoint's status, then the wIndex field holds the endpoint number.
//
// \return None.
//
//*****************************************************************************
static void
USBDGetStatus(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned short usData;
    tDeviceInstance *psUSBControl;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // Determine what type of status was requested.
    //
    switch(pUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        //
        // This was a Device Status request.
        //
        case USB_RTYPE_DEVICE:
        {
            //
            // Return the current status for the device.
            //
            usData = (unsigned short)psUSBControl->ucStatus;

            break;
        }

        //
        // This was a Interface status request.
        //
        case USB_RTYPE_INTERFACE:
        {
            //
            // Interface status always returns 0.
            //
            usData = (unsigned short)0;

            break;
        }

        //
        // This was an endpoint status request.
        //
        case USB_RTYPE_ENDPOINT:
        {
            unsigned short usIndex;
            unsigned long ulDir;

            //
            // Which endpoint are we dealing with?
            //
            usIndex = pUSBRequest->wIndex & USB_REQ_EP_NUM_M;

            //
            // Check if this was a valid endpoint request.
            //
            if((usIndex == 0) || (usIndex >= NUM_USB_EP))
            {
                USBDCDStallEP0(0);
                return;
            }
            else
            {
                //
                // Are we dealing with an IN or OUT endpoint?
                //
                ulDir = ((pUSBRequest->wIndex & USB_REQ_EP_DIR_M) ==
                         USB_REQ_EP_DIR_IN) ? HALT_EP_IN : HALT_EP_OUT;

                //
                // Get the current halt status for this endpoint.
                //
                usData =
                      (unsigned short)psUSBControl->ucHalt[ulDir][usIndex - 1];
            }
            break;
        }

        //
        // This was an unknown request.
        //
        default:
        {
            //
            // Anything else causes a stall condition to indicate that the
            // command was not supported.
            //
            USBDCDStallEP0(0);
            return;
        }
    }

    //
    // Send the two byte status response.
    //
    psUSBControl->ulEP0DataRemain = 2;
    psUSBControl->pEP0Data = (unsigned char *)&usData;

    //
    // Send the response.
    //
    USBDEP0StateTx(0);
}

//*****************************************************************************
//
// This function handles the CLEAR_FEATURE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the options for the Clear Feature USB request.
//
// This function handles device or endpoint clear feature requests.  The
// \e pUSBRequest structure holds the type of the request in the bmRequestType
// field and the feature is held in the wValue field.  The device can only
// clear the Remote Wake feature.  This device request should only be made if
// the descriptor indicates that Remote Wake is implemented by the device.
// Endpoints can only clear a halt on a given endpoint.  If any other
// requests are made, then the device will stall the request to indicate to
// the host that the command was not supported.
//
// \return None.
//
//*****************************************************************************
static void
USBDClearFeature(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDeviceInstance *psUSBControl;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

    //
    // Determine what type of status was requested.
    //
    switch(pUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        //
        // This is a clear feature request at the device level.
        //
        case USB_RTYPE_DEVICE:
        {
            //
            // Only remote wake is can be cleared by this function.
            //
            if(USB_FEATURE_REMOTE_WAKE & pUSBRequest->wValue)
            {
                //
                // Clear the remote wake up state.
                //
                psUSBControl->ucStatus &= ~USB_STATUS_REMOTE_WAKE;
            }
            else
            {
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // This is a clear feature request at the endpoint level.
        //
        case USB_RTYPE_ENDPOINT:
        {
            unsigned long ulDir;
            unsigned short usIndex;

            //
            // Which endpoint are we dealing with?
            //
            usIndex = pUSBRequest->wIndex & USB_REQ_EP_NUM_M;

            //
            // Not a valid endpoint.
            //
            if((usIndex == 0) || (usIndex > NUM_USB_EP))
            {
                USBDCDStallEP0(0);
            }
            else
            {
                //
                // Only the halt feature is supported.
                //
                if(USB_FEATURE_EP_HALT == pUSBRequest->wValue)
                {
                    //
                    // Are we dealing with an IN or OUT endpoint?
                    //
                    ulDir = ((pUSBRequest->wIndex & USB_REQ_EP_DIR_M) ==
                             USB_REQ_EP_DIR_IN) ? HALT_EP_IN : HALT_EP_OUT;

                    //
                    // Clear the halt condition on this endpoint.
                    //
                    psUSBControl->ucHalt[ulDir][usIndex - 1] = 0;

                    if(ulDir == HALT_EP_IN)
                    {
                        MAP_USBDevEndpointStallClear(USB0_BASE,
                                                     INDEX_TO_USB_EP(usIndex),
                                                     USB_EP_DEV_IN);
                    }
                    else
                    {
                        MAP_USBDevEndpointStallClear(USB0_BASE,
                                                     INDEX_TO_USB_EP(usIndex),
                                                     USB_EP_DEV_OUT);
                    }
                }
                else
                {
                    //
                    // If any other feature is requested, this is an error.
                    //
                    USBDCDStallEP0(0);
                    return;
                }
            }
            break;
        }

        //
        // This is an unknown request.
        //
        default:
        {
            USBDCDStallEP0(0);
            return;
        }
    }
}

//*****************************************************************************
//
// This function handles the SET_FEATURE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the feature in the wValue field of the USB
// request.
//
// This function handles device or endpoint set feature requests.  The
// \e pUSBRequest structure holds the type of the request in the bmRequestType
// field and the feature is held in the wValue field.  The device can only
// set the Remote Wake feature.  This device request should only be made if the
// descriptor indicates that Remote Wake is implemented by the device.
// Endpoint requests can only issue a halt on a given endpoint.  If any other
// requests are made, then the device will stall the request to indicate to the
// host that the command was not supported.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetFeature(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDeviceInstance *psUSBControl;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

    //
    // Determine what type of status was requested.
    //
    switch(pUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        //
        // This is a set feature request at the device level.
        //
        case USB_RTYPE_DEVICE:
        {
            //
            // Only remote wake is the only feature that can be set by this
            // function.
            //
            if(USB_FEATURE_REMOTE_WAKE & pUSBRequest->wValue)
            {
                //
                // Set the remote wake up state.
                //
                psUSBControl->ucStatus |= USB_STATUS_REMOTE_WAKE;
            }
            else
            {
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // This is a set feature request at the endpoint level.
        //
        case USB_RTYPE_ENDPOINT:
        {
            unsigned short usIndex;
            unsigned long ulDir;

            //
            // Which endpoint are we dealing with?
            //
            usIndex = pUSBRequest->wIndex & USB_REQ_EP_NUM_M;

            //
            // Not a valid endpoint?
            //
            if((usIndex == 0) || (usIndex >= NUM_USB_EP))
            {
                USBDCDStallEP0(0);
            }
            else
            {
                //
                // Only the Halt feature can be set.
                //
                if(USB_FEATURE_EP_HALT == pUSBRequest->wValue)
                {
                    //
                    // Are we dealing with an IN or OUT endpoint?
                    //
                    ulDir = ((pUSBRequest->wIndex & USB_REQ_EP_DIR_M) ==
                             USB_REQ_EP_DIR_IN) ? HALT_EP_IN : HALT_EP_OUT;

                    //
                    // Clear the halt condition on this endpoint.
                    //
                    psUSBControl->ucHalt[ulDir][usIndex - 1] = 1;
                }
                else
                {
                    //
                    // No other requests are supported.
                    //
                    USBDCDStallEP0(0);
                    return;
                }
            }
            break;
        }

        //
        // This is an unknown request.
        //
        default:
        {
            USBDCDStallEP0(0);
            return;
        }
    }
}

//*****************************************************************************
//
// This function handles the SET_ADDRESS standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the new address to use in the wValue field of the
// USB request.
//
// This function is called to handle the change of address request from the
// host controller.  This can only start the sequence as the host must
// acknowledge that the device has changed address.  Thus this function sets
// the address change as pending until the status phase of the request has
// been completed successfully.  This prevents the devices address from
// changing and not properly responding to the status phase.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetAddress(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDeviceInstance *psUSBControl;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

    //
    // Save the device address as we cannot change address until the status
    // phase is complete.
    //
    psUSBControl->ulDevAddress = pUSBRequest->wValue | DEV_ADDR_PENDING;

    //
    // Transition directly to the status state since there is no data phase
    // for this request.
    //
    psUSBControl->eEP0State = USB_STATE_STATUS;
}

//*****************************************************************************
//
// This function handles the GET_DESCRIPTOR standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This function will return most of the descriptors requested by the host
// controller.  The descriptor specified by \e
// pvInstance->psInfo->pDeviceDescriptor will be returned when the device
// descriptor is requested.  If a request for a specific configuration
// descriptor is made, then the appropriate descriptor from the \e
// g_pConfigDescriptors will be returned.  When a request for a string
// descriptor is made, the appropriate string from the
// \e pvInstance->psInfo->pStringDescriptors will be returned.  If the \e
// pvInstance->psInfo->sCallbacks.GetDescriptor is specified it will be
// called to handle the request.  In this case it must call the
// USBDCDSendDataEP0() function to send the data to the host controller.  If
// the callback is not specified, and the descriptor request is not for a
// device, configuration, or string descriptor then this function will stall
// the request to indicate that the request was not supported by the device.
//
// \return None.
//
//*****************************************************************************
static void
USBDGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tBoolean bConfig;
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->psInfo;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // Assume we are not sending the configuration descriptor until we
    // determine otherwise.
    //
    bConfig = false;

    //
    // Which descriptor are we being asked for?
    //
    switch(pUSBRequest->wValue >> 8)
    {
        //
        // This request was for a device descriptor.
        //
        case USB_DTYPE_DEVICE:
        {
            //
            // Return the externally provided device descriptor.
            //
            psUSBControl->pEP0Data =
                (unsigned char *)psDevice->pDeviceDescriptor;

            //
            // The size of the device descriptor is in the first byte.
            //
            psUSBControl->ulEP0DataRemain = psDevice->pDeviceDescriptor[0];

            break;
        }

        //
        // This request was for a configuration descriptor.
        //
        case USB_DTYPE_CONFIGURATION:
        {
            const tConfigHeader *psConfig;
            const tDeviceDescriptor *psDeviceDesc;
            unsigned char ucIndex;

            //
            // Which configuration are we being asked for?
            //
            ucIndex = (unsigned char)(pUSBRequest->wValue & 0xFF);

            //
            // Is this valid?
            //
            psDeviceDesc =
                (const tDeviceDescriptor *)psDevice->pDeviceDescriptor;

            if(ucIndex >= psDeviceDesc->bNumConfigurations)
            {
                //
                // This is an invalid configuration index.  Stall EP0 to
                // indicate a request error.
                //
                USBDCDStallEP0(0);
                psUSBControl->pEP0Data = 0;
                psUSBControl->ulEP0DataRemain = 0;
            }
            else
            {
                //
                // Return the externally specified configuration descriptor.
                //
                psConfig = psDevice->ppConfigDescriptors[ucIndex];

                //
                // Start by sending data from the beginning of the first
                // descriptor.
                //
                psUSBControl->ucConfigSection = 0;
                psUSBControl->ucSectionOffset = 0;
                psUSBControl->pEP0Data = (unsigned char *)
                                          psConfig->psSections[0]->pucData;

                //
                // Determine the total size of the configuration descriptor
                // by counting the sizes of the sections comprising it.
                //
                psUSBControl->ulEP0DataRemain =
                                            USBDCDConfigDescGetSize(psConfig);

                //
                // Remember that we need to send the configuration descriptor
                // and which descriptor we need to send.
                //
                psUSBControl->ucConfigIndex = ucIndex;

                bConfig = true;
            }
            break;
        }

        //
        // This request was for a string descriptor.
        //
        case USB_DTYPE_STRING:
        {
            long lIndex;

            //
            // Determine the correct descriptor index based on the requested
            // language ID and index.
            //
            lIndex = USBDStringIndexFromRequest(pUSBRequest->wIndex,
                                                pUSBRequest->wValue & 0xFF);

            //
            // If the mapping function returned -1 then stall the request to
            // indicate that the request was not valid.
            //
            if(lIndex == -1)
            {
                USBDCDStallEP0(0);
                break;
            }

            //
            // Return the externally specified configuration descriptor.
            //
            psUSBControl->pEP0Data =
                (unsigned char *)psDevice->ppStringDescriptors[lIndex];

            //
            // The total size of a string descriptor is in byte 0.
            //
            psUSBControl->ulEP0DataRemain =
                psDevice->ppStringDescriptors[lIndex][0];

            break;
        }

        //
        // Any other request is not handled by the default enumeration handler
        // so see if it needs to be passed on to another handler.
        //
        default:
        {
            //
            // If there is a handler for requests that are not handled then
            // call it.
            //
            if(psDevice->sCallbacks.pfnGetDescriptor)
            {
                psDevice->sCallbacks.pfnGetDescriptor(psUSBControl->pvInstance,
                                                      pUSBRequest);
            }
            else
            {
                //
                // Whatever this was this handler does not understand it so
                // just stall the request.
                //
                USBDCDStallEP0(0);
            }

            return;
        }
    }

    //
    // If this request has data to send, then send it.
    //
    if(psUSBControl->pEP0Data)
    {
        //
        // If there is more data to send than is requested then just
        // send the requested amount of data.
        //
        if(psUSBControl->ulEP0DataRemain > pUSBRequest->wLength)
        {
            psUSBControl->ulEP0DataRemain = pUSBRequest->wLength;
        }

        //
        // Now in the transmit data state.  Be careful to call the correct
        // function since we need to handle the configuration descriptor
        // differently from the others.
        //
        if(!bConfig)
        {
            USBDEP0StateTx(0);
        }
        else
        {
            USBDEP0StateTxConfig(0);
        }
    }
}

//*****************************************************************************
//
// This function determines which string descriptor to send to satisfy a
// request for a given index and language.
//
// \param usLang is the requested string language ID.
// \param usIndex is the requested string descriptor index.
//
// When a string descriptor is requested, the host provides a language ID and
// index to identify the string ("give me string number 5 in French").  This
// function maps these two parameters to an index within our device's string
// descriptor array which is arranged as multiple groups of strings with
// one group for each language advertised via string descriptor 0.
//
// We assume that there are an equal number of strings per language and
// that the first descriptor is the language descriptor and use this fact to
// perform the mapping.
//
// \return The index of the string descriptor to return or -1 if the string
// could not be found.
//
//*****************************************************************************
static long
USBDStringIndexFromRequest(unsigned short usLang, unsigned short usIndex)
{
    tString0Descriptor *pLang;
    unsigned long ulNumLangs;
    unsigned long ulNumStringsPerLang;
    unsigned long ulLoop;

    //
    // Make sure we have a string table at all.
    //
    if((g_psUSBDevice[0].psInfo == 0) ||
       (g_psUSBDevice[0].psInfo->ppStringDescriptors == 0))
    {
        return(-1);
    }

    //
    // First look for the trivial case where descriptor 0 is being
    // requested.  This is the special case since descriptor 0 contains the
    // language codes supported by the device.
    //
    if(usIndex == 0)
    {
        return(0);
    }

    //
    // How many languages does this device support?  This is determined by
    // looking at the length of the first descriptor in the string table,
    // subtracting 2 for the header and dividing by two (the size of each
    // language code).
    //
    ulNumLangs = (g_psUSBDevice[0].psInfo->ppStringDescriptors[0][0] - 2) / 2;

    //
    // We assume that the table includes the same number of strings for each
    // supported language.  We know the number of entries in the string table,
    // so how many are there for each language?  This may seem an odd way to
    // do this (why not just have the application tell us in the device info
    // structure?) but it's needed since we didn't want to change the API
    // after the first release which did not support multiple languages.
    //
    ulNumStringsPerLang = ((g_psUSBDevice[0].psInfo->ulNumStringDescriptors - 1) /
                           ulNumLangs);

    //
    // Just to be sure, make sure that the calculation indicates an equal
    // number of strings per language.  We expect the string table to contain
    // (1 + (strings_per_language * languages)) entries.
    //
    if((1 + (ulNumStringsPerLang * ulNumLangs)) !=
       g_psUSBDevice[0].psInfo->ulNumStringDescriptors)
    {
        return(-1);
    }

    //
    // Now determine which language we are looking for.  It is assumed that
    // the order of the groups of strings per language in the table is the
    // same as the order of the language IDs listed in the first descriptor.
    //
    pLang = (tString0Descriptor *)(g_psUSBDevice[0].psInfo->ppStringDescriptors[0]);

    //
    // Look through the supported languages looking for the one we were asked
    // for.
    //
    for(ulLoop = 0; ulLoop < ulNumLangs; ulLoop++)
    {
        //
        // Have we found the requested language?
        //
        if(pLang->wLANGID[ulLoop] == usLang)
        {
            //
            // Yes - calculate the index of the descriptor to send.
            //
            return((ulNumStringsPerLang * ulLoop) + usIndex);
        }
    }

    //
    // If we drop out of the loop, the requested language was not found so
    // return -1 to indicate the error.
    //
    return(-1);
}

//*****************************************************************************
//
// This function handles the SET_DESCRIPTOR standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This function currently is not supported and will respond with a Stall
// to indicate that this command is not supported by the device.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // This function is not handled by default.
    //
    USBDCDStallEP0(0);
}

//*****************************************************************************
//
// This function handles the GET_CONFIGURATION standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This function responds to a host request to return the current
// configuration of the USB device.  The function will send the configuration
// response to the host and return.  This value will either be 0 or the last
// value received from a call to SetConfiguration().
//
// \return None.
//
//*****************************************************************************
static void
USBDGetConfiguration(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned char ucValue;
    tDeviceInstance *psUSBControl;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // If we still have an address pending then the device is still not
    // configured.
    //
    if(psUSBControl->ulDevAddress & DEV_ADDR_PENDING)
    {
        ucValue = 0;
    }
    else
    {
        ucValue = (unsigned char)psUSBControl->ulConfiguration;
    }

    psUSBControl->ulEP0DataRemain = 1;
    psUSBControl->pEP0Data = &ucValue;

    //
    // Send the single byte response.
    //
    USBDEP0StateTx(0);
}

//*****************************************************************************
//
// This function handles the SET_CONFIGURATION standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This function responds to a host request to change the current
// configuration of the USB device.  The actual configuration number is taken
// from the structure passed in via \e pUSBRequest.  This number should be one
// of the configurations that was specified in the descriptors.  If the
// \e ConfigChange callback is specified in \e pvInstance->psInfo->sCallbacks,
// it will be called so that the application can respond to a change in
// configuration.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetConfiguration(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->psInfo;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

    //
    // Cannot set the configuration to one that does not exist so check the
    // enumeration structure to see how many valid configurations are present.
    //
    if(pUSBRequest->wValue > psUSBControl->psInfo->pDeviceDescriptor[17])
    {
        //
        // The passed configuration number is not valid.  Stall the endpoint to
        // signal the error to the host.
        //
        USBDCDStallEP0(0);
    }
    else
    {
        //
        // Save the configuration.
        //
        psUSBControl->ulConfiguration = pUSBRequest->wValue;

        //
        // If passed a configuration other than 0 (which tells us that we are
        // not currently configured), configure the endpoints (other than EP0)
        // appropriately.
        //
        if(psUSBControl->ulConfiguration)
        {
            const tConfigHeader *psHdr;
            const tConfigDescriptor *psDesc;

            //
            // Get a pointer to the configuration descriptor.  This will always
            // be the first section in the current configuration.
            //
            psHdr = psDevice->ppConfigDescriptors[pUSBRequest->wValue - 1];
            psDesc = (const tConfigDescriptor *)(psHdr->psSections[0]->pucData);

            //
            // Remember the new self- or bus-powered state if the user has not
            // already called us to tell us the state to report.
            //
            if(!psUSBControl->bPwrSrcSet)
            {
                if((psDesc->bmAttributes & USB_CONF_ATTR_PWR_M) ==
                    USB_CONF_ATTR_SELF_PWR)
                {
                    psUSBControl->ucStatus |= USB_STATUS_SELF_PWR;
                }
                else
                {
                    psUSBControl->ucStatus &= ~USB_STATUS_SELF_PWR;
                }
            }

            //
            // Configure endpoints for the new configuration.
            //
            USBDeviceConfig(psUSBControl,
                         psDevice->ppConfigDescriptors[pUSBRequest->wValue - 1]);
        }

        //
        // If there is a configuration change callback then call it.
        //
        if(psDevice->sCallbacks.pfnConfigChange)
        {
            psDevice->sCallbacks.pfnConfigChange(
                psUSBControl->pvInstance, psUSBControl->ulConfiguration);
        }
    }
}

//*****************************************************************************
//
// This function handles the GET_INTERFACE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This function is called when the host controller request the current
// interface that is in use by the device.  This simply returns the value set
// by the last call to SetInterface().
//
// \return None.
//
//*****************************************************************************
static void
USBDGetInterface(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned char ucValue;
    tDeviceInstance *psUSBControl;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // If we still have an address pending then the device is still not
    // configured.
    //
    if(psUSBControl->ulDevAddress & DEV_ADDR_PENDING)
    {
        ucValue = (unsigned char)0;
    }
    else
    {
        //
        // Is the interface number valid?
        //
        if(pUSBRequest->wIndex < USB_MAX_INTERFACES_PER_DEVICE)
        {
            //
            // Read the current alternate setting for the required interface.
            //
            ucValue = psUSBControl->pucAltSetting[pUSBRequest->wIndex];
        }
        else
        {
            //
            // An invalid interface number was specified.
            //
            USBDCDStallEP0(0);
            return;
        }
    }

    //
    // Send the single byte response.
    //
    psUSBControl->ulEP0DataRemain = 1;
    psUSBControl->pEP0Data = &ucValue;

    //
    // Send the single byte response.
    //
    USBDEP0StateTx(0);
}

//*****************************************************************************
//
// This function handles the SET_INTERFACE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This function is called when a standard request for changing the interface
// is received from the host controller.  If this is a valid request the
// function will call the function specified by the InterfaceChange in the
// \e pvInstance->psInfo->sCallbacks variable to notify the application that the
// interface has changed and will pass it the new alternate interface number.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetInterface(void *pvInstance, tUSBRequest *pUSBRequest)
{
    const tConfigHeader *psConfig;
    tInterfaceDescriptor *psInterface;
    unsigned long ulLoop;
    unsigned long ulSection;
    unsigned long ulNumInterfaces;
    unsigned char ucInterface;
    tBoolean bRetcode;
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;

    ASSERT(pUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->psInfo;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

    //
    // Use the current configuration.
    //
    psConfig = psDevice->ppConfigDescriptors[psUSBControl->ulConfiguration - 1];

    //
    // How many interfaces are included in the descriptor?
    //
    ulNumInterfaces = USBDCDConfigDescGetNum(psConfig,
                                             USB_DTYPE_INTERFACE);

    //
    // Find the interface descriptor for the supplied interface and alternate
    // setting numbers.
    //
    for(ulLoop = 0; ulLoop < ulNumInterfaces; ulLoop++)
    {
        //
        // Get the next interface descriptor in the configuration descriptor.
        //
        psInterface = USBDCDConfigGetInterface(psConfig, ulLoop, USB_DESC_ANY,
                                               &ulSection);

        //
        // Is this the required interface with the correct alternate setting?
        //
        if(psInterface &&
           (psInterface->bInterfaceNumber == pUSBRequest->wIndex) &&
           (psInterface->bAlternateSetting == pUSBRequest->wValue))
        {
            ucInterface = psInterface->bInterfaceNumber;

            //
            // Make sure we don't write outside the bounds of the pucAltSetting
            // array (in a debug build, anyway, since this indicates an error
            // in the device descriptor).
            //
            ASSERT(ucInterface < USB_MAX_INTERFACES_PER_DEVICE);

            //
            // This is the correct interface descriptor so save the
            // setting.
            //
            psUSBControl->pucAltSetting[ucInterface] =
                                            psInterface->bAlternateSetting;

            //
            // Reconfigure the endpoints to match the requirements of the
            // new alternate setting for the interface.
            //
            bRetcode = USBDeviceConfigAlternate(psUSBControl, psConfig, ucInterface,
                                           psInterface->bAlternateSetting);

            //
            // If there is a callback then notify the application of the
            // change to the alternate interface.
            //
            if(bRetcode && psDevice->sCallbacks.pfnInterfaceChange)
            {
                psDevice->sCallbacks.pfnInterfaceChange(
                                                psUSBControl->pvInstance,
                                                pUSBRequest->wIndex,
                                                pUSBRequest->wValue);
            }

            //
            // All done.
            //
            return;
        }
    }

    //
    // If we drop out of the loop, we didn't find an interface descriptor
    // matching the requested number and alternate setting or there was an
    // error while trying to set up for the new alternate setting.
    //
    USBDCDStallEP0(0);
}

//*****************************************************************************
//
// This function handles the SYNC_FRAME standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param pUSBRequest holds the data for this request.
//
// This is currently a stub function that will stall indicating that the
// command is not supported.
//
// \return None.
//
//*****************************************************************************
static void
USBDSyncFrame(void *pvInstance, tUSBRequest *pUSBRequest)
{
    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, true);

    //
    // Not handled yet so stall this request.
    //
    USBDCDStallEP0(0);
}

//*****************************************************************************
//
// This internal function handles sending data on endpoint zero.
//
// \param ulIndex is the index of the USB controller which is to be
// initialized.
//
// \return None.
//
//*****************************************************************************
static void
USBDEP0StateTx(unsigned long ulIndex)
{
    unsigned long ulNumBytes;
    unsigned char *pData;

    ASSERT(ulIndex == 0);

    //
    // In the TX state on endpoint zero.
    //
    g_psUSBDevice[0].eEP0State = USB_STATE_TX;

    //
    // Set the number of bytes to send this iteration.
    //
    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

    //
    // Limit individual transfers to 64 bytes.
    //
    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }

    //
    // Save the pointer so that it can be passed to the USBEndpointDataPut()
    // function.
    //
    pData = (unsigned char *)g_psUSBDevice[0].pEP0Data;

    //
    // Advance the data pointer and counter to the next data to be sent.
    //
    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;
    g_psUSBDevice[0].pEP0Data += ulNumBytes;

    //
    // Put the data in the correct FIFO.
    //
    MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, pData, ulNumBytes);

    //
    // If this is exactly 64 then don't set the last packet yet.
    //
    if(ulNumBytes == EP0_MAX_PACKET_SIZE)
    {
        //
        // There is more data to send or exactly 64 bytes were sent, this
        // means that there is either more data coming or a null packet needs
        // to be sent to complete the transaction.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_IN);
    }
    else
    {
        //
        // Now go to the status state and wait for the transmit to complete.
        //
        g_psUSBDevice[0].eEP0State = USB_STATE_STATUS;

        //
        // Send the last bit of data.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_IN_LAST);

        //
        // If there is a sent callback then call it.
        //
        if((g_psUSBDevice[0].psInfo->sCallbacks.pfnDataSent) &&
           (g_psUSBDevice[0].ulOUTDataSize != 0))
        {
            //
            // Call the custom handler.
            //
            g_psUSBDevice[0].psInfo->sCallbacks.pfnDataSent(
                g_psUSBDevice[0].pvInstance, g_psUSBDevice[0].ulOUTDataSize);

            //
            // There is no longer any data pending to be sent.
            //
            g_psUSBDevice[0].ulOUTDataSize = 0;
        }
    }
}

//*****************************************************************************
//
// This internal function handles sending the configuration descriptor on
// endpoint zero.
//
// \param ulIndex is the index of the USB controller which is to be used.
//
//
// \return None.
//
//*****************************************************************************
static void
USBDEP0StateTxConfig(unsigned long ulIndex)
{
    unsigned long ulNumBytes;
    unsigned long ulSecBytes;
    unsigned long ulToSend;
    unsigned char *pData;
    tConfigDescriptor sConfDesc;
    const tConfigHeader *psConfig;
    const tConfigSection *psSection;

    ASSERT(ulIndex == 0);

    //
    // In the TX state on endpoint zero.
    //
    g_psUSBDevice[0].eEP0State = USB_STATE_TX_CONFIG;

    //
    // Find the current configuration descriptor definition.
    //
    psConfig = g_psUSBDevice[0].psInfo->ppConfigDescriptors[
               g_psUSBDevice[0].ucConfigIndex];

    //
    // Set the number of bytes to send this iteration.
    //
    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

    //
    // Limit individual transfers to 64 bytes.
    //
    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }

    //
    // If this is the first call, we need to fix up the total length of the
    // configuration descriptor.  This has already been determined and set in
    // g_sUSBDeviceState.ulEP0DataRemain.
    //
    if((g_psUSBDevice[0].ucSectionOffset == 0) &&
       (g_psUSBDevice[0].ucConfigSection == 0))
    {
        //
        // Copy the USB configuration descriptor from the beginning of the
        // first section of the current configuration.
        //
        sConfDesc = *(tConfigDescriptor *)g_psUSBDevice[0].pEP0Data;

        //
        // Update the total size.
        //
        sConfDesc.wTotalLength = (unsigned short)USBDCDConfigDescGetSize(
                                                                   psConfig);

        //
        // Write the descriptor to the USB FIFO.
        //
        ulToSend = (ulNumBytes < sizeof(tConfigDescriptor)) ? ulNumBytes :
                        sizeof(tConfigDescriptor);
        MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, (unsigned char *)&sConfDesc,
                           ulToSend);

        //
        // Did we reach the end of the first section?
        //
        if(psConfig->psSections[0]->usSize == ulToSend)
        {
            //
            // Update our tracking indices to point to the start of the next
            // section.
            //
            g_psUSBDevice[0].ucSectionOffset = 0;
            g_psUSBDevice[0].ucConfigSection = 1;
        }
        else
        {
            //
            // Note that we have sent the first few bytes of the descriptor.
            //
            g_psUSBDevice[0].ucSectionOffset = (unsigned char)ulToSend;
        }

        //
        // How many bytes do we have remaining to send on this iteration?
        //
        ulToSend = ulNumBytes - ulToSend;
    }
    else
    {
        //
        // Set the number of bytes we still have to send on this call.
        //
        ulToSend = ulNumBytes;
    }

    //
    // Add the relevant number of bytes to the USB FIFO
    //
    while(ulToSend)
    {
        //
        // Get a pointer to the current configuration section.
        //
        psSection = psConfig->psSections[g_psUSBDevice[0].ucConfigSection];

        //
        // Calculate bytes are available in the current configuration section.
        //
        ulSecBytes = (unsigned long)(psSection->usSize -
                     g_psUSBDevice[0].ucSectionOffset);

        //
        // Save the pointer so that it can be passed to the
        // USBEndpointDataPut() function.
        //
        pData = (unsigned char *)psSection->pucData +
                g_psUSBDevice[0].ucSectionOffset;

        //
        // Are there more bytes in this section that we still have to send?
        //
        if(ulSecBytes > ulToSend)
        {
            //
            // Yes - send only the remaining bytes in the transfer.
            //
            ulSecBytes = ulToSend;
        }

        //
        // Put the data in the correct FIFO.
        //
        MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, pData, ulSecBytes);

        //
        // Fix up our pointers for the next iteration.
        //
        ulToSend -= ulSecBytes;
        g_psUSBDevice[0].ucSectionOffset += (unsigned char)ulSecBytes;

        //
        // Have we reached the end of a section?
        //
        if(g_psUSBDevice[0].ucSectionOffset == psSection->usSize)
        {
            //
            // Yes - move to the next one.
            //
            g_psUSBDevice[0].ucConfigSection++;
            g_psUSBDevice[0].ucSectionOffset = 0;
        }
    }

    //
    // Fix up the number of bytes remaining to be sent and the start pointer.
    //
    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;

    //
    // If we ran out of bytes in the configuration section, bail and just
    // send out what we have.
    //
    if(psConfig->ucNumSections <= g_psUSBDevice[0].ucConfigSection)
    {
        g_psUSBDevice[0].ulEP0DataRemain = 0;
    }

    //
    // If there is no more data don't keep looking or ucConfigSection might
    // overrun the available space.
    //
    if(g_psUSBDevice[0].ulEP0DataRemain != 0)
    {
        pData =(unsigned char *)
            psConfig->psSections[g_psUSBDevice[0].ucConfigSection]->pucData;
        ulToSend = g_psUSBDevice[0].ucSectionOffset;
        g_psUSBDevice[0].pEP0Data = (pData + ulToSend);
    }

    //
    // If this is exactly 64 then don't set the last packet yet.
    //
    if(ulNumBytes == EP0_MAX_PACKET_SIZE)
    {
        //
        // There is more data to send or exactly 64 bytes were sent, this
        // means that there is either more data coming or a null packet needs
        // to be sent to complete the transaction.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_IN);
    }
    else
    {
        //
        // Send the last bit of data.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_IN_LAST);

        //
        // If there is a sent callback then call it.
        //
        if((g_psUSBDevice[0].psInfo->sCallbacks.pfnDataSent) &&
           (g_psUSBDevice[0].ulOUTDataSize != 0))
        {
            //
            // Call the custom handler.
            //
            g_psUSBDevice[0].psInfo->sCallbacks.pfnDataSent(
                g_psUSBDevice[0].pvInstance, g_psUSBDevice[0].ulOUTDataSize);

            //
            // There is no longer any data pending to be sent.
            //
            g_psUSBDevice[0].ulOUTDataSize = 0;
        }

        //
        // Now go to the status state and wait for the transmit to complete.
        //
        g_psUSBDevice[0].eEP0State = USB_STATE_STATUS;
    }
}

//*****************************************************************************
//
// The internal USB device interrupt handler.
//
// \param ulIndex is the USB controller associated with this interrupt.
// \param ulStatus is the current interrupt status as read via a call to
// USBIntStatusControl().
//
// This function is called from either \e USB0DualModeIntHandler() or
// \e USB0DeviceIntHandler() to process USB interrupts when in device mode.
// This handler will branch the interrupt off to the appropriate application or
// stack handlers depending on the current status of the USB controller.
//
// The two-tiered structure for the interrupt handler ensures that it is
// possible to use the same handler code in both device and OTG modes and
// means that host code can be excluded from applications that only require
// support for USB device mode operation.
//
// \return None.
//
//*****************************************************************************
void
USBDeviceIntHandlerInternal(unsigned long ulIndex, unsigned long ulStatus)
{
    static unsigned long ulSOFDivide = 0;
    tDeviceInfo *psInfo;
    void *pvInstance;

    //
    // If device initialization has not been performed then just disconnect
    // from the USB bus and return from the handler.
    //
    if(g_psUSBDevice[0].psInfo == 0)
    {
        MAP_USBDevDisconnect(USB0_BASE);
        return;
    }

    psInfo = g_psUSBDevice[0].psInfo;
    pvInstance = g_psUSBDevice[0].pvInstance;

    //
    // Received a reset from the host.
    //
    if(ulStatus & USB_INTCTRL_RESET)
    {
        USBDeviceEnumResetHandler(&g_psUSBDevice[0]);
    }

    //
    // Suspend was signaled on the bus.
    //
    if(ulStatus & USB_INTCTRL_SUSPEND)
    {
        //
        // Call the SuspendHandler() if it was specified.
        //
        if(psInfo->sCallbacks.pfnSuspendHandler)
        {
            psInfo->sCallbacks.pfnSuspendHandler(pvInstance);
        }
    }

    //
    // Resume was signaled on the bus.
    //
    if(ulStatus & USB_INTCTRL_RESUME)
    {
        //
        // Call the ResumeHandler() if it was specified.
        //
        if(psInfo->sCallbacks.pfnResumeHandler)
        {
            psInfo->sCallbacks.pfnResumeHandler(pvInstance);
        }
    }

    //
    // USB device was disconnected.
    //
    if(ulStatus & USB_INTCTRL_DISCONNECT)
    {
        //
        // Call the DisconnectHandler() if it was specified.
        //
        if(psInfo->sCallbacks.pfnDisconnectHandler)
        {
            psInfo->sCallbacks.pfnDisconnectHandler(pvInstance);
        }
    }

    //
    // Start of Frame was received.
    //
    if(ulStatus & USB_INTCTRL_SOF)
    {
        //
        // Increment the global Start of Frame counter.
        //
        g_ulUSBSOFCount++;

        //
        // Increment our SOF divider.
        //
        ulSOFDivide++;

        //
        // Handle resume signaling if required.
        //
        USBDeviceResumeTickHandler(&g_psUSBDevice[0]);

        //
        // Have we counted enough SOFs to allow us to call the tick function?
        //
        if(ulSOFDivide == USB_SOF_TICK_DIVIDE)
        {
            //
            // Yes - reset the divider and call the SOF tick handler.
            //
            ulSOFDivide = 0;
            InternalUSBStartOfFrameTick(USB_SOF_TICK_DIVIDE);
        }
    }

    //
    // Get the controller interrupt status.
    //
    ulStatus = MAP_USBIntStatusEndpoint(USB0_BASE);

    //
    // Handle end point 0 interrupts.
    //
    if(ulStatus & USB_INTEP_0)
    {
        USBDeviceEnumHandler(&g_psUSBDevice[0]);
    }

    //
    // Because there is no way to detect if a uDMA interrupt has occurred,
    // check for an endpoint callback and call it if it is available.
    //
    if(psInfo->sCallbacks.pfnEndpointHandler)
    {
        psInfo->sCallbacks.pfnEndpointHandler(pvInstance, ulStatus);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
