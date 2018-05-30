/*
 * Copyright (C) 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_fe310 SiFive FE310
 * @ingroup     cpu
 * @brief       Common implementations and headers for RISC-V
 * @{
 *
 * @file
 * @brief       Basic definitions for the RISC-V common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs
 * registers, just include this file. It will then make sure that the MCU
 * specific headers are included.
 *
 * @author      Ken Rabold
 */

#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

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
 * @brief   Initialization of the Newlib-nano stub
 */
void nanostubs_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
