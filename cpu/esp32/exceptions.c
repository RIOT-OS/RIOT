/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       ESP32 exception handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG  0
#include "debug.h"

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "esp_common.h"
#include "irq.h"
#include "log.h"
#include "periph/pm.h"
#include "ps.h"

#include "esp/common_macros.h"
#include "esp/xtensa_ops.h"
#include "rom/ets_sys.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "sdk_conf.h"
#include "xtensa/corebits.h"
#include "freertos/xtensa_api.h"

#ifdef MODULE_ESP_GDBSTUB
#include "esp_gdbstub.h"
#endif

#ifdef MODULE_ESP_IDF_HEAP
#include "heap/esp_heap_caps.h"
#endif

extern void malloc_stats (void);
extern unsigned int get_free_heap_size (void);
extern uint8_t _eheap;     /* end of heap (defined in esp32.common.ld) */
extern uint8_t _sheap;     /* start of heap (defined in esp32.common.ld) */

static const char* exception_names [] =
{
        "IllegalInstructionCause",     /* 0 */
        "SyscallCause",                /* 1 */
        "InstructionFetchErrorCause",  /* 2 */
        "LoadStoreErrorCause",         /* 3 */
        "Level1InterruptCause",        /* 4 */
        "AllocaCause",                 /* 5 */
        "IntegerDivideByZeroCause",    /* 6 */
        "",                            /* 7 - reserved */
        "PrivilegedCause",             /* 8 */
        "LoadStoreAlignmentCause",     /* 9 */
        "",                            /* 10 - reserved */
        "",                            /* 11 - reserved */
        "InstrPIFDataErrorCause",      /* 12 */
        "LoadStorePIFDataErrorCause",  /* 13 */
        "InstrPIFAddrErrorCause",      /* 14 */
        "LoadStorePIFAddrErrorCause",  /* 15 */
        "InstTLBMissCause",            /* 16 */
        "InstTLBMultiHitCause",        /* 17 */
        "InstFetchPrivilegeCause",     /* 18 */
        "",                            /* 19 - reserved */
        "InstFetchProhibitedCause",    /* 20 */
        "",                            /* 21 - reserved */
        "",                            /* 22 - reserved */
        "",                            /* 23 - reserved */
        "LoadStoreTLBMissCause",       /* 24 */
        "LoadStoreTLBMultiHitCause",   /* 25 */
        "LoadStorePrivilegeCause",     /* 26 */
        "",                            /* 27 - reserved */
        "LoadProhibitedCause",         /* 28 */
        "StoreProhibitedCause",        /* 29 */
        "",                            /* 30 - reserved */
        "",                            /* 31 - reserved */
        "Coprocessor0Disabled",        /* 32 */
        "Coprocessor1Disabled",        /* 33 */
        "Coprocessor2Disabled",        /* 34 */
        "Coprocessor3Disabled",        /* 35 */
        "Coprocessor4Disabled",        /* 36 */
        "Coprocessor5Disabled",        /* 37 */
        "Coprocessor6Disabled",        /* 38 */
        "Coprocessor7Disabled",        /* 39 */
};

const char *reg_names[] = {
    "pc      ", "ps      ",
    "a0      ", "a1      ", "a2      ", "a3      ", "a4      ", "a5      ",
    "a6      ", "a7      ", "a8      ", "a9      ", "a10     ", "a11     ",
    "a12     ", "a13     ", "a14     ", "A15     ", "SAR     ",
    "exccause", "excvaddr", "lbeg    ", "lend    ", "lcount  "
};

