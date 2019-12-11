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

#include "thread.h"

#include "vendor/platform.h"
#include "vendor/plic_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Initialization of the clock
 */
void clock_init(void);

/**
 * @brief   Get and eventually compute the current CPU core clock frequency
 *
 * @return  the cpu core clock frequency in Hz
 */
uint32_t cpu_freq(void);

/**
 * @brief   Initialization of interrupts
 */
void irq_init(void);

/**
 * @brief   External ISR callback
 */
typedef void (*external_isr_ptr_t)(int intNum);

/**
 * @brief   Set External ISR callback
 */
void set_external_isr_cb(int intNum, external_isr_ptr_t cbFunc);

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
