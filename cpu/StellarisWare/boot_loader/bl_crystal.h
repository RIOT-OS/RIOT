//*****************************************************************************
//
// bl_crystal.h - Macros to convert a CRYSTAL_FREQ value into the appropriate
//                RCC XTAL field define.
//
// Copyright (c) 2010-2012 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __BL_CRYSTAL_H__
#define __BL_CRYSTAL_H__

//*****************************************************************************
//
// Convert the CRYSTAL_FREQ value into the corresponding SYSCTL_RCC_XTAL_???
// value.
//
//*****************************************************************************
#if CRYSTAL_FREQ == 3579545
#define XTAL_VALUE              SYSCTL_RCC_XTAL_3_57MHZ
#elif CRYSTAL_FREQ == 3686400
#define XTAL_VALUE              SYSCTL_RCC_XTAL_3_68MHZ
#elif CRYSTAL_FREQ == 4000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_4MHZ
#elif CRYSTAL_FREQ == 4096000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_4_09MHZ
#elif CRYSTAL_FREQ == 4915200
#define XTAL_VALUE              SYSCTL_RCC_XTAL_4_91MHZ
#elif CRYSTAL_FREQ == 5000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_5MHZ
#elif CRYSTAL_FREQ == 5120000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_5_12MHZ
#elif CRYSTAL_FREQ == 6000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_6MHZ
#elif CRYSTAL_FREQ == 6144000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_6_14MHZ
#elif CRYSTAL_FREQ == 7372800
#define XTAL_VALUE              SYSCTL_RCC_XTAL_7_37MHZ
#elif CRYSTAL_FREQ == 8000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_8MHZ
#elif CRYSTAL_FREQ == 8192000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_8_19MHZ
#elif CRYSTAL_FREQ == 10000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_10MHZ
#elif CRYSTAL_FREQ == 12000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_12MHZ
#elif CRYSTAL_FREQ == 12288000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_12_2MHZ
#elif CRYSTAL_FREQ == 13560000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_13_5MHZ
#elif CRYSTAL_FREQ == 14318180
#define XTAL_VALUE              SYSCTL_RCC_XTAL_14_3MHZ
#elif CRYSTAL_FREQ == 16000000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_16MHZ
#elif CRYSTAL_FREQ == 16384000
#define XTAL_VALUE              SYSCTL_RCC_XTAL_16_3MHZ
#else
#error ERROR: Unknown CRYSTAL_FREQ value specified!
#endif

#endif // __BL_CRYSTAL_H__
