//*****************************************************************************
//
// usbdaudio.c - USB audio device class driver.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
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
#include "driverlib/udma.h"
#include "usblib/usblib.h"
#include "usblib/usbaudio.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdaudio.h"

//*****************************************************************************
//
//! \addtogroup audio_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The following are the USB audio descriptor identifiers.
//
//*****************************************************************************
#define AUDIO_IN_TERMINAL_ID    1
#define AUDIO_OUT_TERMINAL_ID   2
#define AUDIO_CONTROL_ID        3

//*****************************************************************************
//
// The following are the USB interface numbers for this audio device.
//
//*****************************************************************************
#define AUDIO_INTERFACE_CONTROL 0
#define AUDIO_INTERFACE_OUTPUT  1

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define ISOC_OUT_ENDPOINT       USB_EP_1
#define ISOC_OUT_DMA_CHANNEL    UDMA_CHANNEL_USBEP1RX

//*****************************************************************************
//
// Max size is (48000 samples/sec * 4 bytes/sample) * 0.001 seconds/frame.
//
//*****************************************************************************
#define ISOC_OUT_EP_MAX_SIZE    ((48000*4)/1000)

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
static unsigned char g_pAudioDeviceDescriptor[] =
{
    18,                     // Size of this structure.
    USB_DTYPE_DEVICE,       // Type of this structure.
    USBShort(0x110),        // USB version 1.1 (if we say 2.0, hosts assume
                            // high-speed - see USB 2.0 spec 9.2.6.6)
    0,                      // USB Device Class (spec 5.1.1)
    0,                      // USB Device Sub-class (spec 5.1.1)
    0,                      // USB Device protocol (spec 5.1.1)
    64,                     // Maximum packet size for default pipe.
    USBShort(0),            // Vendor ID (filled in during USBDAudioInit).
    USBShort(0),            // Product ID (filled in during USBDAudioInit).
    USBShort(0x100),        // Device Version BCD.
    1,                      // Manufacturer string identifier.
    2,                      // Product string identifier.
    3,                      // Product serial number.
    1                       // Number of configurations.
};

//*****************************************************************************
//
// Audio class device configuration descriptor.
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
static unsigned char g_pAudioDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(32),               // The total size of this full structure.
    2,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    0,                          // The string identifier that describes this
                                // configuration.
    USB_CONF_ATTR_BUS_PWR,      // Bus Powered, Self Powered, remote wake up.
    250,                        // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// This is the Interface Association Descriptor for the serial device used in
// composite devices.
//
//*****************************************************************************
unsigned char g_pIADAudioDescriptor[] =
{

    8,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE_ASC,    // Interface Association Type.
    0x0,                        // Default starting interface is 0.
    0x2,                        // Number of interfaces in this association.
    USB_CLASS_AUDIO,            // The device class for this association.
    USB_SUBCLASS_UNDEFINED,     // The device subclass for this association.
    USB_PROTOCOL_UNDEFINED,     // The protocol for this association.
    0                           // The string index for this association.
};

const tConfigSection g_sIADAudioConfigSection =
{
    sizeof(g_pIADAudioDescriptor),
    g_pIADAudioDescriptor
};

