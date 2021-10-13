/**************************************************************************//**
 * @file
 * @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File
 *           for the NXP LPC17xx Device Series
 * @version  V1.09
 * @date     09. November 2013
 *
 * @note     Integrated, adopted, and renamed for RIOT by Oliver Hahm.
 *
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#include "vendor/LPC17xx.h"
#include "vendor/conf.h"

/**
 * Initialize the system
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System. */
void SystemInit(void)
{
#if (CLOCK_SETUP)                       /* Clock Setup */
    LPC_SC->SCS       = SCS_Val;

    if (SCS_Val & (1 << 5)) {             /* If Main Oscillator is enabled */
        while ((LPC_SC->SCS & (1 << 6)) == 0); /* Wait for Oscillator to be ready */
    }

    LPC_SC->CCLKCFG   = CCLKCFG_Val;      /* Setup Clock Divider */

    LPC_SC->PCLKSEL0  = PCLKSEL0_Val;     /* Peripheral Clock Selection */
    LPC_SC->PCLKSEL1  = PCLKSEL1_Val;

    LPC_SC->CLKSRCSEL = CLKSRCSEL_Val;    /* Select Clock Source for PLL0 */

#if (PLL0_SETUP)
    LPC_SC->PLL0CFG   = PLL0CFG_Val;      /* configure PLL0 */
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;

    LPC_SC->PLL0CON   = 0x01;             /* PLL0 Enable */
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;

    while (!(LPC_SC->PLL0STAT & (1 << 26))); /* Wait for PLOCK0 */

    LPC_SC->PLL0CON   = 0x03;             /* PLL0 Enable & Connect */
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;

    while (!(LPC_SC->PLL0STAT & ((1 << 25) | (1 << 24)))); /* Wait for PLLC0_STAT & PLLE0_STAT */

#endif

#if (PLL1_SETUP)
    LPC_SC->PLL1CFG   = PLL1CFG_Val;
    LPC_SC->PLL1FEED  = 0xAA;
    LPC_SC->PLL1FEED  = 0x55;

    LPC_SC->PLL1CON   = 0x01;             /* PLL1 Enable */
    LPC_SC->PLL1FEED  = 0xAA;
    LPC_SC->PLL1FEED  = 0x55;

    while (!(LPC_SC->PLL1STAT & (1 << 10))); /* Wait for PLOCK1 */

    LPC_SC->PLL1CON   = 0x03;             /* PLL1 Enable & Connect */
    LPC_SC->PLL1FEED  = 0xAA;
    LPC_SC->PLL1FEED  = 0x55;

    while (!(LPC_SC->PLL1STAT & ((1 << 9) | (1 << 8)))); /* Wait for PLLC1_STAT & PLLE1_STAT */

#else
    LPC_SC->USBCLKCFG = USBCLKCFG_Val;    /* Setup USB Clock Divider */
#endif

    LPC_SC->PCONP     = PCONP_Val;        /* Power Control for Peripherals */

    LPC_SC->CLKOUTCFG = CLKOUTCFG_Val;    /* Clock Output Configuration */
#endif

#if (FLASH_SETUP == 1)                  /* Flash Accelerator Setup */
    LPC_SC->FLASHCFG  = (LPC_SC->FLASHCFG & ~0x0000F000) | FLASHCFG_Val;
#endif
}
