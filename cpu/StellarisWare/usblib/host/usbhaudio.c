//*****************************************************************************
//
// usbhaudio.c - USB host audio driver.
//
// Copyright (c) 2010-2012 Texas Instruments Incorporated.  All rights reserved.
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
#include "usblib/usbaudio.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhaudio.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These defines are used with the USBHostAudioFormatSet()
// USBHostAudioFormatGet() to parse out interface number and alternate
// setting number for an interface.
//
//*****************************************************************************
#define INTERFACE_NUM_M         0x000000FF
#define INTERFACE_ALTSETTING_M  0x0000FF00
#define INTERFACE_ALTSETTING_S  8

//*****************************************************************************
//
// Used to indicate an invalid interface descriptor number.
//
//*****************************************************************************
#define INVALID_INTERFACE       0xffffffff

//*****************************************************************************
//
// Forward declarations for the driver open and close calls.
//
//*****************************************************************************
static void *USBAudioOpen(tUSBHostDevice *pDevice);
static void USBAudioClose(void *pvInstance);

//*****************************************************************************
//
// This is the structure for an instance of a USB host audio driver.
//
//*****************************************************************************
typedef struct
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *pDevice;

    //
    // Used to save the call back.
    //
    tUSBHostAudioCallback pfnCallback;

    //
    // This is the control interface.
    //
    unsigned char ucIControl;

    //
    // This is the output streaming interface.
    //
    unsigned char ucOutInterface;

    //
    // This is the currently selected active output interface used with
    // ucOutInterface interface.
    //
    unsigned char ucOutAltSetting;

    //
    // This is the streaming interface.
    //
    unsigned char ucInInterface;

    //
    // This is the currently selected active input interface used with
    // ucInInterface interface.
    //
    unsigned char ucInAltSetting;

    //
    // The Isochronous endpoint addresses.
    //
    unsigned char ucIsochInAddress;
    unsigned char ucIsochOutAddress;

    tACInputTerminal *pInTerminal;
    tACOutputTerminal *pOutTerminal;

    //
    // Holds the identifier for the Feature Unit for controlling volume.
    //
    unsigned char ucVolumeID;
    
    tACFeatureUnit *pFeatureUnit;

    //
    // Holds what types of controls are enabled on the device.
    //
    unsigned short pusControls[3];

    //
    // Isochronous IN pipe.
    //
    unsigned long ulIsochInPipe;
    unsigned short usPipeSizeIn;
    tUSBHostAudioCallback pfnInCallback;
    void *pvInBuffer;

    //
    // Isochronous OUT pipe.
    //
    unsigned long ulIsochOutPipe;
    unsigned short usPipeSizeOut;
    tUSBHostAudioCallback pfnOutCallback;
    void *pvOutBuffer;

    //
    // State flags for this audio instance.
    //
    unsigned long ulFlags;
}
tUSBHostAudioInstance;

//*****************************************************************************
//
// The internal flags for an audio interface.
//
//*****************************************************************************
#define AUDIO_FLAG_OUT_ACTIVE   1   // Audio output is active.
#define AUDIO_FLAG_IN_ACTIVE    2   // Audio input is active.

//*****************************************************************************
//
// The USB Host audio instance.
//
//*****************************************************************************
static tUSBHostAudioInstance g_AudioDevice =
{
    0
};

//*****************************************************************************
//
//! This constant global structure defines the Audio Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_USBHostAudioClassDriver =
{
    USB_CLASS_AUDIO,
    USBAudioOpen,
    USBAudioClose,
    0
};

//*****************************************************************************
//
// This is the internal function that handles callbacks from the USB IN pipe.
//
//*****************************************************************************
static void
PipeCallbackIN(unsigned long ulPipe, unsigned long ulEvent)
{
    //
    // Only handle the data available callback and pass it on to the
    // application.
    //
    if(ulEvent == USB_EVENT_RX_AVAILABLE)
    {
        if(g_AudioDevice.pfnInCallback)
        {
            g_AudioDevice.pfnInCallback(
                g_AudioDevice.pvInBuffer, 0, USB_EVENT_RX_AVAILABLE);
        }
    }
}

//*****************************************************************************
//
// This is the internal function that handles callbacks from the USB OUT pipe.
//
//*****************************************************************************
static void
PipeCallbackOUT(unsigned long ulPipe, unsigned long ulEvent)
{
    //
    // Only handle the transmit complete callback and pass it on to the
    // application.
    //
    if(ulEvent == USB_EVENT_TX_COMPLETE)
    {
        if(g_AudioDevice.pfnOutCallback)
        {
            g_AudioDevice.pfnOutCallback(
                g_AudioDevice.pvOutBuffer, 0, USB_EVENT_TX_COMPLETE);
        }
    }
}

