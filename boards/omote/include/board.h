/*
 * Copyright (C) 2020 Oppila Microsystems - http://www.oppila.in
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_omote
 * @{
 *
 * @file
 * @brief       Board specific definitions for the omote board
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "cc2538_eui_primary.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED0_PIN        GPIO_PIN(3, 0)
#define LED1_PIN        GPIO_PIN(3, 4)

#define LED0_MASK       (1 << 0)
#define LED1_MASK       (1 << 4)

#define LED0_ON         (GPIO_D->DATA |=  LED0_MASK)
#define LED0_OFF        (GPIO_D->DATA &= ~LED0_MASK)
#define LED0_TOGGLE     (GPIO_D->DATA ^=  LED0_MASK)

#define LED1_ON         (GPIO_B->DATA |=  LED1_MASK)
#define LED1_OFF        (GPIO_B->DATA &= ~LED1_MASK)
#define LED1_TOGGLE     (GPIO_B->DATA ^=  LED1_MASK)

#define LED_ALL_OFF        LED0_OFF;   \
                           LED1_OFF;
#define LED_ALL_ON         LED0_ON;    \
                           LED1_ON;
/** @} */

/**
 * @name User button pin definition
 * @{
 */
#define BTN0_PIN        GPIO_PIN(1, 0)
#define BTN0_MODE       GPIO_IN_PU
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

#define BOOT_PIN    GPIO_PIN(0, CCA_BACKDOOR_PORT_A_PIN) /**< BSL_BOOT Pin */
/** @} */

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @name USB configuration
 * @{
 */
#define INTERNAL_PERIPHERAL_VID         (0x0451)
#define INTERNAL_PERIPHERAL_PID         (0x16C8)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
#endif /* BOARD_H */
