/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef _PERIPH_CONF_H_
#define _PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)

/* This value must exist even if no uart in use */
#define UART_NUMOF          (4)

/* Route debug output to UART 4 (the USB one via FTDI chip) */
#define DEBUG_VIA_UART      (3)
#define DEBUG_UART_BAUD     (9600)

#ifdef __cplusplus
}
#endif

#endif
