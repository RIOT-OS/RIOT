/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_lpc11u34 NXP LPC11U34
 * @ingroup         cpu
 * @brief           CPU specific implementations for the NXP LPC11U34 cpu
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Paul RATHGEB <paul.rathgeb@skynet.be>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "LPC11Uxx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (35U)
#define CPU_FLASH_BASE                  LPC_FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