//*****************************************************************************
//
// The remainder of the configuration descriptor is stored in flash since we
// don't need to modify anything in it at runtime.
//
//*****************************************************************************
const unsigned char g_pAudioControlInterface[] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    AUDIO_INTERFACE_CONTROL,    // The index for this interface.
    0,                          // The alternate setting for this interface.
    0,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_AUDIO,            // The interface class
    USB_ASC_AUDIO_CONTROL,      // The interface sub-class.
    0,                          // The interface protocol for the sub-class
                                // specified above.
    0,                          // The string index for this interface.

    //
    // Audio Header Descriptor.
    //
    9,                          // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_HEADER,        // Descriptor sub-type is HEADER.
    USBShort(0x0100),           // Audio Device Class Specification Release
                                // Number in Binary-Coded Decimal.
                                // Total number of bytes in
                                // g_pAudioControlInterface
    USBShort((9 + 9 + 12 + 13 + 9)),
    1,                          // Number of streaming interfaces.
    1,                          // Index of the first and only streaming
                                // interface.

    //
    // Audio Input Terminal Descriptor.
    //
    12,                         // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_IN_TERMINAL,   // Descriptor sub-type is INPUT_TERMINAL.
    AUDIO_IN_TERMINAL_ID,       // Terminal ID for this interface.
                                // USB streaming interface.
    USBShort(USB_TTYPE_STREAMING),
    0,                          // ID of the Output Terminal to which this
                                // Input Terminal is associated.
    2,                          // Number of logical output channels in the
                                // Terminal's output audio channel cluster.
    USBShort((USB_CHANNEL_L |   // Describes the spatial location of the
             USB_CHANNEL_R)),   // logical channels.
    0,                          // Channel Name string index.
    0,                          // Terminal Name string index.

    //
    // Audio Feature Unit Descriptor
    //
    13,                         // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_FEATURE_UNIT,  // Descriptor sub-type is FEATURE_UNIT.
    AUDIO_CONTROL_ID,           // Unit ID for this interface.
    AUDIO_IN_TERMINAL_ID,       // ID of the Unit or Terminal to which this
                                // Feature Unit is connected.
    2,                          // Size in bytes of an element of the
                                // bmaControls() array that follows.
                                // Master Mute control.
    USBShort(USB_ACONTROL_MUTE),
                                // Left channel volume control.
    USBShort(USB_ACONTROL_VOLUME),
                                // Right channel volume control.
    USBShort(USB_ACONTROL_VOLUME),
    0,                          // Feature unit string index.

    //
    // Audio Output Terminal Descriptor.
    //
    9,                          // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_OUT_TERMINAL,  // Descriptor sub-type is INPUT_TERMINAL.
    AUDIO_OUT_TERMINAL_ID,      // Terminal ID for this interface.
                                // Output type is a generic speaker.
    USBShort(USB_ATTYPE_SPEAKER),
    AUDIO_IN_TERMINAL_ID,       // ID of the input terminal to which this
                                // output terminal is connected.
    AUDIO_CONTROL_ID,           // ID of the feature unit that this output
                                // terminal is connected to.
    0,                          // Output terminal string index.

};

//*****************************************************************************
//
// The audio streaming interface descriptor.  This describes the two valid
// interfaces for this class.  The first interface has no endpoints and is used
// by host operating systems to put the device in idle mode, while the second
// is used when the audio device is active.
//
//*****************************************************************************
const unsigned char g_pAudioStreamInterface[] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    AUDIO_INTERFACE_OUTPUT,     // The index for this interface.
    0,                          // The alternate setting for this interface.
    0,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_AUDIO,            // The interface class
    USB_ASC_AUDIO_STREAMING,    // The interface sub-class.
    0,                          // Unused must be 0.
    0,                          // The string index for this interface.

    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    1,                          // The index for this interface.
    1,                          // The alternate setting for this interface.
    1,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_AUDIO,            // The interface class
    USB_ASC_AUDIO_STREAMING,    // The interface sub-class.
    0,                          // Unused must be 0.
    0,                          // The string index for this interface.

    //
    // Class specific Audio Streaming Interface descriptor.
    //
    7,                          // Size of the interface descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ASDSTYPE_GENERAL,       // General information.
    AUDIO_IN_TERMINAL_ID,       // ID of the terminal to which this streaming
                                // interface is connected.
    1,                          // One frame delay.
    USBShort(USB_ADF_PCM),      //

    //
    // Format type Audio Streaming descriptor.
    //
    11,                         // Size of the interface descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ASDSTYPE_FORMAT_TYPE,   // Audio Streaming format type.
    USB_AF_TYPE_TYPE_I,         // Type I audio format type.
    2,                          // Two audio channels.
    2,                          // Two bytes per audio sub-frame.
    16,                         // 16 bits per sample.
    1,                          // One sample rate provided.
    USB3Byte(48000),            // Only 48000 sample rate supported.

    //
    // Endpoint Descriptor
    //
    9,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
                                    // OUT endpoint with address
                                    // ISOC_OUT_ENDPOINT.
    USB_EP_DESC_OUT | USB_EP_TO_INDEX(ISOC_OUT_ENDPOINT),
    USB_EP_ATTR_ISOC |              // Endpoint is an adaptive isochronous data
    USB_EP_ATTR_ISOC_ADAPT |        //  endpoint.
    USB_EP_ATTR_USAGE_DATA,
    USBShort(ISOC_OUT_EP_MAX_SIZE), // The maximum packet size.
    1,                              // The polling interval for this endpoint.
    0,                              // Refresh is unused.
    0,                              // Synch endpoint address.

    //
    // Audio Streaming Isochronous Audio Data Endpoint Descriptor
    //
    7,                              // The size of the descriptor.
    USB_ACSDT_ENDPOINT,             // Audio Class Specific Endpoint Descriptor.
    USB_ASDSTYPE_GENERAL,           // This is a general descriptor.
    USB_EP_ATTR_ACG_SAMPLING,       // Sampling frequency is supported.
    USB_EP_LOCKDELAY_UNDEF,         // Undefined lock delay units.
    USBShort(0),                    // No lock delay.
};

