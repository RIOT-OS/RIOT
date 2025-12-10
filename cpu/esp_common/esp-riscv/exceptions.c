/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Exception handling for RISC-V-based ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <inttypes.h>

#include "kernel_defines.h"
#include "panic.h"
#include "periph/pm.h"

#include "esp_attr.h"
#include "riscv/rvruntime-frames.h"
#include "rom/ets_sys.h"

static const char *exceptions[] = {
    "nil",
    "0x1: PMP Instruction access fault",
    "0x2: Illegal Instruction",
    "0x3: Hardware Breakpoint/Watchpoint or EBREAK",
    "nil",
    "0x5: PMP Load access fault",
    "nil",
    "0x7: PMP Store access fault",
    "0x8: ECALL from U mode",
    "nil",
    "nil",
    "0xb: ECALL from M mode",
};

static RvExcFrame *_frame = NULL;

void init_exceptions (void)
{
}

void IRAM_ATTR xt_unhandled_exception(RvExcFrame *frame)
{
    _frame = frame;
    core_panic(PANIC_GENERAL_ERROR, "Unhandled exception");
}

void IRAM_ATTR panicHandler(RvExcFrame *frame)
{
    _frame = frame;
    core_panic(PANIC_GENERAL_ERROR, "Panic handler");
}

extern void heap_stats(void);

void panic_arch(void)
{
    if (_frame) {
        /* TODO */
        ets_printf("Exception @0x%08"PRIx32", cause %s\n",
                   _frame->mepc, exceptions[_frame->mcause]);
    }
#if defined(DEVELHELP)
    heap_stats();
#endif
}
