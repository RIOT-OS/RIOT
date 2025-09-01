/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_cc2538
 * @{
 *
 * @file
 * @brief       Default clock configuration for cc2538 based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration
 * @{
 */

/*
 * 0: use internal 32KHz RCOSC
 * 1: use external 32KHz XOSC
 */
#ifndef SYS_CTRL_OSC32K_USE_XTAL
#define SYS_CTRL_OSC32K_USE_XTAL        (1)
#endif
/*
 * 0: use internal 16MHz RCOSC
 * 1: use external 32MHz XOSC, required for RF operation
 */
#ifndef SYS_CTRL_OSC_USE_XTAL
#define SYS_CTRL_OSC_USE_XTAL           (1)
#endif

#if SYS_CTRL_OSC_USE_XTAL
#define CLOCK_OSC           (XOSC32M_FREQ)
#else
#define CLOCK_OSC           (RCOSC16M_FREQ)
#endif

#if SYS_CTRL_OSC32K_USE_XTAL
#define CLOCK_OSC32K        (XOSC32K_FREQ)    /* XCOSC frequency */
#else
#define CLOCK_OSC32K        (RCOSC32K_FREQ)    /* XCOSC frequency */
#endif

/* System clock frequency 32MHz */
#ifndef CLOCK_CORECLOCK
#define CLOCK_CORECLOCK     (CLOCK_OSC)
#endif
/* I/O clock rate setting 16MHz */
#ifndef CLOCK_IO
#define CLOCK_IO            (CLOCK_OSC / 2)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
