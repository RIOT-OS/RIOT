//*****************************************************************************
//
// sine.h - Prototypes for the fixed point sine trigonometric function.
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

#ifndef __SINE_H__
#define __SINE_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup sine_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Computes an approximation of the cosine of the input angle.
//!
//! \param ulAngle is an angle expressed as a 0.32 fixed-point value that is
//! the percentage of the way around a circle.
//!
//! This function computes the cosine for the given input angle.  The angle is
//! specified in 0.32 fixed point format, and is therefore always between 0 and
//! 360 degrees, inclusive of 0 and exclusive of 360.
//!
//! \return Returns the cosine of the angle, in 16.16 fixed point format.
//
//*****************************************************************************
#define cosine(ulAngle)         sine((ulAngle + 0x40000000))

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Prototype for the fixed point sine function.
//
//*****************************************************************************
extern long sine(unsigned long ulAngle);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SINE_H__
