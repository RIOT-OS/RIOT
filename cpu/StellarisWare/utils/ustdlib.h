//*****************************************************************************
//
// ustdlib.h - Prototypes for simple standard library functions.
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

#ifndef __USTDLIB_H__
#define __USTDLIB_H__

#include <stdarg.h>

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
//! \addtogroup ustdlib_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! A structure that contains the broken down date and time.
//
//*****************************************************************************
typedef struct
{
    //
    //! The number of years since 0 AD.
    //
    unsigned short usYear;

    //
    //! The month, where January is 0 and December is 11.
    //
    unsigned char ucMon;

    //
    //! The day of the month.
    //
    unsigned char ucMday;

    //
    //! The day of the week, where Sunday is 0 and Saturday is 6.
    //
    unsigned char ucWday;

    //
    //! The number of hours.
    //
    unsigned char ucHour;

    //
    //! The number of minutes.
    //
    unsigned char ucMin;

    //
    //! The number of seconds.
    //
    unsigned char ucSec;
}
tTime;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern int uvsnprintf(char *pcBuf, unsigned long ulSize, const char *pcString,
                      va_list vaArgP);
extern int usprintf(char *pcBuf, const char *pcString, ...);
extern int usnprintf(char *pcBuf, unsigned long ulSize, const char *pcString,
                     ...);
extern void ulocaltime(unsigned long ulTime, tTime *psTime);
extern unsigned long umktime(tTime *psTime);
extern int ustrlen (const char *pcStr);
extern char *ustrncpy (char *pcDst, const char *pcSrc, int iNum);
extern unsigned long ustrtoul(const char *pcStr, const char **ppcStrRet,
                              int iBase);
extern char *ustrstr(const char *pcHaystack, const char *pcNeedle);
extern int ustrnicmp(const char *pcStr1, const char *pcStr2, int iCount);
extern int ustrncmp(const char *pcStr1, const char *pcStr2, int iCount);
extern int ustrcmp(const char *pcStr1, const char *pcStr2);
extern int ustrcasecmp(const char *pcStr1, const char *pcStr2);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USTDLIB_H__
