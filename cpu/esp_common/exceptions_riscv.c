/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#if __riscv

#include <inttypes.h>

#include "kernel_defines.h"
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

void init_exceptions (void)
{
}

void IRAM_ATTR xt_unhandled_exception(RvExcFrame *frame)
{
    /* TODO */
    ets_printf("#! Unhandled exception @0x%08"PRIx32", cause %s, "
               "powering off\n", frame->mepc, exceptions[frame->mcause]);
    if (!IS_USED(MODULE_ESP_GDB)) {
        pm_off();
    }
    while (1) { };
}

void IRAM_ATTR panicHandler(RvExcFrame *frame)
{
    /* TODO */
    ets_printf("#! panicHandler called from 0x%08"PRIx32", cause: %08"
               PRIx32", powering off", frame->mepc, frame->mcause);
    if (!IS_USED(MODULE_ESP_GDB)) {
        pm_off();
    }
    while (1) { };
}

#endif /* __riscv */
