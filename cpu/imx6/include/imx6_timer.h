/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation for the i.MX6 CPU
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

#ifndef __IMX6_TIMER_H
#define __IMX6_TIMER_H

#include "registers/regsepit.h"
#include "registers/regsgpt.h"

//! @brief Available clock sources for the timers.
enum _timer_clock_sources
{
    CLKSRC_OFF = 0,     //!< clock source is OFF
    CLKSRC_IPG_CLK = 1, //!< clock source is peripheral clock
    CLKSRC_PER_CLK = 2, //!< clock source is high-freq reference clock
    CLKSRC_CLKIN = 3,   //!< clock source is external from a CLKIN input
    CLKSRC_CKIL = 3     //!< clock source is low-freq reference clock
};

//! @brief Options for low power mode support for the timers.
//!
//! These constants are bit masks that are or'd together to select in which low
//! power modes the timer will continue counting.
enum _timer_low_power_modes
{
    WAIT_MODE_EN = 1,   //!< Timer is enabled in wait mode.
    STOP_MODE_EN = 2    //!< Timer is enabled in stop mode.
};

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (3U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1

#define TIMER_CLKSRC        CLKSRC_IPG_CLK
#define TIMER_PRESCALE      (get_main_clock(IPG_CLK) / 1000000) // TODO is this 12 bits long?

/* Timer 0 configuration */
#define TIMER_0_ISEPIT      0
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffffffff)

/* Timer 1 configuration */
#define TIMER_1_EPITDEV     HW_EPIT1
#define TIMER_1_ISEPIT      1
#define TIMER_1_CHANNELS    1
#define TIMER_1_MAX_VALUE   (0xffffffff)

/* Timer 2 configuration */
#define TIMER_2_EPITDEV     HW_EPIT2
#define TIMER_2_ISEPIT      1
#define TIMER_2_CHANNELS    1
#define TIMER_2_MAX_VALUE   (0xffffffff)

#define TIMER_0_OFFSET      0
#define TIMER_1_OFFSET      (TIMER_0_EN * TIMER_0_CHANNELS)
#define TIMER_2_OFFSET      (TIMER_1_OFFSET + (TIMER_1_EN * TIMER_1_CHANNELS))
/** @} */


#endif