//*****************************************************************************
//
// The audio device configuration descriptor is defined as three sections,
// one containing just the 9 byte USB configuration descriptor.  The second
// holds the audio streaming interface and the third holds the audio control
// interface.
//
//*****************************************************************************
const tConfigSection g_sAudioConfigSection =
{
    sizeof(g_pAudioDescriptor),
    g_pAudioDescriptor
};

const tConfigSection g_sAudioStreamInterfaceSection =
{
    sizeof(g_pAudioStreamInterface),
    g_pAudioStreamInterface
};

const tConfigSection g_sAudioControlInterfaceSection =
{
    sizeof(g_pAudioControlInterface),
    g_pAudioControlInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete audio device configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psAudioSections[] =
{
    &g_sAudioConfigSection,
    &g_sIADAudioConfigSection,
    &g_sAudioControlInterfaceSection,
    &g_sAudioStreamInterfaceSection
};

#define NUM_AUDIO_SECTIONS      (sizeof(g_psAudioSections) /                  \
                                 sizeof(tConfigSection *))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sAudioConfigHeader =
{
    NUM_AUDIO_SECTIONS,
    g_psAudioSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_pAudioConfigDescriptors[] =
{
    &g_sAudioConfigHeader
};

//*****************************************************************************
//
// Various internal handlers needed by this class.
//
//*****************************************************************************
static void HandleDisconnect(void *pvInstance);
static void InterfaceChange(void *pvInstance, unsigned char ucInterface,
                            unsigned char ucAlternateSetting);
static void ConfigChangeHandler(void *pvInstance, unsigned long ulValue);
static void DataReceived(void *pvInstance, unsigned long ulInfo);
static void HandleEndpoints(void *pvInstance, unsigned long ulStatus);
static void HandleRequests(void *pvInstance, tUSBRequest *pUSBRequest);
static void HandleDevice(void *pvInstance, unsigned long ulRequest,
                         void *pvRequestData);

//*****************************************************************************
//
// The FIFO configuration for USB audio device class.
//
//*****************************************************************************
const tFIFOConfig g_sUSBAudioFIFOConfig =
{
    //
    // IN endpoints.
    //
    {
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN },
        { false, USB_EP_DEV_IN }
    },

    //
    // OUT endpoints.
    //
    {
        { false, USB_EP_DEV_OUT | USB_EP_DMA_MODE_1 | USB_EP_AUTO_CLEAR },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT },
        { false, USB_EP_DEV_OUT }
    },
};

