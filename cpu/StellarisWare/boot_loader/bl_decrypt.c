//*****************************************************************************
//
// bl_decrypt.c - Code for performing an in-place decryption of the firmware
//                image as it is downloaded.
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
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "bl_config.h"
#include "boot_loader/bl_decrypt.h"

//*****************************************************************************
//
//! \addtogroup bl_decrypt_api
//! @{
//
//*****************************************************************************
#if defined(ENABLE_DECRYPTION) || defined(DOXYGEN)

//*****************************************************************************
//
//! Performs an in-place decryption of downloaded data.
//!
//! \param pucBuffer is the buffer that holds the data to decrypt.
//! \param ulSize is the size, in bytes, of the buffer that was passed in via
//! the pucBuffer parameter.
//!
//! This function is a stub that could provide in-place decryption of the data
//! that is being downloaded to the device.
//!
//! \return None.
//
//*****************************************************************************
void
DecryptData(unsigned char *pucBuffer, unsigned long ulSize)
{
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif

