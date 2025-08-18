/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_nrf51 NRF51 common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for some nRF51 boards.
 * @{
 *
 * @file
 * @brief       Shared configuration for some nRF51-based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 *
 * Some external packages (e.g. NimBLE) require dedicated access to timer 0.
 * Setting the BOARD_NRF51_XTIMER_ALT variable allows to change the default
 * xtimer configuration to use
 *
 * @{
 */
#ifndef BOARD_NRF51_XTIMER_ALT
/* default xtimer mapping */
#define XTIMER_WIDTH                (24)
#else
/* alternative mapping to TIMER_DEV(1) */
#define XTIMER_DEV                  TIMER_DEV(1)
#define XTIMER_WIDTH                (16)
#define XTIMER_CHAN                 (0)
#endif
#define XTIMER_BACKOFF              (40)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
