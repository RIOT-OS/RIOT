/*
 * cpu.h - mc1322x specific definitions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    mc1322x Freescale MC1322x
 * @ingroup     cpu
 * @brief       Freescale MC1322x specific code
 * @{
 */

#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include "arm_cpu.h"
#include "mc1322x.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uintptr_t __stack_start;     ///< end of user stack memory space
bool install_irq(int int_number, void *handler_addr, int priority);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
