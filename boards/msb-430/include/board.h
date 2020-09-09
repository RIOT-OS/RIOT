/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_msb430
 *
 * @details
 * See
 * https://www.mi.fu-berlin.de/inf/groups/ag-tech/projects/ScatterWeb/modules/mod_MSB-430.html
 * for circuit diagram etc.
 *
 * <h2>Components</h2>
 * \li MSP430
 * \li CC1020
 * \li SHT11
 * \li MMA7260Q
 * \li LED
 *
 * @{
 *
 * @file
 * @brief       Central definitions for the ScatterWeb MSB-430 board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

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
#define MSP430_INITIAL_CPU_SPEED    2457600uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             1
#define MSP430_HAS_EXTERNAL_CRYSTAL 0
/** @} */

/**
 * @name   Configure on-board SHT11 device
 * @{
 */
#define SHT1X_PARAM_CLK             (GPIO_PIN(3, 5))
#define SHT1X_PARAM_DATA            (GPIO_PIN(3, 4))
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