//*****************************************************************************
//
// Finds a given terminal and type in an audio configuration descriptor.
//
//*****************************************************************************
static tDescriptorHeader *
AudioTerminalGet(tConfigDescriptor *pConfigDesc, unsigned long ulTerminal,
                 unsigned long ulTerminalType)
{
    tACOutputTerminal *pOutput;
    tDescriptorHeader *pHeader;
    long lBytesRemaining;

    pHeader = (tDescriptorHeader *)pConfigDesc;
    lBytesRemaining = pConfigDesc->wTotalLength;

    while(lBytesRemaining > 0)
    {
        //
        // Output and input terminals are the same past the bDescriptorSubtype
        // and wTerminalType that are being searched for.
        //
        pOutput = (tACOutputTerminal *)pHeader;

        //
        // Only CS_INTERFACE descriptors can be a terminal.
        //
        if((pHeader->bDescriptorType == USB_DTYPE_CS_INTERFACE) &&
           (ulTerminal == pOutput->bDescriptorSubtype))
        {
            if((pOutput->bDescriptorSubtype == USB_AI_OUTPUT_TERMINAL) ||
               (pOutput->bDescriptorSubtype == USB_AI_INPUT_TERMINAL))

            {
                //
                // If this was the terminal type that was requested, the
                // return it.
                //
                if(pOutput->wTerminalType == ulTerminalType)
                {
                    return(pHeader);
                }
            }
            else if(pOutput->bDescriptorSubtype == USB_AI_FEATURE_UNIT)
            {
                return(pHeader);
            }
        }

        //
        // Decrease the bytes remaining by the size of this descriptor.
        //
        lBytesRemaining -=  pHeader->bLength;

        //
        // Move the pointer to the next header.
        //
        pHeader = (tDescriptorHeader*)((unsigned long)pHeader +
                                       pHeader->bLength);
    }
    return((tDescriptorHeader *)0);
}

//*****************************************************************************
//
// This function returns the interface number for the control interface
// in the structure passed in the pConfigDesc.
//
// \param pConfigDescriptor is a pointer to the memory containing a valid
//        configuration descriptor for a device.
//
// This function searches a configuration descriptor for a control interface
// descriptor.  The function only search for the first descriptor and then
// returns when it finds one.
//
// \return The first control interface descriptor number for an audio device
//         or INVALID_INTERFACE if no control interface descriptor was found.
//
//*****************************************************************************
static unsigned long
AudioControlGet(tConfigDescriptor *pConfigDesc)
{
    tDescriptorHeader *pHeader;
    tInterfaceDescriptor *pInterface;
    unsigned long ulInterface;
    long lBytes;

    pHeader = (tDescriptorHeader *)pConfigDesc;
    lBytes = pConfigDesc->wTotalLength;

    //
    // Initialize the interface number to an invalid value.
    //
    ulInterface = INVALID_INTERFACE;

    //
    // Search the whole configuration descriptor.
    //
    while(lBytes > 0)
    {
        //
        // Find an interface descriptor and see if it is a control interface.
        //
        if(pHeader->bDescriptorType == USB_DTYPE_INTERFACE)
        {
            pInterface = (tInterfaceDescriptor *)pHeader;

            //
            // If this is the control interface then return the value to the
            // caller.
            //
            if(pInterface->bInterfaceSubClass == USB_ASC_AUDIO_CONTROL)
            {
                ulInterface = pInterface->bInterfaceNumber;

                break;
            }
        }

        //
        // Decrease the bytes remaining by the size of this descriptor.
        //
        lBytes -= pHeader->bLength;

        //
        // Move the pointer to the next header.
        //
        pHeader = (tDescriptorHeader*)((unsigned long)pHeader +
                                       pHeader->bLength);
    }
    return(ulInterface);
}

