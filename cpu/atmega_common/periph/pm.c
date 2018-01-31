/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of common AVR periph/pm functions
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <avr/wdt.h>

#include "irq.h"
#include "periph/pm.h"

void pm_reboot(void)
{
    #if defined(CPU_ATMEGA256RFR2)
    /* set a variable to stack to signalizes soft reset */
    /* clear MCU Status Register Interrupt flags */
    MCUSR = 0x00;
    __asm__ __volatile__("mov r3, %0\n" :: "r" (0xAA));
    #endif /* CPU_ATMEGA256RFR2 */

    /*
     * Since the AVR doesn't support a real software reset, we set the Watchdog
     * Timer on a 250ms timeout. Consider this a kludge.
     */
    irq_disable();
    wdt_enable(WDTO_250MS);
    while(1){}
}
