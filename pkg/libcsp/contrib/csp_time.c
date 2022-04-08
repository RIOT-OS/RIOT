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
 * @brief       LibCSP system time
 *
 * @author      Pierre Millot <pierre.millot@grenoble-inp.org>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <csp/arch/csp_time.h>

#include "ztimer.h"
#include "timex.h"

uint32_t csp_get_ms(void)
{
    return ztimer_now(ZTIMER_MSEC);
}

uint32_t csp_get_ms_isr(void)
{
    return ztimer_now(ZTIMER_MSEC);
}

uint32_t csp_get_s(void)
{
    return ztimer_now(ZTIMER_SEC);
}

uint32_t csp_get_s_isr(void)
{
    return ztimer_now(ZTIMER_SEC);
}
