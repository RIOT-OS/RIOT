//*****************************************************************************
//
// usbringbuf.c - USB library ring buffer management utilities.
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
#include "driverlib/interrupt.h"
#include "usblib/usblib.h"

//*****************************************************************************
//
//! \addtogroup usblib_buffer_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Define NULL, if not already defined.
//
//*****************************************************************************
#ifndef NULL
#define NULL                    ((void *)0)
#endif

//*****************************************************************************
//
// Change the value of a variable atomically.
//
// \param pulVal points to the index whose value is to be modified.
// \param ulDelta is the number of bytes to increment the index by.
// \param ulSize is the size of the buffer the index refers to.
//
// This function is used to increment a read or write buffer index that may be
// written in various different contexts.  It ensures that the
// read/modify/write sequence is not interrupted and, hence, guards against
// corruption of the variable.  The new value is adjusted for buffer wrap.
//
// \return None.
//
//*****************************************************************************
static void
UpdateIndexAtomic(volatile unsigned long *pulVal, unsigned long ulDelta,
                  unsigned long ulSize)
{
    tBoolean bIntsOff;

    //
    // Turn interrupts off temporarily.
    //
    bIntsOff = IntMasterDisable();

    //
    // Update the variable value.
    //
    *pulVal += ulDelta;

    //
    // Correct for wrap.  We use a loop here since we don't want to use a
    // modulus operation with interrupts off but we don't want to fail in
    // case ulDelta is greater than ulSize (which is extremely unlikely but...)
    //
    while(*pulVal >= ulSize)
    {
        *pulVal -= ulSize;
    }

    //
    // Restore the interrupt state
    //
    if(!bIntsOff)
    {
        IntMasterEnable();
    }
}

//*****************************************************************************
//
//! Determines whether a ring buffer is full or not.
//!
//! \param ptUSBRingBuf is the ring buffer object to empty.
//!
//! This function is used to determine whether or not a given ring buffer is
//! full.  The structure is specifically to ensure that we do not see
//! warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b true if the buffer is full or \b false otherwise.
//
//*****************************************************************************
tBoolean
USBRingBufFull(tUSBRingBufObject *ptUSBRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptUSBRingBuf->ulWriteIndex;
    ulRead = ptUSBRingBuf->ulReadIndex;

    //
    // Return the full status of the buffer.
    //
    return((((ulWrite + 1) % ptUSBRingBuf->ulSize) == ulRead) ? true : false);
}

//*****************************************************************************
//
//! Determines whether a ring buffer is empty or not.
//!
//! \param ptUSBRingBuf is the ring buffer object to empty.
//!
//! This function is used to determine whether or not a given ring buffer is
//! empty.  The structure is specifically to ensure that we do not see
//! warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b true if the buffer is empty or \b false otherwise.
//
//*****************************************************************************
tBoolean
USBRingBufEmpty(tUSBRingBufObject *ptUSBRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptUSBRingBuf->ulWriteIndex;
    ulRead = ptUSBRingBuf->ulReadIndex;

    //
    // Return the empty status of the buffer.
    //
    return((ulWrite == ulRead) ? true : false);
}

//*****************************************************************************
//
//! Empties the ring buffer.
//!
//! \param ptUSBRingBuf is the ring buffer object to empty.
//!
//! Discards all data from the ring buffer.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufFlush(tUSBRingBufObject *ptUSBRingBuf)
{
    tBoolean bIntsOff;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Set the Read/Write pointers to be the same.  Do this with interrupts
    // disabled to prevent the possibility of corruption of the read index.
    //
    bIntsOff = IntMasterDisable();
    ptUSBRingBuf->ulReadIndex = ptUSBRingBuf->ulWriteIndex;
    if(!bIntsOff)
    {
        IntMasterEnable();
    }
}

