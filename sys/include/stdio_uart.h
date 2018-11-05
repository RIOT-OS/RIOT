/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio_uart STDIO over UART
 * @ingroup     sys
 *
 * @brief       Standard input/output backend using UART
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef STDIO_UART_H
#define STDIO_UART_H

/* Boards may override the default STDIO UART device */
#include "board.h"
#include "stdio_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STDIO_UART_DEV
/**
 * @brief UART device to use for STDIO
 */
#define STDIO_UART_DEV          UART_DEV(0)
#endif

#ifndef STDIO_UART_BAUDRATE
/**
 * @brief Baudrate for STDIO
 */
#define STDIO_UART_BAUDRATE     (115200)
#endif

#ifndef STDIO_UART_RX_BUFSIZE
/**
 * @brief Buffer size for STDIO
 */
#define STDIO_UART_RX_BUFSIZE   (64)
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_UART_H */
