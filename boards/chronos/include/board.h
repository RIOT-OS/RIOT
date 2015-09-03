/*
 * Copyright (C) 2013,2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_chronos Chronos
 * @ingroup     boards
 * @brief       Support for the chronos board
 * @{
 *
 * @file
 * @brief       Header for the chronos board
 *
 * @author      unknwon
 */

#ifndef CHRONOS_BOARD_H_
#define CHRONOS_BOARD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// for correct inclusion of <msp430.h>
#ifndef __CC430F6137__
#define __CC430F6137__
#endif

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER                      (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_MASK                 (0xffff0000)
/** @} */

/**
 * @brief   Standard input/output device configuration
 *
 * This defines are for compatibility with the CPU implementation but they are
 * not used for this board (as it has no UART interface accessible...)
 * @{
 */
#define STDIO                       (0)
#define STDIO_BAUDRATE              (115200U)
#define STDIO_RX_BUFSIZE            (64U)
/** @} */

#define MSP430_INITIAL_CPU_SPEED    7372800uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

#ifdef __cplusplus
}
#endif

#endif /* _CHRONOS_BOARD_H_ */
/** @} */
