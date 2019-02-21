/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       ESP8266 exception handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG  0
#include "debug.h"

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "log.h"
#include "periph/pm.h"
#include "ps.h"

#include "esp/common_macros.h"
#include "esp/xtensa_ops.h"
#include "sdk/ets.h"
#include "xtensa/corebits.h"

#ifndef MODULE_ESP_SDK_INT_HANDLING
#include "xtensa/xtensa_api.h"
#endif

extern void malloc_stats (void);
extern unsigned int get_free_heap_size (void);
extern uint8_t _eheap;  /* end of heap (defined in esp8266.riot-os.app.ld) */
extern uint8_t _sheap;  /* start of heap (defined in esp8266.riot-os.app.ld) */

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

#ifdef MODULE_ESP_SDK_INT_HANDLING

void IRAM NORETURN exception_handler (void *arg)
{
    (void)arg;
    uint32_t excsave1;
    uint32_t excvaddr;
    uint32_t exccause;
    RSR(excsave1, excsave1);
    RSR(excvaddr, excvaddr);
    RSR(exccause, exccause);
    (void)exception_names;

    ets_printf("EXCEPTION!! exccause=%d (%s) @%08lx excvaddr=%08lx\n",
               exccause, exception_names[exccause],
               excsave1, excvaddr);

    #if defined(DEVELHELP)
    #if defined(MODULE_PS)
    ps();
    #endif
    struct mallinfo minfo = mallinfo();
    ets_printf("heap: %lu (free %lu) byte\n", &_eheap - &_sheap, get_free_heap_size());
    ets_printf("sysmem: %d (used %d, free %d)\n", minfo.arena, minfo.uordblks, minfo.fordblks);
    #endif
    /* flushing the buffer */
    ets_printf("                                                          \n");
    ets_printf("                                                          \n");
    ets_printf("                                                          \n");

    /* hard reset */
    __asm__ volatile (" call0 0x40000080 ");

    UNREACHABLE();
}

void init_exceptions (void)
{
    _xtos_set_exception_handler(EXCCAUSE_UNALIGNED, exception_handler);
    _xtos_set_exception_handler(EXCCAUSE_ILLEGAL, exception_handler);
    _xtos_set_exception_handler(EXCCAUSE_INSTR_ERROR, exception_handler);
    _xtos_set_exception_handler(EXCCAUSE_LOAD_STORE_ERROR, exception_handler);
    _xtos_set_exception_handler(EXCCAUSE_LOAD_PROHIBITED, exception_handler);
    _xtos_set_exception_handler(EXCCAUSE_STORE_PROHIBITED, exception_handler);
    _xtos_set_exception_handler(EXCCAUSE_PRIVILEGED, exception_handler);
}

#else /* MODULE_ESP_SDK_INT_HANDLING */

void IRAM NORETURN exception_handler (XtExcFrame *frame)
{
    uint32_t excsave1;
    RSR(excsave1, excsave1);

    ets_printf("EXCEPTION!! exccause=%d (%s) @%08lx excvaddr=%08lx\n",
               frame->exccause, exception_names[frame->exccause],
               excsave1, frame->excvaddr);
    #if defined(DEVELHELP)
    #if defined(MODULE_PS)
    ps();
    #endif
    struct mallinfo minfo = mallinfo();
    ets_printf("heap: %lu (free %lu) byte\n", &_eheap - &_sheap, get_free_heap_size());
    ets_printf("sysmem: %d (used %d, free %d)\n", minfo.arena, minfo.uordblks, minfo.fordblks);
    #endif
    /* flushing the buffer */
    ets_printf("                                                          \n");
    ets_printf("                                                          \n");
    ets_printf("                                                          \n");

    /* hard reset */
    __asm__ volatile (" call0 0x40000080 ");

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
#endif /* MODULE_ESP_SDK_INT_HANDLING */

void IRAM NORETURN panic_arch(void)
{
    #if defined(DEVELHELP)
    struct mallinfo minfo = mallinfo();
    ets_printf("heap: %lu (free %lu) byte\n", &_eheap - &_sheap, get_free_heap_size());
    ets_printf("sysmem: %d (used %d, free %d)\n", minfo.arena, minfo.uordblks, minfo.fordblks);
    ets_printf("                                                          \n");
    ets_printf("                                                          \n");
    #endif

    /* hard reset */
    __asm__ volatile (" call0 0x40000080 ");

    UNREACHABLE();
}