void IRAM NORETURN exception_handler (XtExcFrame *frame)
{
    uint32_t excsave1;
    uint32_t epc1;
    uint32_t epc2;
    uint32_t epc3;
    uint32_t epc4;
    RSR(excsave1, excsave1);
    RSR(epc1, epc1);
    RSR(epc2, epc2);
    RSR(epc3, epc3);
    RSR(epc4, epc4);

    #ifdef MODULE_ESP_GDBSTUB
    esp_gdbstub_panic_handler(frame);
    #endif

    ets_printf("EXCEPTION!! exccause=%d (%s) @%08x excvaddr=%08x\n\n",
               frame->exccause, exception_names[frame->exccause],
               excsave1, frame->excvaddr);

    #if defined(DEVELHELP)
    #if defined(MODULE_PS)
    ets_printf("processes:\n");
    ps();
    ets_printf("\n");
    #endif /* MODULE_PS */
    #ifdef MODULE_ESP_IDF_HEAP
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
    #else
    ets_printf("\nheap: %u (free %u) byte\n", &_eheap - &_sheap, get_free_heap_size());
    #endif /* MODULE_ESP_IDF_HEAP */

    ets_printf("\nregister set\n");
    ets_printf("pc      : %08x\t", frame->pc);
    ets_printf("ps      : %08x\t", frame->ps);
    ets_printf("exccause: %08x\t", frame->exccause);
    ets_printf("excvaddr: %08x\n", frame->excvaddr);
    ets_printf("epc1    : %08x\t", epc1);
    ets_printf("epc2    : %08x\t", epc2);
    ets_printf("epc3    : %08x\t", epc3);
    ets_printf("epc4    : %08x\n", epc4);
    ets_printf("a0      : %08x\t", frame->a0);
    ets_printf("a1      : %08x\t", frame->a1);
    ets_printf("a2      : %08x\t", frame->a2);
    ets_printf("a3      : %08x\n", frame->a3);
    ets_printf("a4      : %08x\t", frame->a4);
    ets_printf("a5      : %08x\t", frame->a5);
    ets_printf("a6      : %08x\t", frame->a6);
    ets_printf("a7      : %08x\n", frame->a7);
    ets_printf("a8      : %08x\t", frame->a8);
    ets_printf("a9      : %08x\t", frame->a9);
    ets_printf("a10     : %08x\t", frame->a10);
    ets_printf("a11     : %08x\n", frame->a11);
    ets_printf("a12     : %08x\t", frame->a12);
    ets_printf("a13     : %08x\t", frame->a13);
    ets_printf("a14     : %08x\t", frame->a14);
    ets_printf("a15     : %08x\n", frame->a15);
    ets_printf("lbeg    : %08x\t", frame->lbeg);
    ets_printf("lend    : %08x\t", frame->lend);
    ets_printf("lcount  : %08x\n", frame->lcount);
    #endif /* DEVELHELP */

    /* restart */
    /* TODO: Improvement
       Normally, we should try to restart the system. However, this
       will not work after some exceptions, e.g., the LoadStoreErrorCause.
       Therefore, we break the execution and wait for the WDT reset. Maybe
       there is better way. If debugger is active, 'break 0,0' stops
       execution in debugger. */
    __asm__ volatile ("break 0,0");

    UNREACHABLE();
}

void init_exceptions (void)
{
    xt_set_exception_handler(EXCCAUSE_UNALIGNED, exception_handler);
    xt_set_exception_handler(EXCCAUSE_ILLEGAL, exception_handler);
    xt_set_exception_handler(EXCCAUSE_INSTR_ERROR, exception_handler);
    xt_set_exception_handler(EXCCAUSE_LOAD_STORE_ERROR, exception_handler);
    xt_set_exception_handler(EXCCAUSE_LOAD_PROHIBITED, exception_handler);
    xt_set_exception_handler(EXCCAUSE_STORE_PROHIBITED, exception_handler);
    xt_set_exception_handler(EXCCAUSE_PRIVILEGED, exception_handler);
}

void IRAM NORETURN panic_arch(void)
{
    #if defined(DEVELHELP)

    #ifdef MODULE_ESP_IDF_HEAP
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
    #else
    ets_printf("\nheap: %u (free %u) byte\n", &_eheap - &_sheap, get_free_heap_size());
    #endif /* MODULE_ESP_IDF_HEAP */

    /* break in debugger or reboot after WDT */
    __asm__ volatile ("break 0,0");

    #else /* DEVELHELP */

    /* restart */
    software_reset();

    #endif /* DEVELHELP */

    UNREACHABLE();
}

void _panic_handler(uint32_t addr)
{
    ets_printf("#! _xt_panic called from 0x%08x: powering off\n", addr);
    pm_off();
    while (1) { };
}
