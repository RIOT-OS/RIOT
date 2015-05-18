//*****************************************************************************
//
// bl_flash.c - Flash programming functions used by the boot loader.
//
// Copyright (c) 2006-2012 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "inc/hw_flash.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_memmap.h"
#include "bl_config.h"
#include "boot_loader/bl_flash.h"

//*****************************************************************************
//
//! Erases a single 1KB block of internal flash.
//!
//! \param ulAddress is the address of the block of flash to erase.
//!
//! This function erases a single 1KB block of the internal flash, blocking
//! until the erase has completed.
//!
//! \return None
//
//*****************************************************************************
void
BLInternalFlashErase(unsigned long ulAddress)
{
    //
    // Erase this block of the flash.
    //
    HWREG(FLASH_FMA) = ulAddress;
    HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;

    //
    // Wait until the flash has been erased.
    //
    while(HWREG(FLASH_FMC) & FLASH_FMC_ERASE)
    {
    }
}

//*****************************************************************************
//
//! Programs a block of data at a given address in the internal flash.
//!
//! \param ulDstAddr is the address of the first word to be programmed in flash.
//! \param pucSrcData is a pointer to the first byte to be programmed.
//! \param ulLength is the number of bytes to program.  This must be a multiple
//! of 4.
//!
//! This function writes a block of data to the internal flash at a given
//! address.  Since the flash is written a word at a time, the data must be a
//! multiple of 4 bytes and the destination address, ulDstAddr, must be on a
//! word boundary.
//!
//! \return None
//
//*****************************************************************************
void
BLInternalFlashProgram(unsigned long ulDstAddr, unsigned char *pucSrcData,
                       unsigned long ulLength)
{
    unsigned long ulLoop;

    for(ulLoop = 0; ulLoop < ulLength; ulLoop += 4)
    {
        //
        // Program this word into flash.
        //
        HWREG(FLASH_FMA) = ulDstAddr + ulLoop;
        HWREG(FLASH_FMD) = *(unsigned long *)(pucSrcData + ulLoop);
        HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;

        //
        // Wait until the flash has been programmed.
        //
        while(HWREG(FLASH_FMC) & FLASH_FMC_WRITE)
        {
        }
    }
}

//*****************************************************************************
//
//! Returns the size of the internal flash in bytes.
//!
//! This function returns the total number of bytes of internal flash in the
//! current part.  No adjustment is made for any sections reserved via
//! options defined in bl_config.h.
//!
//! \return Returns the total number of bytes of internal flash.
//
//*****************************************************************************
unsigned long
BLInternalFlashSizeGet(void)
{
    return(((HWREG(SYSCTL_DC0) & SYSCTL_DC0_FLASHSZ_M) + 1) << 11);
}

//*****************************************************************************
//
//! Checks whether a given start address is valid for a download.
//!
//! This function checks to determine whether the given address is a valid
//! download image start address given the options defined in bl_config.h.
//!
//! \return Returns non-zero if the address is valid or 0 otherwise.
//
//*****************************************************************************
unsigned long
BLInternalFlashStartAddrCheck(unsigned long ulAddr, unsigned long ulImgSize)
{
    unsigned long ulFlashSize;

    //
    // Determine the size of the flash available on the part in use.
    //
    ulFlashSize = ((HWREG(SYSCTL_DC0) & SYSCTL_DC0_FLASHSZ_M) + 1) << 11;

    //
    // If we are reserving space at the top of flash then this space is not
    // available for application download but it is availble to be updated
    // directly.
    //
#ifdef FLASH_RSVD_SPACE
    if((ulFlashSize - FLASH_RSVD_SPACE) != ulAddr)
    {
        ulFlashSize -= FLASH_RSVD_SPACE;
    }
#endif

    //
    // Is the address we were passed a valid start address?  We allow:
    //
    // 1. Address 0 if configured to update the boot loader.
    // 2. The start of the reserved block if parameter space is reserved (to
    //    allow a download of the parameter block contents).
    // 3. The application start address specified in bl_config.h.
    //
    // The function fails if the address is not one of these, if the image
    // size is larger than the available space or if the address is not word
    // aligned.
    //
    if((
#ifdef ENABLE_BL_UPDATE
#ifdef FLASH_PATCH_COMPATIBLE
                        (ulAddr != 0x1000) &&
#else
                        (ulAddr != 0) &&
#endif
#endif
#ifdef FLASH_RSVD_SPACE
                        (ulAddr != (ulFlashSize - FLASH_RSVD_SPACE)) &&
#endif
                        (ulAddr != APP_START_ADDRESS)) ||
                       ((ulAddr + ulImgSize) > ulFlashSize) ||
                       ((ulAddr & 3) != 0))
    {
        return(0);
    }
    else
    {
        return(1);
    }
}

//*****************************************************************************
//
//! Checks whether a flash access violation occurred.
//!
//! This function checks whether an access violation error occurred during
//! the previous program or erase operation.
//!
//! \return Returns 0 if no error occurred or a non-zero value if an error was
//! reported.
//
//*****************************************************************************
void
BLInternalFlashErrorClear(void)
{
    //
    // Clear the flash controller access interrupt.
    //
    HWREG(FLASH_FCMISC) = FLASH_FCMISC_AMISC;
}

//*****************************************************************************
//
//! Checks whether a flash access violation occurred.
//!
//! This function checks whether an access violation error occurred since the
//! last call to BLInternalFlashErrorClear().
//!
//! \return Returns 0 if no error occurred or a non-zero value if an error was
//! reported.
//
//*****************************************************************************
unsigned long
BLInternalFlashErrorCheck(void)
{
    return(HWREG(FLASH_FCRIS) & FLASH_FCRIS_ARIS);
}
