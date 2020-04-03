/*
** ###################################################################
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    KEA128RM, Rev. 22, Jun 2014
**     Version:             rev. 1.4, 2015-07-24
**     Build:               b150730
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright (c) 2015 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2013-07-30)
**         Initial version.
**     - rev. 1.1 (2013-10-29)
**         Definition of BITBAND macros updated to support peripherals with 32-bit access disabled.
**     - rev. 1.2 (2014-01-10)
**         CAN - Corrected address of TSIDR1 register.
**         CAN - Corrected name of MSCAN_TDLR bit DLC to TDLC.
**         FTM0 - Added access macro for EXTTRIG register.
**         NVIC - Registers access macros improved.
**         SCB - Unused bits removed, mask, shift macros improved.
**         Defines of interrupt vectors aligned to RM.
**     - rev. 1.3 (2014-06-18)
**         The declaration of clock configurations has been moved to separate header file system_MKE02Z2.h
**         Module access macro {module}_BASES replaced by {module}_BASE_PTRS.
**         I2C - Renamed status register S to S1 to match RM naming.
**         Renamed interrupts: INT_PMC to INT_LVD_LVW, INT_ADC to ADC0,INT_WDOG to INT_Watchdog.
**     - rev. 1.4 (2015-07-24)
**         Correction of backward compatibility.
**
** ###################################################################
*/

/**
 * @defgroup    cpu_kinetis_ics Kinetis ICS
 * @ingroup     cpu_kinetis
 * @brief       Implementation of the Kinetis internal clock source controller
 *              (ICS) driver
 *
 * @{
 *
 * @file
 * @brief       ICS and watchdog initialization code
 *
 * Originates from NXP's examples (called system_SKEAZ1284.c there)
 *
 */

#include <stdint.h>
#include "cpu.h"



/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemDisableWatchdog(void)
{
    /* WDOG->TOVAL: TOVAL=0xE803 */
    WDOG->TOVAL = WDOG_TOVAL_TOVAL(0xE803); /* Timeout value */
    /* WDOG->CS2: WIN=0,FLG=0,?=0,PRES=0,?=0,?=0,CLK=1 */
    WDOG->CS2 = WDOG_CS2_CLK(0x01);         /* 1-kHz clock source */
    /* WDOG->CS1: EN=0,INT=0,UPDATE=1,TST=0,DBG=0,WAIT=1,STOP=1 */
    WDOG->CS1 = WDOG_CS1_UPDATE_MASK |
                WDOG_CS1_TST(0x00) |
                WDOG_CS1_WAIT_MASK |
                WDOG_CS1_STOP_MASK;
}

void SystemInit(void)
{

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

#if  (CLOCK_SETUP)
    /* System clock initialization */

    /* Set system prescalers and clock sources */
    SIM->CLKDIV = SIM_CLKDIV_VALUE;                                                                 /* Set system prescalers */
    ICS->C2 = ((ICS->C2) & (uint32_t)(~(ICS_C2_BDIV_MASK))) | ((ICS_C2_VALUE) &(ICS_C2_BDIV_MASK)); /* Set system prescaler */
#if (ICS_MODE == ICS_MODE_FEI || ICS_MODE == ICS_MODE_FBI || ICS_MODE == ICS_MODE_BLPI)
    ICS->C1 = ICS_C1_VALUE;                                                                         /* Set C1 (clock source selection, int. reference enable etc.) */
    ICS->C2 = ICS_C2_VALUE;                                                                         /* Set C2 (ext. and int. reference clock selection) */
    ICS->C4 = ICS_C4_VALUE;                                                                         /* Set C4 (loss of lock interrupt, clock monitor) */
    OSC->CR = OSC_CR_VALUE;                                                                         /* Set OSC_CR (OSCERCLK disable) */
#else /* ICS_MODE */
    /* Set ICS and OSC */
    ICS->C2 = ICS_C2_VALUE;                                 /* Set C2 (ext. and int. reference clock selection) */
    OSC->CR = OSC_CR_VALUE;                                 /* Set OSC_CR (OSCERCLK enable, range select) */
    ICS->C4 = ICS_C4_VALUE;                                 /* Set C4 (loss of lock interrupt, clock monitor) */
    ICS->C1 = ICS_C1_VALUE;                                 /* Set C1 (clock source selection, int. reference disable etc.) */
    if (((OSC_CR_VALUE) &OSC_CR_OSCEN_MASK) != 0U) {
        while ((OSC->CR & OSC_CR_OSCINIT_MASK) == 0x00U) {  /* Check that the oscillator is running */
        }
    }
#endif /* ICS_MODE */

    /* Common for all ICS modes */

#if (ICS_MODE == ICS_MODE_FEI || ICS_MODE == ICS_MODE_FEE)
    while ((ICS->S & ICS_S_CLKST_MASK) != 0x00U) { /* Wait until output of FLL is selected */
    }
#elif (ICS_MODE == ICS_MODE_FBI || ICS_MODE == ICS_MODE_BLPI)
    while ((ICS->S & ICS_S_CLKST_MASK) != 0x04U) { /* Wait until FLL bypassed, internal reference clock is selected */
    }
#elif (ICS_MODE == ICS_MODE_FBE || ICS_MODE == ICS_MODE_BLPE)
    while ((ICS->S & ICS_S_CLKST_MASK) != 0x08U) { /* Wait until FLL bypassed, external reference clock is selected */
    }
#endif
#endif /* (CLOCK_SETUP) */
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate(void)
{

    uint32_t ICSOUTClock;                                                      /* Variable to store output clock frequency of the ICS module */

    if ((ICS->C1 & ICS_C1_CLKS_MASK) == 0x0u) {
        /* Output of FLL is selected */
        if ((ICS->C1 & ICS_C1_IREFS_MASK) == 0x0u) {
            uint8_t Divider;

            /* External reference clock is selected */
            ICSOUTClock = CPU_XTAL_CLK_HZ;          /* System oscillator drives ICS clock */
            Divider = (uint8_t)(1u << ((ICS->C1 & ICS_C1_RDIV_MASK) >> ICS_C1_RDIV_SHIFT));
            ICSOUTClock = (ICSOUTClock / Divider);  /* Calculate the divided FLL reference clock */
            if ((OSC->CR & OSC_CR_RANGE_MASK) != 0x0u) {
                ICSOUTClock /= 32u;                 /* If high range is enabled, additional 32 divider is active */
            }
        }
        else {
            ICSOUTClock = CPU_INT_CLK_HZ;                                       /* The internal reference clock is selected */
        }
        ICSOUTClock *= 1280u;                                                   /* Apply 1280 FLL multiplier */
    }
    else if ((ICS->C1 & ICS_C1_CLKS_MASK) == 0x40u) {
        /* Internal reference clock is selected */
        ICSOUTClock = CPU_INT_CLK_HZ;
    }
    else if ((ICS->C1 & ICS_C1_CLKS_MASK) == 0x80u) {
        /* External reference clock is selected */
        ICSOUTClock = CPU_XTAL_CLK_HZ;
    }
    else {
        /* Reserved value */
        return;
    }
    ICSOUTClock = ICSOUTClock >> ((ICS->C2 & ICS_C2_BDIV_MASK) >> ICS_C2_BDIV_SHIFT);
    SystemCoreClock = (ICSOUTClock / (1u + ((SIM->CLKDIV & SIM_CLKDIV_OUTDIV1_MASK) >> SIM_CLKDIV_OUTDIV1_SHIFT)));

}

/** @} */
