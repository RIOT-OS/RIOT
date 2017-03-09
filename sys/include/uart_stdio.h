/*
 * Copyright (C) 2017 Anthony Merlino <anthony@vergeaero.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_uart_stdio UART stdio
 * @ingroup     sys_stdio
 *
 * @brief       stdio over UART defines
 *
 * @{
 * @file
 *
 * @author      Anthony Merlino  <anthony@vergeaero.com>
 */

#ifndef UART_STDIO_H
#define UART_STDIO_H

/* Boards may override the default STDIO UART device */
#include <stdint.h>
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UART_STDIO_DEV
/**
 * @brief UART device to use for STDIO
 */
#define UART_STDIO_DEV           UART_DEV(0)
#endif

#ifndef UART_STDIO_BAUDRATE
/**
 * @brief Baudrate for STDIO
 */
#define UART_STDIO_BAUDRATE      (115200)
#endif

#ifndef UART_STDIO_RX_BUFSIZE
/**
 * @brief Buffer size for STDIO
 */
#define UART_STDIO_RX_BUFSIZE    (64)
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* UART_STDIO_H */
