/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_cortexm4_common ARM Cortex-M4 common
 * @ingroup     cpu
 * @brief       Common implementations and headers for Cortex-M4 family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the Cortex-M4 common module
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

#endif /* __CORTEXM_COMMON_H */
/** @} */
