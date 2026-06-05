/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       Self-contained garbage-collector
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <setjmp.h>
#include <stdint.h>

#include "py/mpstate.h"
#include "py/gc.h"

#if MICROPY_ENABLE_GC

void gc_collect(void)
{
    gc_collect_start();

    // Spill the callee-saved registers onto the stack so that they are scanned
    // as part of the stack below, then scan from the current stack pointer up
    // to the top of the stack recorded at startup.
    jmp_buf regs;
    setjmp(regs);

    void **sp = (void **)&regs;
    gc_collect_root(sp,
                    ((uintptr_t)MP_STATE_THREAD(stack_top) - (uintptr_t)sp) / sizeof(uintptr_t));

    gc_collect_end();
}

#endif // MICROPY_ENABLE_GC
