/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio RIOT stdio
 * @ingroup     sys
 *
 * @brief       stdio init/read/write functions
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Anthony Merlino  <anthony@vergeaero.com>
 */
#ifndef RIOT_STDIO_H
#define RIOT_STDIO_H

#include <stdint.h>

#ifdef MODULE_UART_STDIO
#include "uart_stdio.h"
#endif

#ifdef MODULE_RTT_STDIO
#include "rtt_stdio.h"
#endif

#ifdef MODULE_ETHOS_STDIO
#include "ethos_stdio.h"
#endif

/**
 * @brief initialize the module
 */
void stdio_init(void);

/**
 * @brief read @p len bytes from stdio into @p buffer
 *
 * @param[out]  buffer  buffer to read into
 * @param[in]   len     nr of bytes to read
 *
 * @return nr of bytes read
 * @return <0 on error
 */
int stdio_read(char* buffer, int len);

/**
 * @brief write @p len bytes from @p buffer
 *
 * @param[in]   buffer  buffer to read from
 * @param[in]   len     nr of bytes to write
 *
 * @return nr of bytes written
 * @return <0 on error
 */
int stdio_write(const char* buffer, int len);

/** @} */
#endif /* RIOT_STDIO_H */
