/*
 * SPDX-FileCopyrightText: 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  UART implementation based on /dev/tty devices on host
 * @author Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

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

#ifdef __cplusplus
}
#endif

/** @} */
