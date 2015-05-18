//*****************************************************************************
//
// usbbuffer.c - USB buffer object.
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
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup usblib_buffer_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Workspace variables required by each buffer instance.  This structure is
// overlaid on the pvWorkspace memory provided in the tUSBBuffer structure
// passed to USBBufferInit().
//
//*****************************************************************************
typedef struct
{
    tUSBRingBufObject sRingBuf;
    unsigned long ulLastSent;
    unsigned long ulFlags;
}
tUSBBufferVars;

//*****************************************************************************
//
// Flags which may be set in the tUSBBufferVars ulFlags field.
//
//*****************************************************************************
#define USB_BUFFER_FLAG_SEND_ZLP 0x00000001

//*****************************************************************************
//
// Schedule the next packet transmission to the host if data remains to be
// sent.
//
// \param psBuffer points to the buffer from which a packet transmission is
// to be scheduled.
//
// This function checks to determine whether the lower layer is capable of
// accepting a new packet for transmission and, if so, schedules the next
// packet transmission if data remains in the buffer.
//
// \return None.
//
//*****************************************************************************
static void
ScheduleNextTransmission(const tUSBBuffer *psBuffer)
{
    tUSBBufferVars *psVars;
    unsigned long ulPacket, ulSpace, ulTotal, ulSent;

    //
    // Get a pointer to our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Ask the lower layer if it has space to accept another packet of data.
    //
    ulPacket = psBuffer->pfnAvailable(psBuffer->pvHandle);

    //
    // If we were returned something other than zero, we can write that number
    // of bytes to the lower layer.
    //
    if(ulPacket)
    {
        //
        // How much contiguous data do we have in the buffer?
        //
        ulSpace = USBRingBufContigUsed(&psVars->sRingBuf);

        //
        // How much total data do we have in the buffer?
        //
        ulTotal = USBRingBufUsed(&psVars->sRingBuf);

        //
        // How much data will we be sending as a result of this call?
        //
        ulSent = (ulPacket < ulTotal) ? ulPacket : ulTotal;

        //
        // Write the contiguous bytes to the lower layer assuming there is
        // something to send.
        //
        if(ulSpace)
        {
            //
            // There is data available to send.  Update our state to indicate
            // the amount we will be sending in this packet.
            //
            psVars->ulLastSent = ulSent;

            //
            // Determine the maximum sized block we can send in this transfer.
            //
            ulSpace = (ulSpace < ulPacket) ? ulSpace : ulPacket;
            //
            // Call the lower layer to send the new packet.  If the current
            // data spans the buffer wrap, tell the lower layer that it can
            // expect a second call to fill the whole packet before it
            // transmits it.
            //
            psBuffer->pfnTransfer(psBuffer->pvHandle,
                                 (psVars->sRingBuf.pucBuf +
                                  psVars->sRingBuf.ulReadIndex), ulSpace,
                                  (((ulSpace < ulPacket) &&
                                    (ulSpace < ulTotal)) ? false : true));

            //
            // Do we need to send a second part to fill out the packet?  This
            // will occur if the current packet spans the buffer wrap.
            //
            if((ulSpace < ulPacket) && (ulSpace < ulTotal))
            {
                //
                // The packet straddled the wrap.  How much space remains in
                // the packet?
                //
                ulPacket -= ulSpace;

                //
                // How much data can we actually send?
                //
                ulSpace = ulTotal - ulSpace;
                ulSpace = (ulSpace > ulPacket) ? ulPacket : ulSpace;

                psBuffer->pfnTransfer(psBuffer->pvHandle,
                                      psVars->sRingBuf.pucBuf, ulSpace, true);
            }
        }
        else
        {
            //
            // There is no data to send.  Did we last send a full packet?
            //
            if(psVars->ulLastSent == ulPacket)
            {
                //
                // Yes - if necessary, send a zero-length packet back to the
                // host to complete the last transaction.
                //
                if(psVars->ulFlags & USB_BUFFER_FLAG_SEND_ZLP)
                {
                    psVars->ulLastSent = 0;
                    psBuffer->pfnTransfer(psBuffer->pvHandle,
                                          psVars->sRingBuf.pucBuf, 0, true);
                }
            }
        }

        //
        // Don't update the ring buffer read index yet.  We do this once we are
        // sure the packet was correctly transmitted.
        //
    }
}

