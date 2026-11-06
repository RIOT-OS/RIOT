/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_ztimer_periph_rtc  ztimer periph/rtc backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/rtc backend
 *
 * This ztimer module implements a ztimer virtual clock on top of periph/rtc.
 *
 * @{
 *
 * @file
 * @brief       ztimer rtc/timer backend API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer_periph_rtc structure definition
 *
 * rtc has no private fields, thus this is just a typedef to ztimer_clock_t.
 */
typedef ztimer_clock_t ztimer_periph_rtc_t;

/**
 * @brief   ztimer rtc backend initialization function
 *
 * @param[in, out]  clock   ztimer_periph_rtc object to initialize
 */
void ztimer_periph_rtc_init(ztimer_periph_rtc_t *clock);

#ifdef __cplusplus
}
#endif

/** @} */
