/*
 * Copyright (C) 2013 - 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu
 * @defgroup    cpu_native  Native
 * @brief       Native CPU specific code
 * @details     The native CPU uses system calls to simulate hardware access.
 * @ingroup     cpu
 * @brief       CPU abstraction for the native port
 * @{
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The CPU supports unaligned memory access.
 *          Even if the underlying architecture does not support it, the kernel will take care of it.
 */
#define CPU_HAS_UNALIGNED_ACCESS

/**
 * @brief   Prints the address the callee will return to
 */
__attribute__((always_inline)) static inline void cpu_print_last_instruction(void)
{
    /* __builtin_return_address will return the address the calling function
     * will return to - since cpu_print_last_instruction is forced inline,
     * it is the return address of the user of this function */
    printf("%p\n", __builtin_return_address(0));
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
