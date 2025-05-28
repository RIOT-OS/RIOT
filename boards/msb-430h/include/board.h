/*
 * Copyright 2009, 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_msb430h
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-430H board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
