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

#include <inttypes.h>

#include "kernel_defines.h"
#include "panic.h"
#include "periph/pm.h"

#include "esp_attr.h"
#include "hal/cpu_ll.h"
#include "riscv/rvruntime-frames.h"
#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"

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

static void IRAM_ATTR exception_handler_entry(RvExcFrame *frame)
{
    /* save the pointer to the RISC-V exception frame */
    _frame = frame;

    /* Ensure that the exception handler is called only once. */
    static uint8_t exception_handler_calls = 0;
    if (++exception_handler_calls > 1) {
        /* If the exception handler is called more than once, break to the
         * debugger if attached or stop the execution. This normally indicates
         * an exception while handling another exception, for example trying
         * to use a function from IROM while the cache is disabled. */
        if (cpu_ll_is_debugger_attached()) {
            cpu_ll_break();
        }
        else {
            while (1) { }
        }
    }

    /* Before the literal strings in the IROM can be accessed by the exception
     * handler, it has to be ensured that the IROM cache is enabled. However,
     * before its activation it is necessary to wait until possible write
     * operations to the SPI flash have been finished. */
#if defined(CPU_FAM_ESP32C3) || defined(CPU_FAM_ESP32H2)
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    Cache_Invalidate_ICache_All();
    Cache_Resume_ICache(1);
#else
#error Platform implementation required
#endif
}

void IRAM_ATTR xt_unhandled_exception(RvExcFrame *frame)
{
    exception_handler_entry(frame);
    core_panic(PANIC_GENERAL_ERROR, "Unhandled exception");
}

void IRAM_ATTR panicHandler(RvExcFrame *frame)
{
    exception_handler_entry(frame);
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
    /* print heap statistics */
    heap_stats();
    /* break in debugger if attached or busy wait until WDT resets the MCU */
    if (cpu_ll_is_debugger_attached()) {
        cpu_ll_break();
    }
    else {
        while (1) { }
    }
#endif
}
