/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup        boards_cc3200-launchxl
 * @{
 *
 * @file
 * @brief           CPU specific definitions and functions for peripheral
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer configuration
 * @{
 */
#define XTIMER_BACKOFF 9
#define XTIMER_OVERHEAD 5
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define LED_RED GPIO_PIN(PORT_A1, 64)
#define LED_ORANGE GPIO_PIN(PORT_A1, 1)
#define LED_GREEN GPIO_PIN(PORT_A1, 2)
/** @} */

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
