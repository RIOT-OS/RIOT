/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>

#include "assert.h"
#include "architecture.h"
#include "cpu.h"
#include "debug.h"
#include "panic.h"
#if IS_USED(MODULE_BACKTRACE)
#include "backtrace.h"
#endif

__NORETURN void _assert_failure(const char *file, unsigned line)
{
    printf("%s:%u => ", file, line);
#if IS_USED(MODULE_BACKTRACE)
    printf("failed assertion. Backtrace:\n");
    backtrace_print();
#endif
#ifdef DEBUG_ASSERT_BREAKPOINT
    DEBUG_BREAKPOINT(1);
#endif
    core_panic(PANIC_ASSERT_FAIL, "FAILED ASSERTION.");
}

__NORETURN void _assert_panic(void)
{
    printf("%" PRIxTXTPTR "\n", cpu_get_caller_pc());
#if IS_USED(MODULE_BACKTRACE)
    backtrace_print();
#endif
#ifdef DEBUG_ASSERT_BREAKPOINT
    DEBUG_BREAKPOINT(1);
#endif
    core_panic(PANIC_ASSERT_FAIL, "FAILED ASSERTION.");
}

/** @} */
