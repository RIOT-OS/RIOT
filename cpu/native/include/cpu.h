/*
 * SPDX-FileCopyrightText: 2013-2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup cpu_native  Native CPU
 * @ingroup  cpu
 * @brief    CPU implementation for running RIOT on a Linux and BSD
 * @author   Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * The native CPU uses system calls to simulate hardware access.
 *
 * @{
 */

/**
 * @file
 * @brief Native CPU header
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#include <stdint.h>
#include <stdio.h>
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Basics */
/**
 * @name
 * @{
 */
/**
 * @brief   The CPU supports unaligned memory access.
 *          Even if the underlying architecture does not support it, the kernel will take care of it.
 */
#define CPU_HAS_UNALIGNED_ACCESS

/**
 * @brief   Gets the address the callee will return to
 */
__attribute__((always_inline)) static inline uintptr_t cpu_get_caller_pc(void)
{
    /* __builtin_return_address will return the address the calling function
     * will return to - since cpu_get_caller_pc is forced inline,
     * it is the return address of the user of this function */
    return (uintptr_t)__builtin_return_address(0);
}
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
