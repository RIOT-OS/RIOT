/*
 * Copyright (C) 2014 INRIA
 *               2014, 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430-common
 * @{
 *
 * @file
 * @brief       Hardware timer configuration for MSP430 based CPUs
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#include "board.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS       (TIMER_CHAN)
#define HWTIMER_SPEED           (F_RC_OSCILLATOR)
#define HWTIMER_MAXTICKS        (0x0000FFFF)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HWTIMER_CPU_H_ */
