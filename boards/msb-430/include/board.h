/*
 * SPDX-FileCopyrightText: 2009 Freie Universitaet Berlin (FUB)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
