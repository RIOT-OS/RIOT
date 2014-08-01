
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

#include "kernel_types.h"
#include "cpu-conf.h"   /* To give user access to UART0_BUFSIZE */

extern kernel_pid_t uart0_handler_pid;

void board_uart0_init(void);
void uart0_handle_incoming(int c);
void uart0_notify_thread(void);

int uart0_readc(void);
void uart0_putc(int c);

/** @} */
#endif /* __BOARD_UART0_H */
