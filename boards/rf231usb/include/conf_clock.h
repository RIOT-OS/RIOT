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
 * @brief       Board specific clock settings for the rf231usb
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef CONF_CLOCK_H_INCLUDED
#define CONF_CLOCK_H_INCLUDED

#include "sam3s.h"

/* "The RF231USB-RD is populated with an external 18.432MHz crystal
   oscillator, assembled on the top side of the printed circuit board
   close to the MCU. Connected to XIN/XOUT (pins 61 and 62) it
   delivers an accurate clock for the ATSAM3S4BA." */
#define BOARD_FREQ_MAINCK_XTAL    (18432000UL)
#define BOARD_FREQ_MAINCK_BYPASS  BOARD_FREQ_MAINCK_XTAL
#define BOARD_OSC_STARTUP_US      (15625UL)

/* Oscillator identifiers */
#define OSC_MAINCK_4M_RC            3    /*< internal 4MHz oscillator */
#define OSC_MAINCK_8M_RC            4    /*< internal 8MHz oscillator */
#define OSC_MAINCK_12M_RC           5    /*< internal 12MHz oscillator */
#define OSC_MAINCK_XTAL             6    /*< external oscillator */

/* "Since the design is a USB device, low power modes are not expected
 * ('always on'). Therefore no external 32.768kHz */
/* crystal is provided." */
#define BOARD_FREQ_SLCK_XTAL      (0)
#define BOARD_FREQ_SLCK_BYPASS    (0)

/* Configure PLL0 (PLLA): 18.432 Mhz * 125 / 24 = 96.000 Mhz */
#define CONFIG_PLL0_SOURCE        OSC_MAINCK_XTAL
#define CONFIG_PLL0_MUL           (125)
#define CONFIG_PLL0_DIV           (24)

/* Use PLL0 as system clock source */
#define CONFIG_SYSCLK_SOURCE      (8)

/* Use a prescaler of /2 on the clock source: 48.000 Mhz */
#define CONFIG_SYSCLK_PRES        PMC_MCKR_PRES_CLK_2

/* Configure PLL1 (PLLB): 18.432 Mhz * 125 / 24 = 96.000 Mhz */
#define CONFIG_PLL1_SOURCE        PLL_SRC_MAINCK_XTAL
#define CONFIG_PLL1_MUL           (125)
#define CONFIG_PLL1_DIV           (24)

/* Use PLL1 as USB clock source */
#define CONFIG_USBCLK_SOURCE      (1)

/* Also with a divider of 2: 48.000 Mhz like USB needs it */
#define CONFIG_USBCLK_DIV         (2)

/* Initialize the boards clock sources and setup MCU */
void clock_init(void);

#endif /* CONF_CLOCK_H_INCLUDED */
