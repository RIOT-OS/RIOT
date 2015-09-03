/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_telosb
 * @{
 *
 * @file
 * @brief       TelosB peripheral configuration
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timer configuration
 * @{
 */
#define TIMER_DEV           (TIMER_A)
#define TIMER_CHAN          (3)
#define TIMER_ISR_CC0       (TIMERA0_VECTOR)
#define TIMER_ISR_CCX       (TIMERA1_VECTOR)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
