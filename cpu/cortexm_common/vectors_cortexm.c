/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Default implementations for Cortex-M specific interrupt and
 *              exception handlers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "kernel_init.h"
#include "board.h"
#include "mpu.h"
#include "panic.h"
#include "sched.h"
#include "vectors_cortexm.h"
#ifdef MODULE_PUF_SRAM
#include "puf_sram.h"
#endif
#ifdef MODULE_DBGPIN
#include "dbgpin.h"
#endif

#ifndef SRAM_BASE
#define SRAM_BASE 0
#endif

#ifndef CPU_BACKUP_RAM_NOT_RETAINED
#define CPU_BACKUP_RAM_NOT_RETAINED 0
#endif

/**
 * @brief   Memory markers, defined in the linker script
 * @{
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern uint8_t _sram;
extern uint8_t _eram;

/* Support for LPRAM. */
#ifdef CPU_HAS_BACKUP_RAM
extern const uint32_t _sbackup_data_load[];
extern uint32_t _sbackup_data[];
extern uint32_t _ebackup_data[];
extern uint32_t _sbackup_bss[];
extern uint32_t _ebackup_bss[];
#endif /* CPU_HAS_BACKUP_RAM */
/** @} */

/**
 * @brief   Allocation of the interrupt stack
 */
__attribute__((used,section(".isr_stack"))) uint8_t isr_stack[ISR_STACKSIZE];

/**
 * @brief   Pre-start routine for CPU-specific settings
 */
__attribute__((weak)) void pre_startup (void)
{
}

/**
 * @brief   Post-start routine for CPU-specific settings
 */
__attribute__((weak)) void post_startup (void)
{
}

void reset_handler_default(void)
{
    uint32_t *dst;
    const uint32_t *src = &_etext;

#ifdef __ARM_ARCH_8M_MAIN__
    /* Set the lower limit of the exception stack into MSPLIM register */
    __set_MSPLIM((uint32_t)&_sstack);
#endif

    cortexm_init_fpu();

#ifdef MODULE_PUF_SRAM
    puf_sram_init((uint8_t *)&_srelocate, SEED_RAM_LEN);
#endif

    pre_startup();

#ifdef DEVELHELP
    /* cppcheck-suppress constVariable
     * (top is modified by asm) */
    uint32_t *top;
    /* Fill stack space with canary values up until the current stack pointer */
    /* Read current stack pointer from CPU register */
    __asm__ volatile ("mov %[top], sp" : [top] "=r" (top) : : );
    dst = &_sstack;
    while (dst < top) {
        *(dst++) = STACK_CANARY_WORD;
    }
#endif

    /* load data section from flash to ram */
    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

#ifdef CPU_HAS_BACKUP_RAM
#if BACKUP_RAM_HAS_INIT
    backup_ram_init();
#endif
    if (!cpu_woke_from_backup() ||
        CPU_BACKUP_RAM_NOT_RETAINED) {

        /* load low-power data section. */
        for (dst = _sbackup_data, src = _sbackup_data_load;
             dst < _ebackup_data;
             dst++, src++) {
            *dst = *src;
        }

        /* zero-out low-power bss. */
        /* cppcheck-suppress comparePointers
         * (addresses exported as symbols via linker script and look unrelated
         * to cppcheck) */
        for (dst = _sbackup_bss; dst < _ebackup_bss; dst++) {
            *dst = 0;
        }
    }
#endif /* CPU_HAS_BACKUP_RAM */

#ifdef MODULE_MPU_NOEXEC_RAM
    /* This marks the memory region from 0x20000000 to 0x3FFFFFFF as non
     * executable. This is the Cortex-M SRAM region used for on-chip RAM.
     */
    mpu_configure(
        0,                                               /* Region 0 (lowest priority) */
        (uintptr_t)&_sram,                               /* RAM base address */
        MPU_ATTR(1, AP_RW_RW, 0, 1, 0, 1, MPU_SIZE_512M) /* Allow read/write but no exec */
    );
#endif

#ifdef MODULE_MPU_STACK_GUARD
    if (((uintptr_t)&_sstack) != SRAM_BASE) {
        mpu_configure(
            1,                                              /* MPU region 1 */
            (uintptr_t)&_sstack + 31,                       /* Base Address (rounded up) */
            MPU_ATTR(1, AP_RO_RO, 0, 1, 0, 1, MPU_SIZE_32B) /* Attributes and Size */
        );

    }
#endif

#if defined(MODULE_MPU_STACK_GUARD) || defined(MODULE_MPU_NOEXEC_RAM)
    mpu_enable();
#endif

    post_startup();

#ifdef MODULE_DBGPIN
    dbgpin_init();
#endif

#if !DISABLE_CPU_INIT
    /* initialize the CPU */
    extern void cpu_init(void);
    cpu_init();
#endif
#if !DISABLE_BOARD_INIT
    /* initialize the board */
    board_init();
#endif

#if MODULE_NEWLIB || MODULE_PICOLIBC
    /* initialize std-c library (this must be done after board_init) */
    extern void __libc_init_array(void);
    __libc_init_array();
#endif

    /* startup the kernel */
    kernel_init();
}

