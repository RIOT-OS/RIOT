/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Clock configuration implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "periph_cpu.h"
#include "regs/clocks.h"
#include "regs/ticks.h"

void clock_reset(void) {
    /* Reset the clock system */
    reset_component(RESET_PLL_SYS, RESET_PLL_SYS);
}

/**
 * @brief Starts a tick generator with the given number of CLK_REF cycles per tick
 *
 * @param ctrl  CTRL register of the tick generator, followed by its CYCLES register
 */
static void _tick_start(volatile uint32_t *ctrl, uint32_t cycles)
{
    atomic_clear(ctrl, TICKS_PROC0_CTRL_ENABLE_BITS);
    *(ctrl+1) = cycles;
    atomic_set(ctrl, TICKS_PROC0_CTRL_ENABLE_BITS);
}

/**
 * @brief Configures the XOSC and then sets CLK_SYS, PLL_SYS and CLK_PERI to it
 * @warning Make sure to call clock_reset() before this function to reset the
 *          clock system.
 * @see RP2350 Docs Chapter 8, mostly 8.2 for more details
 */
void cpu_clock_init(void) {
    /* Enable the XOSC */
    xosc_start();

    /* Switch CLK_REF from ROSC to XOSC, so that everything fed by CLK_REF
     * (e.g. the tick generators of TIMER0/1) runs from the crystal. */
    CLOCKS->CLK_REF_CTRL = CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC;
    /* CLK_REF_SELECTED is one-hot encoded */
    while (CLOCKS->CLK_REF_SELECTED != (1u << CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC)) {}

    /* Let the tick generators produce 1 µs ticks from CLK_REF */
    _tick_start(&TICKS->PROC0_CTRL, TICK_CYCLES); /* time base for SysTick */
    _tick_start(&TICKS->PROC1_CTRL, TICK_CYCLES);
    _tick_start(&TICKS->TIMER0_CTRL, TICK_CYCLES); /* time base for the timers */
    _tick_start(&TICKS->TIMER1_CTRL, TICK_CYCLES);
    _tick_start(&TICKS->WATCHDOG_CTRL, TICK_CYCLES); /* time base for watchdog */
    _tick_start(&TICKS->RISCV_CTRL, TICK_CYCLES); /* time base for mtime */

    /* Setup the PLL using the XOSC as the reference clock. */
    PLL_SYS->CS = PLL_REF_DIV; /* Set the reference divider */
    PLL_SYS->FBDIV_INT = PLL_FEEDBACK_DIVIDER_VALUE; /* Set the feedback divider */

    /* Set the post-dividers for the PLL output.*/
    PLL_SYS->PRIM = PDIV;
    /* Turn on PLL */
    atomic_clear(&PLL_SYS->PWR,
        PLL_PWR_PD_BITS | PLL_PWR_VCOPD_BITS | PLL_PWR_POSTDIVPD_BITS);

    /* sleep 10ms to allow the PLL to stabilize */
    xosc_sleep(10);

    /* Based on the description in chapter 8 this is something that should be done
     * However, it appears to cause issues and is not done by other examples on the
     * internet. This needs to be investigated further. */

    /* Wait for lock */
    /* while (!(PLL_SYS->CS & PLL_CS_LOCK_BITS)) { */
    /* Wait for the PLL to lock */
    /* } */

    /* AUXSRC = 0x0 7:5 && SRC == 0x0 0 */
    CLOCKS->CLK_SYS_CTRL = CLK_SYS_PERI_CTRL_ENABLE_BIT;

    /* This register contains one decoded bit for each of the clock sources
     * enumerated in the CTRL SRC field. The bit does not directly correlate with
     * the value of the SRC field For example 0x0 is the first bit while 0x1 is
     * the second bit. In some way this makes sense, in some way I lost too much
     * time on this. */
    while (CLOCKS->CLK_SYS_SELECTED != CLK_SYS_SELECTED_PERI_FIELD_VALUE) {}

    /* AUXSRC = 0x0 -> CLK_SYS Indirectly through lower line */
    CLOCKS->CLK_PERI_CTRL = CLK_PERI_CTRL_ENABLE_BIT;
}

/** @} */