//*****************************************************************************
//
// The device information structure for the USB Audio device.
//
//*****************************************************************************
tDeviceInfo g_sAudioDeviceInfo =
{
    //
    // Device event handler callbacks.
    //
    {
        //
        // GetDescriptor
        //
        0,

        //
        // RequestHandler
        //
        HandleRequests,

        //
        // InterfaceChange
        //
        InterfaceChange,

        //
        // ConfigChange
        //
        ConfigChangeHandler,

        //
        // DataReceived
        //
        DataReceived,

        //
        // DataSentCallback
        //
        0,

        //
        // ResetHandler
        //
        0,

        //
        // SuspendHandler
        //
        0,

        //
        // ResumeHandler
        //
        0,

        //
        // DisconnectHandler
        //
        HandleDisconnect,

        //
        // EndpointHandler
        //
        HandleEndpoints,

        //
        // Device handler
        //
        HandleDevice
    },
    g_pAudioDeviceDescriptor,
    g_pAudioConfigDescriptors,
    0,
    0,
    &g_sUSBAudioFIFOConfig
};

//*****************************************************************************
//
// This function is called to handle data being received back from the host so
// that the application callback can be called when the new data is ready.
//
//*****************************************************************************
static void
DataReceived(void *pvInstance, unsigned long ulInfo)
{
    tAudioInstance *psInst;
    const tUSBDAudioDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // Make a copy of this pointer for ease of use in this function.
    //
    psInst = psDevice->psPrivateData;

    //
    // If there is an update pending and the request was to set a current
    // value then check which value was set.
    //
    if(psInst->usUpdate && (psInst->ucRequest == USB_AC_SET_CUR))
    {
        //
        // Only handling interface requests.
        //
        if((psInst->usRequestType & USB_RTYPE_RECIPIENT_M) ==
           USB_RTYPE_INTERFACE)
        {
            if(psInst->usUpdate == VOLUME_CONTROL)
            {
                //
                // Inform the callback of the new volume.
                //
                psDevice->pfnCallback(0, USBD_AUDIO_EVENT_VOLUME,
                                      psInst->sVolume, 0);
            }
            else if(psDevice->psPrivateData->usUpdate == MUTE_CONTROL)
            {
                //
                // Inform the callback of the new data.
                //
                psDevice->pfnCallback(0, USBD_AUDIO_EVENT_MUTE, psInst->ucMute,
                                      0);
            }
        }
        psInst->usUpdate = 0;
    }
}