//*****************************************************************************
//
// If it exists, finds the correct audio interface for a given audio format.
//
//*****************************************************************************
static unsigned long
AudioGetInterface(tUSBHostAudioInstance *pAudioDevice,
                  unsigned short usFormat, unsigned long ulSampleRate,
                  unsigned long ulBytes, unsigned long ulChannels,
                  unsigned long ulFlags)
{
    tDescriptorHeader *pHeader;
    tInterfaceDescriptor *pInterface;
    tEndpointDescriptor *pINEndpoint, *pOUTEndpoint;
    tACHeader *pACHeader;
    tACGeneral *pGeneral;
    tASFormat *pFormat;
    tEndpointDescriptor *pEndpoint;
    unsigned char *pucValue;
    unsigned long ulValue;
    long lBytes, lIdx;

    //
    // Initialize the Interface pointer to null.
    //
    pInterface = 0;
    pINEndpoint = 0;
    pOUTEndpoint = 0;

    //
    // Start at the top of the configuration descriptor.
    //
    pHeader = (tDescriptorHeader *)pAudioDevice->pDevice->pConfigDescriptor;

    lBytes = pAudioDevice->pDevice->pConfigDescriptor->wTotalLength;

    while(lBytes > 0)
    {
        if(pHeader->bDescriptorType == USB_DTYPE_INTERFACE)
        {
            //
            // If a new interface was found and the last one satisfied all
            // requirements then a valid interface was found so break out.
            //
            if(pInterface)
            {
                break;
            }

            //
            // Get the new interface pointer.
            //
            pInterface = (tInterfaceDescriptor *)pHeader;

            //
            // Reset the endpoints on finding a new interface descriptor.
            //
            pINEndpoint = 0;
            pOUTEndpoint = 0;

            //
            // If this is not a valid audio streaming interface then reset
            // the interface pointer to null.
            //
            if((pInterface->bNumEndpoints == 0) ||
               (pInterface->bInterfaceClass != USB_CLASS_AUDIO) ||
               (pInterface->bInterfaceSubClass != USB_ASC_AUDIO_STREAMING))
            {
                pInterface = 0;
            }
        }
        if((pInterface) && (pHeader->bDescriptorType == USB_DTYPE_CS_INTERFACE))
        {
            pACHeader = (tACHeader *)pHeader;

            //
            // If this is a General descriptor the check if the format matches.
            //
            if(pACHeader->bDescriptorSubtype == USB_AS_GENERAL)
            {
                //
                // Just save the pointer to the format descriptor.
                //
                pGeneral = (tACGeneral *)pHeader;

                //
                // If this interface has the wrong format then set it to null
                // so that the rest of this interface is ignored.
                //
                if(pGeneral->wFormatTag != usFormat)
                {
                    pInterface = 0;
                }
            }
            else if(pACHeader->bDescriptorSubtype == USB_AS_FORMAT_TYPE)
            {
                pFormat = (tASFormat *)pHeader;

                //
                // If the number of bytes per sample and number of channels do
                // not match then reset the interface pointer so that the rest
                // of this interface is ignored.
                //
                if((pFormat->bNrChannels != ulChannels) ||
                   (pFormat->bSubFrameSize != ulBytes))
                {
                    pInterface = 0;
                }
                else
                {
                    pucValue = &pFormat->tSamFreq;

                    //
                    // Attempt to find the sample rate in the sample rate
                    // table for this interface.
                    //
                    for(lIdx = 0; lIdx < pFormat->bSamFreqType; lIdx++)
                    {
                        ulValue = (*((unsigned long *)&pucValue[lIdx * 3]) &
                                   0xffffff);

                        if(ulValue == ulSampleRate)
                        {
                            break;
                        }
                    }

                    //
                    // If the sample rate was not found then set the interface
                    // pointer to null so that the rest of this interface is
                    // ignored.
                    //
                    if(lIdx == pFormat->bSamFreqType)
                    {
                        pInterface = 0;
                    }
                }
            }
        }
        else if((pInterface) &&
                (pHeader->bDescriptorType == USB_DTYPE_ENDPOINT))
        {
            pEndpoint = (tEndpointDescriptor *)pHeader;

            //
            // See what direction is being requested.
            //
            if(ulFlags & USBH_AUDIO_FORMAT_IN)
            {
                //
                // If this is an input endpoint and is just a feed back input
                // then ignore it.
                //
                if(pEndpoint->bEndpointAddress & USB_EP_DESC_IN)
                {
                    if((pEndpoint->bmAttributes & USB_EP_ATTR_USAGE_M)
                        == USB_EP_ATTR_USAGE_FEEDBACK)
                    {
                        pInterface = 0;
                    }
                    else
                    {
                        //
                        // Save this endpoint as a possible valid endpoint
                        //
                        pINEndpoint = pEndpoint;
                    }
                }
            }
            else
            {
                //
                // If this is an output endpoint and is just a feed back input
                // then ignore it.
                //
                if((pEndpoint->bEndpointAddress & USB_EP_DESC_IN) == 0)
                {
                    if((pEndpoint->bmAttributes & USB_EP_ATTR_USAGE_M)
                        == USB_EP_ATTR_USAGE_FEEDBACK)
                    {
                        pInterface = 0;
                    }
                    else
                    {
                        //
                        // Save this endpoint as a possible valid endpoint;
                        //
                        pOUTEndpoint = pEndpoint;
                    }
                }
            }
        }

        //
        // Decrease the bytes remaining by the size of this descriptor.
        //
        lBytes -= pHeader->bLength;

        //
        // Move the pointer to the next header.
        //
        pHeader = (tDescriptorHeader*)((unsigned long)pHeader +
                                       pHeader->bLength);
    }

    //
    // If there is still a valid interface then return the values.
    //
    if(pInterface)
    {
        //
        // Check a valid IN endpoint descriptor.
        //
        if(pINEndpoint)
        {
            //
            // Save the endpoint address.
            //
            g_AudioDevice.ucIsochInAddress = pINEndpoint->bEndpointAddress &
                                             USB_EP_DESC_NUM_M;

            //
            // If there is no current pipe then just allocate a new one with
            // the settings for this interface.
            //
            if(g_AudioDevice.ulIsochInPipe == 0)
            {
                //
                // Allocate the USB Pipe for this Isochronous IN end point.
                //
                g_AudioDevice.ulIsochInPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_IN_DMA,
                                        g_AudioDevice.pDevice,
                                        pINEndpoint->wMaxPacketSize,
                                        PipeCallbackIN);
            }
            else if(g_AudioDevice.usPipeSizeIn < pINEndpoint->wMaxPacketSize)
            {
                //
                // Free the old endpoint and allocate a new one.
                //
                USBHCDPipeFree(g_AudioDevice.ulIsochInPipe);

                //
                // Allocate the USB Pipe for this Isochronous IN end point.
                //
                g_AudioDevice.ulIsochInPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_IN_DMA,
                                        g_AudioDevice.pDevice,
                                        pINEndpoint->wMaxPacketSize,
                                        PipeCallbackIN);

                //
                // Save the new size of the maximum packet size for this
                // USB pipe.
                //
                g_AudioDevice.usPipeSizeIn = pINEndpoint->wMaxPacketSize;
            }

            //
            // Configure the USB pipe as a Isochronous IN end point.
            //
            USBHCDPipeConfig(g_AudioDevice.ulIsochInPipe,
                             pINEndpoint->wMaxPacketSize,
                             0,
                             g_AudioDevice.ucIsochInAddress);
        }

        //
        // Check a valid OUT endpoint descriptor.
        //
        if(pOUTEndpoint)
        {
            //
            // Save the endpoint address.
            //
            g_AudioDevice.ucIsochOutAddress = pOUTEndpoint->bEndpointAddress &
                                              USB_EP_DESC_NUM_M;

            //
            // If there is no current pipe then just allocate a new one with
            // the settings for this interface.
            //
            if(g_AudioDevice.ulIsochOutPipe == 0)
            {
                //
                // Allocate the USB Pipe for this Isochronous OUT end point.
                //
                g_AudioDevice.ulIsochOutPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_OUT_DMA,
                                        g_AudioDevice.pDevice,
                                        pOUTEndpoint->wMaxPacketSize,
                                        PipeCallbackOUT);
            }
            else if(g_AudioDevice.usPipeSizeOut < pOUTEndpoint->wMaxPacketSize)
            {
                //
                // Free the old endpoint and allocate a new one.
                //
                USBHCDPipeFree(g_AudioDevice.ulIsochOutPipe);

                //
                // Allocate the USB Pipe for this Isochronous OUT end point.
                //
                g_AudioDevice.ulIsochOutPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_OUT_DMA,
                                        g_AudioDevice.pDevice,
                                        pOUTEndpoint->wMaxPacketSize,
                                        PipeCallbackOUT);

                //
                // Save the new size of the maximum packet size for this
                // USB pipe.
                //
                g_AudioDevice.usPipeSizeOut = pOUTEndpoint->wMaxPacketSize;
            }

            //
            // Configure the USB pipe as a Isochronous OUT end point.
            //
            USBHCDPipeConfig(g_AudioDevice.ulIsochOutPipe,
                             pOUTEndpoint->wMaxPacketSize, 0,
                             g_AudioDevice.ucIsochOutAddress);
        }

        return(pInterface->bInterfaceNumber |
               (pInterface->bAlternateSetting << INTERFACE_ALTSETTING_S));
    }
    return(INVALID_INTERFACE);
}

