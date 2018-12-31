/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (24)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   Initialize the platform
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
