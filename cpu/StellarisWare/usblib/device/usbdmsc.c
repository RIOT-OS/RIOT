//*****************************************************************************
//
// usbdmsc.c - USB mass storage device class driver.
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
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"
#include "usblib/usblib.h"
#include "usblib/usbmsc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdmsc.h"

//*****************************************************************************
//
//! \addtogroup msc_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These are the internal flags used with the ulFlags member variable.
//
//*****************************************************************************
#define USBD_FLAG_DMA_IN        0x00000001
#define USBD_FLAG_DMA_OUT       0x00000002

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
// These are fields that are used by the USB descriptors for the Mass Storage
// Class.
//
//*****************************************************************************
#define USB_MSC_SUBCLASS_SCSI   0x6
#define USB_MSC_PROTO_BULKONLY  0x50

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define DATA_IN_ENDPOINT        USB_EP_1
#define DATA_IN_DMA_CHANNEL     UDMA_CHANNEL_USBEP1TX
#define DATA_OUT_ENDPOINT       USB_EP_1
#define DATA_OUT_DMA_CHANNEL    UDMA_CHANNEL_USBEP1RX

//*****************************************************************************
//
// Maximum packet size for the bulk endpoints is 64 bytes.
//
//*****************************************************************************
#define DATA_IN_EP_MAX_SIZE     64
#define DATA_OUT_EP_MAX_SIZE    64

//*****************************************************************************
//
// These defines control the sizes of USB transfers for data and commands.
//
//*****************************************************************************
#define MAX_TRANSFER_SIZE       512
#define COMMAND_BUFFER_SIZE     64

//*****************************************************************************
//
// The local buffer used to read in commands and process them.
//
//*****************************************************************************
static unsigned char g_pucCommand[COMMAND_BUFFER_SIZE];

//*****************************************************************************
//
// The current transfer state is held in these variables.
//
//*****************************************************************************
static tMSCCSW g_sSCSICSW;

//*****************************************************************************
//
// The current state for the SCSI commands that are being handled and are
// stored in the tMSCInstance.ucSCSIState structure member.
//
//*****************************************************************************

//
// No command in process.
//
#define STATE_SCSI_IDLE             0x00

//
// Sending and reading logical blocks.
//
#define STATE_SCSI_SEND_BLOCKS      0x01

//
// Receiving and writing logical blocks.
//
#define STATE_SCSI_RECEIVE_BLOCKS   0x02

//
// Send the status once the previous transfer is complete.
//
#define STATE_SCSI_SEND_STATUS      0x03

//
// Status was prepared to be sent and now waiting for it to have gone out.
//
#define STATE_SCSI_SENT_STATUS      0x04

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
static unsigned char g_pMSCDeviceDescriptor[] =
{
    18,                     // Size of this structure.
    USB_DTYPE_DEVICE,       // Type of this structure.
    USBShort(0x110),        // USB version 1.1 (if we say 2.0, hosts assume
                            // high-speed - see USB 2.0 spec 9.2.6.6)
    0,                      // USB Device Class (spec 5.1.1)
    0,                      // USB Device Sub-class (spec 5.1.1)
    0,                      // USB Device protocol (spec 5.1.1)
    64,                     // Maximum packet size for default pipe.
    USBShort(0),            // Vendor ID (filled in during USBDCDCInit).
    USBShort(0),            // Product ID (filled in during USBDCDCInit).
    USBShort(0x100),        // Device Version BCD.
    1,                      // Manufacturer string identifier.
    2,                      // Product string identifier.
    3,                      // Product serial number.
    1                       // Number of configurations.
};

//*****************************************************************************
//
// Mass storage device configuration descriptor.
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
static unsigned char g_pMSCDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(32),               // The total size of this full structure.
    1,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    0,                          // The string identifier that describes this
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
const unsigned char g_pMSCInterface[] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,                          // The index for this interface.
    0,                          // The alternate setting for this interface.
    2,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_MASS_STORAGE,     // The interface class
    USB_MSC_SUBCLASS_SCSI,      // The interface sub-class.
    USB_MSC_PROTO_BULKONLY,     // The interface protocol for the sub-class
                                // specified above.
    0,                          // The string index for this interface.

    //
    // Endpoint Descriptor
    //
    7,                               // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,              // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USB_EP_TO_INDEX(DATA_IN_ENDPOINT),
    USB_EP_ATTR_BULK,                // Endpoint is a bulk endpoint.
    USBShort(DATA_IN_EP_MAX_SIZE),   // The maximum packet size.
    0,                               // The polling interval for this endpoint.

    //
    // Endpoint Descriptor
    //
    7,                               // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,              // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | USB_EP_TO_INDEX(DATA_OUT_ENDPOINT),
    USB_EP_ATTR_BULK,                // Endpoint is a bulk endpoint.
    USBShort(DATA_OUT_EP_MAX_SIZE),  // The maximum packet size.
    0,                               // The polling interval for this endpoint.
};

//*****************************************************************************
//
// The mass storage configuration descriptor is defined as two sections,
// one containing just the 9 byte USB configuration descriptor and the other
// containing everything else that is sent to the host along with it.
//
//*****************************************************************************
const tConfigSection g_sMSCConfigSection =
{
    sizeof(g_pMSCDescriptor),
    g_pMSCDescriptor
};

