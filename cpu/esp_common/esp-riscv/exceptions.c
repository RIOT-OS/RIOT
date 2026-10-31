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
#include "riscv/csr.h"
#include "riscv/rvruntime-frames.h"
#include "soc/soc_caps.h"
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

#if defined(DEVELHELP)
#define _PMP_PRINT(n) \
    ets_printf("pmp%02d: cfg 0x%02"PRIx32" addr 0x%08"PRIx32"\n", n, \
               (uint32_t)PMP_ENTRY_CFG_READ(n), \
               (uint32_t)RV_READ_CSR(CSR_PMPADDR0 + n))

#if SOC_CPU_HAS_PMA
#define _PMA_PRINT(n) \
    ets_printf("pma%02d: cfg 0x%08"PRIx32" addr 0x%08"PRIx32"\n", n, \
               (uint32_t)RV_READ_CSR(CSR_PMACFG0 + n), \
               (uint32_t)RV_READ_CSR(CSR_PMAADDR0 + n))
#else
#define _PMA_PRINT(n)
#endif

#define _PROT_PRINT(n) do { _PMP_PRINT(n); _PMA_PRINT(n); } while (0)

/* print the PMP and PMA configuration to analyze access faults */
static void _print_memory_protection(void)
{
    _PROT_PRINT(0);  _PROT_PRINT(1);  _PROT_PRINT(2);  _PROT_PRINT(3);
    _PROT_PRINT(4);  _PROT_PRINT(5);  _PROT_PRINT(6);  _PROT_PRINT(7);
    _PROT_PRINT(8);  _PROT_PRINT(9);  _PROT_PRINT(10); _PROT_PRINT(11);
    _PROT_PRINT(12); _PROT_PRINT(13); _PROT_PRINT(14); _PROT_PRINT(15);
}
#endif /* defined(DEVELHELP) */

void panic_arch(void)
{
    if (_frame) {
        /* TODO */
        ets_printf("Exception @0x%08"PRIx32", cause %s, mtval 0x%08"PRIx32
                   ", ra 0x%08"PRIx32", sp 0x%08"PRIx32"\n",
                   _frame->mepc, exceptions[_frame->mcause], _frame->mtval,
                   _frame->ra, _frame->sp);
#if defined(DEVELHELP)
        /* instruction, load or store access fault */
        if ((_frame->mcause == 1) || (_frame->mcause == 5) ||
            (_frame->mcause == 7)) {
            _print_memory_protection();
        }
#endif
    }
#if defined(DEVELHELP)
    heap_stats();
#endif
}
