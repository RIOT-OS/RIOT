/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nrf51
 * @{
 *
 * @file
 * @brief       Shared timer peripheral configuration mapping timers 0 and 1
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_24Bit,
        .irqn     = TIMER0_IRQn,
    },
    {
        .dev      = NRF_TIMER1,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_16Bit,
        .irqn     = TIMER1_IRQn,
    }
};

#define TIMER_0_ISR         isr_timer0
#define TIMER_1_ISR         isr_timer1

/** See @ref timer_init */
#define TIMER_0_MAX_VALUE 0xffffffff
/** See @ref timer_init */
#define TIMER_1_MAX_VALUE 0xffffffff

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
