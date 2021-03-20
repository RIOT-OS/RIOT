/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level PM driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <avr/sleep.h>

#include "periph_conf.h"
#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void pm_reboot(void)
{
    DEBUG("Reboot Software Reset\n" );

    /* XMEGA AU [MANUAL] p. 116 CTRL->Control register
     * page 13 3.12.1 Sequence for write operation to protected I/O registers
     * page 15 3.14.1 CCP – Configuration Change Protection register
     */

    /* Disable CCP for Protected IO registerand set new value*/
    _PROTECTED_WRITE(RST_CTRL, RST_SWRST_bm);
    while (1) {}
}

/*
 * DEBUG may affect this routine.
 *
 * --- Do NOT add DEBUG macro here ---
 *
 */
void pm_set(unsigned mode)
{
    unsigned irq_state = irq_disable();

    if (avr8_is_uart_tx_pending() && mode < 4) {
        irq_restore(irq_state);
        return;
    }

    switch (mode) {
    case 0:
        set_sleep_mode(SLEEP_SMODE_PDOWN_gc);
        break;
    case 1:
        set_sleep_mode(SLEEP_SMODE_PSAVE_gc);
        break;
    case 2:
        set_sleep_mode(SLEEP_SMODE_STDBY_gc);
        break;
    case 3:
        set_sleep_mode(SLEEP_SMODE_ESTDBY_gc);
        break;
    default:
        set_sleep_mode(SLEEP_SMODE_IDLE_gc);
    }
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
    irq_restore(irq_state);
}
