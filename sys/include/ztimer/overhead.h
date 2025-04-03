/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef ZTIMER_OVERHEAD_H
#define ZTIMER_OVERHEAD_H

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
#endif /* ZTIMER_OVERHEAD_H */
