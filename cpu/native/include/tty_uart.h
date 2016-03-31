/*
 * Copyright (C) 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     native_cpu
 * @{
 *
 * @file
 * @brief       UART implementation based on /dev/tty devices on host
 *
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

#ifndef TTY_UART_H
#define TTY_UART_H

#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   register /dev/tty device to be used for UART
 *
 * @param[in] uart  UART id
 * @param[in] name  path name for /dev/tty device
 */
void tty_uart_setup(uart_t uart, const char *name);

/**
 * @brief   closes files opened
 */
void uart_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
