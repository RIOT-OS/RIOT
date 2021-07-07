/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core bootstrapping  functions
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>

#include "os/os_cputime.h"
#include "hal/hal_timer.h"

void mynewt_core_init(void)
{
#if (MYNEWT_VAL_OS_CPUTIME_TIMER_NUM >= 0) && (defined(CPU_NRF51) || defined(CPU_NRF52))
    int rc = hal_timer_init(5, NULL);
    assert(rc == 0);
    rc = os_cputime_init(MYNEWT_VAL_OS_CPUTIME_FREQ);
    assert(rc == 0);
    (void) rc;
#endif
}