//*****************************************************************************
//
// Handles USB_EVENT_RX_AVAILABLE for a receive buffer.
//
// \param psBuffer points to the buffer which is receiving the event.
// \param ulSize is the size reported in the event.
// \param pucData is the pointer provided in the event.
//
// This function is responsible for reading data from the lower layer into
// the buffer or, if we had previously passed a section of the buffer to the
// lower layer for it to write into directly, updating the buffer write pointer
// to add the new data to the buffer.
//
// If the pointer provided is NULL, we call the low level pfnTransfer function
// to get the new data.  If the pointer is not NULL and not within the existing
// ring buffer, we copy the data directly from the pointer to the buffer and
// return the number of bytes read.
//
// \return Returns the number of bytes read from the lower layer.
//
//*****************************************************************************
static unsigned long
HandleRxAvailable(tUSBBuffer *psBuffer, unsigned long ulSize,
                  unsigned char *pucData)
{
    tUSBBufferVars *psVars;
    unsigned long ulAvail, ulRead, ulPacket, ulRetCount;

    //
    // Get a pointer to our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Has the data already been read into memory?
    //
    if(pucData)
    {
        //
        // Yes - is it already in our ring buffer?
        //
        if((pucData >= psBuffer->pcBuffer) &&
           (pucData < psBuffer->pcBuffer + psBuffer->ulBufferSize))
        {
            //
            // The data is already in our ring buffer so merely update the
            // write pointer to add the new data.
            //
            USBRingBufAdvanceWrite(&psVars->sRingBuf, ulSize);

            //
            // In this case, we pass back 0 to indicate that the lower layer
            // doesn't need to make any buffer pointer updates.
            //
            ulRetCount = 0;
        }
        else
        {
            //
            // The data is not within our buffer so we need to copy it into
            // the buffer.
            //
            // How much space does the buffer have available?
            //
            ulAvail = USBRingBufFree(&psVars->sRingBuf);

            //
            // How much should we copy?
            //
            ulRead = (ulAvail < ulSize) ? ulAvail : ulSize;

            //
            // Copy the data into the buffer.
            //
            USBRingBufWrite(&psVars->sRingBuf, pucData, ulRead);

            //
            // We need to return the number of bytes we read in this case
            // since the buffer supplied to us was owned by the lower layer and
            // it may need to update its read pointer.
            //
            ulRetCount = ulRead;
        }
    }
    else
    {
        //
        // We were passed a NULL pointer so the low level driver has not read
        // the data into memory yet.  We need to call the transfer function to
        // get the packet.
        //
        // How big is the packet that we need to receive?
        //
        ulPacket = psBuffer->pfnAvailable(psBuffer->pvHandle);

        //
        // How much contiguous space do we have in the buffer?
        //
        ulAvail = USBRingBufContigFree(&psVars->sRingBuf);

        //
        // Get as much of the packet as we can in the available space.
        //
        ulRead = psBuffer->pfnTransfer(psBuffer->pvHandle,
                                       (psVars->sRingBuf.pucBuf +
                                        psVars->sRingBuf.ulWriteIndex),
                                       ulAvail, true);

        //
        // Advance the ring buffer write pointer to add our new data.
        //
        if(ulRead)
        {
            USBRingBufAdvanceWrite(&psVars->sRingBuf, ulRead);
        }

        //
        // Did we get the whole packet?
        //
        if(ulRead < ulPacket)
        {
            //
            // No - how much space do we have in the buffer?
            //
            ulAvail = USBRingBufContigFree(&psVars->sRingBuf);

            //
            // If there is any space left, read as much of the remainder of
            // the packet as we can.
            //
            if(ulAvail)
            {
                ulPacket =
                    psBuffer->pfnTransfer(psBuffer->pvHandle,
                                          (psVars->sRingBuf.pucBuf +
                                           psVars->sRingBuf.ulWriteIndex),
                                          ulAvail, true);

                //
                // Update the write pointer after we read more data into the
                // buffer.
                //
                if(ulPacket)
                {
                    USBRingBufAdvanceWrite(&psVars->sRingBuf, ulPacket);
                }
            }
        }

        //
        // We need to return 0 in this case to indicate that the lower layer
        // need not perform any buffer maintenance as a result of the callback.
        //
        ulRetCount = 0;
    }

    //
    // How much data do we have in the buffer?
    //
    ulAvail = USBRingBufUsed(&psVars->sRingBuf);

    //
    // Pass the event on to the client with the current read pointer and
    // available data size.  The client is expected to understand the ring
    // structure and be able to deal with wrap if it wants to read the data
    // directly from the buffer.
    //
    ulRead = psBuffer->pfnCallback(psBuffer->pvCBData,
                                   USB_EVENT_RX_AVAILABLE,
                                   ulAvail,
                                   (psVars->sRingBuf.pucBuf +
                                    psVars->sRingBuf.ulReadIndex));

    //
    // If the client read anything from the buffer, update the read pointer.
    //
    USBRingBufAdvanceRead(&psVars->sRingBuf, ulRead);

    //
    // Return the correct value to the low level driver.
    //
    return(ulRetCount);
}

