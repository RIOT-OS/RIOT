//*****************************************************************************
//
// sine.c - Fixed point sine trigonometric function.
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

#include "utils/sine.h"

//*****************************************************************************
//
//! \addtogroup sine_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// A table of the value of the sine function for the first ninety degrees with
// 129 entries (that is, [0] = 0 degrees, [128] = 90 degrees).  Each entry is
// in 0.16 fixed point notation.
//
//*****************************************************************************
static const unsigned short g_pusFixedSineTable[] =
{
    0x0000, 0x0324, 0x0648, 0x096C, 0x0C8F, 0x0FB2, 0x12D5, 0x15F6, 0x1917,
    0x1C37, 0x1F56, 0x2273, 0x2590, 0x28AA, 0x2BC4, 0x2EDB, 0x31F1, 0x3505,
    0x3817, 0x3B26, 0x3E33, 0x413E, 0x4447, 0x474D, 0x4A50, 0x4D50, 0x504D,
    0x5347, 0x563E, 0x5931, 0x5C22, 0x5F0E, 0x61F7, 0x64DC, 0x67BD, 0x6A9B,
    0x6D74, 0x7049, 0x7319, 0x75E5, 0x78AD, 0x7B70, 0x7E2E, 0x80E7, 0x839C,
    0x864B, 0x88F5, 0x8B9A, 0x8E39, 0x90D3, 0x9368, 0x95F6, 0x987F, 0x9B02,
    0x9D7F, 0x9FF6, 0xA267, 0xA4D2, 0xA736, 0xA994, 0xABEB, 0xAE3B, 0xB085,
    0xB2C8, 0xB504, 0xB73A, 0xB968, 0xBB8F, 0xBDAE, 0xBFC7, 0xC1D8, 0xC3E2,
    0xC5E4, 0xC7DE, 0xC9D1, 0xCBBB, 0xCD9F, 0xCF7A, 0xD14D, 0xD318, 0xD4DB,
    0xD695, 0xD848, 0xD9F2, 0xDB94, 0xDD2D, 0xDEBE, 0xE046, 0xE1C5, 0xE33C,
    0xE4AA, 0xE60F, 0xE76B, 0xE8BF, 0xEA09, 0xEB4B, 0xEC83, 0xEDB2, 0xEED8,
    0xEFF5, 0xF109, 0xF213, 0xF314, 0xF40B, 0xF4FA, 0xF5DE, 0xF6BA, 0xF78B,
    0xF853, 0xF912, 0xF9C7, 0xFA73, 0xFB14, 0xFBAC, 0xFC3B, 0xFCBF, 0xFD3A,
    0xFDAB, 0xFE13, 0xFE70, 0xFEC4, 0xFF0E, 0xFF4E, 0xFF84, 0xFFB1, 0xFFD3,
    0xFFEC, 0xFFFB, 0xFFFF
};

//*****************************************************************************
//
//! Computes an approximation of the sine of the input angle.
//!
//! \param ulAngle is an angle expressed as a 0.32 fixed-point value that is
//! the percentage of the way around a circle.
//!
//! This function computes the sine for the given input angle.  The angle is
//! specified in 0.32 fixed point format, and is therefore always between 0 and
//! 360 degrees, inclusive of 0 and exclusive of 360.
//!
//! \return Returns the sine of the angle, in 16.16 fixed point format.
//
//*****************************************************************************
long
sine(unsigned long ulAngle)
{
    unsigned long ulIdx;

    //
    // Add 0.5 to the angle.  Since only the upper 9 bits are used to compute
    // the sine value, adding one to the tenth bit is 0.5 from the point of
    // view of the sine table.
    //
    ulAngle += 0x00400000;

    //
    // Get the index into the sine table from bits 30:23.
    //
    ulIdx = (ulAngle >> 23) & 255;

    //
    // If bit 30 is set, the angle is between 90 and 180 or 270 and 360.  In
    // these cases, the sine value is decreasing from one instead of increasing
    // from zero.  The indexing into the table needs to be reversed.
    //
    if(ulAngle & 0x40000000)
    {
        ulIdx = 256 - ulIdx;
    }

    //
    // Get the value of the sine.
    //
    ulIdx = g_pusFixedSineTable[ulIdx];

    //
    // If bit 31 is set, the angle is between 180 and 360.  In this case, the
    // sine value is negative; otherwise it is positive.
    //
    if(ulAngle & 0x80000000)
    {
        return(0 - ulIdx);
    }
    else
    {
        return(ulIdx);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
