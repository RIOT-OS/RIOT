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
 * @brief       Support for the RE-Mote boards
 * @{
 *
 * @file
 * @brief       Board specific definitions for the RE-Mote boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef BOARD_COMMON__H
#define BOARD_COMMON__H

#include "cpu.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Macros for controlling the on-board RGB LEDs
 * @{
 */
#define LED_ALL_OFF        LED0_OFF;   \
                           LED1_OFF;   \
                           LED2_OFF
/* Output is color white */
#define LED_ALL_ON         LED0_ON;    \
                           LED1_ON;    \
                           LED2_ON

/* Yellow */
#define LED3_ON            LED2_OFF;    \
                           LED0_ON;     \
                           LED1_ON
#define LED3_OFF           LED1_OFF;    \
                           LED0_OFF
#define LED3_TOGGLE        LED1_TOGGLE; \
                           LED0_TOGGLE

/* Purple */
#define LED4_ON            LED1_OFF;     \
                           LED2_ON;      \
                           LED0_ON
#define LED4_OFF           LED2_OFF;     \
                           LED0_OFF
#define LED4_TOGGLE        LED2_TOGGLE;  \
                           LED0_TOGGLE
/** @} */

/**
 * @name Flash Customer Configuration Area (CCA) parameters
 * @{
 */
#ifndef UPDATE_CCA
#define UPDATE_CCA                (1)
#endif
#define CCA_BACKDOOR_ENABLE       (1)
#define CCA_BACKDOOR_PORT_A_PIN   (3) /**< Select button */
#define CCA_BACKDOOR_ACTIVE_LEVEL (0) /**< Active low */
/** @} */

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER_DEV          (0)
#define XTIMER_CHAN         (0)
#define XTIMER_WIDTH        (16)
#define XTIMER_SHIFT        (4)
#define XTIMER_HZ           (16000000UL)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif /* BOARD_COMMON__H */
/** @} */
