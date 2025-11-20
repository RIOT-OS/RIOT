/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of kernel's architecture dependent interface
 *
 * This file implements kernel's architecture dependent interface for RISC-V
 * based ESP32x SoCs
 *
 * @note The implementation in `$(RIOTCPU)/risc_common/thread_arch.c` cannot
 *       be used because
 *       - it requires some modifications for compatibility with ESP-IDF code
 *       - the ESP-IDF uses a different context frame
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <string.h>

#include "esp_attr.h"

#include "irq.h"
#include "thread.h"
#include "sched.h"
#include "syscalls.h"

#include "freertos/FreeRTOS.h"
#include "riscv/rvruntime-frames.h"
#include "soc/soc.h"
#include "soc/system_reg.h"

/*
 * The FreeRTOS implementation for interrupt/exception handling of ESP-IDF,
 * which is also used for RIOT to ensure compatibility with other ESP-IDF code,
 * uses a separate stack in the interrupt context. Therefore, the stack and
 * a pointer to the top of this stack are defined here.
 *
 * For compatibility reasons with xtensa implementation, we call them
 * port_IntStack and port_IntStackTop.
 */

/* bottom and top of ISR stack */
extern uint8_t port_IntStack;
extern uint8_t port_IntStackTop;

/* pointer the top of the ISR stack as required by ESP-IDF */
uint8_t *xIsrStackTop = &port_IntStackTop;

/* context frame as used by the ESP-IDF */
typedef struct context_switch_frame {
    uint32_t mepc;  /**< machine exception program counter instead of x0 */
    uint32_t ra;    /**< x1  - return address (caller saved) */
    uint32_t gp;    /**< x3  - global pointer (-) */
    uint32_t sp;    /**< x2  - stack pointer  (callee saved) */
    uint32_t tp;    /**< x4  - thread pointer (-) */
    uint32_t t0;    /**< x5  - temporary register (caller saved) */
    uint32_t t1;    /**< x6  - temporary register (caller saved) */
    uint32_t t2;    /**< x7  - temporary register (caller saved) */
    uint32_t s0;    /**< x8  - saved register / frame pointer (callee saved) */
    uint32_t s1;    /**< x9  - saved register (callee saved) */
    uint32_t a0;    /**< x10 - function argument / return value (caller saved) */
    uint32_t a1;    /**< x11 - function argument / return value (caller saved) */
    uint32_t a2;    /**< x12 - function argument (caller saved) */
    uint32_t a3;    /**< x13 - function argument (caller saved) */
    uint32_t a4;    /**< x14 - function argument (caller saved) */
    uint32_t a5;    /**< x15 - function argument (caller saved) */
    uint32_t a6;    /**< x16 - function argument (caller saved) */
    uint32_t a7;    /**< x17 - function argument (caller saved) */
    uint32_t s2;    /**< x18 - saved register (callee saved) */
    uint32_t s3;    /**< x19 - saved register (callee saved) */
    uint32_t s4;    /**< x20 - saved register (callee saved) */
    uint32_t s5;    /**< x21 - saved register (callee saved) */
    uint32_t s6;    /**< x22 - saved register (callee saved) */
    uint32_t s7;    /**< x23 - saved register (callee saved) */
    uint32_t s8;    /**< x24 - saved register (callee saved) */
    uint32_t s9;    /**< x25 - saved register (callee saved) */
    uint32_t s10;   /**< x26 - saved register (callee saved) */
    uint32_t s11;   /**< x27 - saved register (callee saved) */
    uint32_t t3;    /**< x28 - temporary register (caller saved) */
    uint32_t t4;    /**< x29 - temporary register (caller saved) */
    uint32_t t5;    /**< x30 - temporary register (caller saved) */
    uint32_t t6;    /**< x31 - temporary register (caller saved) */
} context_switch_frame_t;

