/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels atomic interface
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "atomic.h"
#include "irq.h"
#include "cpu.h"

#if ARCH_HAS_ATOMIC_COMPARE_AND_SWAP
int atomic_cas(atomic_int_t *var, int old, int now)
{
    int tmp;
    int status;

    /* Load exclusive */
    tmp = __LDREXW((volatile uint32_t *)(&ATOMIC_VALUE(*var)));

    if (tmp != old) {
        /* Clear memory exclusivity */
        __CLREX();
        return 0;
    }

    /* Try to write the new value */
    status = __STREXW(now, (volatile uint32_t *)(&ATOMIC_VALUE(*var)));

    return (status == 0);
}
#endif
