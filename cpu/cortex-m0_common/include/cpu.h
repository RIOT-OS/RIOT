/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_cortexm0_common ARM Cortex-M0 common
 * @ingroup     cpu
 * @brief       Common implementations and headers for Cortex-M0 family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the Cortex-M0 common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CPU_H
#define __CPU_H

#include "cpu_conf.h"


/**
 * For downwards compatibility with old RIOT code.
 * TODO: remove once core was adjusted
 */
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Deprecated interrupt control function for backward compatibility
 * @{
 */
#define eINT            enableIRQ
#define dINT            disableIRQ
/** @} */

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __CPU_H */
/** @} */
