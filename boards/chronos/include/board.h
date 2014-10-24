/*
 * Copyright (C) 2013 Freie Universität Berlin
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

#ifndef _MSB_BOARD_H
#define _MSB_BOARD_H

#include <stdint.h>

// for correct inclusion of <msp430.h>
#ifndef __CC430F6137__
#define __CC430F6137__
#endif

#define MSP430_INITIAL_CPU_SPEED    7372800uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

typedef uint8_t radio_packet_length_t;

#endif // _MSB_BOARD_H
