/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for ATmega MCUs
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <avr/wdt.h>
#include "lpm.h"

void panic_arch(void)
{
    wdt_disable();
#ifdef DEVELHELP
    /* enter infinite loop, into deepest possible sleep mode */
    while (1) {
        lpm_set(LPM_OFF);
    }
#endif

}