const tConfigSection g_sMSCInterfaceSection =
{
    sizeof(g_pMSCInterface),
    g_pMSCInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete bulk device configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psMSCSections[] =
{
    &g_sMSCConfigSection,
    &g_sMSCInterfaceSection
};

#define NUM_MSC_SECTIONS (sizeof(g_psMSCSections) / sizeof(tConfigSection *))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sMSCConfigHeader =
{
    NUM_MSC_SECTIONS,
    g_psMSCSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_pMSCConfigDescriptors[] =
{
    &g_sMSCConfigHeader
};

//*****************************************************************************
//
// Various internal handlers needed by this class.
//
//*****************************************************************************
static void HandleDisconnect(void *pvInstance);
static void ConfigChangeHandler(void *pvInstance, unsigned long ulValue);
static void HandleEndpoints(void *pvInstance, unsigned long ulStatus);
static void HandleRequests(void *pvInstance, tUSBRequest *pUSBRequest);
static void USBDSCSISendStatus(const tUSBDMSCDevice *psDevice);
unsigned long USBDSCSICommand(const tUSBDMSCDevice *psDevice,
                              tMSCCBW *pSCSICBW);
static void HandleDevice(void *pvInstance, unsigned long ulRequest,
                         void *pvRequestData);

//*****************************************************************************
//
// The FIFO configuration for USB mass storage class device.
//
//*****************************************************************************
const tFIFOConfig g_sUSBMSCFIFOConfig =
{
    //
    // IN endpoints.
    //
    {
        { false, USB_EP_DEV_IN | USB_EP_DMA_MODE_1 | USB_EP_AUTO_SET },
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
// The device information structure for the USB MSC device.
//
//*****************************************************************************
tDeviceInfo g_sMSCDeviceInfo =
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
        0,

        //
        // ConfigChange
        //
        ConfigChangeHandler,

        //
        // DataReceived
        //
        0,

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
    g_pMSCDeviceDescriptor,
    g_pMSCConfigDescriptors,
    0,
    0,
    &g_sUSBMSCFIFOConfig
};

//*****************************************************************************
//
//! This function is used by an application if it can detect insertion or
//! removal of the media.
//!
//! \param pvInstance is the mass storage device instance that had a media
//! change.
//! \param eMediaStatus is the updated status for the media.
//!
//! This function should be called by an application when it detects a change
//! in the status of the media in use by the USB mass storage class.  The
//! \e eMediaStatus parameter will indicate the new status of the media and
//! can also indicate that the application has no knowledge of the media state.
//!
//! There are currently the three following values for the \e eMediaStatus
//! parameter:
//! - USBDMSC_MEDIA_PRESENT indicates that the media is present or has been
//! added.
//! - USBDMSC_MEDIA_NOTPRESENT indicates that the media is not present or was
//! removed.
//! - USBDMSC_MEDIA_UNKNOWN indicates that the application has no knowledge of
//! the media state and the USB mass storage class.
//!
//! It will be left up to the application to call this function whenever it
//! detects a change or simply call it once with USBDMSC_MEDIA_UNKNOWN and
//! allow the mass storage class to infer the state from the remaining device
//! APIs.
//!
//! \note It is recommended that the application use this function to inform
//! the mass storage class of media state changes as it will lead to a more
//! responsive system.
//!
//! \return None.
//
//*****************************************************************************
void
USBDMSCMediaChange(void *pvInstance, tUSBDMSCMediaStatus eMediaStatus)
{
    const tUSBDMSCDevice *psDevice;

    //
    // Create a device instance pointer.
    //
    psDevice = pvInstance;

    //
    // Save the current media status.
    //
    psDevice->psPrivateData->eMediaStatus = eMediaStatus;
}

//*****************************************************************************
//
// This function is called to handle the interrupts on the Bulk endpoints for
// the mass storage class.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvInstance, unsigned long ulStatus)
{
    const tUSBDMSCDevice *psDevice;
    tMSCInstance *psInst;
    tMSCCBW *pSCSICBW;
    unsigned long ulEPStatus;
    unsigned long ulSize;

    ASSERT(pvInstance != 0);

    //
    // Determine if the serial device is in single or composite mode because
    // the meaning of ulIndex is different in both cases.
    //
    psDevice = pvInstance;

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = psDevice->psPrivateData;

    //
    // Get the endpoints status.
    //
    ulEPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ucOUTEndpoint);

    //
    // Handler for the bulk IN data endpoint.
    //
    if((ulStatus & (1 << USB_EP_TO_INDEX(psInst->ucINEndpoint))) ||
       ((psInst->ulFlags & USBD_FLAG_DMA_IN) &&
        (MAP_uDMAChannelModeGet(psInst->ucINDMA) == UDMA_MODE_STOP)))
    {
        switch(psInst->ucSCSIState)
        {
            //
            // Handle the case where we are sending out data due to a read
            // command.
            //
            case STATE_SCSI_SEND_BLOCKS:
            {
                //
                // Decrement the number of bytes left to send.
                //
                psInst->ulBytesToTransfer -= MAX_TRANSFER_SIZE;

                //
                // If we are done then move on to the status phase.
                //
                if(psInst->ulBytesToTransfer == 0)
                {
                    //
                    // Set the status so that it can be sent when this
                    // response has has be successfully sent.
                    //
                    g_sSCSICSW.bCSWStatus = 0;
                    g_sSCSICSW.dCSWDataResidue = 0;

                    //
                    // DMA has completed for the IN endpoint.
                    //
                    psInst->ulFlags &= ~USBD_FLAG_DMA_IN;

                    //
                    // Disable uDMA on the endpoint
                    //
                    MAP_USBEndpointDMADisable(USB0_BASE, psInst->ucINEndpoint,
                                              USB_EP_DEV_IN);

                    //
                    // Send back the status once this transfer is complete.
                    //
                    psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;

                    if(psDevice->pfnEventCallback)
                    {
                        psDevice->pfnEventCallback(0, USBD_MSC_EVENT_IDLE, 0,
                                                   0);
                    }

                    //
                    // The transfer is complete so don't read anymore data.
                    //
                    break;
                }

                //
                // Move on to the next Logical Block.
                //
                psInst->ulCurrentLBA++;

                //
                // Read the new data and send it out.
                //
                if(psDevice->sMediaFunctions.BlockRead(psInst->pvMedia,
                        (unsigned char *)psInst->pulBuffer,
                        psInst->ulCurrentLBA, 1) == 0)
                {
                }

                //
                // Reset the DMA transfer and enable the DMA channel.
                //
                MAP_uDMAChannelTransferSet(psInst->ucINDMA,
                                           UDMA_MODE_BASIC,
                                           psInst->pulBuffer,
                                           (void *)USBFIFOAddrGet(USB0_BASE,
                                               psInst->ucINEndpoint),
                                           (MAX_TRANSFER_SIZE >> 2));
                MAP_uDMAChannelEnable(psInst->ucINDMA);

                break;
            }

            //
            // Handle sending status.
            //
            case STATE_SCSI_SEND_STATUS:
            {
                //
                // Indicate success and no extra data coming.
                //
                USBDSCSISendStatus(psDevice);

                break;
            }

            //
            // Handle completing sending status.
            //
            case STATE_SCSI_SENT_STATUS:
            {
                psInst->ucSCSIState = STATE_SCSI_IDLE;

                break;
            }

            //
            // These cases should not occur as the being in the IDLE state due
            // to an IN interrupt is invalid.
            //
            case STATE_SCSI_IDLE:
            default:
            {
                break;
            }
        }
    }

    //
    // Handler for the bulk OUT data endpoint.
    //
    if((ulStatus & (0x10000 << USB_EP_TO_INDEX(psInst->ucOUTEndpoint))) ||
        ((psInst->ulFlags & USBD_FLAG_DMA_OUT) &&
         (MAP_uDMAChannelModeGet(psInst->ucOUTDMA) == UDMA_MODE_STOP)))
    {
        //
        // Get the endpoint status to see why we were called.
        //
        ulEPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ucOUTEndpoint);

        switch(psInst->ucSCSIState)
        {
            //
            // Receiving and writing bytes to the storage device.
            //
            case STATE_SCSI_RECEIVE_BLOCKS:
            {
                //
                // Update the current status for the buffer.
                //
                psInst->ulBytesToTransfer -= MAX_TRANSFER_SIZE;

                //
                // Write the new data.
                //
                psDevice->sMediaFunctions.BlockWrite(psInst->pvMedia,
                    (unsigned char *)psInst->pulBuffer,
                    psInst->ulCurrentLBA, 1);

                //
                // Move on to the next Logical Block.
                //
                psInst->ulCurrentLBA++;

                //
                // Check if all bytes have been received.
                //
                if(psInst->ulBytesToTransfer == 0)
                {
                    //
                    // Set the status so that it can be sent when this response
                    // has be successfully sent.
                    //
                    g_sSCSICSW.bCSWStatus = 0;
                    g_sSCSICSW.dCSWDataResidue = 0;

                    //
                    // DMA has completed for the OUT endpoint.
                    //
                    psInst->ulFlags &= ~USBD_FLAG_DMA_OUT;

                    //
                    // Indicate success and no extra data coming.
                    //
                    USBDSCSISendStatus(psDevice);

                    //
                    // Disable uDMA on the endpoint
                    //
                    MAP_USBEndpointDMADisable(USB0_BASE, psInst->ucOUTEndpoint,
                                              USB_EP_DEV_OUT);

                    //
                    // If there is an event callback then call it to notify
                    // that last operation has completed.
                    //
                    if(psDevice->pfnEventCallback)
                    {
                        psDevice->pfnEventCallback(0, USBD_MSC_EVENT_IDLE, 0,
                                                   0);
                    }
                }
                else
                {
                    //
                    // Configure and enable DMA for the OUT transfer.
                    //
                    MAP_uDMAChannelTransferSet(psInst->ucOUTDMA,
                                               UDMA_MODE_BASIC,
                                               (void *)USBFIFOAddrGet(USB0_BASE,
                                                   psInst->ucOUTEndpoint),
                                               psInst->pulBuffer,
                                               (MAX_TRANSFER_SIZE >> 2));

                    //
                    // Start the DMA transfer.
                    //
                    MAP_uDMAChannelEnable(psInst->ucOUTDMA);
                }

                break;
            }

            //
            // If there is an OUT transfer in idle state then it was a new
            // command.
            //
            case STATE_SCSI_IDLE:
            {
                //
                // Attempt to handle the new command.
                //

                //
                // Receive the command.
                //
                ulSize = COMMAND_BUFFER_SIZE;
                MAP_USBEndpointDataGet(psInst->ulUSBBase, psInst->ucOUTEndpoint,
                                       g_pucCommand, &ulSize);
                pSCSICBW = (tMSCCBW *)g_pucCommand;

                //
                // Acknowledge the OUT data packet.
                //
                MAP_USBDevEndpointDataAck(psInst->ulUSBBase,
                                          psInst->ucOUTEndpoint,
                                          false);

                //
                // If this is a valid CBW then handle it.
                //
                if(pSCSICBW->dCBWSignature == CBW_SIGNATURE)
                {
                    g_sSCSICSW.dCSWSignature = CSW_SIGNATURE;
                    g_sSCSICSW.dCSWTag = pSCSICBW->dCBWTag;
                    g_sSCSICSW.dCSWDataResidue = 0;
                    g_sSCSICSW.bCSWStatus = 0;

                    USBDSCSICommand(psDevice, pSCSICBW);
                }
                else
                {
                    //
                    // Just return to the idle state since we are now out of
                    // sync with the host.  This should not happen, but this
                    // should allow the device to synchronize with the host
                    // controller.
                    //
                    psInst->ucSCSIState = STATE_SCSI_IDLE;
                }

                break;
            }
            default:
            {
                break;
            }
        }

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
    tMSCInstance *psInst;
    unsigned char *pucData;

    //
    // Create the serial instance data.
    //
    psInst = ((tUSBDMSCDevice *)pvInstance)->psPrivateData;

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
                psInst->ucINEndpoint = INDEX_TO_USB_EP((pucData[1] & 0x7f));

                psInst->ucINDMA = UDMA_CHANNEL_USBEP1TX +
                                  (((pucData[1] & 0x7f) - 1) * 2);

                //
                // Basic configuration for DMA on the IN endpoint.
                //
                MAP_uDMAChannelControlSet(psInst->ucINDMA,
                                          (UDMA_SIZE_32 | UDMA_SRC_INC_32|
                                           UDMA_DST_INC_NONE | UDMA_ARB_16));

                //
                // Select this channel for this endpoint, this only affects
                // devices that have this feature.
                //
                MAP_USBEndpointDMAChannel(USB0_BASE, psInst->ucINEndpoint,
                                          psInst->ucINDMA);
            }
            else
            {
                //
                // Extract the new endpoint number.
                //
                psInst->ucOUTEndpoint = INDEX_TO_USB_EP(pucData[1] & 0x7f);
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
    const tUSBDMSCDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDMSCDevice *)pvInstance;

    //
    // Close the drive requested.
    //
    if(psDevice->psPrivateData->pvMedia != 0)
    {
        psDevice->psPrivateData->pvMedia = 0;
        psDevice->sMediaFunctions.Close(0);
    }

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psDevice->pfnEventCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psDevice->pfnEventCallback(pvInstance, USB_EVENT_DISCONNECTED, 0, 0);
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
    tMSCInstance *psInst;
    const tUSBDMSCDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Create the instance pointer.
    //
    psDevice = (const tUSBDMSCDevice *)pvInstance;

    //
    // Create the serial instance data.
    //
    psInst = psDevice->psPrivateData;

    //
    // Insure that DMA is disable whenever the configuration is set.
    //
    MAP_USBEndpointDMADisable(USB0_BASE, psInst->ucINEndpoint, USB_EP_DEV_IN);
    MAP_USBEndpointDMADisable(USB0_BASE, psInst->ucOUTEndpoint, USB_EP_DEV_OUT);

    //
    // Basic configuration for DMA on the OUT endpoint.
    //
    MAP_uDMAChannelControlSet(psInst->ucOUTDMA, UDMA_SIZE_32 |
                                                UDMA_SRC_INC_NONE|
                                                UDMA_DST_INC_32 |
                                                UDMA_ARB_16);

    //
    // Select this channel for this endpoint, this only affects devices that
    // have this feature.
    //
    MAP_USBEndpointDMAChannel(USB0_BASE, psInst->ucOUTEndpoint, psInst->ucOUTDMA);

    //
    // Basic configuration for DMA on the IN endpoint.
    //
    MAP_uDMAChannelControlSet(psInst->ucINDMA, UDMA_SIZE_32 | UDMA_SRC_INC_32|
                                               UDMA_DST_INC_NONE | UDMA_ARB_16);

    //
    // Select this channel for this endpoint, this only affects devices that
    // have this feature.
    //
    MAP_USBEndpointDMAChannel(USB0_BASE, psInst->ucINEndpoint, psInst->ucINDMA);

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psDevice->pfnEventCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psDevice->pfnEventCallback(pvInstance, USB_EVENT_CONNECTED, 0, 0);
    }
}