__attribute__((weak))
void nmi_handler(void)
{
    core_panic(PANIC_NMI_HANDLER, "NMI HANDLER");
}

#ifdef DEVELHELP

/* The hard fault handler requires some stack space as a working area for local
 * variables and printf function calls etc. If the stack pointer is located
 * closer than HARDFAULT_HANDLER_REQUIRED_STACK_SPACE from the lowest address of
 * RAM we will reset the stack pointer to the top of available RAM.
 * Measured from trampoline entry to breakpoint:
 *  - Cortex-M0+ 344 Byte
 *  - Cortex-M4  344 Byte
 */
#define HARDFAULT_HANDLER_REQUIRED_STACK_SPACE          (344U)

static inline int _stack_size_left(uint32_t required)
{
    uint32_t* sp;
    __asm__ volatile ("mov %[sp], sp" : [sp] "=r" (sp) : : );
    return ((int)((uint32_t)sp - (uint32_t)&_sstack) - required);
}

void hard_fault_handler(uint32_t* sp, uint32_t corrupted, uint32_t exc_return,
                        uint32_t* r4_to_r11_stack);

/* Trampoline function to save stack pointer before calling hard fault handler */
__attribute__((naked)) void hard_fault_default(void)
{
    /* Get stack pointer where exception stack frame lies */
    __asm__ volatile
    (
        ".syntax unified                    \n"
        /* Check that msp is valid first because we want to stack all the
         * r4-r11 registers so that we can use r0, r1, r2, r3 for other things. */
        "mov r0, sp                         \n" /* r0 = msp                   */
        "cmp r0, %[eram]                    \n" /* if(msp > &_eram) {         */
        "bhi fix_msp                        \n" /*   goto fix_msp }           */
        "cmp r0, %[sram]                    \n" /* if(msp <= &_sram) {        */
        "bls fix_msp                        \n" /*   goto fix_msp }           */
        "movs r1, #0                        \n" /* else { corrupted = false   */
        "b   test_sp                        \n" /*   goto test_sp     }       */
        " fix_msp:                          \n" /*                            */
        "mov r1, %[estack]                  \n" /*     r1 = _estack           */
        "mov sp, r1                         \n" /*     sp = r1                */
        "movs r1, #1                        \n" /*     corrupted = true       */
        " test_sp:                          \n" /*                            */
        "movs r0, #4                        \n" /* r0 = 0x4                   */
        "mov r2, lr                         \n" /* r2 = lr                    */
        "tst r2, r0                         \n" /* if(lr & 0x4)               */
        "bne use_psp                        \n" /* {                          */
        "mrs r0, msp                        \n" /*   r0 = msp                 */
        "b out                              \n" /* }                          */
        " use_psp:                          \n" /* else {                     */
        "mrs r0, psp                        \n" /*   r0 = psp                 */
        " out:                              \n" /* }                          */
#if (defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS)) \
    && defined(MODULE_CPU_CHECK_ADDRESS)
        /* catch intended HardFaults on Cortex-M0 to probe memory addresses */
        "ldr     r1, [r0, #0x04]            \n" /* read R1 from the stack        */
        "ldr     r2, =0xDEADF00D            \n" /* magic number to be found      */
        "cmp     r1, r2                     \n" /* compare with the magic number */
        "bne     regular_handler            \n" /* no magic -> handle as usual   */
        "ldr     r1, [r0, #0x08]            \n" /* read R2 from the stack        */
        "ldr     r2, =0xCAFEBABE            \n" /* 2nd magic number to be found  */
        "cmp     r1, r2                     \n" /* compare with 2nd magic number */
        "bne     regular_handler            \n" /* no magic -> handle as usual   */
        "ldr     r1, [r0, #0x18]            \n" /* read PC from the stack        */
        "adds    r1, r1, #2                 \n" /* move to the next instruction  */
        "str     r1, [r0, #0x18]            \n" /* modify PC in the stack        */
        "ldr     r5, =0                     \n" /* set R5 to indicate HardFault  */
        "bx      lr                         \n" /* exit the exception handler    */
        " regular_handler:                  \n"
#endif
#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
        "push {r4-r7}                       \n" /* save r4..r7 to the stack   */
        "mov r3, r8                         \n" /*                            */
        "mov r4, r9                         \n" /*                            */
        "mov r5, r10                        \n" /*                            */
        "mov r6, r11                        \n" /*                            */
        "push {r3-r6}                       \n" /* save r8..r11 to the stack  */
#else
        "push {r4-r11}                      \n" /* save r4..r11 to the stack  */
#endif
        "mov r3, sp                         \n" /* r4_to_r11_stack parameter  */
        "bl hard_fault_handler              \n" /* hard_fault_handler(r0)     */
          :
          : [sram]   "r" ((uintptr_t)&_sram + HARDFAULT_HANDLER_REQUIRED_STACK_SPACE),
            [eram]   "r" (&_eram),
            [estack] "r" (&_estack)
          : "r0", "r4", "r5", "r6", "r8", "r9", "r10", "r11", "lr"
    );
}

