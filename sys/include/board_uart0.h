/*
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_uart0 UART0
 * @ingroup     sys
 * @brief       UART0 interface abstraction
 * @{
 *
 * @file        board_uart0.h
 * @brief       Interface definitions for the UART0 abstraction
 */

#ifndef __BOARD_UART0_H
#define __BOARD_UART0_H

#include <sys/types.h>

#include "cpu-conf.h"   /* To give user access to UART0_BUFSIZE */

size_t uart0_handle_incoming(const char *buf, size_t n);

ssize_t uart0_read(char *buf, size_t n);
int uart0_readc(void);

void uart0_putc(int c);

/** @} */
#endif /* __BOARD_UART0_H */
