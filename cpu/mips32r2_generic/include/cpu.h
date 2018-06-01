/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_mips32r2_generic Imagination Technologies MIPS32R2 generic
 * @ingroup     cpu
 * @brief       Imagination Technologies MIPS32R2 generic
 * @{
 *
 * @file
 * @brief       Common implementations and headers for Imagination Technologies MIPS32R2 compliant devices
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */

#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <inttypes.h>

#include "irq.h"

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

#endif /* CPU_H */
/** @} */
