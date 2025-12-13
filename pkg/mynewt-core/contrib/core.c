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

#include "buildinfo/cpu.h"
#include "os/os_cputime.h"
#include "hal/hal_timer.h"

#ifdef CPU_FAM_NRF51
#define TIMER_NUM       3
#else
#define TIMER_NUM       5
#endif

void mynewt_core_init(void)
{
#if (MYNEWT_VAL_OS_CPUTIME_TIMER_NUM >= 0) && (defined(CPU_NRF51) || defined(CPU_NRF52))
    /* in mynewt-nimble and uwb-core OS_CPUTIMER_TIMER_NUM == 5 is NRF_RTC0,
       for nimble this must be used for the BLE stack and must go through
       mynewt timer initialization for it to work properly. The RTC frequency
       should be set to the highest possible value, so 32768Hz */
    assert(MYNEWT_VAL_OS_CPUTIME_TIMER_NUM == TIMER_NUM);
    assert(MYNEWT_VAL_OS_CPUTIME_FREQ == 32768);
    int rc = hal_timer_init(MYNEWT_VAL_OS_CPUTIME_TIMER_NUM, NULL);
    assert(rc == 0);
    rc = hal_timer_config(MYNEWT_VAL_OS_CPUTIME_TIMER_NUM,
                          MYNEWT_VAL_OS_CPUTIME_FREQ);
    assert(rc == 0);
    (void)rc;
#endif
}
