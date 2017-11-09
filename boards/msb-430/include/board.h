/*
 * Copyright 2009, 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msb430 MSB-430(h) boards
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-430(h) board
 *
 * @{
 *
 * @file
 * @brief       Central definitions for the ScatterWeb MSB-430(h) board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the CPU model for the <msp430.h>
 */
#ifndef __MSP430F1612__
#define __MSP430F1612__
#endif

/**
 * @name    CPU core configuration
 * @{
 */
/** @todo Move this to the periph_conf.h */
#ifdef BOARDREV_H
#define MSP430_INITIAL_CPU_SPEED    7372800uL
#define MSP430_HAS_EXTERNAL_CRYSTAL 1
#else
#define MSP430_INITIAL_CPU_SPEED    2457600uL
#define MSP430_HAS_EXTERNAL_CRYSTAL 0
#endif
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
