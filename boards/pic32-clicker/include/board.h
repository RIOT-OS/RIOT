/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Peripheral clock frequency need for UART baud rate calculation */
#define PERIPHERAL_CLOCK 120000000  /* Hz */

/* Tick per uS need by timer code */
#define TICKS_PER_US 60

/* PIC32 Interrupt Routing */
#define PIC32MX 1

#ifdef __cplusplus
}
#endif


#endif /* _BOARD_H_ */
