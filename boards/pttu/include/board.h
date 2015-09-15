/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_pttu PTTU
 * @ingroup     boards
 * @brief       Support for the PTTU board
 * @{
 *
 * @file
 * @brief       Basic definitions for the PTTU board
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "lpc2387.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   VICIntEnClear Alias for compatibility
 */
#define VICIntEnClear VICIntEnClr

/**
 * @brief   Clock initialization part one
 */
void init_clks1(void);

/**
 * @brief   Clock initialization part two
 */
void init_clks2(void);

/**
 * @brief   Jump to clock initialization code
 */
void bl_init_clks(void);

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @brief Deprecated HW_TIMER definition (to be removed)
 */
#define HW_TIMER             TIMER_DEV(0)

/**
 * @name dummy-defines for LEDs
 * @{
 */
#define LED_GREEN_ON        /* not available */
#define LED_GREEN_OFF       /* not available */
#define LED_GREEN_TOGGLE    /* not available */
#define LED_RED_ON          /* not available */
#define LED_RED_OFF         /* not available */
#define LED_RED_TOGGLE      /* not available */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H_ */
