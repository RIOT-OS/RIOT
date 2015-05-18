//*****************************************************************************
//
// usbhmsc.c - USB MSC host driver.
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
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbmsc.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhmsc.h"
#include "usblib/host/usbhscsi.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Forward declarations for the driver open and close calls.
//
//*****************************************************************************
static void *USBHMSCOpen(tUSBHostDevice *pDevice);
static void USBHMSCClose(void *pvInstance);

//*****************************************************************************
//
// This is the structure for an instance of a USB MSC host driver.
//
//*****************************************************************************
typedef struct
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *pDevice;

    //
    // Used to save the callback.
    //
    tUSBHMSCCallback pfnCallback;

    //
    // The Maximum LUNs
    //
    unsigned long ulMaxLUN;

    //
    // The total number of blocks associated with this device.
    //
    unsigned long ulNumBlocks;

    //
    // The size of the blocks associated with this device.
    //
    unsigned long ulBlockSize;

    //
    // Bulk IN pipe.
    //
    unsigned long ulBulkInPipe;

    //
    // Bulk OUT pipe.
    //
    unsigned long ulBulkOutPipe;
}
tUSBHMSCInstance;

//*****************************************************************************
//
// The array of USB MSC host drivers.
//
//*****************************************************************************
static tUSBHMSCInstance g_USBHMSCDevice =
{
    0
};

//*****************************************************************************
//
//! This constant global structure defines the Mass Storage Class Driver that
//! is provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_USBHostMSCClassDriver =
{
    USB_CLASS_MASS_STORAGE,
    USBHMSCOpen,
    USBHMSCClose,
    0
};

//*****************************************************************************
//
//! This function is used to open an instance of the MSC driver.
//!
//! \param pDevice is a pointer to the device information structure.
//!
//! This function will attempt to open an instance of the MSC driver based on
//! the information contained in the pDevice structure.  This call can fail if
//! there are not sufficient resources to open the device.  The function will
//! return a value that should be passed back into USBMSCClose() when the
//! driver is no longer needed.
//!
//! \return The function will return a pointer to a MSC driver instance.
//
//*****************************************************************************
static void *
USBHMSCOpen(tUSBHostDevice *pDevice)
{
    long lIdx;
    tEndpointDescriptor *pEndpointDescriptor;
    tInterfaceDescriptor *pInterface;

    //
    // Don't allow the device to be opened without closing first.
    //
    if(g_USBHMSCDevice.pDevice)
    {
        return(0);
    }

    //
    // Save the device pointer.
    //
    g_USBHMSCDevice.pDevice = pDevice;

    //
    // Get the interface descriptor.
    //
    pInterface = USBDescGetInterface(pDevice->pConfigDescriptor, 0, 0);

    //
    // Loop through the endpoints of the device.
    //
    for(lIdx = 0; lIdx < 3; lIdx++)
    {
        //
        // Get the first endpoint descriptor.
        //
        pEndpointDescriptor =
            USBDescGetInterfaceEndpoint(pInterface, lIdx,
                                        pDevice->ulConfigDescriptorSize);

        //
        // If no more endpoints then break out.
        //
        if(pEndpointDescriptor == 0)
        {
            break;
        }

        //
        // See if this is a bulk endpoint.
        //
        if((pEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
           USB_EP_ATTR_BULK)
        {
            //
            // See if this is bulk IN or bulk OUT.
            //
            if(pEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
            {
                //
                // Allocate the USB Pipe for this Bulk IN endpoint.
                //
                g_USBHMSCDevice.ulBulkInPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_BULK_IN_DMA,
                                        pDevice,
                                        pEndpointDescriptor->wMaxPacketSize,
                                        0);
                //
                // Configure the USB pipe as a Bulk IN endpoint.
                //
                USBHCDPipeConfig(g_USBHMSCDevice.ulBulkInPipe,
                                 pEndpointDescriptor->wMaxPacketSize,
                                 0,
                                 (pEndpointDescriptor->bEndpointAddress &
                                  USB_EP_DESC_NUM_M));
            }
            else
            {
                //
                // Allocate the USB Pipe for this Bulk OUT endpoint.
                //
                g_USBHMSCDevice.ulBulkOutPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_BULK_OUT_DMA,
                                        pDevice,
                                        pEndpointDescriptor->wMaxPacketSize,
                                        0);
                //
                // Configure the USB pipe as a Bulk OUT endpoint.
                //
                USBHCDPipeConfig(g_USBHMSCDevice.ulBulkOutPipe,
                                 pEndpointDescriptor->wMaxPacketSize,
                                 0,
                                 (pEndpointDescriptor->bEndpointAddress &
                                  USB_EP_DESC_NUM_M));
            }
        }
    }

    //
    // If the callback exists, call it with an Open event.
    //
    if(g_USBHMSCDevice.pfnCallback != 0)
    {
        g_USBHMSCDevice.pfnCallback((unsigned long)&g_USBHMSCDevice,
                                    MSC_EVENT_OPEN, 0);
    }
    
    
    g_USBHMSCDevice.ulMaxLUN = 0xffffffff;

    //
    // Return the only instance of this device.
    //
    return(&g_USBHMSCDevice);
}