//*****************************************************************************
//
// This function is used to open an instance of the USB host audio driver.
//
// \param pDevice is a pointer to the device information structure.
//
// This function attempts to open an instance of the USB host audio driver
// based on the information contained in the pDevice structure.  This call
// fails if there are not sufficient resources to open the device.  The
// function returns a value that should be passed back into USBHostAudioClose()
// when the driver is no longer needed.
//
// \return The function returns a pointer to a USB host audio driver
// instance.
//
//*****************************************************************************
static void *
USBAudioOpen(tUSBHostDevice *pDevice)
{
    unsigned long ulTemp;
    tConfigDescriptor *pConfigDesc;

    //
    // Don't allow the device to be opened without closing first.
    //
    if(g_AudioDevice.pDevice)
    {
        return(0);
    }

    g_AudioDevice.pDevice = pDevice;

    //
    // Save a shorter name for the configuration descriptor.
    //
    pConfigDesc = pDevice->pConfigDescriptor;

    //
    // Find the input terminal.
    //
    g_AudioDevice.pInTerminal =
        (tACInputTerminal *)AudioTerminalGet(pConfigDesc,
                                             USB_AI_INPUT_TERMINAL,
                                             USB_TTYPE_STREAMING);

    //
    // Find the output terminal.
    //
    g_AudioDevice.pOutTerminal =
        (tACOutputTerminal *)AudioTerminalGet(pConfigDesc,
                                              USB_AI_OUTPUT_TERMINAL,
                                              USB_TTYPE_STREAMING);

    // 
    // Find the feature unit.
    g_AudioDevice.pFeatureUnit = 
        (tACFeatureUnit *)AudioTerminalGet(pConfigDesc,
                                           USB_AI_FEATURE_UNIT,
                                           0);

    //
    // Need some kind of terminal to send or receive audio from.
    //
    if((g_AudioDevice.pOutTerminal == 0) &&
       (g_AudioDevice.pInTerminal == 0))
    {
        return(0);
    }

    //
    // Find the Audio control interface.
    //
    ulTemp = AudioControlGet(pConfigDesc);

    if(ulTemp == INVALID_INTERFACE)
    {
        return(0);
    }

    //
    // Save the control interface index and increment the number
    // of interfaces that have been found.
    //
    g_AudioDevice.ucIControl = (unsigned char)ulTemp;

    //
    // If the call back exists, call it with an Open event.
    //
    if(g_AudioDevice.pfnCallback != 0)
    {
        g_AudioDevice.pfnCallback((void *)&g_AudioDevice,
                                         0, USBH_AUDIO_EVENT_OPEN);
    }

    // 
    // If a feature unit was found, save the ID 
    //
    if(g_AudioDevice.pFeatureUnit != 0) 
    {
        g_AudioDevice.ucVolumeID = g_AudioDevice.pFeatureUnit->bUnitID;
    }

    //
    // Save the device pointer.
    //
    g_AudioDevice.pDevice = pDevice;

    //
    // Allocate the USB Pipe for this Isochronous IN end point.
    //
    g_AudioDevice.ulIsochInPipe =
        USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_IN_DMA,
                            g_AudioDevice.pDevice, 256,
                            PipeCallbackIN);
    g_AudioDevice.usPipeSizeIn = 256;

    //
    // Allocate the USB Pipe for this Isochronous OUT end point.
    //
    g_AudioDevice.ulIsochOutPipe =
        USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_OUT_DMA,
                            g_AudioDevice.pDevice, 256,
                            PipeCallbackOUT);
    g_AudioDevice.usPipeSizeOut = 256;

    //
    // Clear the flags.
    //
    g_AudioDevice.ulFlags = 0;

    //
    // Return the only instance of this device.
    //
    return(&g_AudioDevice);
}

