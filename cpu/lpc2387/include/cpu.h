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
 * @defgroup    cpu_lpc2387     NXP LPC2387
 * @ingroup     cpu
 * @brief       NXP LPC2387 specific code
 * @{
 */

#include <stdio.h>
#include <stdbool.h>

#include "lpc2387.h"
#include "arm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uintptr_t __stack_start;     /**< end of user stack memory space */

/**
 * @brief Scale lpc2387 cpu speed
 */
void lpc2387_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale);


/**
 * @brief install lpc2387 irq
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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
