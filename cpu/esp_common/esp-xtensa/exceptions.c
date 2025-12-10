/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Exception handling for Xtensa-based ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "periph/pm.h"
#include "ps.h"

#include "esp_common.h"
#include "esp/common_macros.h"
#include "esp/xtensa_ops.h"
#include "xtensa/corebits.h"
#include "xtensa/xtensa_api.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern void heap_stats(void);

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

void IRAM NORETURN exception_handler (XtExcFrame *frame)
{
    uint32_t excsave1;
    uint32_t epc1;
    RSR(excsave1, excsave1);
    RSR(epc1, epc1);
#if XCHAL_NMILEVEL >= 2
    uint32_t epc2;
    RSR(epc2, epc2);
#endif
#if XCHAL_NMILEVEL >= 3
    uint32_t epc3;
    RSR(epc3, epc3);
#endif
#if XCHAL_NMILEVEL >= 4
    uint32_t epc4;
    RSR(epc4, epc4);
#endif
#if XCHAL_NMILEVEL >= 5
    uint32_t epc5;
    RSR(epc5, epc5);
#endif
#if XCHAL_NMILEVEL >= 6
    uint32_t epc6;
    RSR(epc6, epc6);
#endif
#if XCHAL_NMILEVEL >= 7
    uint32_t epc7;
    RSR(epc7, epc7);
#endif

    ets_printf("EXCEPTION!! exccause=%d (%s) @%08x excvaddr=%08x\n",
               frame->exccause, exception_names[frame->exccause],
               excsave1, frame->excvaddr);

#if defined(DEVELHELP)

#if defined(MODULE_PS)
    ets_printf("processes:\n");
    ps();
    ets_printf("\n");
#endif /* MODULE_PS */

    heap_stats();

    ets_printf("\nregister set\n");
    ets_printf("pc      : %08x\t", frame->pc);
    ets_printf("ps      : %08x\t", frame->ps);
    ets_printf("exccause: %08x\t", frame->exccause);
    ets_printf("excvaddr: %08x\n", frame->excvaddr);
    ets_printf("epc1    : %08x\t", epc1);
#if XCHAL_NMILEVEL >= 2
    ets_printf("epc2    : %08x\t", epc2);
#endif
#if XCHAL_NMILEVEL >= 3
    ets_printf("epc3    : %08x\t", epc3);
#endif
#if XCHAL_NMILEVEL >= 4
    ets_printf("epc4    : %08x\n", epc4);
#endif
#if XCHAL_NMILEVEL >= 5
    ets_printf("epc5    : %08x\t", epc5);
#endif
#if XCHAL_NMILEVEL >= 6
    ets_printf("epc6    : %08x\t", epc6);
#endif
#if XCHAL_NMILEVEL >= 7
    ets_printf("epc7    : %08x\t", epc7);
#endif
    ets_printf("\n");
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
#if XCHAL_HAVE_LOOPS
    ets_printf("lbeg    : %08x\t", frame->lbeg);
    ets_printf("lend    : %08x\t", frame->lend);
    ets_printf("lcount  : %08x\n", frame->lcount);
#endif /* XCHAL_HAVE_LOOPS */
#endif /* DEVELHELP */

    /* restart */
    /* TODO: Improvement
       Normally, we should try to restart the system. However, this
       will not work after some exceptions, e.g., the LoadStoreErrorCause.
       One option is to break the execution and wait for the WDT reset. Maybe
       there is better way. If debugger is active, 'break 0,0' stops the
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
    heap_stats();
    /* break in debugger or reboot after WDT */
    __asm__ volatile ("break 0,0");
#else /* DEVELHELP */
    /* restart */
    pm_reboot();
#endif /* DEVELHELP */

    UNREACHABLE();
}

void _panic_handler(uint32_t addr)
{
    ets_printf("#! _xt_panic called from 0x%08x: powering off\n", addr);
    pm_off();
    while (1) { };
}
