/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>

 * @}
 */

#include "cpu.h"
#include "cpu_clock.h"
#include "cpu_pm.h"
#include "panic.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CPU_ATXMEGA_CLK_SCALE_INIT
#define CPU_ATXMEGA_CLK_SCALE_INIT    CPU_ATXMEGA_CLK_SCALE_DIV1
#endif
#ifndef CPU_ATXMEGA_BUS_SCALE_INIT
#define CPU_ATXMEGA_BUS_SCALE_INIT    CPU_ATXMEGA_BUS_SCALE_DIV1_1
#endif

extern uint8_t mcusr_mirror;

void avr8_reset_cause(void)
{
    if (mcusr_mirror & (1 << RST_PORF_bp)) {
        DEBUG("Power-on reset.\n");
    }
    if (mcusr_mirror & (1 << RST_EXTRF_bp)) {
        DEBUG("External reset!\n");
    }
    if (mcusr_mirror & (1 << RST_BORF_bp)) {
        DEBUG("Brown-out reset!\n");
    }
    if (mcusr_mirror & (1 << RST_WDRF_bp)) {
        DEBUG("Watchdog reset!\n");
    }
    if (mcusr_mirror & (1 << RST_PDIRF_bp)) {
        DEBUG("Programming and Debug Interface reset!\n");
    }
    if (mcusr_mirror & (1 << RST_SRF_bp)) {
        DEBUG("Software reset!\n");
    }
    if (mcusr_mirror & (1 << RST_SDRF_bp)) {
        DEBUG("Spike Detection reset!\n");
    }
}

void __attribute__((weak)) avr8_clk_init(void)
{
    pm_periph_power_off();

    /* XMEGA A3U [DATASHEET] p.23 After reset, the device starts up running
     * from the 2MHz internal oscillator. The other clock sources, DFLLs
     * and PLL, are turned off by default.
     *
     * Configure clock to 32MHz with calibration
     * application note AVR1003
     *
     * From errata http://www.avrfreaks.net/forum/xmega-dfll-does-it-work
     * In order to use the automatic runtime calibration for the 2 MHz or
     * the 32 MHz internal oscillators, the DFLL for both oscillators and
     * both oscillators has to be enabled for one to work.
     */
    OSC.PLLCTRL = 0;

    /* Enable the internal PLL & 32MHz & 32KHz oscillators */
    OSC.CTRL |= OSC_PLLEN_bm | OSC_RC32MEN_bm | OSC_RC32KEN_bm;

    /* Wait for 32Khz and 32MHz oscillator to stabilize */
    while ((OSC.STATUS & (OSC_RC32KRDY_bm | OSC_RC32MRDY_bm))
        != (OSC_RC32KRDY_bm | OSC_RC32MRDY_bm)) {}

    /* Enable DFLL - defaults to calibrate against internal 32Khz clock */
    DFLLRC2M.CTRL = DFLL_ENABLE_bm;

    /* Enable DFLL - defaults to calibrate against internal 32Khz clock */
    DFLLRC32M.CTRL = DFLL_ENABLE_bm;

    /* Some ATxmega need sync clocks after enable DFLL.  Otherwise clock may
     * stay at 2MHz source when try enable.
     */
    while ((OSC.STATUS & OSC_RC32MRDY_bm) != OSC_RC32MRDY_bm) {}

    atxmega_set_prescaler(CPU_ATXMEGA_CLK_SCALE_INIT,
                          CPU_ATXMEGA_BUS_SCALE_INIT);

    /* Disable CCP for Protected IO register and set new value*/
    /* Switch to 32MHz clock */
    _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc);
}

/* This is a vector which is aliased to __vector_default,
 * the vector executed when an ISR fires with no accompanying
 * ISR handler. This may be used along with the ISR() macro to
 * create a catch-all for undefined but used ISRs for debugging
 * purposes.
 */
ISR(BADISR_vect, ISR_NAKED)
{
    avr8_reset_cause();

#ifdef LED_PANIC
    /* Use LED light to signal ERROR. */
    LED_PANIC;
#endif

    core_panic(PANIC_GENERAL_ERROR, "BADISR");
}
