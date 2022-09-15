/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_blxxxpill
 *
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the bluepill/blackpill board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Macros for controlling the on-board LED.
 * @{
 */
#ifndef LED0_PORT_NUM
#define LED0_PORT_NUM       PORT_C                                  /**< GPIO Port number the LED is connected to */
#endif
#ifndef LED0_PIN_NUM
#define LED0_PIN_NUM        (13)                                    /**< Pin number the LED is connected to */
#endif
#ifndef LED0_IS_INVERTED
#define LED0_IS_INVERTED    1
#endif
/** @} */

/**
 * @brief   Use the fist UART for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(0)

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (19)
/** @} */

/* The boards debug header only exports  SWD, so JTAG-only pins PA15, PB3(*),
 * and PB4 can be remapped as regular GPIOs instead. (Note: PB3 is also used as
 * SWO.  The user needs to take care to not enable SWO with the debugger while
 * at the same time PB3 is used as GPIO. But RIOT does not use SWO in any case,
 * so if a user adds this feature in her/his own code, she/he should be well
 * aware of this.)
 */
#define STM32F1_DISABLE_JTAG    /**< Disable JTAG to allow pins being used as GPIOs */
#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_COMMON_H */
/** @} */
