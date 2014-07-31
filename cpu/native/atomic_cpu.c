/**
 * Native CPU atomic.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include "atomic.h"
#include "irq.h"
#include "debug.h"

unsigned int atomic_set_return(unsigned int *val, unsigned int set)
{
    unsigned int old_val;
    unsigned int old_state;

    DEBUG("atomic_set_return\n");

    old_state = disableIRQ();

    old_val = *val;
    *val = set;

    restoreIRQ(old_state);

    return old_val;
}
