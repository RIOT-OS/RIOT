/**
 * ARM architecture common support functions
 *
 * Copyright (C) 2008, 2009  Heiko Will <hwill@inf.fu-berlin.de>
 * Copyright (C) 2009  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @author Heiko Will <heiko.will@fu-berlin.de>
 * @}
 */
/**
 * 
 *
 *
 */

#include <stdio.h>
#include "arm_cpu.h"
#include "sched.h"
#include "kernel_intern.h"

void thread_yield() {
    asm("svc 0\n");
}

//----------------------------------------------------------------------------
// Processor specific routine - here for ARM7
// sizeof(void*) = sizeof(int)
//----------------------------------------------------------------------------
char * thread_stack_init(void * task_func, void * stack_start)
{
   unsigned int * stk;
   stk = (unsigned int *) stack_start;
    stk--;

    *stk = 0x77777777;
    stk--;

    *stk = (unsigned int)sched_task_exit;       // LR

   stk--;
   *stk = (unsigned int) stack_start - 4;   // SP

   for (int i = 12; i>= 0 ; i--) {          // build base stack
       stk--;
       *stk = i;
   }

   stk--;
   *stk = ((unsigned int) task_func);       // Entry Point
   stk--;
   *stk = (unsigned int) NEW_TASK_CPSR;     // spsr

   return (char*)stk;
}

void thread_print_stack () {
    register void * stack = 0;
    asm( "mov %0, sp" : "=r" (stack));

    register unsigned int * s = (unsigned int*) stack;
    printf("task: %X SP: %X\n", (unsigned int)active_thread, (unsigned int)stack);
    register int i = 0;
    s += 5;
    while (*s != 0x77777777) {
        printf("STACK (%u) addr=%X = %X \n",i,(unsigned int) s, (unsigned int)*s);
        s++;
        i++;
    }
    printf("STACK (%u)= %X \n",i,*s);
}

__attribute__((naked,noreturn)) void
arm_reset(void)
{
    dINT();
    WDTC = 0x00FFF;
    WDMOD = 0x03;
    WDFEED= 0xAA;
    WDFEED= 0x55;
    while(1);
}
