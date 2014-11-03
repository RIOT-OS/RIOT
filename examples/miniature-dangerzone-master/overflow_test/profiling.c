#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <cpu.h>
#include <ltc4150.h>
#include "tcb.h"
#include "sched.h"
#include "io.h"
#include "board.h"

uint8_t profiling = 0;

void __attribute__((__no_instrument_function__))  profiling_init(void)
{
    profiling = 1;
}

void __attribute__((__no_instrument_function__))  __cyg_profile_func_enter(void *func,  void *caller)
{
    (void) caller;

    if (!profiling) {
        return;
    }

    profiling = 0;
    register unsigned long *sp;
    char tmp[100];

    extern uintptr_t __stack_irq_start;
    __asm__ __volatile__("mov %0, sp" : "=r"(sp));              // copy sp
    if (inISR()) {
        sprintf(tmp, "ISR: %s %p enter: Stack usage %lu\n", active_thread->name, func, (((uint32_t) &__stack_irq_start) - (uint32_t) sp));
    }
    else {
        if ((uint32_t)active_thread->stack_start > (uint32_t) sp) {
            sprintf(tmp, "OVERFLOW! Stackpointer is %u bytes above stack start address\n", ((uint32_t)active_thread->stack_start - (uint32_t) sp));
            fw_puts(tmp, strlen(tmp));
            arm_reset();
        }
        sprintf(tmp, "%s %p enter: Stack usage %" PRIu32 "\n", active_thread->name, func, (((uint32_t)active_thread->stack_start + (uint32_t) active_thread->stack_size) - (uint32_t) sp));
    }
    profiling = 1;
    fw_puts(tmp, strlen(tmp));
    return;
}

void __attribute__((__no_instrument_function__)) __cyg_profile_func_exit(void *func, void *caller)
{
    (void) caller;

    if (!profiling) {
        return;
    }

    profiling = 0;
    extern uintptr_t __stack_irq_start;
    register unsigned long *sp;
    char tmp[100];
    __asm__ __volatile__("mov %0, sp" : "=r"(sp));              // copy sp
    if (inISR()) {
        sprintf(tmp, "ISR: %s %p exit: Stack usage %lu\n", active_thread->name, func, (((uint32_t) &__stack_irq_start) - (uint32_t) sp));
    }
    else {
        if ((uint32_t)active_thread->stack_start > (uint32_t) sp) {
            sprintf(tmp, "OVERFLOW! Stackpointer is %u bytes above stack start address\n", ((uint32_t)active_thread->stack_start - (uint32_t) sp));
            fw_puts(tmp, strlen(tmp));
            arm_reset();
        }
        sprintf(tmp, "%s %p exit: Stack usage %" PRIu32 "\n", active_thread->name, func, (((uint32_t)active_thread->stack_start + (uint32_t) active_thread->stack_size) - (uint32_t) sp));
    }
    profiling = 1;
    fw_puts(tmp, strlen(tmp));
    return;
}
