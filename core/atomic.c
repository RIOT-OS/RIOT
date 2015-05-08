/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */


/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Generic implementation of the kernel's atomic interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "irq.h"
#include "cpu.h"
#include "atomic.h"

#if (ARCH_HAS_ATOMIC_SET_RETURN == 0)

unsigned int atomic_set_return(unsigned int *val, unsigned int set)
{
    unsigned int mask = disableIRQ();
    unsigned int old_val = *val;
    *val = set;
    restoreIRQ(mask);
    return old_val;
}

#endif

/* Set ARCH_HAS_ATOMIC_COMPARE_AND_SWAP within cpu.h to override this function */
#if (ARCH_HAS_ATOMIC_COMPARE_AND_SWAP == 0)

int atomic_cas(atomic_int_t *var, int old, int now)
{
    unsigned int mask = disableIRQ();

    if (ATOMIC_VALUE(*var) != old) {
        restoreIRQ(mask);
        return 0;
    }

    ATOMIC_VALUE(*var) = now;
    restoreIRQ(mask);
    return 1;
}

#endif
