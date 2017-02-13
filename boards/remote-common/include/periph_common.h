/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_remote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Re-Mote boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef PERIPH_COMMON_H
#define PERIPH_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000U) /* 32MHz */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = GPTIMER0,
        .channels = 2,
        .cfg      = GPTMCFG_16_BIT_TIMER, /* required for XTIMER */
    },
    {
        .dev      = GPTIMER1,
        .channels = 1,
        .cfg      = GPTMCFG_32_BIT_TIMER,
    },
    {
        .dev      = GPTIMER2,
        .channels = 1,
        .cfg      = GPTMCFG_32_BIT_TIMER,
    },
    {
        .dev      = GPTIMER3,
        .channels = 1,
        .cfg      = GPTMCFG_32_BIT_TIMER,
    },
};

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))

#define TIMER_IRQ_PRIO      1
/** @} */

/**
 * @name Radio peripheral configuration
 * @{
 */
#define RADIO_IRQ_PRIO      1
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_COMMON_H */
/** @} */