//*****************************************************************************
//
// This function is called to handle the interrupts on the isochronous endpoint
// for the audio device class.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvInstance, unsigned long ulStatus)
{
    unsigned long ulEPStatus;
    tAudioInstance *psInst;
    unsigned char *pucData;
    const tUSBDAudioDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // Make a copy of this pointer for ease of use later in this function.
    //
    psInst = psDevice->psPrivateData;

    //
    // Make sure this was for the isochronous out endpoint.
    //
    if((psInst->sBuffer.pvData != 0) &&
       (MAP_uDMAChannelModeGet(psInst->ucOUTDMA) == UDMA_MODE_STOP))
    {
        //
        // Save the pointer to the data buffer.
        //
        pucData = psInst->sBuffer.pvData;

        //
        // Clear out the buffer pointer to indicate it is no longer in use.
        //
        psInst->sBuffer.pvData = 0;

        //
        // Inform the callback of the new data.
        //
        psInst->sBuffer.pfnCallback(pucData, psInst->sBuffer.ulSize,
                                    USBD_AUDIO_EVENT_DATAOUT);

        //
        // Read out the current endpoint status.
        //
        ulEPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ucOUTEndpoint);

        //
        // Acknowledge that the data was read, this will not cause a bus
        // acknowledgment.
        //
        MAP_USBDevEndpointDataAck(USB0_BASE, psInst->ucOUTEndpoint, 0);

        //
        // Clear the status bits.
        //
        MAP_USBDevEndpointStatusClear(USB0_BASE, psInst->ucOUTEndpoint,
                                      ulEPStatus);
    }
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvInstance, unsigned long ulRequest, void *pvRequestData)
{
    tAudioInstance *psInst;
    unsigned char *pucData;

    //
    // Create the serial instance data.
    //
    psInst = ((tUSBDAudioDevice *)pvInstance)->psPrivateData;

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
            //
            // Save the change to the appropriate interface number.
            //
            if(pucData[0] == AUDIO_INTERFACE_CONTROL)
            {
                psInst->ucInterfaceControl = pucData[1];
            }
            else if(pucData[0] == AUDIO_INTERFACE_OUTPUT)
            {
                psInst->ucInterfaceAudio = pucData[1];
            }
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
            if((pucData[0] & USB_EP_DESC_IN) == 0)
            {
                //
                // Extract the new endpoint number without the DIR bit.
                //
                psInst->ucOUTEndpoint = INDEX_TO_USB_EP(pucData[1] & 0x7f);

                //
                // Extract the new DMA channel.
                //
                psInst->ucOUTDMA = UDMA_CHANNEL_USBEP1RX +
                                   (((pucData[1] & 0x7f) - 1) * 2);

                //
                // Basic configuration for DMA on the OUT endpoint.
                //
                MAP_uDMAChannelControlSet(psInst->ucOUTDMA,
                                          (UDMA_SIZE_32 | UDMA_SRC_INC_NONE|
                                           UDMA_DST_INC_32 | UDMA_ARB_16));

                //
                // Select this channel for this endpoint, this only affects
                // devices that have this feature.
                //
                MAP_USBEndpointDMAChannel(USB0_BASE, psInst->ucOUTEndpoint,
                                          psInst->ucOUTDMA);
            }
            break;
        }

        //
        // Handle class specific reconfiguring of the configuration descriptor
        // once the composite class has built the full descriptor.
        //
        case USB_EVENT_COMP_CONFIG:
        {
            //
            // This sets the bFirstInterface of the Interface Association
            // descriptor to the first interface which is the control
            // interface used by this instance.
            //
            pucData[2] = psInst->ucInterfaceControl;

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
    const tUSBDAudioDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // Inform the application that the device has been disconnected.
    //
    psDevice->pfnCallback(0, USB_EVENT_DISCONNECTED, 0, 0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device
// interface changes.  This occurs when the audio device transitions between
// being active and inactive.  Interface AUDIO_INTERFACE_CONTROL is the
// inactive interface that has no endpoints, while interface
// AUDIO_INTERFACE_AUDIO has the single Isochronous OUT endpoint.
//
//*****************************************************************************
static void
InterfaceChange(void *pvInstance, unsigned char ucInterface,
                unsigned char ucAlternateSetting)
{
    const tUSBDAudioDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // Check which interface to change into.
    //
    if(ucAlternateSetting == 0)
    {
        //
        // Alternate setting 0 is an inactive state.
        //
        if(psDevice->pfnCallback)
        {
            psDevice->pfnCallback(0, USBD_AUDIO_EVENT_IDLE, 0, 0);
        }
    }
    else
    {
        //
        // Alternate setting 1 is the active state.
        //
        if(psDevice->pfnCallback)
        {
            psDevice->pfnCallback(0, USBD_AUDIO_EVENT_ACTIVE, 0, 0);
        }

        //
        // Enable uDMA on the endpoint now that the active configuration
        // has been selected.
        //
        MAP_USBEndpointDMAEnable(USB0_BASE,
                                 psDevice->psPrivateData->ucOUTEndpoint,
                                 USB_EP_DEV_OUT);
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device
// configuration changes.
//
//*****************************************************************************
static void
ConfigChangeHandler(void *pvInstance, unsigned long ulValue)
{
    const tUSBDAudioDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psDevice->pfnCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psDevice->pfnCallback(pvInstance, USB_EVENT_CONNECTED, 0, 0);
    }
}

//*****************************************************************************
//
//! This function should be called once for the audio class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ulIndex is the index of the USB controller to initialize for
//! audio class device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the audio device.
//!
//! In order for an application to initialize the USB audio device class, it
//! must first call this function with the a valid audio device class structure
//! in the \e psDevice parameter.  This allows this function to initialize the
//! USB controller and device code to be prepared to enumerate and function as
//! a USB audio device.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the audio class.
//!
//! See the documentation on the tUSBDAudioDevice structure for more
//! information on how to properly fill the structure members.
//!
//! \return Returns 0 on failure or a non-zero void pointer on success.
//
//*****************************************************************************
void *
USBDAudioInit(unsigned long ulIndex, const tUSBDAudioDevice *psDevice)
{
    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateData);

    USBDAudioCompositeInit(ulIndex, psDevice);

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the bulk device on the bus.
    //
    USBDCDInit(ulIndex, psDevice->psPrivateData->psDevInfo);

    //
    // Basic configuration for DMA on the OUT endpoint.
    //
    MAP_uDMAChannelControlSet(psDevice->psPrivateData->ucOUTDMA,
                              (UDMA_SIZE_32 | UDMA_SRC_INC_NONE|
                               UDMA_DST_INC_32 | UDMA_ARB_16));

    //
    // Select this channel for this endpoint, this only affects devices that
    // have this feature.
    //
    MAP_USBEndpointDMAChannel(USB0_BASE, psDevice->psPrivateData->ucOUTEndpoint,
                              psDevice->psPrivateData->ucOUTDMA);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! This function should be called once for the audio class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ulIndex is the index of the USB controller to initialize for
//! audio class device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the audio device.
//!
//! In order for an application to initialize the USB audio device class, it
//! must first call this function with the a valid audio device class structure
//! in the \e psDevice parameter.  This allows this function to initialize the
//! USB controller and device code to be prepared to enumerate and function as
//! a USB audio device.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the audio class.
//!
//! See the documentation on the tUSBDAudioDevice structure for more
//! information on how to properly fill the structure members.
//!
//! \return Returns 0 on failure or a non-zero void pointer on success.
//
//*****************************************************************************
void *
USBDAudioCompositeInit(unsigned long ulIndex, const tUSBDAudioDevice *psDevice)
{
    tAudioInstance *psInst;
    tDeviceDescriptor *psDevDesc;

    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateData);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = psDevice->psPrivateData;
    psInst->psConfDescriptor = (tConfigDescriptor *)g_pAudioDescriptor;
    psInst->psDevInfo = &g_sAudioDeviceInfo;
    psInst->ulUSBBase = USB0_BASE;

    //
    // The Control interface is at index 0.
    //
    psInst->ucInterfaceControl = AUDIO_INTERFACE_CONTROL;

    //
    // The Audio interface is at index 1.
    //
    psInst->ucInterfaceAudio = AUDIO_INTERFACE_OUTPUT;

    //
    // Set the default Isochronous OUT endpoint.
    //
    psInst->ucOUTEndpoint = ISOC_OUT_ENDPOINT;
    psInst->ucOUTDMA = ISOC_OUT_DMA_CHANNEL;

    //
    // Set the initial buffer to null.
    //
    psInst->sBuffer.pvData = 0;

    //
    // Save the volume settings.
    //
    psInst->sVolumeMax = psDevice->sVolumeMax;
    psInst->sVolumeMin = psDevice->sVolumeMin;
    psInst->sVolumeStep = psDevice->sVolumeStep;

    //
    // No update pending to any command.
    //
    psInst->usUpdate = 0;

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
    psInst->psDevInfo->ulNumStringDescriptors =
        psDevice->ulNumStringDescriptors;
    psInst->psDevInfo->pvInstance = (void *)psDevice;

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! Shuts down the audio device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDAudioInit().
//!
//! This function terminates audio interface for the instance supplied.  This
//! function should not be called if the audio device is part of a composite
//! device and instead the USBDCompositeTerm() function should be called for
//! the full composite device.
//! Following this call, the \e pvInstance instance should not me used in any
//! other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDAudioTerm(void *pvInstance)
{
    ASSERT(pvInstance != 0);

    //
    // Cleanly exit device mode.
    //
    USBDCDTerm(0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvInstance is the instance data for this request.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure to the type of request and
// will respond to all commands that are understood by the class.
//
// \return None.
//
//*****************************************************************************
static void
HandleRequests(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned long ulControl;
    unsigned long ulRecipient;
    unsigned long ulStall;
    tAudioInstance *psInst;
    const tUSBDAudioDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // Make a copy of this pointer for ease of use in this function.
    //
    psInst = psDevice->psPrivateData;

    //
    // Make sure to acknowledge that the data was read, this will not send and
    // ACK that has already been done at this point.  This just tells the
    // hardware that the data was read.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // Don't stall by default.
    //
    ulStall = 0;

    //
    // Get the request type.
    //
    ulRecipient = pUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M;

    //
    // Save the request type and request value.
    //
    psInst->usRequestType = pUSBRequest->bmRequestType;
    psInst->ucRequest = pUSBRequest->bRequest;

    //
    // Check if this is an endpoint request to the audio streaming endpoint.
    //
    if((ulRecipient == USB_RTYPE_ENDPOINT) &&
       (pUSBRequest->wIndex == USB_EP_TO_INDEX(psInst->ucOUTEndpoint)))
    {
        //
        // Determine the type of request.
        //
        switch(psInst->ucRequest)
        {
            case USB_AC_SET_CUR:
            {
                //
                // Handle retrieving the sample rate.
                //
                if(pUSBRequest->wValue == SAMPLING_FREQ_CONTROL)
                {
                    //
                    // Retrieve the requested sample rate.
                    //
                    USBDCDRequestDataEP0(0,
                                         (unsigned char *)&psInst->ulSampleRate,
                                         3);

                    //
                    // Save what we are updating.
                    //
                    psInst->usUpdate = SAMPLING_FREQ_CONTROL;
                }
                break;
            }
            case USB_AC_GET_CUR:
            {
                //
                // Handle retrieving the sample rate.
                //
                if(pUSBRequest->wValue == SAMPLING_FREQ_CONTROL)
                {
                    //
                    // Send back the current sample rate.
                    //
                    USBDCDSendDataEP0(0,
                                      (unsigned char *)&psInst->ulSampleRate,
                                      3);
                }
                break;
            }
            default:
            {
                //
                // Stall on unknown commands.
                //
                ulStall = 1;
                break;
            }
        }
    }
    else if(ulRecipient == USB_RTYPE_INTERFACE)
    {
        //
        // Make sure the request was for the control interface.
        //
        if((unsigned char)pUSBRequest->wIndex != psInst->ucInterfaceControl)
        {
            return;
        }

        //
        // Extract the control value from the message.
        //
        ulControl = pUSBRequest->wValue & USB_CS_CONTROL_M;

        //
        // Handle an audio control request to the feature control unit.
        //
        if((AUDIO_CONTROL_ID << 8) == (pUSBRequest->wIndex & USB_CS_CONTROL_M))
        {
            //
            // Determine the type of request.
            //
            switch(psInst->ucRequest)
            {
                case USB_AC_GET_MAX:
                {
                    if(ulControl == VOLUME_CONTROL)
                    {
                        //
                        // Return the maximum volume setting.
                        //
                        USBDCDSendDataEP0(0,
                                          (unsigned char *)&psInst->sVolumeMax,
                                          2);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ulStall = 1;
                    }
                    break;
                }
                case USB_AC_GET_MIN:
                {
                    if(ulControl == VOLUME_CONTROL)
                    {
                        //
                        // Return the minimum volume setting.
                        //
                        USBDCDSendDataEP0(0,
                                          (unsigned char *)&psInst->sVolumeMin,
                                          2);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ulStall = 1;
                    }
                    break;
                }
                case USB_AC_GET_RES:
                {
                    if(ulControl == VOLUME_CONTROL)
                    {
                        //
                        // Return the volume step setting.
                        //
                        USBDCDSendDataEP0(0,
                                          (unsigned char *)&psInst->sVolumeStep,
                                          2);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ulStall = 1;
                    }
                    break;
                }
                case USB_AC_GET_CUR:
                {
                    if(ulControl == VOLUME_CONTROL)
                    {
                        //
                        // Send back the current volume level.
                        //
                        USBDCDSendDataEP0(0,
                                          (unsigned char *)&psInst->sVolume,
                                          2);
                    }
                    else if(ulControl == MUTE_CONTROL)
                    {
                        //
                        // Send back the current mute value.
                        //
                        USBDCDSendDataEP0(0,
                                          (unsigned char *)&psInst->ucMute, 1);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ulStall = 1;
                    }
                    break;
                }
                case USB_AC_SET_CUR:
                {
                    if(ulControl == VOLUME_CONTROL)
                    {
                        //
                        // Read the new volume level.
                        //
                        USBDCDRequestDataEP0(0,
                                             (unsigned char *)&psInst->sVolume,
                                             2);

                        //
                        // Save what we are updating.
                        //
                        psInst->usUpdate = VOLUME_CONTROL;
                    }
                    else if(ulControl == MUTE_CONTROL)
                    {
                        //
                        // Read the new mute setting.
                        //
                        USBDCDRequestDataEP0(0,
                                             (unsigned char *)&psInst->ucMute,
                                             1);

                        //
                        // Save what we are updating.
                        //
                        psInst->usUpdate = MUTE_CONTROL;
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ulStall = 1;
                    }
                    break;
                }
                case USB_AC_SET_RES:
                {
                    if(ulControl == VOLUME_CONTROL)
                    {
                        //
                        // Read the new volume step setting.
                        //
                        USBDCDRequestDataEP0(0,
                            (unsigned char *)&psInst->sVolumeStep, 2);

                        //
                        // Save what we are updating.
                        //
                        psInst->usUpdate = VOLUME_CONTROL;
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ulStall = 1;
                    }
                    break;
                }
                default:
                {
                    //
                    // Stall on unknown commands.
                    //
                    ulStall = 1;
                    break;
                }
            }
        }
    }

    //
    // Stall on all unknown commands.
    //
    if(ulStall)
    {
        USBDCDStallEP0(0);
    }
}

//*****************************************************************************
//
//! This function is used to supply buffers to the audio class to be filled
//! from the USB host device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDAudioInit() or USBDAudioInitComposite().
//! \param pvBuffer is a pointer to the buffer to fill with audio data.
//! \param ulSize is the size in bytes of the buffer pointed to by the pvBuffer
//! parameter.
//! \param pfnCallback is a callback that will provide notification when this
//! buffer has valid data.
//!
//! This function fills the buffer pointed to by the \e pvBuffer parameter with
//! at most \e ulSize one packet of data from the host controller.  The ulSize
//! has a minimum value of \b ISOC_OUT_EP_MAX_SIZE since each USB packet can be
//! at most \b ISOC_OUT_EP_MAX_SIZE bytes in size.  Since the audio data may
//! not be received in amounts that evenly fit in the buffer provided, the
//! buffer may not be completely filled.  The \e pfnCallback function will
//! provide the amount of valid data that was actually stored in the buffer
//! provided.  The function will return zero if the buffer could be scheduled
//! to be filled, otherwise the function will return a non-zero value if there
//! was some reason that the buffer could not be added.
//!
//! \return Returns 0 to indicate success any other value indicates that the
//! buffer will not be filled.
//
//*****************************************************************************
long
USBAudioBufferOut(void *pvInstance, void *pvBuffer, unsigned long ulSize,
                  tUSBAudioBufferCallback pfnCallback)
{
    tAudioInstance *psInst;
    const tUSBDAudioDevice *psDevice;

    //
    // Make sure we were not passed NULL pointers.
    //
    ASSERT(pvInstance != 0);
    ASSERT(pvBuffer != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDAudioDevice *)pvInstance;

    //
    // Buffer must be at least one packet in size.
    //
    ASSERT(ulSize >= ISOC_OUT_EP_MAX_SIZE);
    ASSERT(pfnCallback);

    //
    // Create a pointer of the correct type from the private pointer.
    //
    psInst = psDevice->psPrivateData;

    //
    // Initialize the buffer instance.
    //
    psInst->sBuffer.pvData = pvBuffer;
    psInst->sBuffer.ulSize = ulSize;
    psInst->sBuffer.ulNumBytes = 0;
    psInst->sBuffer.pfnCallback = pfnCallback;

    //
    // Configure and enable DMA for the OUT transfer.
    //
    MAP_uDMAChannelTransferSet(psInst->ucOUTDMA, UDMA_MODE_BASIC,
                               (void *)USBFIFOAddrGet(USB0_BASE,
                                                      psInst->ucOUTEndpoint),
                               psInst->sBuffer.pvData, ulSize >> 2);

    //
    // Start the DMA transfer.
    //
    MAP_uDMAChannelEnable(psInst->ucOUTDMA);

    return(0);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