//*****************************************************************************
//
// Handles USB_EVENT_DATA_REMAINING for a receive buffer.
//
// \param psBuffer points to the buffer which is receiving the event.
//
// This function determines the total number of bytes of data that remain
// unprocessed in the client and buffer and reports this back to the caller.
//
// \return Returns the number of bytes remaining to be processed.
//
//*****************************************************************************
static unsigned long
HandleDataRemaining(tUSBBuffer *psBuffer)
{
    unsigned long ulBufData, ulClientData;
    tUSBBufferVars *psVars;

    //
    // Get a pointer to our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // How much data does the client currently have buffered?
    //
    ulClientData = psBuffer->pfnCallback(psBuffer->pvCBData,
                                         USB_EVENT_DATA_REMAINING, 0,
                                         (void *)0);

    //
    // How much data do we have in the buffer?
    //
    ulBufData = USBRingBufUsed(&psVars->sRingBuf);

    //
    // Return the total number of bytes of unprocessed data to the lower layer.
    //
    return(ulBufData + ulClientData);
}

//*****************************************************************************
//
// Handles USB_EVENT_TX_COMPLETE for a transmit buffer.
//
// \param psBuffer points to the buffer which is receiving the event.
// \param ulSize is the number of bytes that have been transmitted and
// acknowledged.
//
// This function informs us that data written to the lower layer from a
// transmit buffer has been successfully transmitted.  We use this to update
// the buffer read pointer and attempt to schedule the next transmission if
// data remains in the buffer.
//
// \return Returns the number of bytes remaining to be processed.
//
//*****************************************************************************
static unsigned long
HandleTxComplete(tUSBBuffer *psBuffer, unsigned long ulSize)
{
    tUSBBufferVars *psVars;

    //
    // Get a pointer to our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Update the transmit buffer read pointer to remove the data that has
    // now been transmitted.
    //
    USBRingBufAdvanceRead(&psVars->sRingBuf, ulSize);

    //
    // Try to schedule the next packet transmission if data remains to be
    // sent.
    //
    ScheduleNextTransmission(psBuffer);

    //
    // The return code from this event is ignored.
    //
    return(0);
}

//*****************************************************************************
//
// Handles USB_EVENT_REQUEST_BUFFER for a receive buffer.
//
// \param psBuffer points to the buffer which is receiving the event.
// \param ulSize is the size of the buffer requested.
// \param ppucBuffer is a pointer which is to be written with a pointer to
// the returned buffer.
//
// This function is called by a low level driver that wishes to receive data
// automatically and write it directly to a memory buffer, either using
// software or DMA prior to issuing USB_EVENT_RX_AVAILABLE.  The event is sent
// in advance of receiving data to provide storage for whatever is received
// next.
//
// If we have a contiguous block of space in the buffer of at least ulSize
// bytes immediately in front of the current write pointer, we pass this back
// otherwise we send NULL indicating that the next packet should be notified
// using a standard USB_EVENT_RX_AVAILABLE event without being received
// automatically.  Note that the USB_EVENT_REQUEST_BUFFER protocol allows us to
// return less than \e ulSize bytes if we know how much data is expected next
// but this is not possible here since the USBBuffer knows nothing about the
// protocol whose data it is handling.
//
// \return Returns the number of bytes remaining to be processed.
//
//*****************************************************************************
static unsigned long
HandleRequestBuffer(tUSBBuffer *psBuffer, unsigned long ulSize,
                    unsigned char **ppucBuffer)
{
    tUSBBufferVars *psVars;
    unsigned long ulSpace;

    //
    // Get a pointer to our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // How much contiguous space do we have available?
    //
    ulSpace = USBRingBufContigFree(&psVars->sRingBuf);

    //
    // Is there enough space available to satisfy the request?
    //
    if(ulSpace >= ulSize)
    {
        //
        // Yes - return the current write pointer
        //
        *ppucBuffer = psVars->sRingBuf.pucBuf + psVars->sRingBuf.ulWriteIndex;
        return(ulSize);
    }
    else
    {
        //
        // We do not have enough contiguous space following the current write
        // pointer to satisfy the request so do not provide a buffer.
        //
        *ppucBuffer = (unsigned char *)0;
        return(0);
    }
}