//*****************************************************************************
//
//! Returns number of bytes stored in ring buffer.
//!
//! \param ptUSBRingBuf is the ring buffer object to check.
//!
//! This function returns the number of bytes stored in the ring buffer.
//!
//! \return Returns the number of bytes stored in the ring buffer.
//
//*****************************************************************************
unsigned long
USBRingBufUsed(tUSBRingBufObject *ptUSBRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptUSBRingBuf->ulWriteIndex;
    ulRead = ptUSBRingBuf->ulReadIndex;

    //
    // Return the number of bytes contained in the ring buffer.
    //
    return((ulWrite >= ulRead) ? (ulWrite - ulRead) :
           (ptUSBRingBuf->ulSize - (ulRead - ulWrite)));
}

//*****************************************************************************
//
//! Returns number of bytes available in a ring buffer.
//!
//! \param ptUSBRingBuf is the ring buffer object to check.
//!
//! This function returns the number of bytes available in the ring buffer.
//!
//! \return Returns the number of bytes available in the ring buffer.
//
//*****************************************************************************
unsigned long
USBRingBufFree(tUSBRingBufObject *ptUSBRingBuf)
{
    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Return the number of bytes available in the ring buffer.
    //
    return((ptUSBRingBuf->ulSize - 1) - USBRingBufUsed(ptUSBRingBuf));
}

//*****************************************************************************
//
//! Returns number of contiguous bytes of data stored in ring buffer ahead of
//! the current read pointer.
//!
//! \param ptUSBRingBuf is the ring buffer object to check.
//!
//! This function returns the number of contiguous bytes of data available in
//! the ring buffer ahead of the current read pointer.  This represents the
//! largest block of data which does not straddle the buffer wrap.
//!
//! \return Returns the number of contiguous bytes available.
//
//*****************************************************************************
unsigned long
USBRingBufContigUsed(tUSBRingBufObject *ptUSBRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptUSBRingBuf->ulWriteIndex;
    ulRead = ptUSBRingBuf->ulReadIndex;

    //
    // Return the number of contiguous bytes available.
    //
    return((ulWrite >= ulRead) ? (ulWrite - ulRead) :
           (ptUSBRingBuf->ulSize - ulRead));
}

//*****************************************************************************
//
//! Returns number of contiguous free bytes available in a ring buffer.
//!
//! \param ptUSBRingBuf is the ring buffer object to check.
//!
//! This function returns the number of contiguous free bytes ahead of the
//! current write pointer in the ring buffer.
//!
//! \return Returns the number of contiguous bytes available in the ring
//! buffer.
//
//*****************************************************************************
unsigned long
USBRingBufContigFree(tUSBRingBufObject *ptUSBRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptUSBRingBuf->ulWriteIndex;
    ulRead = ptUSBRingBuf->ulReadIndex;

    //
    // Return the number of contiguous bytes available.
    //
    if(ulRead > ulWrite)
    {
        //
        // The read pointer is above the write pointer so the amount of free
        // space is the difference between the two indices minus 1 to account
        // for the buffer full condition (write index one behind read index).
        //
        return((ulRead - ulWrite) - 1);
    }
    else
    {
        //
        // If the write pointer is above the read pointer, the amount of free
        // space is the size of the buffer minus the write index.  We need to
        // add a special-case adjustment if the read index is 0 since we need
        // to leave 1 byte empty to ensure we can tell the difference between
        // the buffer being full and empty.
        //
        return(ptUSBRingBuf->ulSize - ulWrite - ((ulRead == 0) ? 1 : 0));
    }
}

//*****************************************************************************
//
//! Returns the size in bytes of a ring buffer.
//!
//! \param ptUSBRingBuf is the ring buffer object to check.
//!
//! This function returns the size of the ring buffer.
//!
//! \return Returns the size in bytes of the ring buffer.
//
//*****************************************************************************
unsigned long
USBRingBufSize(tUSBRingBufObject *ptUSBRingBuf)
{
    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Return the number of bytes available in the ring buffer.
    //
    return(ptUSBRingBuf->ulSize);
}

