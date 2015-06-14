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
 * @file
 * @brief       Interface definitions for the UART0 abstraction
 */

#ifndef __BOARD_UART0_H
#define __BOARD_UART0_H

#include "kernel_types.h"
#include "cpu.h"        /* To give user access to UART0_BUFSIZE */
#include "cpu_conf.h"   /* Some CPUs define this in cpu_conf.h... */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Process identifier for the UART0 module.
 */
extern kernel_pid_t uart0_handler_pid;

/**
 * @brief Initialize and starts the UART0 handler.
 */
void board_uart0_init(void);

/**
 * @brief To be called from the uart interrupt handler for every incoming
 *        character.
 *
 * @param[in] c The received character.
 */
void uart0_handle_incoming(int c);

/**
 * @brief Notify the chardev thread that new characters are available in the
 *        ringbuffer.
 */
void uart0_notify_thread(void);

/**
 * @brief Reads one character from the ringbuffer.
 *
 * @returns The first available character from the ringbuffer.
 */
int uart0_readc(void);

/**
 * @brief Wrapper to putchar.
 *
 * @param[in] c The character to put on the UART.
 */
int uart0_putc(int c);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __BOARD_UART0_H */
