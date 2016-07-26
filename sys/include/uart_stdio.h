/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_uart_stdio UART stdio
 * @ingroup     sys
 *
 * @brief       stdio init/read/write functions for UARTs
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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

/**
 * @brief initialize the module
 */
void uart_stdio_init(void);

/**
 * @brief read @p len bytes from stdio uart into @p buffer
 *
 * @param[out]  buffer  buffer to read into
 * @param[in]   len     nr of bytes to read
 *
 * @return nr of bytes read
 * @return <0 on error
 */
int uart_stdio_read(char* buffer, int len);

/**
 * @brief write @p len bytes from @p buffer into uart
 *
 * @param[in]   buffer  buffer to read from
 * @param[in]   len     nr of bytes to write
 *
 * @return nr of bytes written
 * @return <0 on error
 */
int uart_stdio_write(const char* buffer, int len);

/**
 * @brief internal callback for periph/uart drivers
 *
 * @param[in]   arg     (unused)
 * @param[in]   data    character that has been received
 */
void uart_stdio_rx_cb(void *arg, uint8_t data);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* UART_STDIO_H */