//*****************************************************************************
//
// This function is used to release an instance of the USB host audio driver.
//
// \param pvInstance is an instance pointer that needs to be released.
//
// This function frees up any resources in use by the USB host audio
// driver instance that is passed in.  The \e pvInstance pointer should be a
// valid value that was returned from a call to USBHostAudioOpen().
//
// \return None.
//
//*****************************************************************************
static void
USBAudioClose(void *pvInstance)
{
    //
    // Do nothing if there is not a driver open.
    //
    if(g_AudioDevice.pDevice == 0)
    {
        return;
    }

    //
    // Reset the device pointer.
    //
    g_AudioDevice.pDevice = 0;

    //
    // Free the Isochronous IN pipe.
    //
    if(g_AudioDevice.ulIsochInPipe != 0)
    {
        USBHCDPipeFree(g_AudioDevice.ulIsochInPipe);
    }

    //
    // Free the Isochronous OUT pipe.
    //
    if(g_AudioDevice.ulIsochOutPipe != 0)
    {
        USBHCDPipeFree(g_AudioDevice.ulIsochOutPipe);
    }

    //
    // If the call back exists then call it.
    //
    if(g_AudioDevice.pfnCallback != 0)
    {
        g_AudioDevice.pfnCallback((void *)&g_AudioDevice,
                                         0, USBH_AUDIO_EVENT_CLOSE);
    }
}

//*****************************************************************************
//
//! This function should be called before any devices are present to enable
//! the host audio class driver.
//!
//! \param ulIndex is the audio device to open (currently only 0 is supported).
//! \param pfnCallback is the driver call back for host audio events.
//!
//! This function is called to open an instance of a host audio device and
//! should provide a valid callback function for host audio events in the
//! \e pfnCallback parameter.  This function must be called before the USB
//! host code can successfully enumerate an audio device.
//!
//! \return This function returns the driver instance to use for the other
//! host audio functions.  If there is no instance available at the time of
//! this call, this function returns zero.
//
//*****************************************************************************
unsigned long
USBHostAudioOpen(unsigned long ulIndex, tUSBHostAudioCallback pfnCallback)
{
    //
    // Only one audio device is supported at this time and on one instance
    // is supported so if there is already a call back then fail.
    //
    if((ulIndex != 0) || (g_AudioDevice.pfnCallback))
    {
        return(0);
    }

    //
    // Save the call back.
    //
    g_AudioDevice.pfnCallback = pfnCallback;

    //
    // Return the requested device instance.
    //
    return((unsigned long)&g_AudioDevice);
}

//*****************************************************************************
//
//! This function should be called to release an audio device instance.
//!
//! \param ulInstance is the device instance that is to be released.
//!
//! This function is called when a host audio device needs to be released.
//! This could be in preparation for shutdown or a switch to USB device mode,
//! for example.  Following this call, the audio device is available and can
//! be opened again using a call to USBHostAudioOpen().  After calling this
//! function, the host audio driver will no longer provide any callbacks or
//! accept calls to other audio driver APIs.
//!
//! \return None.
//
//*****************************************************************************
void
USBHostAudioClose(unsigned long ulInstance)
{
    tUSBHostAudioInstance *pAudioDevice;

    //
    // Get a pointer to the device instance data from the handle.
    //
    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    //
    // Close the audio device.
    //
    USBAudioClose((void *)pAudioDevice);

    //
    // Clear the call back indicating that the device is now closed.
    //
    pAudioDevice->pfnCallback = 0;
}

//*****************************************************************************
//
// This function is used to request settings from a given audio interface.
//
// \param ulInstance is an instance value for the audio device to access.
// \param ulInterface is the interface to access.
// \param ulChannel is the channel number to access.
// \param ulRequest is the audio device request.
//
// This function is used to get volume control parameters from a given
// interface and on a given channel.  The \e ulInterface is the interface to
// make the request specified by \e ulChannel and \e ulRequest.  The
// \e ulRequest parameter must be one of the USB_AC_GET_* values.
//
// \return This function returns the requested value.
//
//*****************************************************************************
static unsigned long
VolumeSettingGet(unsigned long ulInstance, unsigned long ulInterface,
                 unsigned long ulChannel, unsigned long ulRequest)
{
    unsigned long ulValue;
    tUSBHostAudioInstance *pAudioDevice;
    tUSBRequest SetupPacket;

    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    ulValue = 0;

    //
    // This is a Class specific Interface IN request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_CLASS | USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = (ulRequest & 0xff);

    //
    // Request for a string descriptor.
    //
    SetupPacket.wValue = VOLUME_CONTROL | (ulChannel & 0xff);

    //
    // Set the language ID.
    //
    SetupPacket.wIndex = (pAudioDevice->ucVolumeID << 8) |
                         (ulInterface & 0xff);

    //
    // Only request the space available.
    //
    SetupPacket.wLength = 2;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &SetupPacket, pAudioDevice->pDevice,
                          (unsigned char *)&ulValue, 4,
                          pAudioDevice->pDevice->DeviceDescriptor.bMaxPacketSize0);

    return(ulValue);
}

