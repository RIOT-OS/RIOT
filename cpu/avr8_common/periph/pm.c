/*
 * Copyright (C) 2022-2023 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @ingroup     cpu_avr8_common_periph
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

#include "irq.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @note: The pm_set assumes that interrupts are disable and there is no reason
 *        to save SREG here.
 *
 * @note: DEBUG affects this routine.
 */
void pm_set(unsigned mode)
{
    DEBUG("pm_set(%d)\n", mode);

    if (avr8_is_uart_tx_pending()) {
        mode = PM_NUM_MODES - 1;
    }

    switch (mode) {
    case 0:
        set_sleep_mode(AVR8_PM_SLEEP_MODE_0);
        break;
#if PM_NUM_MODES > 1
    case 1:
        set_sleep_mode(AVR8_PM_SLEEP_MODE_1);
        break;
#endif
#if PM_NUM_MODES > 2
    case 2:
        set_sleep_mode(AVR8_PM_SLEEP_MODE_2);
        break;
#endif
#if PM_NUM_MODES > 3
    case 3:
        set_sleep_mode(AVR8_PM_SLEEP_MODE_3);
        break;
#endif
    default:
        set_sleep_mode(SLEEP_MODE_IDLE);
        break;
    }

    DEBUG("mode selected: %d\n", mode);

/*
 * Critical Section to correct enable SLEEP instruction on RIOT-OS
 */
#if (AVR8_PM_DISABLE_BOD_ON_SLEEP == 1)
#ifdef sleep_bod_disable
    sleep_bod_disable();
#endif
#endif
    sleep_enable();
    sei();
    sleep_cpu();
    cli();
    sleep_disable();
}