//*****************************************************************************
//
//! This function should be called once for the mass storage class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ulIndex is the index of the USB controller to initialize for
//! mass storage class device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the mass storage device.
//!
//! In order for an application to initialize the USB device mass storage
//! class, it must first call this function with the a valid mass storage
//! device class structure in the \e psDevice parameter.  This allows this
//! function to initialize the USB controller and device code to be prepared to
//! enumerate and function as a USB mass storage device.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the mass storage class.
//!
//! See the documentation on the tUSBDMSCDevice structure for more information
//! on how to properly fill the structure members.
//!
//! \return Returns 0 on failure or a non-zero void pointer on success.
//
//*****************************************************************************
void *
USBDMSCInit(unsigned long ulIndex, const tUSBDMSCDevice *psDevice)
{
    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateData);

    USBDMSCCompositeInit(ulIndex, psDevice);

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the bulk device on the bus.
    //
    USBDCDInit(ulIndex, psDevice->psPrivateData->psDevInfo);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! This function should be called once for the mass storage class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ulIndex is the index of the USB controller to initialize for
//! mass storage class device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the mass storage device.
//!
//! In order for an application to initialize the USB device mass storage
//! class, it must first call this function with the a valid mass storage
//! device class structure in the \e psDevice parameter.  This allows this
//! function to initialize the USB controller and device code to be prepared to
//! enumerate and function as a USB mass storage device.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the mass storage class.
//!
//! See the documentation on the tUSBDMSCDevice structure for more information
//! on how to properly fill the structure members.
//!
//! \return Returns 0 on failure or a non-zero void pointer on success.
//
//*****************************************************************************
void *
USBDMSCCompositeInit(unsigned long ulIndex, const tUSBDMSCDevice *psDevice)
{
    tMSCInstance *psInst;
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
    psInst->psConfDescriptor = (tConfigDescriptor *)g_pMSCDescriptor;
    psInst->psDevInfo = &g_sMSCDeviceInfo;
    psInst->ulUSBBase = USB0_BASE;
    psInst->bConnected = false;
    psInst->eMediaStatus = USBDMSC_MEDIA_UNKNOWN;

    //
    // Set the initial interface and endpoints.
    //
    psInst->ucInterface = 0;
    psInst->ucOUTEndpoint = DATA_OUT_ENDPOINT;
    psInst->ucOUTDMA = DATA_OUT_DMA_CHANNEL;
    psInst->ucINEndpoint = DATA_IN_ENDPOINT;
    psInst->ucINDMA = DATA_IN_DMA_CHANNEL;

    //
    // Set the initial SCSI state to idle.
    //
    psInst->ucSCSIState = STATE_SCSI_IDLE;

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
    psInst->psDevInfo->ulNumStringDescriptors
        = psDevice->ulNumStringDescriptors;
    psInst->psDevInfo->pvInstance = (void *)psDevice;

    //
    // If DMA is in use then clear all DMA attributes.
    //
    MAP_uDMAChannelAttributeDisable(psInst->ucINDMA, UDMA_ATTR_ALL);
    MAP_uDMAChannelAttributeDisable(psInst->ucOUTDMA, UDMA_ATTR_ALL);

    //
    // Open the drive requested.
    //
    psInst->pvMedia = psDevice->sMediaFunctions.Open(0);

    if(psInst->pvMedia == 0)
    {
        //
        // There is no media currently present.
        //
        psInst->ucSenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->usAddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }
    else
    {
        //
        // Media is now ready for use.
        //
        psInst->ucSenseKey = SCSI_RS_KEY_UNIT_ATTN;
        psInst->usAddSenseCode = SCSI_RS_MED_NOTRDY2RDY;
    }

    //
    // Enable Clocking to the USB controller.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Turn on USB Phy clock.
    //
    MAP_SysCtlUSBPLLEnable();

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! Shuts down the mass storage device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDMSCInit() or USBDMSCInitComposite().
//!
//! This function terminates mass storage operation for the instance supplied
//! and removes the device from the USB bus.  Following this call, the
//! \e psDevice instance may not me used in any other call to the mass storage
//! device other than USBDMSCInit() or USBDMSCInitComposite().
//!
//! \return None.
//
//*****************************************************************************
void
USBDMSCTerm(void *pvInstance)
{
    const tUSBDMSCDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Cleanly exit device mode.
    //
    USBDCDTerm(0);

    //
    // Create a device instance pointer.
    //
    psDevice = pvInstance;

    //
    // If the media was opened the close it out.
    //
    if(psDevice->psPrivateData->pvMedia != 0)
    {
        psDevice->psPrivateData->pvMedia = 0;
        psDevice->sMediaFunctions.Close(0);
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvInstance is instance data for this request.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure to determine the command.
// The only mass storage command supported over endpoint 0 is the Get Max LUN
// command.
//
// \return None.
//
//*****************************************************************************
static void
HandleRequests(void *pvInstance, tUSBRequest *pUSBRequest)
{
    //
    // This class only support a single LUN.
    //
    const static unsigned char ucMaxLun = 0;

    ASSERT(pvInstance != 0);

    //
    // Determine the type of request.
    //
    switch(pUSBRequest->bRequest)
    {
        //
        // A Set Report request is received from the host when it sends an
        // Output report via endpoint 0.
        //
        case USBREQ_GET_MAX_LUN:
        {
            //
            // Send our response to the host.
            //
            USBDCDSendDataEP0(0, (unsigned char *)&ucMaxLun, 1);

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
// This function is used to handle the SCSI Inquiry command when it is received
// from the host.
//
//*****************************************************************************
static void
USBDSCSIInquiry(const tUSBDMSCDevice *psDevice)
{
    long lIdx;
    tMSCInstance *psInst;
    unsigned long *pulData;

    //
    // Create a local unsigned long pointer to the command.
    //
    pulData = (unsigned long *)g_pucCommand;

    //
    // Create the serial instance data.
    //
    psInst = psDevice->psPrivateData;

    //
    // Direct Access device, Removable storage and SCSI 1 responses.
    //
    pulData[0] = SCSI_INQ_PDT_SBC | (SCSI_INQ_RMB << 8);

    //
    // Additional Length is fixed at 31 bytes.
    //
    pulData[1] = 31;

    //
    // Copy the Vendor string.
    //
    for(lIdx = 0; lIdx < 8; lIdx++)
    {
        g_pucCommand[lIdx + 8] = psDevice->pucVendor[lIdx];
    }

    //
    // Copy the Product string.
    //
    for(lIdx = 0; lIdx < 16; lIdx++)
    {
        g_pucCommand[lIdx + 16] = psDevice->pucProduct[lIdx];
    }

    //
    // Copy the Version string.
    //
    for(lIdx = 0; lIdx < 4; lIdx++)
    {
        g_pucCommand[lIdx + 32] = psDevice->pucVersion[lIdx];
    }

    //
    // Send the SCSI Inquiry Response.
    //
    MAP_USBEndpointDataPut(USB0_BASE, psInst->ucINEndpoint, g_pucCommand, 36);

    //
    // Send the data to the host.
    //
    MAP_USBEndpointDataSend(USB0_BASE, psInst->ucINEndpoint, USB_TRANS_IN);

    //
    // Set the status so that it can be sent when this response has
    // has be successfully sent.
    //
    g_sSCSICSW.bCSWStatus = 0;
    g_sSCSICSW.dCSWDataResidue = 0;

    psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read Capacities command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIReadCapacities(const tUSBDMSCDevice *psDevice)
{
    unsigned long ulBlocks;
    tMSCInstance *psInst;
    unsigned long *pulData;

    //
    // Create a local unsigned long pointer to the command.
    //
    pulData = (unsigned long *)g_pucCommand;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    if(psInst->pvMedia != 0)
    {
        ulBlocks = psDevice->sMediaFunctions.NumBlocks(psInst->pvMedia);

        pulData[0] = 0x08000000;

        //
        // Fill in the number of blocks, the bytes endianness must be changed.
        //
        g_pucCommand[4] = ulBlocks >> 24;
        g_pucCommand[5] = 0xff & (ulBlocks >> 16);
        g_pucCommand[6] = 0xff & (ulBlocks >> 8);
        g_pucCommand[7] = 0xff & (ulBlocks);

        //
        // Current media capacity
        //
        g_pucCommand[8] = 0x2;

        //
        // Fill in the block size, which is fixed at DEVICE_BLOCK_SIZE.
        //
        g_pucCommand[9] = 0xff & (DEVICE_BLOCK_SIZE >> 16);
        g_pucCommand[10] = 0xff & (DEVICE_BLOCK_SIZE >> 8);
        g_pucCommand[11] = 0xff & DEVICE_BLOCK_SIZE;

        //
        // Send out the 12 bytes that are in this response.
        //
        MAP_USBEndpointDataPut(USB0_BASE, psInst->ucINEndpoint, g_pucCommand,
                               12);
        MAP_USBEndpointDataSend(USB0_BASE, psInst->ucINEndpoint, USB_TRANS_IN);

        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 0;
        g_sSCSICSW.dCSWDataResidue = 0;
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ucINEndpoint, USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ucErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ucSenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->usAddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }

    psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read Capacity command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIReadCapacity(const tUSBDMSCDevice *psDevice)
{
    unsigned long ulBlocks;
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    ulBlocks = psDevice->sMediaFunctions.NumBlocks(psInst->pvMedia);

    //
    // Only decrement if any blocks were found.
    //
    if(ulBlocks != 0)
    {
        //
        // One less than the maximum number is the last addressable
        // block.
        //
        ulBlocks--;
    }

    if(psInst->pvMedia != 0)
    {
        //
        // Fill in the number of blocks, the bytes endianness must be changed.
        //
        g_pucCommand[0] = 0xff & (ulBlocks >> 24);
        g_pucCommand[1] = 0xff & (ulBlocks >> 16);
        g_pucCommand[2] = 0xff & (ulBlocks >> 8);
        g_pucCommand[3] = 0xff & (ulBlocks);

        g_pucCommand[4] = 0;

        //
        // Fill in the block size, which is fixed at DEVICE_BLOCK_SIZE.
        //
        g_pucCommand[5] = 0xff & (DEVICE_BLOCK_SIZE >> 16);
        g_pucCommand[6] = 0xff & (DEVICE_BLOCK_SIZE >> 8);
        g_pucCommand[7] = 0xff & DEVICE_BLOCK_SIZE;

        //
        // Send the SCSI Inquiry Response.
        //
        MAP_USBEndpointDataPut(USB0_BASE, psInst->ucINEndpoint, g_pucCommand,
                               8);
        MAP_USBEndpointDataSend(USB0_BASE, psInst->ucINEndpoint, USB_TRANS_IN);

        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 0;
        g_sSCSICSW.dCSWDataResidue = 0;
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ucINEndpoint, USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ucErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ucSenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->usAddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }

    psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Request Sense command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIRequestSense(const tUSBDMSCDevice *psDevice)
{
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    //
    // The request sense response.
    //
    g_pucCommand[0] = psInst->ucErrorCode;
    g_pucCommand[1] = 0;
    g_pucCommand[2] = psInst->ucSenseKey;
    *(unsigned long *)&g_pucCommand[3] = 0;

    //
    // There are 10 more bytes of data.
    //
    g_pucCommand[7] = 10;

    *(unsigned long *)&g_pucCommand[8] = 0;

    //
    // Transition from not ready to ready.
    //
    *(unsigned short *)&g_pucCommand[12] = psInst->usAddSenseCode;
    *(unsigned long *)&g_pucCommand[14] = 0;

    //
    // Send the SCSI Inquiry Response.
    //
    MAP_USBEndpointDataPut(USB0_BASE, psInst->ucINEndpoint, g_pucCommand, 18);
    MAP_USBEndpointDataSend(USB0_BASE, psInst->ucINEndpoint, USB_TRANS_IN);

    //
    // Reset the valid flag on errors.
    //
    psInst->ucErrorCode = SCSI_RS_CUR_ERRORS;

    //
    // Set the status so that it can be sent when this response has
    // has be successfully sent.
    //
    g_sSCSICSW.bCSWStatus = 0;
    g_sSCSICSW.dCSWDataResidue = 0;

    //
    // Move on to the status phase.
    //
    psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read 10 command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIRead10(const tUSBDMSCDevice *psDevice, tMSCCBW *pSCSICBW)
{
    unsigned short usNumBlocks;
    tMSCInstance *psInst;

    //
    // Default the number of blocks.
    //
    usNumBlocks = 0;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    if(psInst->pvMedia != 0)
    {
        //
        // Get the logical block from the CBW structure. This switching
        // is required to convert from big to little endian.
        //
        psInst->ulCurrentLBA = (pSCSICBW->CBWCB[2] << 24) |
                               (pSCSICBW->CBWCB[3] << 16) |
                               (pSCSICBW->CBWCB[4] << 8) |
                               (pSCSICBW->CBWCB[5] << 0);

        //
        // More bytes to read.
        //
        usNumBlocks = (pSCSICBW->CBWCB[7] << 8) | pSCSICBW->CBWCB[8];

        //
        // Read the next logical block from the storage device.
        //
        if(psDevice->sMediaFunctions.BlockRead(psInst->pvMedia,
               (unsigned char *)psInst->pulBuffer,
               psInst->ulCurrentLBA, 1) == 0)
        {
            psInst->pvMedia = 0;
            psDevice->sMediaFunctions.Close(0);
        }
    }

    //
    // If there is media present then start transferring the data.
    //
    if(psInst->pvMedia != 0)
    {
        //
        // Enable DMA on the endpoint
        //
        MAP_USBEndpointDMAEnable(USB0_BASE, psInst->ucINEndpoint,
                                 USB_EP_DEV_IN);

        //
        // Configure and DMA for the IN transfer.
        //
        MAP_uDMAChannelTransferSet(psInst->ucINDMA,
                                   UDMA_MODE_BASIC,
                                   psInst->pulBuffer,
                                   (void *)USBFIFOAddrGet(USB0_BASE,
                                                          psInst->ucINEndpoint),
                                   (MAX_TRANSFER_SIZE >> 2));

        //
        // Remember that a DMA is in progress.
        //
        psInst->ulFlags |= USBD_FLAG_DMA_IN;

        //
        // Schedule the remaining bytes to send.
        //
        psInst->ulBytesToTransfer = (DEVICE_BLOCK_SIZE * usNumBlocks);

        //
        // Start the DMA transfer.
        //
        MAP_uDMAChannelEnable(psInst->ucINDMA);

        //
        // Move on and start sending blocks.
        //
        psInst->ucSCSIState = STATE_SCSI_SEND_BLOCKS;

        if(psDevice->pfnEventCallback)
        {
            psDevice->pfnEventCallback(0, USBD_MSC_EVENT_READING, 0, 0);
        }
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ucINEndpoint, USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ucErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ucSenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->usAddSenseCode = SCSI_RS_MED_NOT_PRSNT;

        psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
    }
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read 10 command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIWrite10(const tUSBDMSCDevice *psDevice, tMSCCBW *pSCSICBW)
{
    unsigned short usNumBlocks;
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    //
    // If there is media present then start transferring the data.
    //
    if(psInst->pvMedia != 0)
    {
        //
        // Get the logical block from the CBW structure. This switching
        // is required to convert from big to little endian.
        //
        psInst->ulCurrentLBA = (pSCSICBW->CBWCB[2] << 24) |
                         (pSCSICBW->CBWCB[3] << 16) |
                         (pSCSICBW->CBWCB[4] << 8) |
                         (pSCSICBW->CBWCB[5] << 0);

        //
        // More bytes to read.
        //
        usNumBlocks = (pSCSICBW->CBWCB[7] << 8) | pSCSICBW->CBWCB[8];

        psInst->ulBytesToTransfer = DEVICE_BLOCK_SIZE * usNumBlocks;

        //
        // Start sending logical blocks, these are always multiples of
        // DEVICE_BLOCK_SIZE bytes.
        //
        psInst->ucSCSIState = STATE_SCSI_RECEIVE_BLOCKS;

        //
        // Enable uDMA on the endpoint
        //
        MAP_USBEndpointDMAEnable(USB0_BASE, psInst->ucOUTEndpoint,
                                 USB_EP_DEV_OUT);

        //
        // Configure the DMA for the OUT transfer.
        //
        MAP_uDMAChannelTransferSet(psInst->ucOUTDMA,
                                   UDMA_MODE_BASIC,
                                   (void *)USBFIFOAddrGet(USB0_BASE,
                                                         psInst->ucOUTEndpoint),
                                                         psInst->pulBuffer,
                                   (MAX_TRANSFER_SIZE >> 2));

        //
        // Remember that a DMA is in progress.
        //
        psInst->ulFlags |= USBD_FLAG_DMA_OUT;

        //
        // Enable the OUT DMA transfer.
        //
        MAP_uDMAChannelEnable(psInst->ucOUTDMA);

        //
        // Notify the application of the write event.
        //
        if(psDevice->pfnEventCallback)
        {
            psDevice->pfnEventCallback(0, USBD_MSC_EVENT_WRITING, 0, 0);
        }
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ucOUTEndpoint,
                                USB_EP_DEV_OUT);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ucErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ucSenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->usAddSenseCode = SCSI_RS_MED_NOT_PRSNT;

        psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
    }
}

//*****************************************************************************
//
// This function is used to handle the SCSI Mode Sense 6 command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIModeSense6(const tUSBDMSCDevice *psDevice, tMSCCBW *pSCSICBW)
{
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    //
    // If there is media present send the response.
    //
    if(psInst->pvMedia != 0)
    {
        //
        // Three extra bytes in this response.
        //
        g_pucCommand[0] = 3;
        g_pucCommand[1] = 0;
        g_pucCommand[2] = 0;
        g_pucCommand[3] = 0;

        //
        // Manually send the response back to the host.
        //
        MAP_USBEndpointDataPut(USB0_BASE, psInst->ucINEndpoint, g_pucCommand,
                               4);
        MAP_USBEndpointDataSend(USB0_BASE, psInst->ucINEndpoint, USB_TRANS_IN);

        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 0;
        g_sSCSICSW.dCSWDataResidue = pSCSICBW->dCBWDataTransferLength - 4;
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ucINEndpoint, USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ucErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ucSenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->usAddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }

    psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to send out the response data based on the current
// status of the mass storage class.
//
//*****************************************************************************
static void
USBDSCSISendStatus(const tUSBDMSCDevice *psDevice)
{
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    //
    // Respond with the requested status.
    //
    MAP_USBEndpointDataPut(USB0_BASE, psInst->ucINEndpoint,
                           (unsigned char *)&g_sSCSICSW, 13);
    MAP_USBEndpointDataSend(USB0_BASE, psInst->ucINEndpoint, USB_TRANS_IN);

    //
    // Move the state to status sent so that the next interrupt will move the
    // statue to idle.
    //
    psInst->ucSCSIState = STATE_SCSI_SENT_STATUS;
}

//*****************************************************************************
//
// This function is used to handle all SCSI commands.
//
//*****************************************************************************
unsigned long
USBDSCSICommand(const tUSBDMSCDevice *psDevice, tMSCCBW *pSCSICBW)
{
    unsigned long ulRetCode;
    unsigned long ulTransferLength;
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = psDevice->psPrivateData;

    //
    // Initialize the return code.
    //
    ulRetCode = 1;

    //
    // Save the transfer length because it may be overwritten by some calls.
    //
    ulTransferLength = pSCSICBW->dCBWDataTransferLength;

    switch(pSCSICBW->CBWCB[0])
    {
        //
        // Respond to the SCSI Inquiry command.
        //
        case SCSI_INQUIRY_CMD:
        {
            USBDSCSIInquiry(psDevice);

            break;
        }

        //
        // Respond to the test unit ready command.
        //
        case SCSI_TEST_UNIT_READY:
        {
            g_sSCSICSW.dCSWDataResidue = 0;

            if(psInst->pvMedia != 0)
            {
                //
                // Set the status to success for now, this could be different
                // if there is no media present.
                //
                g_sSCSICSW.bCSWStatus = 0;
            }
            else
            {
                //
                // Since there was no media, check for media here.
                //
                psInst->pvMedia = psDevice->sMediaFunctions.Open(0);

                //
                // If it is still not present then fail this command.
                //
                if(psInst->pvMedia != 0)
                {
                    g_sSCSICSW.bCSWStatus = 0;
                }
                else
                {
                    g_sSCSICSW.bCSWStatus = 1;
                }
            }
            break;
        }

        //
        // Handle the Read Capacities command.
        //
        case SCSI_READ_CAPACITIES:
        {
            USBDSCSIReadCapacities(psDevice);

            break;
        }

        //
        // Handle the Read Capacity command.
        //
        case SCSI_READ_CAPACITY:
        {
            USBDSCSIReadCapacity(psDevice);

            break;
        }

        //
        // Handle the Request Sense command.
        //
        case SCSI_REQUEST_SENSE:
        {
            USBDSCSIRequestSense(psDevice);

            break;
        }

        //
        // Handle the Read 10 command.
        //
        case SCSI_READ_10:
        {
            USBDSCSIRead10(psDevice, pSCSICBW);

            break;
        }

        //
        // Handle the Write 10 command.
        //
        case SCSI_WRITE_10:
        {
            USBDSCSIWrite10(psDevice, pSCSICBW);

            break;
        }

        //
        // Handle the Mode Sense 6 command.
        //
        case SCSI_MODE_SENSE_6:
        {
            USBDSCSIModeSense6(psDevice, pSCSICBW);

            break;
        }
        default:
        {
            //
            // Set the status so that it can be sent when this response has
            // has be successfully sent.
            //
            g_sSCSICSW.bCSWStatus = 1;
            g_sSCSICSW.dCSWDataResidue = pSCSICBW->dCBWDataTransferLength;

            //
            // If there is data then there is more work to do.
            //
            if(pSCSICBW->dCBWDataTransferLength != 0)
            {
                if(pSCSICBW->bmCBWFlags & CBWFLAGS_DIR_IN)
                {
                    //
                    // Stall the IN endpoint
                    //
                    MAP_USBDevEndpointStall(USB0_BASE, psInst->ucINEndpoint,
                                            USB_EP_DEV_IN);
                }
                else
                {
                    //
                    // Stall the OUT endpoint
                    //
                    MAP_USBDevEndpointStall(USB0_BASE, psInst->ucOUTEndpoint,
                                            USB_EP_DEV_OUT);

                }
                //
                // Send the status once the stall occurs.
                //
                psInst->ucSCSIState = STATE_SCSI_SEND_STATUS;
            }

            //
            // Set the sense codes.
            //
            psInst->ucErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            psInst->ucSenseKey = SCSI_RS_KEY_ILGL_RQST;
            psInst->usAddSenseCode = SCSI_RS_PV_INVALID;

            break;
        }
    }

    //
    // If there is no data then send out the current status.
    //
    if(ulTransferLength == 0)
    {
        USBDSCSISendStatus(psDevice);
    }
    return(ulRetCode);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