//*****************************************************************************
//
//! This function is used to get the current volume setting for a given
//! audio device.
//!
//! \param ulInstance is an instance of the USB audio device.
//! \param ulInterface is the interface number to use to query the current
//! volume setting.
//! \param ulChannel is the 0 based channel number to query.
//!
//! The function is used to retrieve the current volume setting for an audio
//! device on the channel specified by \e ulChannel.  The \e ulInterface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ulChannel value starts with 0 which is the master audio
//! volume control interface.  The remaining \e ulChannel values provide
//! access to various other audio channels, with 1 and 2 being left and right
//! audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the current volume setting for the requested interface.
//
//*****************************************************************************
unsigned long
USBHostAudioVolumeGet(unsigned long ulInstance, unsigned long ulInterface,
                      unsigned long ulChannel)
{
    return(VolumeSettingGet(ulInstance, ulInterface, ulChannel,
                            USB_AC_GET_CUR));
}

//*****************************************************************************
//
//! This function is used to get the maximum volume setting for a given
//! audio device.
//!
//! \param ulInstance is an instance of the USB audio device.
//! \param ulInterface is the interface number to use to query the maximum
//! volume control value.
//! \param ulChannel is the 0 based channel number to query.
//!
//! The function is used to retrieve the maximum volume setting for an audio
//! device on the channel specified by \e ulChannel.  The \e ulInterface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ulChannel value starts with 0 which is the master audio
//! volume control interface.  The remaining \e ulChannel values provide
//! access to various other audio channels, with 1 and 2 being left and right
//! audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the maximum volume setting for the requested interface.
//
//*****************************************************************************
unsigned long
USBHostAudioVolumeMaxGet(unsigned long ulInstance, unsigned long ulInterface,
                         unsigned long ulChannel)
{
    return(VolumeSettingGet(ulInstance, ulInterface, ulChannel,
                            USB_AC_GET_MAX));
}

//*****************************************************************************
//
//! This function is used to get the minimum volume setting for a given
//! audio device.
//!
//! \param ulInstance is an instance of the USB audio device.
//! \param ulInterface is the interface number to use to query the minimum
//! volume control value.
//! \param ulChannel is the 0 based channel number to query.
//!
//! The function is used to retrieve the minimum volume setting for an audio
//! device on the channel specified by \e ulChannel.  The \e ulInterface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ulChannel value starts with 0 which is the master audio
//! volume control interface.  The remaining \e ulChannel values provide
//! access to various other audio channels, with 1 and 2 being left and right
//! audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the minimum volume setting for the requested interface.
//
//*****************************************************************************
unsigned long
USBHostAudioVolumeMinGet(unsigned long ulInstance, unsigned long ulInterface,
                         unsigned long ulChannel)
{
    return(VolumeSettingGet(ulInstance, ulInterface, ulChannel,
                            USB_AC_GET_MIN));
}

//*****************************************************************************
//
//! This function is used to get the volume control resolution for a given
//! audio device.
//!
//! \param ulInstance is an instance of the USB audio device.
//! \param ulInterface is the interface number to use to query the resolution
//! for the volume control.
//! \param ulChannel is the 0 based channel number to query.
//!
//! The function is used to retrieve the volume control resolution for an audio
//! device on the channel specified by \e ulChannel.  The \e ulInterface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ulChannel value starts with 0 which is the master audio
//! volume control interface.  The remaining \e ulChannel values provide
//! access to various other audio channels, with 1 and 2 being left and right
//! audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the volume control resolution for the requested interface.
//
//*****************************************************************************
unsigned long
USBHostAudioVolumeResGet(unsigned long ulInstance, unsigned long ulInterface,
                         unsigned long ulChannel)
{
    return(VolumeSettingGet(ulInstance, ulInterface, ulChannel,
                            USB_AC_GET_RES));
}

//*****************************************************************************
//
//! This function is used to set the current volume setting for a given
//! audio device.
//!
//! \param ulInstance is an instance of the USB audio device.
//! \param ulInterface is the interface number to use to set the current
//! volume setting.
//! \param ulChannel is the 0 based channel number to query.
//! \param ulValue is the value to write to the USB audio device.
//!
//! The function is used to set the current volume setting for an audio
//! device on the channel specified by \e ulChannel.  The \e ulInterface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ulChannel value starts with 0 which is the master audio
//! volume control interface.  The remaining \e ulChannel values provide
//! access to various other audio channels, with 1 and 2 being left and right
//! audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return None.
//
//*****************************************************************************
void
USBHostAudioVolumeSet(unsigned long ulInstance, unsigned ulInterface,
                      unsigned long ulChannel, unsigned long ulValue)
{
    tUSBHostAudioInstance *pAudioDevice;
    tUSBRequest SetupPacket;

    //
    // Create an audio instance pointer.
    //
    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    //
    // This is a Class specific Interface OUT request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS | USB_RTYPE_INTERFACE;

    //
    // Request is to set the current value.
    //
    SetupPacket.bRequest = USB_AC_SET_CUR;

    //
    // Request the volume control.
    //
    SetupPacket.wValue = VOLUME_CONTROL | (ulChannel & 0xff);

    //
    // Set Volume control ID and interface to 0.
    //
    SetupPacket.wIndex = pAudioDevice->ucVolumeID << 8;

    //
    // Only request the space available.
    //
    SetupPacket.wLength = 2;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &SetupPacket, pAudioDevice->pDevice,
            (unsigned char *)&ulValue, 2,
            pAudioDevice->pDevice->DeviceDescriptor.bMaxPacketSize0);
}