//*****************************************************************************
//
//! This function is used to release an instance of the MSC driver.
//!
//! \param pvInstance is an instance pointer that needs to be released.
//!
//! This function will free up any resources in use by the MSC driver instance
//! that is passed in.  The \e pvInstance pointer should be a valid value that
//! was returned from a call to USBMSCOpen().
//!
//! \return None.
//
//*****************************************************************************
static void
USBHMSCClose(void *pvInstance)
{
    //
    // Do nothing if there is not a driver open.
    //
    if(g_USBHMSCDevice.pDevice == 0)
    {
        return;
    }

    //
    // Reset the device pointer.
    //
    g_USBHMSCDevice.pDevice = 0;

    //
    // Free the Bulk IN pipe.
    //
    if(g_USBHMSCDevice.ulBulkInPipe != 0)
    {
        USBHCDPipeFree(g_USBHMSCDevice.ulBulkInPipe);
    }

    //
    // Free the Bulk OUT pipe.
    //
    if(g_USBHMSCDevice.ulBulkOutPipe != 0)
    {
        USBHCDPipeFree(g_USBHMSCDevice.ulBulkOutPipe);
    }

    //
    // If the callback exists then call it.
    //
    if(g_USBHMSCDevice.pfnCallback != 0)
    {
        g_USBHMSCDevice.pfnCallback((unsigned long)&g_USBHMSCDevice,
                                    MSC_EVENT_CLOSE, 0);
    }
}

//*****************************************************************************
//
//! This function retrieves the maximum number of the logical units on a
//! mass storage device.
//!
//! \param pDevice is the device instance pointer for this request.
//! \param ulInterface is the interface number on the device specified by the
//! \e ulAddress parameter.
//! \param pucMaxLUN is the byte value returned from the device for the
//! device's maximum logical unit.
//!
//! The device will return one byte of data that contains the maximum LUN
//! supported by the device.  For example, if the device supports four LUNs
//! then the LUNs would be numbered from 0 to 3 and the return value would be
//! 3.  If no LUN is associated with the device, the value returned shall be 0.
//!
//! \return None.
//
//*****************************************************************************
static void
USBHMSCGetMaxLUN(tUSBHostDevice *pDevice, unsigned long ulInterface,
                 unsigned char *pucMaxLUN)
{
    tUSBRequest SetupPacket;

    //
    // This is a Class specific interface IN request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_CLASS | USB_RTYPE_INTERFACE;

    //
    // Request a the Max LUN for this interface.
    //
    SetupPacket.bRequest = USBREQ_GET_MAX_LUN;
    SetupPacket.wValue = 0;

    //
    // Indicate the interface to use.
    //
    SetupPacket.wIndex = (unsigned short)ulInterface;

    //
    // Only request a single byte of data.
    //
    SetupPacket.wLength = 1;

    //
    // Put the setup packet in the buffer and send the command.
    //
    if(USBHCDControlTransfer(0, &SetupPacket, pDevice, pucMaxLUN, 1,
                             MAX_PACKET_SIZE_EP0) != 1)
    {
        *pucMaxLUN = 0;
    }
}

