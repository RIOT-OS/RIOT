/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @file
 * @brief       LibCSP clock implementation
 *
 * @author      Pierre Millot <pierre.millot@grenoble-inp.org>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "csp/csp_types.h"

#include "cpu.h"
#include "ztimer.h"

__attribute__((weak)) void csp_clock_get_time(csp_timestamp_t *time)
{
    time->tv_sec = ztimer_now(ZTIMER_SEC);
    time->tv_nsec = 0;
}

__attribute__((weak)) int csp_clock_set_time(const csp_timestamp_t *time)
{
    (void)time;
    return CSP_ERR_NOTSUP;
}
