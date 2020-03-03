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
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000U) /* 32MHz */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CFG_CLK_DEFAULT_H */
/** @} */
