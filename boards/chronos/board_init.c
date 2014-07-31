/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup chronos
 * @{
 */

/**
 * @file
 * @brief       eZ430-chronos board initialization
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 */

#include <stdint.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "debug.h"

#include "display.h"
#include "display_putchar.h"

void cc430_cpu_init(void)
{
    volatile uint16_t i;
    volatile unsigned char *ptr;

    /* disable watchdog */
    WDTCTL = WDTPW + WDTHOLD;

    // ---------------------------------------------------------------------
    // Enable 32kHz ACLK
    P5SEL |= 0x03;                            // Select XIN, XOUT on P5.0 and P5.1
    UCSCTL6 &= ~XT1OFF;                       // XT1 On, Highest drive strength
    UCSCTL6 |= XCAP_3;                        // Internal load cap

    UCSCTL3 = SELA__XT1CLK;                   // Select XT1 as FLL reference
    UCSCTL4 = SELA__XT1CLK | SELS__DCOCLKDIV | SELM__DCOCLKDIV;

    // ---------------------------------------------------------------------
    // Configure CPU clock for 12MHz
    _BIS_SR(SCG0);                  // Disable the FLL control loop
    UCSCTL0 = 0x0000;          // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_5;       // Select suitable range
    UCSCTL2 = FLLD_1 + 0x16E;  // Set DCO Multiplier
    _BIC_SR(SCG0);                  // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
    for (i = 0xFF; i > 0; i--); // Time for flag to set

    // Loop until XT1 & DCO stabilizes, use do-while to insure that
    // body is executed at least once
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }
    while ((SFRIFG1 & OFIFG));

    // Disable all interrupts
    __disable_interrupt();
    // Get write-access to port mapping registers:
    PMAPPWD = 0x02D52;
    // Allow reconfiguration during runtime:
    PMAPCTL = PMAPRECFG;

    // P2.7 = TA0CCR1A or TA1CCR0A output (buzzer output)
    ptr  = &P2MAP0;
    *(ptr + 7) = PM_TA1CCR0A;
    P2OUT &= ~BIT7;
    P2DIR |= BIT7;

    // P1.5 = SPI MISO input
    ptr  = &P1MAP0;
    *(ptr + 5) = PM_UCA0SOMI;
    // P1.6 = SPI MOSI output
    *(ptr + 6) = PM_UCA0SIMO;
    // P1.7 = SPI CLK output
    *(ptr + 7) = PM_UCA0CLK;

    // Disable write-access to port mapping registers:
    PMAPPWD = 0;
    // Re-enable all interrupts
    enableIRQ();

}

void board_init(void)
{
    cc430_cpu_init();
    lcd_init();
    init_display_putchar();
    DEBUG("DISP OK");
}
