/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  cpu_cortex-m3
 * @{
 *
 * @file        cpu.h
 * @brief       Basic definitions for the Cortex-M3 common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CORTEXM_COMMON_H
#define __CORTEXM_COMMON_H

#include "cpu-conf.h"

/**
 * For downwards compatibility with old RIOT code.
 * TODO: remove once core was adjusted
 */
#include "irq.h"
#define eINT            enableIRQ
#define dINT            disableIRQ

/**
 * @brief Macro has to be called in the beginning of each ISR
 */
#define ISR_ENTER()     asm("push {LR}")

/**
 * @brief Macro has to be called on each exit of an ISR
 */
#define ISR_EXIT()      asm("pop {r0} \n bx r0")

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);


#endif /* __CORTEXM_COMMON_H */
/** @} */
