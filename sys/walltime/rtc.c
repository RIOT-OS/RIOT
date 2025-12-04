/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_walltime
 * @{
 *
 * @file
 * @brief       Wall-Clock RTC backend
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "periph/rtc.h"
#include "modules.h"

#ifdef MODULE_WALLTIME_IMPL_RTC

int walltime_impl_get(struct tm *time, uint16_t *ms)
{
    if (IS_USED(MODULE_PERIPH_RTC_MS)) {
        rtc_get_time_ms(time, ms);
    } else {
        rtc_get_time(time);
    }
    return 0;
}

int walltime_impl_set(struct tm *time)
{
    rtc_set_time(time);
    return 0;
}

#endif
