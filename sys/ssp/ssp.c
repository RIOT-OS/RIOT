/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_ssp
 * @{
 *
 * @file
 * @brief       Stack Smashing Protector (SSP) helper functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>

#include "panic.h"

uintptr_t __stack_chk_guard = (uintptr_t) STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void)
{
    core_panic(PANIC_SSP, "ssp: stack smashing detected");
}
