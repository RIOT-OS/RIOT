/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_ztimer_overhead  ztimer overhead utility
 * @ingroup     sys_ztimer
 * @brief       ztimer overhead measurement functionality
 *
 * @{
 *
 * @file
 * @brief       ztimer_overhead API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Measure overhead for ztimer_set()
 *
 * This function can be used to measure the overhead incurred by ztimer_set().
 * It will configure a callback to trigger after @p base ticks, then return the
 * number of ticks that have passed, minus @p base.
 *
 * @param[in]   clock   ztimer clock to operate on
 * @param[in]   base    base interval to use
 * @return  (time from ztimer_set() until callback) - base
 */
int32_t ztimer_overhead_set(ztimer_clock_t *clock, uint32_t base);

/**
 * @brief   Measure overhead for ztimer_sleep()
 *
 * This function can be used to measure the overhead incurred by ztimer_sleep().
 *
 * @param[in]   clock   ztimer clock to operate on
 * @param[in]   base    base interval to use
 * @return  (time(ztimer_sleep(base))) - base
 */
int32_t ztimer_overhead_sleep(ztimer_clock_t *clock, uint32_t base);

/** @} */
