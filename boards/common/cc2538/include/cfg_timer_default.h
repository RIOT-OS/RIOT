/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_cc2538
 * @{
 *
 * @file
 * @brief       Common default timer configuration for cc2538 boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer configuration
 *
 * General purpose timers (GPT[0-3]) are configured consecutively and in order
 * (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
 *
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .chn = 2,
        .cfg = GPTMCFG_16_BIT_TIMER, /* required for XTIMER */
    },
    {
        .chn = 2,
        .cfg = GPTMCFG_16_BIT_TIMER,
    },
    {
        .chn = 1,
        .cfg = GPTMCFG_32_BIT_TIMER,
    },
    {
        .chn = 1,
        .cfg = GPTMCFG_32_BIT_TIMER,
    },
};

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)

#define TIMER_IRQ_PRIO      1
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