//*****************************************************************************
//
//! Initializes a USB buffer object to be used with a given USB controller and
//! device or host class driver.
//!
//! \param psBuffer points to a structure containing information on the buffer
//! memory to be used and the underlying device or host class driver whose data
//! is to be buffered.  This structure must remain accessible for as long as
//! the buffer is in use.
//!
//! This function is used to initialize a USB buffer object and insert it
//! into the function and callback interfaces between an underlying driver
//! and the application.  The caller supplies information on both the RAM
//! to be used to buffer data, the type of buffer to be created (transmit or
//! receive) and the functions to be called in the lower layer to transfer
//! data to or from the USB controller.
//!
//! \return Returns the original buffer structure pointer if successful or
//! NULL if an error is detected.
//
//*****************************************************************************
const tUSBBuffer *
USBBufferInit(const tUSBBuffer *psBuffer)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer && psBuffer->pvWorkspace && psBuffer->pcBuffer &&
           psBuffer->ulBufferSize && psBuffer->pfnAvailable &&
           psBuffer->pfnTransfer && psBuffer->pfnCallback);

    //
    // Get a pointer to the buffer workspace and initialize the variables it
    // contains.
    //
    psVars = psBuffer->pvWorkspace;
    psVars->ulFlags = 0;
    USBRingBufInit(&psVars->sRingBuf, psBuffer->pcBuffer,
                   psBuffer->ulBufferSize);

    //
    // If all is well, return the same pointer we were originally passed.
    //
    return(psBuffer);
}

//*****************************************************************************
//
//! Enables or disables zero-length packet insertion.
//!
//! \param psBuffer is the pointer to the buffer instance whose information
//! is being queried.
//! \param bSendZLP is \b true to send zero-length packets or \b false to
//! prevent them from being sent.
//!
//! This function allows the use of zero-length packets to be controlled by
//! an application.  In cases where the USB buffer has sent a full (64 byte)
//! packet and then discovers that the transmit buffer is empty, the default
//! behavior is to do nothing.  Some protocols, however, require that a zero-
//! length packet be inserted to signal the end of the data.  When using such
//! a protocol, this function should be called with \e bSendZLP set to \b true
//! to enable the desired behavior.
//!
//! \return None.
//
//*****************************************************************************
void
USBBufferZeroLengthPacketInsert(const tUSBBuffer *psBuffer, tBoolean bSendZLP)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Set the flag telling us whether or not to send a zero-length packet
    // after sending a 64 bytes packet and finding no more data to send.
    //
    if(bSendZLP)
    {
        //
        // Enable ZLP transmission.
        //
        psVars->ulFlags |= USB_BUFFER_FLAG_SEND_ZLP;
    }
    else
    {
        //
        // Disable ZLP transmission.
        //
        psVars->ulFlags &= ~ USB_BUFFER_FLAG_SEND_ZLP;
    }
}

//*****************************************************************************
//
//! Returns the current ring buffer indices for this USB buffer.
//!
//! \param psBuffer is the pointer to the buffer instance whose information
//! is being queried.
//! \param psRingBuf is a pointer to storage that will be written with the
//! current ring buffer control structure for this USB buffer.
//!
//! This function is provided to aid a client wishing to write data directly
//! into the USB buffer rather than using the USBBufferWrite() function.  This
//! may be necessary to control when the USBBuffer starts transmission of a
//! large block of data, for example.
//!
//! A transmit buffer will immediately send a new packet on any call to
//! USBBufferWrite() if the underlying layer indicates that a transmission can
//! be started.  In some cases this is not desirable and a client may wish to
//! wishes to write more data to the buffer in advance of starting transmission
//! to the lower layer.  In such cases, this function may be called to retrieve
//! the current ring buffer indices and the buffer accessed directly.  Once the
//! client has written all data it wishes to send, it should call function
//! USBBufferDataWritten() to indicate that transmission may begin.
//!
//! \return None.
//
//*****************************************************************************
void
USBBufferInfoGet(const tUSBBuffer *psBuffer, tUSBRingBufObject *psRingBuf)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer && psRingBuf);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Copy the current ring buffer settings to the clients storage.
    //
    *psRingBuf = psVars->sRingBuf;
}

