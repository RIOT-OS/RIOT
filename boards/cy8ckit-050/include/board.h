/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_cy8ckit-050 Cypress PSoC 5LP development kit
 * @ingroup     boards
 * @brief       Support for the CY8CKIT-050 PSoC 5LP board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the cy8ckit-050 board
 *
 * @author      Murat Cakmak <mail@muratcakmak.net>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#include "bitarithm.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The nominal CPU core clock in this board
 */
#define F_CPU               (48000000)

/**
 * @brief Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @brief define radio packet length
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, include clocks, LEDs and stdio
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
/** @} */