//*****************************************************************************
//
//! Reads a single byte of data from a ring buffer.
//!
//! \param ptUSBRingBuf points to the ring buffer to be written to.
//!
//! This function reads a single byte of data from a ring buffer.
//!
//! \return The byte read from the ring buffer.
//
//*****************************************************************************
unsigned char
USBRingBufReadOne(tUSBRingBufObject *ptUSBRingBuf)
{
    unsigned char ucTemp;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Verify that space is available in the buffer.
    //
    ASSERT(USBRingBufUsed(ptUSBRingBuf) != 0);

    //
    // Write the data byte.
    //
    ucTemp = ptUSBRingBuf->pucBuf[ptUSBRingBuf->ulReadIndex];

    //
    // Increment the read index.
    //
    UpdateIndexAtomic(&ptUSBRingBuf->ulReadIndex, 1, ptUSBRingBuf->ulSize);

    //
    // Return the character read.
    //
    return(ucTemp);
}

//*****************************************************************************
//
//! Reads data from a ring buffer.
//!
//! \param ptUSBRingBuf points to the ring buffer to be read from.
//! \param pucData points to where the data should be stored.
//! \param ulLength is the number of bytes to be read.
//!
//! This function reads a sequence of bytes from a ring buffer.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufRead(tUSBRingBufObject *ptUSBRingBuf, unsigned char *pucData,
               unsigned long ulLength)
{
    unsigned long ulTemp;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);
    ASSERT(pucData != NULL);
    ASSERT(ulLength != 0);

    //
    // Verify that data is available in the buffer.
    //
    ASSERT(ulLength <= USBRingBufUsed(ptUSBRingBuf));

    //
    // Read the data from the ring buffer.
    //
    for(ulTemp = 0; ulTemp < ulLength; ulTemp++)
    {
        pucData[ulTemp] = USBRingBufReadOne(ptUSBRingBuf);
    }
}

//*****************************************************************************
//
//! Removes bytes from the ring buffer by advancing the read index.
//!
//! \param ptUSBRingBuf points to the ring buffer from which bytes are to be
//! removed.
//! \param ulNumBytes is the number of bytes to be removed from the buffer.
//!
//! This function advances the ring buffer read index by a given number of
//! bytes, removing that number of bytes of data from the buffer.  If \e
//! ulNumBytes is larger than the number of bytes currently in the buffer, the
//! buffer is emptied.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufAdvanceRead(tUSBRingBufObject *ptUSBRingBuf,
                      unsigned long ulNumBytes)
{
    unsigned long ulCount;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Make sure that we are not being asked to remove more data than is
    // there to be removed.
    //
    ulCount = USBRingBufUsed(ptUSBRingBuf);
    ulCount =  (ulCount < ulNumBytes) ? ulCount : ulNumBytes;

    //
    // Advance the buffer read index by the required number of bytes.
    //
    UpdateIndexAtomic(&ptUSBRingBuf->ulReadIndex, ulCount,
                      ptUSBRingBuf->ulSize);
}

//*****************************************************************************
//
//! Adds bytes to the ring buffer by advancing the write index.
//!
//! \param ptUSBRingBuf points to the ring buffer to which bytes have been
//! added.
//! \param ulNumBytes is the number of bytes added to the buffer.
//!
//! This function should be used by clients who wish to add data to the buffer
//! directly rather than via calls to USBRingBufWrite() or
//! USBRingBufWriteOne().  It advances the write index by a given number of
//! bytes.
//!
//! \note It is considered an error if the \e ulNumBytes parameter is larger
//! than the amount of free space in the buffer and a debug build of this
//! function will fail (ASSERT) if this condition is detected.  In a release
//! build, the buffer read pointer will be advanced if too much data is written
//! but this will, of course, result in some of the oldest data in the buffer
//! being discarded and also, depending upon how data is being read from
//! the buffer, may result in a race condition which could corrupt the read
//! pointer.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufAdvanceWrite(tUSBRingBufObject *ptUSBRingBuf,
                       unsigned long ulNumBytes)
{
    unsigned long ulCount;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Make sure we were not asked to add a silly number of bytes.
    //
    ASSERT(ulNumBytes <= ptUSBRingBuf->ulSize);

    //
    // Determine how much free space we currently think the buffer has.
    //
    ulCount = USBRingBufFree(ptUSBRingBuf);

    //
    // Check that the client has not added more data to the buffer than there
    // is space for.  In this case, corruption may have occurred since the
    // buffer may have been read under interrupt context while the writer was
    // busy trashing the area around the read pointer.
    //
    ASSERT(ulCount >= ulNumBytes);

    //
    // Update the write pointer.
    //
    ptUSBRingBuf->ulWriteIndex += ulNumBytes;

    //
    // Check and correct for wrap.
    //
    if(ptUSBRingBuf->ulWriteIndex >= ptUSBRingBuf->ulSize)
    {
        ptUSBRingBuf->ulWriteIndex -= ptUSBRingBuf->ulSize;
    }

    //
    // Did the client add more bytes than the buffer had free space for?  This
    // should be considered a bug since, unless this function is called in
    // the same context as the code which is reading from the buffer, writing
    // over the earliest data can cause corrupted data to be read.  The
    // ASSERT above catches this in debug builds but, in release builds, we
    // go ahead and try to fix up the read pointer appropriately.
    //
    if(ulCount < ulNumBytes)
    {
        //
        // Yes - we need to advance the read pointer to ahead of the write
        // pointer to discard some of the oldest data.
        //
        ptUSBRingBuf->ulReadIndex = ptUSBRingBuf->ulWriteIndex + 1;

        //
        // Correct for buffer wrap if necessary.
        //
        if(ptUSBRingBuf->ulReadIndex >= ptUSBRingBuf->ulSize)
        {
            ptUSBRingBuf->ulReadIndex -= ptUSBRingBuf->ulSize;
        }
    }

}