#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
/* Cortex-M0, Cortex-M0+ and Cortex-M23 lack the extended fault status
   registers found in Cortex-M3 and above. */
#define CPU_HAS_EXTENDED_FAULT_REGISTERS 0
#else
#define CPU_HAS_EXTENDED_FAULT_REGISTERS 1
#endif

__attribute__((used)) void hard_fault_handler(uint32_t* sp, uint32_t corrupted, uint32_t exc_return,
                                              uint32_t* r4_to_r11_stack)
{
#if CPU_HAS_EXTENDED_FAULT_REGISTERS
    static const uint32_t BFARVALID_MASK = (0x80 << SCB_CFSR_BUSFAULTSR_Pos);
    static const uint32_t MMARVALID_MASK = (0x80 << SCB_CFSR_MEMFAULTSR_Pos);

    /* Copy status register contents to local stack storage, this must be
     * done before any calls to other functions to avoid corrupting the
     * register contents. */
    uint32_t bfar  = SCB->BFAR;
    uint32_t mmfar = SCB->MMFAR;
    uint32_t cfsr  = SCB->CFSR;
    uint32_t hfsr  = SCB->HFSR;
    uint32_t dfsr  = SCB->DFSR;
    uint32_t afsr  = SCB->AFSR;
#endif

    /* Initialize these variables even if they're never used uninitialized.
     * Fixes wrong compiler warning by gcc < 6.0. */
    uint32_t pc = 0;
    /* cppcheck-suppress variableScope
     * (reason: used within __asm__ which cppcheck doesn't pick up) */
    uint32_t* orig_sp = NULL;

    /* Check if the ISR stack overflowed previously. Not possible to detect
     * after output may also have overflowed it. */
    if (*(&_sstack) != STACK_CANARY_WORD) {
        puts("\nISR stack overflowed");
    }
    /* Sanity check stack pointer and give additional feedback about hard fault */
    if (corrupted) {
        puts("Stack pointer corrupted, reset to top of stack");
        printf("active thread: %"PRIkernel_pid"\n", thread_getpid());
    }
    else {
        uint32_t  r0 = sp[0];
        uint32_t  r1 = sp[1];
        uint32_t  r2 = sp[2];
        uint32_t  r3 = sp[3];
        uint32_t r12 = sp[4];
        uint32_t  lr = sp[5];  /* Link register. */
                  pc = sp[6];  /* Program counter. */
        uint32_t psr = sp[7];  /* Program status register. */

        /* Reconstruct original stack pointer before fault occurred */
        orig_sp = sp + 8;
#ifdef SCB_CCR_STKALIGN_Msk
        if (psr & SCB_CCR_STKALIGN_Msk) {
            /* Stack was not 8-byte aligned */
            orig_sp += 1;
        }
#endif /* SCB_CCR_STKALIGN_Msk */
        puts("\nContext before hardfault:");

        /* TODO: printf in ISR context might be a bad idea */
        printf("   r0: 0x%08" PRIx32 "\n"
               "   r1: 0x%08" PRIx32 "\n"
               "   r2: 0x%08" PRIx32 "\n"
               "   r3: 0x%08" PRIx32 "\n",
               r0, r1, r2, r3);
        printf("  r12: 0x%08" PRIx32 "\n"
               "   lr: 0x%08" PRIx32 "\n"
               "   pc: 0x%08" PRIx32 "\n"
               "  psr: 0x%08" PRIx32 "\n\n",
               r12, lr, pc, psr);
    }
#if CPU_HAS_EXTENDED_FAULT_REGISTERS
    puts("FSR/FAR:");
    printf(" CFSR: 0x%08" PRIx32 "\n", cfsr);
    printf(" HFSR: 0x%08" PRIx32 "\n", hfsr);
    printf(" DFSR: 0x%08" PRIx32 "\n", dfsr);
    printf(" AFSR: 0x%08" PRIx32 "\n", afsr);
    if (cfsr & BFARVALID_MASK) {
        /* BFAR valid flag set */
        printf(" BFAR: 0x%08" PRIx32 "\n", bfar);
    }
    if (cfsr & MMARVALID_MASK) {
        /* MMFAR valid flag set */
        printf("MMFAR: 0x%08" PRIx32 "\n", mmfar);
    }
#endif
    puts("Misc");
    printf("EXC_RET: 0x%08" PRIx32 "\n", exc_return);

    if (!corrupted) {
        /* Test if the EXC_RETURN returns to thread mode,
         * to check if the hard fault happened in ISR context */
        if (exc_return & 0x08) {
            kernel_pid_t active_pid = thread_getpid();
            printf("Active thread: %"PRIi16" \"%s\"\n",
                   active_pid, thread_getname(active_pid));
        }
        else {
            /* Print the interrupt number, NMI being -14, hardfault is -13,
             * IRQ0 is 0 and so on */
            uint32_t psr = sp[7];  /* Program status register. */
            printf("Hard fault occurred in ISR number %d\n",
                   (int)(psr & 0xff) - 16);
        }
        puts("Attempting to reconstruct state for debugging...");
        printf("In GDB:\n  set $pc=0x%" PRIx32 "\n  frame 0\n  bt\n", pc);
        int stack_left = _stack_size_left(HARDFAULT_HANDLER_REQUIRED_STACK_SPACE);
        if (stack_left < 0) {
            printf("\nISR stack overflowed by at least %d bytes.\n", (-1 * stack_left));
        }
        __asm__ volatile (
            "mov r0, %[sp]\n"
            "ldr r2, [r0, #8]\n"
            "ldr r3, [r0, #12]\n"
            "ldr r1, [r0, #16]\n"
            "mov r12, r1\n"
            "ldr r1, [r0, #20]\n"
            "mov lr, r1\n"
            "mov sp, %[orig_sp]\n"
            "mov r1, %[extra_stack]\n"
#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
            "ldm r1!, {r4-r7}\n"
            "mov r8, r4\n"
            "mov r9, r5\n"
            "mov r10, r6\n"
            "mov r11, r7\n"
            "ldm r1!, {r4-r7}\n"
#else
            "ldm r1, {r4-r11}\n"
#endif
            "ldr r1, [r0, #4]\n"
            "ldr r0, [r0, #0]\n"
            :
            : [sp] "r" (sp),
              [orig_sp] "r" (orig_sp),
              [extra_stack] "r" (r4_to_r11_stack)
            : "r0", "r1", "r2", "r3", "r12"
            );
    }

    core_panic(PANIC_HARD_FAULT, "HARD FAULT HANDLER");
}

