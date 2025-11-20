/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Watchdog CPU definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#include "timex.h"
#include "periph/cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
/** @brief  Lower bound time
 *
 *  Actual Lower Limit is ~100us so round up
 */
#define NWDT_TIME_LOWER_LIMIT          (1U)
/** Upper bound time */
#define NWDT_TIME_UPPER_LIMIT          (4U * US_PER_MS * 4096U * (1 << 6U) \
                                        / CLOCK_LSI)
#define WDT_HAS_STOP                   (0U)     /**<  Watchdog can't be stopped once enabled */
/** Whether Watchdog has an init function (only on STM32L4) */
#if defined(CPU_FAM_STM32L4)
#define WDT_HAS_INIT                   (1U)
#else
#define WDT_HAS_INIT                   (0U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
