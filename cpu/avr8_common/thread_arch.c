/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2021-2023 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread
 * interface
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "sched.h"
#include "irq.h"
#include "cpu.h"
#include "board.h"
#include "macros/xtstr.h"

#ifndef __EIND__
#define __EIND__                0x3C
#endif

static void avr8_context_save(void);
static void avr8_context_restore(void);
static void avr8_enter_thread_mode(void);

/**
 * @brief Since AVR doesn't support direct manipulation of the program counter
 * we model a stack like it would be left by avr8_context_save().
 * The resulting layout in memory is the following:
 * ---------------thread_t (not created by thread_stack_init) ----------
 * local variables (a temporary value and the stackpointer)
 * -----------------------------------------------------------------------
 * a marker (AFFE) - for debugging purposes (helps finding the stack
 * -----------------------------------------------------------------------
 * a 16 Bit pointer to sched_task_exit
 * (Optional EIND bits are set to zero for devices with > 128kb FLASH)
 * -----------------------------------------------------------------------
 * a 16 Bit pointer to task_func
 * this is placed exactly at the place where the program counter would be
 * stored normally and thus can be returned to when avr8_context_restore()
 * has been run
 * (Optional EIND bits are set to zero for devices with > 128kb FLASH)
 * -----------------------------------------------------------------------
 * saved registers from context:
 * r0
 * status register
 * (Optional EIND, RAMPZ, RAMPX, RAMPY, RAMPD registers)
 * r1 - r23
 * pointer to arg in r24 and r25
 * r26 - r31
 * -----------------------------------------------------------------------
 *
 * After the invocation of avr8_context_restore() the pointer to task_func is
 * on top of the stack and can be returned to. This way we can actually place
 * it inside of the program counter of the MCU.
 * if task_func returns sched_task_exit gets popped into the PC
 */
char *thread_stack_init(thread_task_func_t task_func, void *arg,
                        void *stack_start, int stack_size)
{
    uint16_t tmp_adress;
    uint8_t *stk;

    /* AVR uses 16 Bit or two 8 Bit registers for storing  pointers*/
    stk = (uint8_t *)((uintptr_t)stack_start + stack_size);

    /* put marker on stack */
    stk--;
    *stk = (uint8_t)0xAF;
    stk--;
    *stk = (uint8_t)0xFE;

    /* save sched_task_exit */
    stk--;
    tmp_adress = (uint16_t)sched_task_exit;
    *stk = (uint8_t)(tmp_adress & (uint16_t)0x00ff);
    stk--;
    tmp_adress >>= 8;
    *stk = (uint8_t)(tmp_adress & (uint16_t)0x00ff);

#if __AVR_3_BYTE_PC__
    /* Devices with more than 128kb FLASH use a PC with more than 16bits, we
     * set whole the top byte forcibly to 0 */
    stk--;
    *stk = (uint8_t)0x00;
#endif

    /* save address to task_func in place of the program counter */
    stk--;
    tmp_adress = (uint16_t)task_func;
    *stk = (uint8_t)(tmp_adress & (uint16_t)0x00ff);
    stk--;
    tmp_adress >>= 8;
    *stk = (uint8_t)(tmp_adress & (uint16_t)0x00ff);

#if __AVR_3_BYTE_PC__
    /* Devices with more than 128kb FLASH use a PC with more than 16bits, we
     * set whole the top byte forcibly to 0 */
    stk--;
    *stk = (uint8_t)0x00;
#endif

    /* r0 */
    stk--;
    *stk = (uint8_t)0x00;

    /* status register (with interrupts enabled) */
    stk--;
    *stk = (uint8_t)0x80;

#if __AVR_HAVE_RAMPZ__
    stk--;
    *stk = (uint8_t)0x00; /* RAMPZ */
#endif

#if __AVR_HAVE_RAMPY__
    stk--;
    *stk = (uint8_t)0x00; /* RAMPY */
#endif
#if __AVR_HAVE_RAMPX__
    stk--;
    *stk = (uint8_t)0x00; /* RAMPX */
#endif
#if __AVR_HAVE_RAMPD__
    stk--;
    *stk = (uint8_t)0x00; /* RAMPD */
#endif

#if __AVR_3_BYTE_PC__
    stk--;
    *stk = (uint8_t)0x00; /* EIND */
#endif

    /* r1 - has always to be 0 */
    stk--;
    *stk = (uint8_t)0x00;
    /*
     * Space for registers r2 -r23
     *
     * use loop for better readability, the compiler unrolls anyways
     */

    int i;

    for (i = 2; i <= 23; i++) {
        stk--;
        *stk = (uint8_t)0;
    }

    /*
     * In accordance with the AVR calling conventions *arg has to be inside
     * r24 and r25
     * */
    stk--;
    tmp_adress = (uint16_t)arg;
    *stk = (uint8_t)(tmp_adress & (uint16_t)0x00ff);
    stk--;
    tmp_adress >>= 8;
    *stk = (uint8_t)(tmp_adress & (uint16_t)0x00ff);

    /*
     * Space for registers r26-r31
     */
    for (i = 26; i <= 31; i++) {
        stk--;
        *stk = (uint8_t)i;
    }

    stk--;
    return (char *)stk;
}

