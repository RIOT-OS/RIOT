/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt, Steffen Robertz
 *               2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega256rfr2
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of common AVR periph/pm functions
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#include <avr/wdt.h>

#include "irq.h"
#include "periph/pm.h"
void pm_reboot(void)
{
    /* set a variable to stack to signalizes soft reset */
    #if defined(MODULE_AT86RFR2)
    /* clear MCU Status Register Interrupt flags */
    MCUSR = 0x00;
    _asm_ _volatile_("mov r3, %0\n" :: "r" (0xAA));
    #endif

    /*
     * Since the AVR doesn't support a real software reset, we set the Watchdog
     * Timer on a 250ms timeout. Consider this a kludge.
     */
    irq_disable();
    wdt_enable(WDTO_250MS);

    while (1) {
    }
}
