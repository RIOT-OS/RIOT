/*
 * Copyright 2009, 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msb430h MSB-430H
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-430H board
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-430H board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MSB_BOARD_H_
#define MSB_BOARD_H_

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* for correct inclusion of <msp430.h> */
#ifndef __MSP430F1612__
#define __MSP430F1612__
#endif

//MSB430 core
#define MSP430_INITIAL_CPU_SPEED    7372800uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MSB_BOARD_H_ */