/**
 * @brief thread_stack_print prints the stack to stdout.
 * It depends on getting the correct values for stack_start, stack_size and sp
 * of the active thread.
 * Maybe it would be good to change that to way that is less dependent on
 * getting correct values elsewhere (since it is a debugging tool and in the
 * presence of bugs the data may be corrupted).
 */
void thread_stack_print(void)
{
    uint8_t found_marker = 1;
    uint8_t *sp = (uint8_t *)thread_get_active()->sp;
    uint16_t size = 0;

    printf("Printing current stack of thread %" PRIkernel_pid "\n", thread_getpid());
    printf("\taddress:\tdata:\n");

    do {
        printf("\t0x%04x:\t\t0x%04x\n", (unsigned int)sp, (unsigned int)*sp);
        sp++;
        size++;

        if ((*sp == 0xFE) && (*(sp + 1) == 0xAF)) {
            found_marker = 0;
        }
    } while (found_marker == 1);

    printf("stack size: %u bytes\n", size);
}

void cpu_switch_context_exit(void)
{
    sched_run();
    avr8_enter_thread_mode();
}

#define STACK_POINTER  ((char *)AVR_STACK_POINTER_REG)
extern size_t __malloc_margin;
extern char * __malloc_heap_start;
extern char * __malloc_heap_end;
extern char *__brkval;

/**
 * @brief Set the MCU into Thread-Mode and load the initial task from the
 * stack and run it
 */
void NORETURN avr8_enter_thread_mode(void)
{
    irq_enable();

    /*
     * Save the current stack pointer to __malloc_heap_end. Since
     * context_restore is always inline, there is no function call and the
     * current stack pointer is the lowest possible stack address outside the
     * thread-mode. Therefore, it can be considered as the top of the heap.
     */
    __malloc_heap_end = STACK_POINTER - __malloc_margin;

    /* __brkval has to be initialized if necessary */
    if (__brkval == NULL) {
        __brkval = __malloc_heap_start;
    }

    avr8_context_restore();
    __asm__ volatile ("ret");

    UNREACHABLE();
}

void thread_yield_higher_arch(void)
{
    if (!IS_USED(MODULE_CORE_THREAD)) {
        return;
    }

    if (irq_is_in() == 0) {
        avr8_context_save();
        sched_run();
        avr8_context_restore();
    }
    else {
        sched_context_switch_request = 1;
    }
}

void avr8_exit_isr(void)
{
    --avr8_state_irq_count;

    /* Force access to avr8_state to take place */
    __asm__ volatile ("" : : : "memory");

    /* schedule should switch context when returning from a non nested interrupt */
    if (sched_context_switch_request && avr8_state_irq_count == 0 &&
        IS_USED(MODULE_CORE_THREAD)) {
        avr8_context_save();
        sched_run();
        avr8_context_restore();
        __asm__ volatile ("reti");
    }
}