#else

void hard_fault_default(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT HANDLER");
}

#endif /* DEVELHELP */

#if defined(CPU_CORE_CORTEX_M3) || defined(CPU_CORE_CORTEX_M33) || \
    defined(CPU_CORE_CORTEX_M4) || defined(CPU_CORE_CORTEX_M4F) || \
    defined(CPU_CORE_CORTEX_M7)
void mem_manage_default(void)
{
    core_panic(PANIC_MEM_MANAGE, "MEM MANAGE HANDLER");
}

void bus_fault_default(void)
{
    core_panic(PANIC_BUS_FAULT, "BUS FAULT HANDLER");
}

void usage_fault_default(void)
{
    core_panic(PANIC_USAGE_FAULT, "USAGE FAULT HANDLER");
}

void debug_mon_default(void)
{
    core_panic(PANIC_DEBUG_MON, "DEBUG MON HANDLER");
}
#endif

void dummy_handler_default(void)
{
    core_panic(PANIC_DUMMY_HANDLER, "DUMMY HANDLER");
}

/* Cortex-M common interrupt vectors */
__attribute__((weak, alias("dummy_handler_default"))) void isr_svc(void);
__attribute__((weak, alias("dummy_handler_default"))) void isr_pendsv(void);
__attribute__((weak, alias("dummy_handler_default"))) void isr_systick(void);

