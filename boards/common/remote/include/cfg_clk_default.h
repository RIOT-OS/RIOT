/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       Common default clock configuration for the RE-Mote board revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef CFG_CLK_DEFAULT_H
#define CFG_CLK_DEFAULT_H

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
#define CLOCK_CORECLOCK     (CLOCK_OSC)
/* I/O clock rate setting 16MHz */
#define CLOCK_IO            (CLOCK_OSC / 2)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CFG_CLK_DEFAULT_H */
/** @} */
