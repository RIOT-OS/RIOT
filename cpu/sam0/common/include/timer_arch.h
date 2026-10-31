/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       CPU specific part of the timer API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

static inline bool timer_poll_channel(tim_t tim, int channel)
{
    /**
     * ~~~~~~~~~~~~~~~{.c}
     * typedef union {
     *   TcCount8                  COUNT8;
     *   TcCount16                 COUNT16;
     *   TcCount32                 COUNT32;
     * } Tc;
     * ~~~~~~~~~~~~~~~
     * INTFLAG has for all timerwidth the same offset, so it does not matter which union member is used.
     */
    switch (channel) {
    case 0:
        return timer_config[tim].dev->COUNT32.INTFLAG.reg & TC_INTFLAG_MC0;
    case 1:
        return timer_config[tim].dev->COUNT32.INTFLAG.reg & TC_INTFLAG_MC1;
    default:
        return false;
    }
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

/** @} */
