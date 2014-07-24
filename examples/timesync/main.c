/*
 * Copyright (C) 2014 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       GTSP timesync test application
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"

#ifdef MODULE_LTC4150
#include "ltc4150.h"
#endif

#ifdef MODULE_TRANSCEIVER
#include "transceiver.h"
#endif

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_DEFAULT)

static int shell_readc(void) {
	char c = 0;
	(void) posix_read(uart0_handler_pid, &c, 1);
	return c;
}

static void shell_putchar(int c) {
	(void) putchar(c);
}

int main(void) {
	shell_t shell;
	(void) posix_open(uart0_handler_pid, 0);

#ifdef MODULE_LTC4150
	ltc4150_start();
#endif
	(void) puts("Welcome to RIOT!");
	shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);
	shell_run(&shell);
	return 0;
}
