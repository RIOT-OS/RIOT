/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_yunjia-nrf51822 Yunjia NRF51822
 * @ingroup     boards
 * @brief       Board specific files for the Yunjia NRF51822 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Yunjia NRF51822 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (24)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