//*****************************************************************************
//
//! Indicates that a client has written data directly into the buffer and
//! wishes to start transmission.
//!
//! \param psBuffer is the pointer to the buffer instance into which data has
//! been written.
//! \param ulLength is the number of bytes of data that the client has written.
//!
//! This function updates the USB buffer write pointer and starts transmission
//! of the data in the buffer assuming the lower layer is ready to receive a
//! new packet.  The function is provided to aid a client wishing to write
//! data directly into the USB buffer rather than using the USBBufferWrite()
//! function.  This may be necessary to control when the USB buffer starts
//! transmission of a large block of data, for example.
//!
//! A transmit buffer will immediately send a new packet on any call to
//! USBBufferWrite() if the underlying layer indicates that a transmission can
//! be started.  In some cases this is not desirable and a client may wish to
//! write more data to the buffer in advance of starting transmission
//! to the lower layer.  In such cases, USBBufferInfoGet() may be called to
//! retrieve the current ring buffer indices and the buffer accessed directly.
//! Once the client has written all data it wishes to send (taking care to
//! handle the ring buffer wrap), it should call this function to indicate that
//! transmission may begin.
//!
//! \return None.
//
//*****************************************************************************
void
USBBufferDataWritten(const tUSBBuffer *psBuffer, unsigned long ulLength)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Advance the ring buffer write pointer to include the newly written
    // data.
    //
    if(ulLength)
    {
        USBRingBufAdvanceWrite(&psVars->sRingBuf, ulLength);
    }

    //
    // Try to schedule a new packet transmission.
    //
    ScheduleNextTransmission(psBuffer);
}

//*****************************************************************************
//
//! Indicates that a client has read data directly out of the buffer.
//!
//! \param psBuffer is the pointer to the buffer instance from which data has
//! been read.
//! \param ulLength is the number of bytes of data that the client has read.
//!
//! This function updates the USB buffer read pointer to remove data that
//! the client has read directly rather than via a call to USBBufferRead().
//! The function is provided to aid a client wishing to minimize data copying.
//! To read directly from the buffer, a client must call USBBufferInfoGet() to
//! retrieve the current buffer indices.  With this information, the data
//! following the current read index can be read.  Once the client has
//! processed much data as it needs, USBBufferDataRemoved() must be called to
//! advance the read pointer past the data that has been read and free up that
//! section of the buffer.  The client must take care to correctly handle the
//! wrap point if accessing the buffer directly.
//!
//! \return None.
//
//*****************************************************************************
void
USBBufferDataRemoved(const tUSBBuffer *psBuffer, unsigned long ulLength)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Advance the ring buffer write pointer to include the newly written
    // data.
    //
    if(ulLength)
    {
        USBRingBufAdvanceRead(&psVars->sRingBuf, ulLength);
    }
}

//*****************************************************************************
//
//! Sets the callback pointer supplied to clients of this buffer.
//!
//! \param psBuffer is the pointer to the buffer instance whose callback data
//! is to be changed.
//! \param pvCBData is the pointer the client wishes to receive on all future
//! callbacks from this buffer.
//!
//! This function sets the callback pointer which this buffer will supply
//! to clients as the \e pvCBData parameter in all future calls to the
//! event callback.
//!
//! \note If this function is to be used, the application must ensure that the
//! tUSBBuffer structure used to describe this buffer is held in RAM rather
//! than flash.  The \e pvCBData value passed is written directly into this
//! structure.
//!
//! \return Returns the previous callback pointer set for the buffer.
//
//*****************************************************************************
void *
USBBufferCallbackDataSet(tUSBBuffer *psBuffer, void *pvCBData)
{
    void *pvOldData;

    //
    // Keep a copy of the old callback data.
    //
    pvOldData = psBuffer->pvCBData;

    //
    // Replace the callback data with the new value.
    //
    psBuffer->pvCBData = pvCBData;

    //
    // Give the caller the old value back.
    //
    return(pvOldData);
}

