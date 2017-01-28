/*
 * Copyright (C) 2017 Anthony Merlino <anthony@vergeaero.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_ethos_stdio ETHOS stdio
 * @ingroup     sys_stdio
 *
 * @brief       stdio over ETHOS defines
 *
 * @{
 * @file
 *
 * @author      Anthony Merlino  <anthony@vergeaero.com>
 */

#ifndef ETHOS_STDIO_H
#define ETHOS_STDIO_H

/* Boards may override the default STDIO UART device */
#include <stdint.h>
#include "board.h"

/* ETHOS defaults to use UART defines if not overridden */
#include "uart_stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ETHOS_UART
/**
 * @brief UART device to use for STDIO
 */
#define ETHOS_UART          UART_STDIO_DEV
#endif

#ifndef ETHOS_BAUDRATE
/**
 * @brief Baudrate for STDIO
 */
#define ETHOS_BAUDRATE      UART_STDIO_BAUDRATE
#endif

#ifndef ETHOS_RX_BUFSIZE
/**
 * @brief Buffer size for STDIO
 */
#define ETHOS_RX_BUFSIZE    UART_STDIO_RX_BUFSIZE
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ETHOS_STDIO_H */
