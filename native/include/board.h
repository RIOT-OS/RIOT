/**
 * Native Board interface
 *
 * The native board uses call level hardware simulation.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @defgroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

void LED_GREEN_OFF(void);
void LED_GREEN_ON(void);
void LED_GREEN_TOGGLE(void);
void LED_RED_OFF(void);
void LED_RED_ON(void);
void LED_RED_TOGGLE(void);

#ifdef MODULE_UART0
#include <sys/select.h>
extern fd_set _native_uart_rfds;
extern void _native_handle_uart0_input(void);
#endif