//*****************************************************************************
//
//! This function is called to determine if an audio format is supported by the
//! connected USB Audio device.
//!
//! \param ulInstance is the device instance for this call.
//! \param ulSampleRate is the sample rate of the audio stream.
//! \param ulBits is the number of bits per sample in the audio stream.
//! \param ulChannels is the number of channels in the audio stream.
//! \param ulFlags is a set of flags to determine what type of interface to
//!        retrieve.
//!
//! This function is called when an application needs to determine which audio
//! formats are supported by a USB audio device that has been connected.  The
//! \e ulInstance value that is used with this call is the value that was
//! returned from the USBHostAudioOpen() function.  This call checks the
//! USB audio device to determine if it can support the values provided in the
//! \e ulSampleRate, \e ulBits, and \e ulChannels values.  The \e ulFlags
//! currently only supports either the \b USBH_AUDIO_FORMAT_IN or
//! \b USBH_AUDIO_FORMAT_OUT values that indicates if a request is for an
//! audio input and an audio output.  If the format is supported this
//! function returns zero, and this function returns a non-zero value if the
//! format is not supported.  This function does not set the current output or
//! input format.
//!
//! \return A value of zero indicates the supplied format is supported and
//! a non-zero value indicates that the format is not supported.
//
//*****************************************************************************
unsigned long
USBHostAudioFormatGet(unsigned long ulInstance, unsigned long ulSampleRate,
                      unsigned long ulBits, unsigned long ulChannels,
                      unsigned long ulFlags)
{
    tUSBHostAudioInstance *pAudioDevice;

    //
    // Get a pointer to the device instance data from the handle.
    //
    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    //
    // Look for the requested format.
    //
    if(AudioGetInterface(pAudioDevice, USB_ADF_PCM, ulSampleRate, ulBits>>3,
                         ulChannels, ulFlags) != INVALID_INTERFACE)
    {
        return(0);
    }
    return(1);
}

//*****************************************************************************
//
//! This function is called to set the current sample rate on an audio
//! interface.
//!
//! \param ulInstance specifies the device instance for this call.
//! \param ulSampleRate is the sample rate in Hz.
//! \param ulBits is the number of bits per sample.
//! \param ulChannels is then number of audio channels.
//! \param ulFlags is a set of flags that determine the access type.
//!
//! This function is called when to set the current audio output or input format
//! for a USB audio device.  The \e ulInstance value that is used with this
//! call is the value that was returned from the USBHostAudioOpen() function.
//! The application can use this call to insure that the audio format is
//! supported and set the format at the same time.  If the application is
//! just checking for supported rates, then it should call the
//! USBHostAudioFormatGet().
//!
//! \note This function must be called before attempting to send or receive
//! audio with the USBHostAudioPlay() or USBHostAudioRecord() functions.
//!
//! \return A non-zero value indicates the supplied format is not supported and
//! a zero value indicates that the format was supported and has been
//! configured.
//
//*****************************************************************************
unsigned long
USBHostAudioFormatSet(unsigned long ulInstance, unsigned long ulSampleRate,
                      unsigned long ulBits, unsigned long ulChannels,
                      unsigned long ulFlags)
{
    tUSBHostAudioInstance *pAudioDevice;
    unsigned long ulInterface;

    //
    // Get a pointer to the device instance data from the handle.
    //
    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    //
    // Look for the requested format.
    //
    ulInterface = AudioGetInterface(pAudioDevice, USB_ADF_PCM, ulSampleRate,
                                    ulBits>>3, ulChannels, ulFlags);

    if(ulInterface == INVALID_INTERFACE)
    {
        return(1);
    }

    //
    // Determine if this is an input or output request.
    //
    if(ulFlags & USBH_AUDIO_FORMAT_IN)
    {
        //
        // Get the active interface number and alternate setting for this
        // format.
        //
        pAudioDevice->ucInInterface =
            (unsigned char)(ulInterface & INTERFACE_NUM_M);
        pAudioDevice->ucInAltSetting =
            (unsigned char)((ulInterface & INTERFACE_ALTSETTING_M) >>
                            INTERFACE_ALTSETTING_S);
    }
    else
    {
        //
        // Get the active interface number and alternate setting for this
        // format.
        //
        pAudioDevice->ucOutInterface =
            (unsigned char)(ulInterface & INTERFACE_NUM_M);
        pAudioDevice->ucOutAltSetting =
            (unsigned char)((ulInterface & INTERFACE_ALTSETTING_M) >>
                            INTERFACE_ALTSETTING_S);
    }
    return(0);
}

