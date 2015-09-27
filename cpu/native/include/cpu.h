/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native
 * @brief       Native CPU specific code
 * @details     The native CPU uses system calls to simulate hardware access.
 * @ingroup     cpu
 * @brief       CPU abstraction for the native port
 * @{
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef _CPU_H
#define _CPU_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: remove once these have been removed from RIOT: */
void dINT(void);
void eINT(void);

/**
 * @brief   Prints the last instruction's address
 */
static inline void cpu_print_last_instruction(void)
{
    void *p;
    __asm__("1: mov 1b, %0" : "=r" (p));
    printf("%p\n", p);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif //_CPU_H
