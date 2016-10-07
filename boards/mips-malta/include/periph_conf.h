/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef _PERIPH_CONF_H_
#define _PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Note this file must exist for xtimer code to build */
#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)

/* This value must exist even if no uart in use */
#define UART_NUMOF          (0)

#define MIPS_DSP            (1)
#ifdef __cplusplus
}
#endif

#endif /*_PERIPH_CONF_H_*/
