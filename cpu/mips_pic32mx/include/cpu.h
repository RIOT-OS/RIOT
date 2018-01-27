/*
 * Copyright (C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.#
 *
 */

/**
 * @defgroup    cpu_mips_pic32mx Microchip PIC32MX
 * @ingroup     cpu
 * @brief       CPU definitions for Microchip PIC32MX devices.
 * @{
 *
 * @file
 * @brief       CPU definitions for pic32mx devices.
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
#include <assert.h>
#include "irq.h"

/**
 * @brief We run from flash on PIC32
 */
#define FLASH_XIP (1)

/**
 * @brief   Print the last instruction's address
 *
 * @todo:   Not supported
 */
static inline void cpu_print_last_instruction(void)
{
    /* This function must exist else RIOT won't compile */
}

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