//*****************************************************************************
//
//! This function checks if a drive is ready to be accessed.
//!
//! \param ulInstance is the device instance to use for this read.
//!
//! This function checks if the current device is ready to be accessed.
//! It uses the \e ulInstance parameter to determine which device to check and
//! will return zero when the device is ready.  Any non-zero return code
//! indicates that the device was not ready.
//!
//! \return This function will return zero if the device is ready and it will
//! return a other value if the device is not ready or if an error occurred.
//
//*****************************************************************************
long
USBHMSCDriveReady(unsigned long ulInstance)
{
    unsigned char ucMaxLUN, pBuffer[SCSI_INQUIRY_DATA_SZ];
    unsigned long ulSize;
    tUSBHMSCInstance *pMSCDevice;

    //
    // Get the instance pointer in a more usable form.
    //
    pMSCDevice = (tUSBHMSCInstance *)ulInstance;

    //
    // If there is no device present then return an error.
    //
    if(pMSCDevice->pDevice == 0)
    {
        return(-1);
    }

    //
    // Only request the maximum number of LUNs once.
    //
    if(g_USBHMSCDevice.ulMaxLUN == 0xffffffff)
    {
        //
        // Get the Maximum LUNs on this device.
        //
        USBHMSCGetMaxLUN(g_USBHMSCDevice.pDevice,
                         g_USBHMSCDevice.pDevice->ulInterface, &ucMaxLUN);

        //
        // Save the Maximum number of LUNs on this device.
        //
        g_USBHMSCDevice.ulMaxLUN = ucMaxLUN;
    }
    
    //
    // Just return if the device is returning not present.
    //
    ulSize = SCSI_REQUEST_SENSE_SZ;
    if(USBHSCSIRequestSense(pMSCDevice->ulBulkInPipe, pMSCDevice->ulBulkOutPipe,
                            pBuffer, &ulSize) != SCSI_CMD_STATUS_PASS)
    {
        return(-1);
    }

    if((pBuffer[SCSI_RS_SKEY] == SCSI_RS_KEY_UNIT_ATTN) &&
       (pBuffer[SCSI_RS_SKEY_AD_SKEY] == SCSI_RS_KEY_NOTPRSNT))
    {
        return(-1);
    }

    //
    // Issue a SCSI Inquiry to get basic information on the device
    //
    ulSize = SCSI_INQUIRY_DATA_SZ;
    if((USBHSCSIInquiry(pMSCDevice->ulBulkInPipe, pMSCDevice->ulBulkOutPipe,
                        pBuffer, &ulSize) != SCSI_CMD_STATUS_PASS))
    {
        return(-1);
    }

    //
    // Get the size of the drive.
    //
    ulSize = SCSI_INQUIRY_DATA_SZ;
    if(USBHSCSIReadCapacity(pMSCDevice->ulBulkInPipe, pMSCDevice->ulBulkOutPipe,
                            pBuffer, &ulSize) != SCSI_CMD_STATUS_PASS)
    {
        //
        // Get the current sense data from the device to see why it failed
        // the Read Capacity command.
        //
        ulSize = SCSI_REQUEST_SENSE_SZ;
        USBHSCSIRequestSense(pMSCDevice->ulBulkInPipe,
                             pMSCDevice->ulBulkOutPipe, pBuffer, &ulSize);

        //
        // If the read capacity failed then check if the drive is ready.
        //
        if(USBHSCSITestUnitReady(pMSCDevice->ulBulkInPipe,
                                 pMSCDevice->ulBulkOutPipe) != SCSI_CMD_STATUS_PASS)
        {
            //
            // Get the current sense data from the device to see why it failed
            // the Test Unit Ready command.
            //
            ulSize = SCSI_REQUEST_SENSE_SZ;
            USBHSCSIRequestSense(pMSCDevice->ulBulkInPipe,
                                 pMSCDevice->ulBulkOutPipe, pBuffer, &ulSize);
        }

        return(-1);
    }
    else
    {
        //
        // Read the block size out, value is stored big endian.
        //
        pMSCDevice->ulBlockSize =
            (pBuffer[7] | (pBuffer[6] << 8) | pBuffer[5] << 16 |
             (pBuffer[4] << 24));

        //
        // Read the block size out.
        //
        pMSCDevice->ulNumBlocks =
            (pBuffer[3] | (pBuffer[2] << 8) | pBuffer[1] << 16 |
             (pBuffer[0] << 24));
    }

    //
    // See if the drive is ready to use.
    //
    if(USBHSCSITestUnitReady(pMSCDevice->ulBulkInPipe,
                             pMSCDevice->ulBulkOutPipe) != SCSI_CMD_STATUS_PASS)
    {
        //
        // Get the current sense data from the device to see why it failed
        // the Test Unit Ready command.
        //
        ulSize = SCSI_REQUEST_SENSE_SZ;
        USBHSCSIRequestSense(pMSCDevice->ulBulkInPipe,
                             pMSCDevice->ulBulkOutPipe, pBuffer, &ulSize);

        return(-1);
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************************
//
//! This function should be called before any devices are present to enable
//! the mass storage device class driver.
//!
//! \param ulDrive is the drive number to open.
//! \param pfnCallback is the driver callback for any mass storage events.
//!
//! This function is called to open an instance of a mass storage device.  It
//! should be called before any devices are connected to allow for proper
//! notification of drive connection and disconnection.  The \e ulDrive
//! parameter is a zero based index of the drives present in the system.
//! There are a constant number of drives, and this number should only
//! be greater than 0 if there is a USB hub present in the system.  The
//! application should also provide the \e pfnCallback to be notified of mass
//! storage related events like device enumeration and device removal.
//!
//! \return This function will return the driver instance to use for the other
//! mass storage functions.  If there is no driver available at the time of
//! this call, this function will return zero.
//
//*****************************************************************************
unsigned long
USBHMSCDriveOpen(unsigned long ulDrive, tUSBHMSCCallback pfnCallback)
{
    //
    // Only the first drive is supported and only one callback is supported.
    //
    if((ulDrive != 0) || (g_USBHMSCDevice.pfnCallback))
    {
        return(0);
    }

    //
    // Save the callback.
    //
    g_USBHMSCDevice.pfnCallback = pfnCallback;

    //
    // Return the requested device instance.
    //
    return((unsigned long)&g_USBHMSCDevice);
}

//*****************************************************************************
//
//! This function should be called to release a drive instance.
//!
//! \param ulInstance is the device instance that is to be released.
//!
//! This function is called when an MSC drive is to be released in preparation
//! for shutdown or a switch to USB device mode, for example.  Following this
//! call, the drive is available for other clients who may open it again using
//! a call to USBHMSCDriveOpen().
//!
//! \return None.
//
//*****************************************************************************
void
USBHMSCDriveClose(unsigned long ulInstance)
{
    tUSBHMSCInstance *pMSCDevice;

    //
    // Get a pointer to the device instance data from the handle.
    //
    pMSCDevice = (tUSBHMSCInstance *)ulInstance;

    //
    // Close the drive (if it is already open)
    //
    USBHMSCClose((void *)pMSCDevice);

    //
    // Clear the callback indicating that the device is now closed.
    //
    pMSCDevice->pfnCallback = 0;
}

//*****************************************************************************
//
//! This function performs a block read to an MSC device.
//!
//! \param ulInstance is the device instance to use for this read.
//! \param ulLBA is the logical block address to read on the device.
//! \param pucData is a pointer to the returned data buffer.
//! \param ulNumBlocks is the number of blocks to read from the device.
//!
//! This function will perform a block sized read from the device associated
//! with the \e ulInstance parameter.  The \e ulLBA parameter specifies the
//! logical block address to read on the device.  This function will only
//! perform \e ulNumBlocks block sized reads.  In most cases this is a read
//! of 512 bytes of data.  The \e *pucData buffer should be at least
//! \e ulNumBlocks * 512 bytes in size.
//!
//! \return The function returns zero for success and any negative value
//! indicates a failure.
//
//*****************************************************************************
long
USBHMSCBlockRead(unsigned long ulInstance, unsigned long ulLBA,
                 unsigned char *pucData, unsigned long ulNumBlocks)
{
    tUSBHMSCInstance *pMSCDevice;
    unsigned long ulSize;

    //
    // Get the instance pointer in a more usable form.
    //
    pMSCDevice = (tUSBHMSCInstance *)ulInstance;

    //
    // If there is no device present then return an error.
    //
    if(pMSCDevice->pDevice == 0)
    {
        return(-1);
    }

    //
    // Calculate the actual byte size of the read.
    //
    ulSize = pMSCDevice->ulBlockSize * ulNumBlocks;

    //
    // Perform the SCSI read command.
    //
    if(USBHSCSIRead10(pMSCDevice->ulBulkInPipe, pMSCDevice->ulBulkOutPipe,
                      ulLBA, pucData, &ulSize,
                      ulNumBlocks) != SCSI_CMD_STATUS_PASS)
    {
        return(-1);
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************************
//
//! This function performs a block write to an MSC device.
//!
//! \param ulInstance is the device instance to use for this write.
//! \param ulLBA is the logical block address to write on the device.
//! \param pucData is a pointer to the data to write out.
//! \param ulNumBlocks is the number of blocks to write to the device.
//!
//! This function will perform a block sized write to the device associated
//! with the \e ulInstance parameter.  The \e ulLBA parameter specifies the
//! logical block address to write on the device.  This function will only
//! perform \e ulNumBlocks block sized writes.  In most cases this is a write
//! of 512 bytes of data.  The \e *pucData buffer should contain at least
//! \e ulNumBlocks * 512 bytes in size to prevent unwanted data being written
//! to the device.
//!
//! \return The function returns zero for success and any negative value
//! indicates a failure.
//
//*****************************************************************************
long
USBHMSCBlockWrite(unsigned long ulInstance, unsigned long ulLBA,
                  unsigned char *pucData, unsigned long ulNumBlocks)
{
    tUSBHMSCInstance *pMSCDevice;
    unsigned long ulSize;

    //
    // Get the instance pointer in a more usable form.
    //
    pMSCDevice = (tUSBHMSCInstance *)ulInstance;

    //
    // If there is no device present then return an error.
    //
    if(pMSCDevice->pDevice == 0)
    {
        return(-1);
    }

    //
    // Calculate the actual byte size of the write.
    //
    ulSize = pMSCDevice->ulBlockSize * ulNumBlocks;

    //
    // Perform the SCSI write command.
    //
    if(USBHSCSIWrite10(pMSCDevice->ulBulkInPipe, pMSCDevice->ulBulkOutPipe,
                       ulLBA, pucData, &ulSize,
                       ulNumBlocks) != SCSI_CMD_STATUS_PASS)
    {
        return(-1);
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
