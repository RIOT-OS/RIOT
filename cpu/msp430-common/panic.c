/*
 * Copyright (C) 2014, 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_common
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for MSP430 MCUs
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "cpu.h"
#include "lpm.h"

void panic_arch(void)
{
    /* disable watchdog and all possible sources of interrupts */
    WDTCTL = WDTPW | WDTHOLD;
#ifdef DEVELHELP
    /* enter infinite loop, into deepest possible sleep mode */
    while (1) {
        lpm_set(LPM_OFF);
    }
#endif

}
