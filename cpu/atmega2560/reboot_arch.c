/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega2560
 * @{
 *
 * @file        reboot_arch.c
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <avr/wdt.h>

#include "arch/reboot_arch.h"
#include "cpu.h"


int reboot_arch(int mode)
{
    printf("Going into reboot, mode %i\n", mode);

    /*
     * Since the AVR doesn't support a real software reset, we set the Watchdog
     * Timer on a 250ms timeout. Consider this a kludge.
     */
    wdt_enable(WDTO_250MS);


    return 0;
}
