/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPU_H
#define CPU_H

/**
 * @defgroup    cpu_lpc23xx     NXP LPC23XX
 * @ingroup     cpu
 * @brief       NXP LPC23XX family specific code
 * @{
 */

#include <stdio.h>
#include <stdbool.h>

#include "lpc23xx.h"
#include "arm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uintptr_t __stack_start;     /**< end of user stack memory space */

/**
 * @brief Initialize the phase lock loop oscillator
 */
void cpu_init_pll(void);

/**
 * @brief Initialize the Memory Acceleration Module
 */
void cpu_init_mam(void);

/**
 * @brief Scale lpc23xx cpu speed
 */
void lpc23xx_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale);

/**
 * @brief Initialize lpc23xx cpu clocks
 */
void cpu_init_clks(void);

/**
 * @brief install lpc23xx irq
 */
bool install_irq(int IntNumber, void (*HandlerAddr)(void), int Priority);

#ifdef MODULE_PERIPH
void gpio_init_ports(void);
#endif

/**
 * @brief   Prints the current content of the link register (lr)
 */
static inline void cpu_print_last_instruction(void)
{
    register uint32_t *lr_ptr;
    __asm__ __volatile__("mov %0, lr" : "=r"(lr_ptr));
    printf("%p\n", (void*) lr_ptr);
}

/**
 * @brief   Returns true if the CPU woke from Deep Sleep
 */
static inline bool cpu_woke_from_backup(void) {
    return RSIR == 0;
}

/**
 * @brief   Returns true if the backup RAM has been initialized
 */
bool cpu_backup_ram_is_initialized(void);

/**
 * @brief   The CPU has RAM that is retained in the deepest sleep mode.
 */
#define CPU_HAS_BACKUP_RAM  (1)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
