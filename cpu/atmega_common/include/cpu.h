/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @brief       Common implementations and headers for ATmega family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the ATmega common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 */

#ifndef __ATMEGA_COMMON_H
#define __ATMEGA_COMMON_H

#include "cpu_conf.h"
#include <avr/interrupt.h>

/**
 * For downwards compatibility with old RIOT code.
 * TODO: remove once core was adjusted
 */
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define eINT            enableIRQ
#define dINT            disableIRQ

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __ATMEGA_COMMON_H */
/** @} */