/* define Cortex-M base interrupt vectors
 * IRQ entries -9 to -6 inclusive (offsets 0x1c to 0x2c of cortexm_base_t)
 * are reserved entries. */
ISR_VECTOR(0) const cortexm_base_t cortex_vector_base = {
    &_estack,
    {
        /* entry point of the program */
        [ 0] = reset_handler_default,
        /* [-14] non maskable interrupt handler */
        [ 1] = nmi_handler,
        /* [-13] hard fault exception */
        [ 2] = hard_fault_default,
        /* [-5] SW interrupt, in RIOT used for triggering context switches */
        [10] = isr_svc,
        /* [-2] pendSV interrupt, in RIOT use to do the actual context switch */
        [13] = isr_pendsv,
        /* [-1] SysTick interrupt, not used in RIOT */
        [14] = isr_systick,

        /* -9 to -6 reserved entries can be defined by the cpu module */
        #ifdef CORTEXM_VECTOR_RESERVED_0X1C
        [6] = (isr_t)(CORTEXM_VECTOR_RESERVED_0X1C),
        #endif  /* CORTEXM_VECTOR_RESERVED_0X1C */
        #ifdef CORTEXM_VECTOR_RESERVED_0X20
        [7] = (isr_t)(CORTEXM_VECTOR_RESERVED_0X20),
        #endif  /* CORTEXM_VECTOR_RESERVED_0X20 */
        #ifdef CORTEXM_VECTOR_RESERVED_0X24
        [8] = (isr_t)(CORTEXM_VECTOR_RESERVED_0X24),
        #endif  /* CORTEXM_VECTOR_RESERVED_0X24 */
        #ifdef CORTEXM_VECTOR_RESERVED_0X28
        [9] = (isr_t)(CORTEXM_VECTOR_RESERVED_0X28),
        #endif  /* CORTEXM_VECTOR_RESERVED_0X28 */

        /* additional vectors used by M3, M33, M4(F), and M7 */
#if defined(CPU_CORE_CORTEX_M3) || defined(CPU_CORE_CORTEX_M33) || \
    defined(CPU_CORE_CORTEX_M4) || defined(CPU_CORE_CORTEX_M4F) || \
    defined(CPU_CORE_CORTEX_M7)
        /* [-12] memory manage exception */
        [ 3] = mem_manage_default,
        /* [-11] bus fault exception */
        [ 4] = bus_fault_default,
        /* [-10] usage fault exception */
        [ 5] = usage_fault_default,
        /* [-4] debug monitor exception */
        [11] = debug_mon_default,
#endif
    }
};