__attribute__((always_inline)) static inline void avr8_context_save(void)
{
    __asm__ volatile (
        "push __tmp_reg__                        \n\t"
        "in   __tmp_reg__, __SREG__              \n\t"
        "cli                                     \n\t"
        "push __tmp_reg__                        \n\t"

#if __AVR_HAVE_RAMPZ__
        "in     __tmp_reg__, __RAMPZ__           \n\t"
        "push   __tmp_reg__                      \n\t"
#endif

#if __AVR_HAVE_RAMPY__
        "in     __tmp_reg__, __RAMPY__           \n\t"
        "push   __tmp_reg__                      \n\t"
#endif

#if __AVR_HAVE_RAMPX__
        "in     __tmp_reg__, __RAMPX__           \n\t"
        "push   __tmp_reg__                      \n\t"
#endif

#if __AVR_HAVE_RAMPD__
        "in     __tmp_reg__, __RAMPD__           \n\t"
        "push   __tmp_reg__                      \n\t"
#endif

#if __AVR_3_BYTE_PC__
        "in     __tmp_reg__, " XTSTR(__EIND__) " \n\t"
        "push   __tmp_reg__                      \n\t"
#endif

        "push r1                                 \n\t"
        "clr  r1                                 \n\t"
        "push r2                                 \n\t"
        "push r3                                 \n\t"
        "push r4                                 \n\t"
        "push r5                                 \n\t"
        "push r6                                 \n\t"
        "push r7                                 \n\t"
        "push r8                                 \n\t"
        "push r9                                 \n\t"
        "push r10                                \n\t"
        "push r11                                \n\t"
        "push r12                                \n\t"
        "push r13                                \n\t"
        "push r14                                \n\t"
        "push r15                                \n\t"
        "push r16                                \n\t"
        "push r17                                \n\t"
        "push r18                                \n\t"
        "push r19                                \n\t"
        "push r20                                \n\t"
        "push r21                                \n\t"
        "push r22                                \n\t"
        "push r23                                \n\t"
        "push r24                                \n\t"
        "push r25                                \n\t"
        "push r26                                \n\t"
        "push r27                                \n\t"
        "push r28                                \n\t"
        "push r29                                \n\t"
        "push r30                                \n\t"
        "push r31                                \n\t"
        "lds  r26, sched_active_thread           \n\t"
        "lds  r27, sched_active_thread + 1       \n\t"
        "in   __tmp_reg__, __SP_L__              \n\t"
        "st   x+, __tmp_reg__                    \n\t"
        "in   __tmp_reg__, __SP_H__              \n\t"
        "st   x+, __tmp_reg__                    \n\t");
}

__attribute__((always_inline)) static inline void avr8_context_restore(void)
{
    __asm__ volatile (
        "lds  r26, sched_active_thread           \n\t"
        "lds  r27, sched_active_thread + 1       \n\t"
        "ld   r28, x+                            \n\t"
        "out  __SP_L__, r28                      \n\t"
        "ld   r29, x+                            \n\t"
        "out  __SP_H__, r29                      \n\t"
        "pop  r31                                \n\t"
        "pop  r30                                \n\t"
        "pop  r29                                \n\t"
        "pop  r28                                \n\t"
        "pop  r27                                \n\t"
        "pop  r26                                \n\t"
        "pop  r25                                \n\t"
        "pop  r24                                \n\t"
        "pop  r23                                \n\t"
        "pop  r22                                \n\t"
        "pop  r21                                \n\t"
        "pop  r20                                \n\t"
        "pop  r19                                \n\t"
        "pop  r18                                \n\t"
        "pop  r17                                \n\t"
        "pop  r16                                \n\t"
        "pop  r15                                \n\t"
        "pop  r14                                \n\t"
        "pop  r13                                \n\t"
        "pop  r12                                \n\t"
        "pop  r11                                \n\t"
        "pop  r10                                \n\t"
        "pop  r9                                 \n\t"
        "pop  r8                                 \n\t"
        "pop  r7                                 \n\t"
        "pop  r6                                 \n\t"
        "pop  r5                                 \n\t"
        "pop  r4                                 \n\t"
        "pop  r3                                 \n\t"
        "pop  r2                                 \n\t"
        "pop  r1                                 \n\t"

#if __AVR_3_BYTE_PC__
        "pop    __tmp_reg__                      \n\t"
        "out    " XTSTR(__EIND__) ", __tmp_reg__ \n\t"
#endif

#if __AVR_HAVE_RAMPD__
        "pop    __tmp_reg__                      \n\t"
        "out    __RAMPD__, __tmp_reg__           \n\t"
#endif
#if __AVR_HAVE_RAMPX__
        "pop    __tmp_reg__                      \n\t"
        "out    __RAMPX__, __tmp_reg__           \n\t"
#endif
#if __AVR_HAVE_RAMPY__
        "pop    __tmp_reg__                      \n\t"
        "out    __RAMPY__, __tmp_reg__           \n\t"
#endif

#if __AVR_HAVE_RAMPZ__
        "pop    __tmp_reg__                      \n\t"
        "out    __RAMPZ__, __tmp_reg__           \n\t"
#endif

        "pop  __tmp_reg__                        \n\t"
        "out  __SREG__, __tmp_reg__              \n\t"
        "pop  __tmp_reg__                        \n\t");
}
