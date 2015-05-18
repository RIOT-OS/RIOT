//*****************************************************************************
//
// bl_flash.h - Flash programming functions used by the boot loader.
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

#ifndef __BL_FLASH_H__
#define __BL_FLASH_H__

#include "driverlib/rom.h"

//*****************************************************************************
//
// Basic functions for erasing and programming internal flash.
//
//*****************************************************************************
extern void BLInternalFlashErase(unsigned long ulAddress);
extern void BLInternalFlashProgram(unsigned long ulDstAddr,
                                   unsigned char *pucSrcData,
                                   unsigned long ulLength);
extern unsigned long BLInternalFlashSizeGet(void);
extern unsigned long BLInternalFlashStartAddrCheck(unsigned long ulAddr,
                                                   unsigned long ulImgSize);
extern unsigned long BLInternalFlashErrorCheck(void);
extern void BLInternalFlashErrorClear(void);

//*****************************************************************************
//
// If the user has not specified which flash programming functions to use,
// default to the basic, internal flash functions on Sandstorm, Fury and
// DustDevil parts or the ROM-resident function for Tempest-class parts.
//
//*****************************************************************************
#ifndef BL_FLASH_ERASE_FN_HOOK
#define BL_FLASH_ERASE_FN_HOOK(ulAddress)                                     \
        {                                                                     \
            HWREG(FLASH_FMA) = (ulAddress);                                   \
            HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;             \
            while(HWREG(FLASH_FMC) & FLASH_FMC_ERASE)                         \
            {                                                                 \
            }                                                                 \
        }
#else
extern void BL_FLASH_ERASE_FN_HOOK(unsigned long ulAddress);
#endif

#ifndef BL_FLASH_PROGRAM_FN_HOOK
#ifdef ROM_FlashProgram
#define BL_FLASH_PROGRAM_FN_HOOK(ulDstAddr, pucSrcData, ulLength)             \
        ROM_FlashProgram((unsigned long *)pucSrcData, ulDstAddr,              \
                         (((ulLength) + 3) & ~3))
#else
#define BL_FLASH_PROGRAM_FN_HOOK(ulDstAddr, pucSrcData, ulLength)             \
        {                                                                     \
            unsigned long ulFlashProgLoop;                                    \
                                                                              \
            for(ulFlashProgLoop = 0; ulFlashProgLoop < (ulLength);            \
                ulFlashProgLoop += 4)                                         \
            {                                                                 \
                HWREG(FLASH_FMA) = (ulDstAddr) + ulFlashProgLoop;             \
                HWREG(FLASH_FMD) = *(unsigned long *)((pucSrcData) +          \
                                                      ulFlashProgLoop);       \
                HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;         \
                while(HWREG(FLASH_FMC) & FLASH_FMC_WRITE)                     \
                {                                                             \
                }                                                             \
            }                                                                 \
        }
#endif
#else
extern unsigned long BL_FLASH_PROGRAM_FN_HOOK(unsigned long ulDstAddr,
                                              unsigned char *pucSrcData,
                                              unsigned long ulLength);
#endif

#ifndef BL_FLASH_CL_ERR_FN_HOOK
#define BL_FLASH_CL_ERR_FN_HOOK() HWREG(FLASH_FCMISC) = FLASH_FCMISC_AMISC
#else
extern void BL_FLASH_CL_ERR_FN_HOOK(void);
#endif

#ifndef BL_FLASH_ERROR_FN_HOOK
#define BL_FLASH_ERROR_FN_HOOK()    (HWREG(FLASH_FCRIS) & FLASH_FCRIS_ARIS)
#else
extern unsigned long BL_FLASH_ERROR_FN_HOOK(void);
#endif

#ifndef BL_FLASH_SIZE_FN_HOOK
#define BL_FLASH_SIZE_FN_HOOK()                                               \
        (((HWREG(SYSCTL_DC0) & SYSCTL_DC0_FLASHSZ_M) + 1) << 11)
#else
extern unsigned long BL_FLASH_SIZE_FN_HOOK(void);
#endif

#ifndef BL_FLASH_END_FN_HOOK
#define BL_FLASH_END_FN_HOOK()                                               \
        (((HWREG(SYSCTL_DC0) & SYSCTL_DC0_FLASHSZ_M) + 1) << 11)
#else
extern unsigned long BL_FLASH_END_FN_HOOK(void);
#endif

#ifndef BL_FLASH_AD_CHECK_FN_HOOK
#define BL_FLASH_AD_CHECK_FN_HOOK(ulAddr, ulSize)                             \
        BLInternalFlashStartAddrCheck((ulAddr), (ulSize))
#else
extern unsigned long BL_FLASH_AD_CHECK_FN_HOOK(unsigned long ulAddress,
                                               unsigned long ulLength);
#endif

#endif // __BL_FLASH_H__