//*****************************************************************************
//
//! Writes a single byte of data to a ring buffer.
//!
//! \param ptUSBRingBuf points to the ring buffer to be written to.
//! \param ucData is the byte to be written.
//!
//! This function writes a single byte of data into a ring buffer.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufWriteOne(tUSBRingBufObject *ptUSBRingBuf, unsigned char ucData)
{
    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);

    //
    // Verify that space is available in the buffer.
    //
    ASSERT(USBRingBufFree(ptUSBRingBuf) != 0);

    //
    // Write the data byte.
    //
    ptUSBRingBuf->pucBuf[ptUSBRingBuf->ulWriteIndex] = ucData;

    //
    // Increment the write index.
    //
    UpdateIndexAtomic(&ptUSBRingBuf->ulWriteIndex, 1, ptUSBRingBuf->ulSize);
}

//*****************************************************************************
//
//! Writes data to a ring buffer.
//!
//! \param ptUSBRingBuf points to the ring buffer to be written to.
//! \param pucData points to the data to be written.
//! \param ulLength is the number of bytes to be written.
//!
//! This function write a sequence of bytes into a ring buffer.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufWrite(tUSBRingBufObject *ptUSBRingBuf, const unsigned char *pucData,
                unsigned long ulLength)
{
    unsigned long ulTemp;

    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);
    ASSERT(pucData != NULL);
    ASSERT(ulLength != 0);

    //
    // Verify that space is available in the buffer.
    //
    ASSERT(ulLength <= USBRingBufFree(ptUSBRingBuf));

    //
    // Write the data into the ring buffer.
    //
    for(ulTemp = 0; ulTemp < ulLength; ulTemp++)
    {
        USBRingBufWriteOne(ptUSBRingBuf, pucData[ulTemp]);
    }
}

//*****************************************************************************
//
//! Initializes a ring buffer object.
//!
//! \param ptUSBRingBuf points to the ring buffer to be initialized.
//! \param pucBuf points to the data buffer to be used for the ring buffer.
//! \param ulSize is the size of the buffer in bytes.
//!
//! This function initializes a ring buffer object, preparing it to store data.
//!
//! \return None.
//
//*****************************************************************************
void
USBRingBufInit(tUSBRingBufObject *ptUSBRingBuf, unsigned char *pucBuf,
               unsigned long ulSize)
{
    //
    // Check the arguments.
    //
    ASSERT(ptUSBRingBuf != NULL);
    ASSERT(pucBuf != NULL);
    ASSERT(ulSize != 0);

    //
    // Initialize the ring buffer object.
    //
    ptUSBRingBuf->ulSize = ulSize;
    ptUSBRingBuf->pucBuf = pucBuf;
    ptUSBRingBuf->ulWriteIndex = ptUSBRingBuf->ulReadIndex = 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
