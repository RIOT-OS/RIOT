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

void clock_reset(void) {
    /* Reset the clock system */
    reset_component(RESET_PLL_SYS, RESET_PLL_SYS);
}

/**
 * @brief Configures the XOSC and then sets CLK_SYS, PLL_SYS and CLK_PERI to it
 * @warning Make sure to call clock_reset() before this function to reset the
 * clock system
 * @see RP2350 Docs Chapter 8, mostly 8.2 for more details
 */
void cpu_clock_init(void) {
    /* Enable the XOSC */
    xosc_start();

    /* Setup the PLL using the XOSC as the reference clock. */
    PLL_SYS->FBDIV_INT =
    PLL_FEEDBACK_DIVIDER_VALUE; /* Set the feedback divider */

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
    while (CLOCKS->CLK_SYS_SELECTED != CLK_SYS_SELECTED_PERI_FIELD_VALUE) {
    }

    /* AUXSRC = 0x0 -> CLK_SYS Indirectly through lower line */
    CLOCKS->CLK_PERI_CTRL = CLK_PERI_CTRL_ENABLE_BIT;
}

/** @} */