//*****************************************************************************
//
//! This function is called to send an audio buffer to the USB audio device.
//!
//! \param ulInstance specifies the device instance for this call.
//! \param pvBuffer is the audio buffer to send.
//! \param ulSize is the size of the buffer in bytes.
//! \param pfnCallback is a pointer to a callback function that is called
//! when the buffer can be used again.
//!
//! This function is called when an application needs to schedule a new buffer
//! for output to the USB audio device.  Since this call schedules the transfer
//! and returns immediately, the application should provide a \e pfnCallback
//! function to be notified when the buffer can be used again by the
//! application.  The \e pfnCallback function provided is called with the
//! \e pvBuffer parameter set to the \e pvBuffer provided by this call, the
//! \e ulParam can be ignored and the \e ulEvent parameter is
//! \b USB_EVENT_TX_COMPLETE.
//!
//! \return This function returns the number of bytes that were scheduled
//! to be sent.  If this function returns zero then there was no USB audio
//! device present or the request could not be satisfied at this time.
//
//*****************************************************************************
long
USBHostAudioPlay(unsigned long ulInstance, void *pvBuffer,
                 unsigned long ulSize, tUSBHostAudioCallback pfnCallback)
{
    tUSBHostAudioInstance *pAudioDevice;
    unsigned long ulBytes;

    //
    // Make sure that there is a device present.
    //
    if(g_AudioDevice.pDevice == 0)
    {
        return(0);
    }

    //
    // Get a pointer to the device instance data from the handle.
    //
    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    //
    // If the audio output interface is not active then select the current
    // active audio interface.
    //
    if(HWREGBITW(&pAudioDevice->ulFlags, AUDIO_FLAG_OUT_ACTIVE) == 0)
    {
        //
        // Indicate the active audio interface has been selected.
        //
        HWREGBITW(&pAudioDevice->ulFlags, AUDIO_FLAG_OUT_ACTIVE) = 1;

        //
        // Configure the USB audio device to use the selected audio interface.
        //
        USBHCDSetInterface(0, (unsigned long)pAudioDevice->pDevice,
                           pAudioDevice->ucOutInterface,
                           pAudioDevice->ucOutAltSetting);
    }

    //
    // Save the callback function and the buffer pointer.
    //
    pAudioDevice->pfnOutCallback = pfnCallback;
    pAudioDevice->pvOutBuffer = (void *)pvBuffer;

    //
    // Schedule the data to be written out to the FIFO.
    //
    ulBytes = USBHCDPipeSchedule(pAudioDevice->ulIsochOutPipe, pvBuffer,
                                 ulSize);

    //
    // Return the number of bytes scheduled to be sent.
    //
    return(ulBytes);
}

//*****************************************************************************
//
//! This function is called to provide an audio buffer to the USB audio device
//! for audio input.
//!
//! \param ulInstance specifies the device instance for this call.
//! \param pvBuffer is the audio buffer to send.
//! \param ulSize is the size of the buffer in bytes.
//! \param pfnCallback is a pointer to a callback function that is called
//! when the buffer has been filled.
//!
//! This function is called when an application needs to schedule a new buffer
//! for input from the USB audio device.  Since this call schedules the
//! transfer and returns immediately, the application should provide a
//! \e pfnCallback function to be notified when the buffer has been filled with
//! audio data.  When the \e pfnCallback function is called, the \e pvBuffer
//! parameter is set to \e pvBuffer provided in this call, the \e ulParam is
//! the number of valid bytes in the pvBuffer and the \e ulEvent is set to
//! \b USB_EVENT_RX_AVAILABLE.
//!
//! \return This function returns the number of bytes that were scheduled
//! to be sent.  If this function returns zero then there was no USB audio
//! device present or the device does not support audio input.
//
//*****************************************************************************
long
USBHostAudioRecord(unsigned long ulInstance, void *pvBuffer,
                   unsigned long ulSize, tUSBHostAudioCallback pfnCallback)
{
    tUSBHostAudioInstance *pAudioDevice;
    unsigned long ulBytes;

    //
    // Make sure that there is a device present.
    //
    if(g_AudioDevice.pDevice == 0)
    {
        return(0);
    }

    //
    // Get a pointer to the device instance data from the handle.
    //
    pAudioDevice = (tUSBHostAudioInstance *)ulInstance;

    //
    // If the audio input interface is not active then select the current
    // active audio interface.
    //
    if(HWREGBITW(&pAudioDevice->ulFlags, AUDIO_FLAG_IN_ACTIVE) == 0)
    {
        //
        // Indicate the active audio interface has been selected.
        //
        HWREGBITW(&pAudioDevice->ulFlags, AUDIO_FLAG_IN_ACTIVE) = 1;

        //
        // Configure the USB audio device to use the selected audio interface.
        //
        USBHCDSetInterface(0, (unsigned long)pAudioDevice->pDevice,
                           pAudioDevice->ucInInterface,
                           pAudioDevice->ucInAltSetting);
    }

    //
    // Save the callback function and the buffer pointer.
    //
    pAudioDevice->pfnInCallback = pfnCallback;
    pAudioDevice->pvInBuffer = (void *)pvBuffer;

    //
    // Schedule the data to be written out to the FIFO.
    //
    ulBytes = USBHCDPipeSchedule(pAudioDevice->ulIsochInPipe, pvBuffer, ulSize);

    //
    // Return the number of bytes scheduled to be sent.
    //
    return(ulBytes);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

