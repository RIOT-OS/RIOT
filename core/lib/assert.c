/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "irq.h"
#include "panic.h"
#if IS_USED(MODULE_BACKTRACE)
#include "backtrace.h"
#endif

__NORETURN static inline void _assert_common(void)
{
#if IS_USED(MODULE_BACKTRACE)
    printf("FAILED ASSERTION. Backtrace:\n");
    backtrace_print();
#endif
#ifdef DEBUG_ASSERT_BREAKPOINT
    DEBUG_BREAKPOINT(1);
#endif
    if (DEBUG_ASSERT_NO_PANIC && !irq_is_in() && irq_is_enabled()) {
        puts("FAILED ASSERTION.");
        while (1) {
            thread_sleep();
        }
    }
    core_panic(PANIC_ASSERT_FAIL, "FAILED ASSERTION.");
}

__NORETURN void _assert_failure(const char *file, unsigned line)
{
    printf("%s:%u => ", file, line);
    _assert_common();
}

__NORETURN void _assert_panic(void)
{
    printf("0x%" PRIxTXTPTR " => ", cpu_get_caller_pc());
    _assert_common();
}

/** @} */