//*****************************************************************************
//
//! Writes a block of data to the transmit buffer and queues it for
//! transmission to the USB controller.
//!
//! \param psBuffer points to the pointer instance into which data is to be
//! written.
//! \param pucData points to the first byte of data which is to be written.
//! \param ulLength is the number of bytes of data to write to the buffer.
//!
//! This function copies the supplied data into the transmit buffer.  The
//! transmit buffer data will be packetized according to the constraints
//! imposed by the lower layer in use and sent to the USB controller as soon as
//! possible.  Once a packet is transmitted and acknowledged, a
//! \b USB_EVENT_TX_COMPLETE event will be sent to the application callback
//! indicating the number of bytes that have been sent from the buffer.
//!
//! Attempts to send more data than there is space for in the transmit buffer
//! will result in fewer bytes than expected being written.  The value returned
//! by the function indicates the actual number of bytes copied to the buffer.
//!
//! \return Returns the number of bytes actually written.
//
//*****************************************************************************
unsigned long
USBBufferWrite(const tUSBBuffer *psBuffer, const unsigned char *pucData,
               unsigned long ulLength)
{
    unsigned long ulSpace;
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer && pucData);
    ASSERT(psBuffer->bTransmitBuffer == true);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // How much space is left in the buffer?
    //
    ulSpace = USBRingBufFree(&psVars->sRingBuf);

    //
    // How many bytes will we write?
    //
    ulLength = (ulLength > ulSpace) ? ulSpace : ulLength;

    //
    // Write the data to the buffer.
    //
    if(ulLength)
    {
        USBRingBufWrite(&psVars->sRingBuf, pucData, ulLength);
    }

    //
    // Try to transmit the next packet to the host.
    //
    ScheduleNextTransmission(psBuffer);

    //
    // Tell the caller how many bytes we wrote to the buffer.
    //
    return(ulLength);
}

//*****************************************************************************
//
//! Flushes a USB buffer, discarding any data that it contains.
//!
//! \param psBuffer is the pointer to the buffer instance which is to be
//! flushed.
//!
//! This function discards all data currently in the supplied buffer without
//! processing (transmitting it via the USB controller or passing it to the
//! client depending upon the buffer mode).
//!
//! \return None.
//
//*****************************************************************************
void
USBBufferFlush(const tUSBBuffer *psBuffer)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Flush the ring buffer.
    //
    USBRingBufFlush(&psVars->sRingBuf);
}

//*****************************************************************************
//
//! Reads a block of data from a USB receive buffer into storage supplied by
//! the caller.
//!
//! \param psBuffer is the pointer to the buffer instance from which data is
//! to be read.
//! \param pucData points to a buffer into which the received data will be
//! written.
//! \param ulLength is the size of the buffer pointed to by pucData.
//!
//! This function reads up to ulLength bytes of data received from the USB
//! host into the supplied application buffer.  If the receive buffer
//! contains fewer than \e ulLength bytes of data, the data that is present
//! will be copied and the return code will indicate the actual number of bytes
//! copied to \e pucData.
//!
//! \return Returns the number of bytes of data read.
//
//*****************************************************************************
unsigned long
USBBufferRead(const tUSBBuffer *psBuffer, unsigned char *pucData,
              unsigned long ulLength)
{
    tUSBBufferVars *psVars;
    unsigned long ulAvail, ulRead;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer && pucData && ulLength);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // How much data is in the buffer?
    //
    ulAvail = USBRingBufUsed(&psVars->sRingBuf);

    //
    // Determine how many bytes we can actually read.
    //
    ulRead = (ulAvail < ulLength) ? ulAvail : ulLength;

    //
    // Read the data from the buffer assuming there is some to read.
    //
    if(ulRead)
    {
        USBRingBufRead(&psVars->sRingBuf, pucData, ulRead);
    }

    //
    // Tell the caller how many bytes we wrote to their buffer.
    //
    return(ulRead);
}

//*****************************************************************************
//
//! Returns the number of bytes of data available in the buffer.
//!
//! \param psBuffer is the pointer to the buffer instance which is to be
//! queried.
//!
//! This function may be used to determine the number of bytes of data in a
//! buffer.  For a receive buffer, this indicates the number of bytes that the
//! client can read from the buffer using USBBufferRead().  For a transmit
//! buffer, this indicates the amount of data that remains to be sent to the
//! USB controller.
//!
//! \return Returns the number of bytes of data in the buffer.
//
//*****************************************************************************
unsigned long
USBBufferDataAvailable(const tUSBBuffer *psBuffer)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Return the amount of data in the buffer.
    //
    return(USBRingBufUsed(&psVars->sRingBuf));
}

