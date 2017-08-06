/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

/* this should be randomized for each build */
#define STACK_CHK_GUARD 0x595e9fbd94fda766

uintptr_t __stack_chk_guard = (uintptr_t) STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void)
{
    core_panic(PANIC_SSP, "ssp: stack smashing detected");
}
