/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_mips32r2_commom MIPS32R2 Common
 * @ingroup     cpu
 * @brief       Common implementations and headers for mips32r2 compliant devices
 * @{
 *
 * @file
 * @brief       Common implementations and headers for mips32r2 compliant devices
 *
 * @author      Imagination Technologies
 */

#ifndef CPU_H_
#define CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <inttypes.h>

#include "irq.h"


/**
 * @brief   Early CPU initialisation
 *
 * @todo:   Not supported for generic mips32r2 implementations the bootloader
 * must have initialised the system
 */
static inline void cpu_init_early(void)
{
    /* This function must exist else RIOT won't compile */
}

/**
 * @brief   Print the last instruction's address
 *
 * @todo:   Not supported
 */
static inline void cpu_print_last_instruction(void)
{
    /* This function must exist else RIOT won't compile */
}

#ifdef __cplusplus
}
#endif

#endif
/** @} */

