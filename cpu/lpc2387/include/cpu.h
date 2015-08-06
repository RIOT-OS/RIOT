/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __CPU_H
#define __CPU_H

/**
 * @defgroup    lpc2387     NXP LPC2387
 * @ingroup     cpu
 * @brief       NXP LPC2387 specific code
 * @{
 */

#include <stdbool.h>
#include "lpc2387.h"
#include "arm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uintptr_t __stack_start;     ///< end of user stack memory space

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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __CPU_H */