/*
 * The following function is a modified copy of function `thread_stack_init`
 * in `$(RIOTCPU)/risc_common/thread_arch.c`, which is under the following
 * copyright:
 *
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * Modifications:
 * - For compatibility with ESP-IDF, the context frame is defined as
 *   used by the ESP-IDF.
 * - The `STACK_MARKER` is not used to identify the top of the stack.
 *   Instead, the stack is initialized in the `thread_create` function in
 *   `$(RIOTBASE)/core/thread.c` as expected in the `thread_measure_stack_free`
 *   function.
 * - Support for `__global_pointer$` and TLS has been added.
 */
char* thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    _Static_assert(sizeof(context_switch_frame_t) == 32 * sizeof(uint32_t),
                   "context frame has to store 32 registers");

    context_switch_frame_t *sf;
    uint8_t *stk_top;

    /* calculate the top of the stack */
    stk_top = (uint8_t *)stack_start + stack_size;
    /* per ABI align stack pointer to 16 byte boundary. */
    stk_top = (uint8_t *)((uintptr_t)stk_top & ~((uintptr_t)0xf));

    /* prepare thread local storage and the thread pointer */
    extern uint32_t __global_pointer$;

#if !defined(RISCV_NO_RELAX)
    extern char _thread_local_start, _thread_local_end, _flash_rodata_start;

    uint8_t *_local_start;
    uint32_t _local_size = (uint32_t)(&_thread_local_end - &_thread_local_start);

    _local_size = ALIGNUP(0x10, _local_size);
    stk_top -= _local_size;

    _local_start = stk_top;
    memcpy(_local_start, &_thread_local_start, _local_size);

    uint8_t *tp;
    tp = _local_start - (&_thread_local_start - &_flash_rodata_start);
#else
    uint8_t *tp = NULL;
#endif

    /* reserve space for the stack frame. */
    stk_top = stk_top - sizeof(*sf);

    /* populate the stack frame with default values for starting the thread. */
    sf = (struct context_switch_frame *)((uintptr_t)stk_top);

    /* Clear stack frame */
    memset(sf, 0, sizeof(*sf));

    /* set initial reg values */
    sf->mepc = (uint32_t)task_func;
    sf->a0 = (uint32_t)arg;
    sf->gp = (uint32_t)__global_pointer$;
    sf->tp = (uint32_t)tp;
    /* if the thread exits go to sched_task_exit() */
    sf->ra = (uint32_t)sched_task_exit;

    return (char *)stk_top;
}

void IRAM_ATTR thread_yield_isr(void* arg)
{
    (void)arg;
    /**
     * Context switches are realized using software interrupts since interrupt
     * entry and exit functions are used save and restore complete
     * context.
     */
    /* clear the interrupt first */
    WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, 0);
    /* set the context switch flag (indicates that context has to be switched
     * on exit from interrupt in rtos_int_exit) */
    sched_context_switch_request = 1;
}

void IRAM_ATTR thread_yield_higher(void)
{
    /* reset hardware watchdog */
    system_wdt_feed();

    /**
     * If we are already in an interrupt handler, the function simply sets the
     * context switch flag, which indicates that the context has to be switched
     * in the rtos_int_exit function when exiting the interrupt. Otherwise, we
     * will generate a software interrupt to force the context switch when
     * exiting from the software interrupt (see thread_yield_isr).
     */
    if (irq_is_in()) {
        /* if already in ISR, only set the sched_context_switch_request flag */
        sched_context_switch_request = 1;
    }
    else {
        /* otherwise trigger a software interrupt for context switch */
        WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, SYSTEM_CPU_INTR_FROM_CPU_0);
        /* small delay of 3-4 instructions required here before we return */
        __asm__ volatile ( "nop" );
        __asm__ volatile ( "nop" );
        __asm__ volatile ( "nop" );
        __asm__ volatile ( "nop" );
        __asm__ volatile ( "nop" );
    }
}

NORETURN void cpu_switch_context_exit(void)
{
    /* enable interrupts */
    irq_enable();

    /* force a context switch to another thread */
    thread_yield_higher();
    UNREACHABLE();
}
