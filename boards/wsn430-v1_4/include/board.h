/*
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wsn430-v1_4 WSN430 v1.4
 * @ingroup     boards
 * @brief       Support for the Senslab WSN430 v1.4 board
 *
 * <h2>Compontents</h2>
 * \li MSP430
 * \li CC2420
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the Senslab WSN430 v1.4 board
 *
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef WSN_BOARD_H
#define WSN_BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the CPU model for the <msp430.h>
 */
#ifndef __MSP430F1611__
#define __MSP430F1611__
#endif

/**
 * @brief   CPU core configuration
 *
 * @todo    Move this to the periph_conf.h
 * @{
 */
#define MSP430_INITIAL_CPU_SPEED    800000uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1
/** @} */

#ifdef __cplusplus
}
#endif

#include <msp430x16x.h>

/** @} */
#endif /*  WSN_BOARD_H */
