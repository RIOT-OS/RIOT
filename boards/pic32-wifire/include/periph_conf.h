/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 */
#ifndef _PERIPH_CONF_H_
#define _PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Note this file must exist for xtimer code to build */
#define TIMER_NUMOF         (1U)
#define TIMER_0_CHANNELS    3

/* This value must exist even if no uart in use */
#define UART_NUMOF          (6U)

/* Route debug output to UART 4 (the USB one via FTDI chip) */
#define DEBUG_VIA_UART 4

#ifdef __cplusplus
}
#endif

#endif
