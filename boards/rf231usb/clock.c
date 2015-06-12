/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_rf231usb
 * @{
 *
 * @file
 * @brief       Clock configuration code for the rf231usb
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include "conf_clock.h"

#define SYS_BOARD_PLLA     (CKGR_PLLAR_ONE                              \
                            | CKGR_PLLAR_MULA((CONFIG_PLL0_MUL - 1))    \
                            | CKGR_PLLAR_PLLACOUNT(0x3F)                \
                            | CKGR_PLLAR_DIVA(CONFIG_PLL0_DIV))

#define SYS_BOARD_PLLB     (CKGR_PLLBR_MULB((CONFIG_PLL1_MUL - 1))      \
                            | CKGR_PLLBR_PLLBCOUNT(0x3F)                \
                            | CKGR_PLLAR_DIVA(CONFIG_PLL1_DIV))         \

/* MCK: PLLA / 2 = 96 / 2 = 48 Mhz */
#define SYS_BOARD_MCK      (CONFIG_SYSCLK_PRES | PMC_MCKR_CSS_PLLA_CLK)

/* USB: PLLB / 2 = 96 / (1 + 1) = 48 Mhz */
#define SYS_BOARD_UDPCK    (CONFIG_USBCLK_SOURCE                        \
                            | PMC_USB_USBDIV(CONFIG_USBCLK_DIV))

void clock_init(void)
{
    /* Set FWS suitable up to 54Mhz operating frequency */
    EFC->EEFC_FMR = EEFC_FMR_FWS(2);

    /* Initialize main oscillator */
    if (!(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL)) {
        PMC->CKGR_MOR =
            CKGR_MOR_KEY(0x37)
            | BOARD_OSC_STARTUP_US
            | CKGR_MOR_MOSCRCEN
            | CKGR_MOR_MOSCXTEN;

        while (!(PMC->PMC_SR & PMC_SR_MOSCXTS)) {
            /* Wait for main oscillator */
        }
    }

    /* Switch to 18.432MHz XTAL oscillator */
    PMC->CKGR_MOR =
        CKGR_MOR_KEY(0x37)
        | BOARD_OSC_STARTUP_US
        | CKGR_MOR_MOSCRCEN
        | CKGR_MOR_MOSCXTEN
        | CKGR_MOR_MOSCSEL;

    while (!(PMC->PMC_SR & PMC_SR_MOSCSELS)) {
        /* Wait for XTAL oscillator to be ready */
    }

    /* Select main clock as master clock source */
    PMC->PMC_MCKR =
        (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk)
        | PMC_MCKR_CSS_MAIN_CLK;

    while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {
        /* Wait for main clock to be ready */
    }

    /* Initialize PLLA/PLL0 */
    PMC->CKGR_PLLAR = SYS_BOARD_PLLA;

    while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {
        /* Wait for PLLA to be locked */
    }

    /* Initialize PLLB/PLL1 */
    PMC->CKGR_PLLBR = SYS_BOARD_PLLB;

    while (!(PMC->PMC_SR & PMC_SR_LOCKB)) {
        /* Wait for PLLB to be locked */
    }

    /* Select USB clock source and divider */
    PMC->PMC_USB = SYS_BOARD_UDPCK;

    /* Switch to main clock */
    PMC->PMC_MCKR =
        (SYS_BOARD_MCK & ~PMC_MCKR_CSS_Msk)
        | PMC_MCKR_CSS_MAIN_CLK;

    while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {
        /* Wait for main clock to be ready */
    }

    /* Switch to PLLA/PLL0 */
    PMC->PMC_MCKR = SYS_BOARD_MCK;

    while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {
        /* Wait for main clock to be ready */
    }
}