//*****************************************************************************
//
//! Returns the number of free bytes in the buffer.
//!
//! \param psBuffer is the pointer to the buffer instance which is to be
//! queried.
//!
//! This function returns the number of free bytes in the buffer.  For a
//! transmit buffer, this indicates the maximum number of bytes that can be
//! passed on a call to USBBufferWrite() and accepted for transmission.  For a
//! receive buffer, it indicates the number of bytes that can be read from the
//! USB controller before the buffer will be full.
//!
//! \return Returns the number of free bytes in the buffer.
//
//*****************************************************************************
unsigned long
USBBufferSpaceAvailable(const tUSBBuffer *psBuffer)
{
    tUSBBufferVars *psVars;

    //
    // Check parameter validity.
    //
    ASSERT(psBuffer);

    //
    // Get our workspace variables.
    //
    psVars = psBuffer->pvWorkspace;

    //
    // Return the amount of space available in the buffer.
    //
    return(USBRingBufFree(&psVars->sRingBuf));
}

//*****************************************************************************
//
//! Called by the USB buffer to notify the client of asynchronous events.
//!
//! \param pvCBData is the client-supplied callback pointer associated with
//! this buffer instance.
//! \param ulEvent is the identifier of the event being sent.  This will be
//! a general event identifier of the form USBD_EVENT_xxxx or a device
//! class-dependent event of the form USBD_CDC_EVENT_xxx or
//! USBD_HID_EVENT_xxx.
//! \param ulMsgValue is an event-specific parameter value.
//! \param pvMsgData is an event-specific data pointer.
//!
//! This function is the USB buffer event handler that applications should
//! register with the USB device class driver as the callback for the channel
//! which is to be buffered using this buffer.
//!
//! \note This function will never be called by an application.  It is the
//! handler that allows the USB buffer to be inserted above the device class
//! driver or host pipe driver and below the application to offer buffering
//! support.
//!
//! \return The return value is dependent upon the event being processed.
//
//*****************************************************************************
unsigned long
USBBufferEventCallback(void *pvCBData, unsigned long ulEvent,
                       unsigned long ulMsgValue, void *pvMsgData)
{
    tUSBBuffer *psBuffer;

    //
    // Get our instance data pointers from the callback data.
    //
    psBuffer = (tUSBBuffer *)pvCBData;
    ASSERT(psBuffer);

    //
    // Which event have we been sent?
    //
    switch(ulEvent)
    {
        //
        // Data is available from the lower layer.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // This event is only relevant to us if we are a receive buffer.
            //
            if(!psBuffer->bTransmitBuffer)
            {
                return(HandleRxAvailable(psBuffer, ulMsgValue, pvMsgData));
            }
            break;
        }

        //
        // We are being asked how much data remains to be processed.
        //
        case USB_EVENT_DATA_REMAINING:
        {
            return(HandleDataRemaining(psBuffer));
        }

        //
        // A previous transmission has completed.
        //
        case USB_EVENT_TX_COMPLETE:
        {
            //
            // This event is only relevant to us if we are a transmit buffer.
            //
            if(psBuffer->bTransmitBuffer)
            {
                //
                // Handle the message then drop out of the switch so that the
                // event is echoed to the layer above.
                //
                HandleTxComplete(psBuffer, ulMsgValue);
            }
            break;
        }

        //
        // We are being asked to provide a buffer into which the next packet
        // can be received.
        //
        case USB_EVENT_REQUEST_BUFFER:
        {
            //
            // This event is only relevant to us if we are a receive buffer.
            //
            if(!psBuffer->bTransmitBuffer)
            {
                return(HandleRequestBuffer(psBuffer, ulMsgValue, pvMsgData));
            }
            break;
        }

        //
        // All other events are merely passed through to the client.
        //
        default:
        {
            break;
        }
    }

    //
    // If we drop out of the switch, we need to pass the event on to the client
    // unmodified and return the relevant return code back to the lower layer.
    //
    return(psBuffer->pfnCallback(psBuffer->pvCBData, ulEvent, ulMsgValue,
                                 pvMsgData));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
