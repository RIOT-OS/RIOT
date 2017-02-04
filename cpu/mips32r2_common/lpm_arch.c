/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <mips/m32c0.h>
#include "arch/lpm_arch.h"

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    if (target == LPM_IDLE || target == LPM_SLEEP) {
        /* Dont wait if interrupts are not enabled - we would never return!*/
        if (mips32_get_c0(C0_STATUS) & SR_IE) {
            asm volatile ("wait");
        }
    }
    return LPM_ON; /* once wait returns we are back 'on' */
}

enum lpm_mode lpm_arch_get(void)
{
    return LPM_ON;
}
