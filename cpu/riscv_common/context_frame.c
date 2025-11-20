/*
 * SPDX-FileCopyrightText: 2017, 2019 JP Bonn, Ken Rabold
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_riscv_common
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       compile-time check of context_switch_frame offsets
 *
 * @author      JP Bonn
 * @}
 */

#include <assert.h>

#include "context_frame.h"
#include "thread.h"

#define CHECK_OFFSET(member)     \
    _Static_assert(offsetof(struct context_switch_frame, \
                            member) == member ## _OFFSET, \
                   "context_switch_frame offset mismatch for offset member");

static void check_context_switch_frame_alignment(void) __attribute__ ((unused));

/**
 * @brief   Check size and offsets of context_switch_frame
 *
 * This does nothing at runtime.  It is optimized out since it's only
 * doing compile-time checks.
 */
static void check_context_switch_frame_alignment(void)
{
    _Static_assert(sizeof(struct context_switch_frame) % 16 == 0,
                   "Stack pointer should be 16 byte aligned");
    _Static_assert(sizeof(struct context_switch_frame) == CONTEXT_FRAME_SIZE,
                   "context_switch_frame size mismatch");
    CHECK_OFFSET(s0);
    CHECK_OFFSET(s1);
    CHECK_OFFSET(s2);
    CHECK_OFFSET(s3);
    CHECK_OFFSET(s4);
    CHECK_OFFSET(s5);
    CHECK_OFFSET(s6);
    CHECK_OFFSET(s7);
    CHECK_OFFSET(s8);
    CHECK_OFFSET(s9);
    CHECK_OFFSET(s10);
    CHECK_OFFSET(s11);
    CHECK_OFFSET(ra);
    CHECK_OFFSET(t0);
    CHECK_OFFSET(t1);
    CHECK_OFFSET(t2);
    CHECK_OFFSET(t3);
    CHECK_OFFSET(t4);
    CHECK_OFFSET(t5);
    CHECK_OFFSET(t6);
    CHECK_OFFSET(a0);
    CHECK_OFFSET(a1);
    CHECK_OFFSET(a2);
    CHECK_OFFSET(a3);
    CHECK_OFFSET(a4);
    CHECK_OFFSET(a5);
    CHECK_OFFSET(a6);
    CHECK_OFFSET(a7);
    CHECK_OFFSET(pc);
    CHECK_OFFSET(pad);

    /*
     * also check the SP offset in the _frame structure
     */
    _Static_assert( offsetof(struct _thread, sp) == SP_OFFSET_IN_THREAD,
                    "Offset of SP in _thread mismatch");

}
