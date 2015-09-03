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
 * @brief       eZ430-chronos beeper
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      mikoff
 *
 */

#include <stdint.h>
#include <stdint.h>
#include <cc430f6137.h>

#include "buzzer.h"
#include "xtimer.h"

void buzzer_beep(uint8_t pitch, uint32_t duration)
{
    // Reset TA1R, set up mode, TA1 runs from 32768Hz ACLK
    TA1CTL = TACLR | MC_1 | TASSEL__ACLK;

    // Set PWM frequency
    TA1CCR0 = pitch;

    // Enable IRQ, set output mode "toggle"
    TA1CCTL0 = OUTMOD_4;

    // Allow buzzer PWM output on P2.7
    P2SEL |= BIT7;

    xtimer_usleep(duration);

    // Stop PWM timer
    TA1CTL &= ~(BIT4 | BIT5);

    // Reset and disable buzzer PWM output
    P2OUT &= ~BIT7;
    P2SEL &= ~BIT7;
    TA1CCTL0 &= ~CCIE;
}
