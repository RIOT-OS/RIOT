/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "log.h"
#include "thread.h"
#include "sched.h"

#include "esp_common.h"
#include "irq_arch.h"
#include "syscalls.h"
#include "tools.h"

#include "esp_attr.h"
#include "rom/ets_sys.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void thread_stack_print(void)
{
    /* Print the current stack to stdout. */

#if defined(DEVELHELP)
    thread_t* task = thread_get_active();
    if (task) {
        char* stack_top = task->stack_start + task->stack_size;
        int   size = stack_top - task->sp;
        printf("Printing current stack of thread %" PRIkernel_pid "\n", thread_getpid());
        esp_hexdump((void*)(task->sp), size >> 2, 'w', 8);
    }
#else
    NOT_SUPPORTED();
#endif
}

void thread_print_stack(void)
{
    /* Prints human readable, ps-like thread information for debugging purposes. */
    NOT_YET_IMPLEMENTED();
    return;
}

#ifdef DEVELHELP

extern uint8_t port_IntStack;
extern uint8_t port_IntStackTop;

void thread_isr_stack_init(void)
{
    /* code from thread.c, please see the copyright notice there */
#ifndef CPU_ESP8266
    #define sp (&port_IntStackTop)
#else /* !CPU_ESP8266 */
    register uint32_t *sp __asm__ ("a1");
#endif /* !CPU_ESP8266 */

    /* assign each int of the stack the value of it's address. We can safely
     * cast, as stack is aligned. Use an intermediate cast to uintptr_t to
     * silence -Wcast-align false positive */
    uintptr_t *stackmax = (uintptr_t *)(uintptr_t)sp;
    uintptr_t *stackp = (uintptr_t *)(uintptr_t)&port_IntStack;

    /* cppcheck-suppress comparePointers */
    while (stackp < stackmax) {
        *stackp = (uintptr_t) stackp;
        stackp++;
    }
}

int thread_isr_stack_usage(void)
{
    size_t stack_size = (uintptr_t)&port_IntStackTop - (uintptr_t)&port_IntStack;
    return stack_size -
           measure_stack_free_internal((char *)&port_IntStack, stack_size);
}

void *thread_isr_stack_pointer(void)
{
    /* Get the current ISR stack pointer. */
    return &port_IntStackTop;
}

void *thread_isr_stack_start(void)
{
    /* Get the start of the ISR stack. */
    return &port_IntStack;
}

void thread_isr_stack_print(void)
{
    printf("Printing current ISR stack\n");
    /* cppcheck-suppress comparePointers
     * (reason: comes from ESP-SDK, so should be valid) */
    esp_hexdump(&port_IntStack, &port_IntStackTop-&port_IntStack, 'w', 8);
}

#else /* DEVELHELP */

void thread_isr_stack_init(void) {}

#endif /* DEVELHELP */
