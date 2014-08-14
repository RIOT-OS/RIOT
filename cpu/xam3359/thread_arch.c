/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xam3359
 * @{
 *
 * @file        thread_arch.c
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Max Dieckmann <dieckmann@zedat.fu-berlin.de>
 * @author      Martin Kessel <martin.kessel@fu-berlin.de> *
 * @}
 */

#include "arch/thread_arch.h"
#include "sched.h"
#include "irq.h"
#include "cpu.h"
#include "kernel_internal.h"



/**
 * @name noticeable marker marking the beginning of a stack segment
 *
 * This marker is used e.g. by *thread_arch_start_threading* to identify the stacks start.
 */
#define STACK_MARKER                (0x77777777)



static void context_save(void);
static void context_restore(void);
static void enter_thread_mode(void);
 
/**
 *
 * Layout of the stack:
 * ----------------------------------------
 * | SP | R0 | ... | R12 | LR | CPSR | PC |
 * ----------------------------------------
 *
 */

typedef int stack_item; 
typedef int mem_address; 

char *thread_arch_stack_init(void *(*task_func)(void *), void *arg, void *stack_start, int stack_size)
{
	int i;
    unsigned int *stk;
    stk = (unsigned int *) (stack_start + stack_size); 

    /* marker */
    stk--;
    *stk = STACK_MARKER;


    /* pointer to task function */
    stk--;
    *stk = (unsigned int) task_func; 

    
    /* store a value of cpsr, interrupts 
       enabled, supervisor mode */
    stk--; 
    *stk = (unsigned int) 0x00000013;

    for (i = 0; i < 14; i++)
    {
        stk--; 
        *stk = 0; 
    }

    return (char*) stk;
  }

void thread_arch_stack_print(void)
{
    /* TODO */
}

void thread_arch_start_threading(void)
{
    sched_run();
    enableIRQ();
    enter_thread_mode();
}


/**
 * @brief Set the MCU into Thread-Mode and load the initial task from the stack and run it
 */
void enter_thread_mode(void)
{
    /* r0 = &sched_active_thread */
    asm("ldr    r0, =sched_active_thread" );      
    /* r0 = *r0 = sched_active_thread */   
    asm("ldr    r0, [r0]"           );
    /* restore stack pointer */ 
    asm("ldr sp,[r0]");
    /* restore registers from stack */
    asm("ldmfd sp!, {r0-r12, r14}");
    /* save r0 on stack */
    asm("stmfd sp!, {r0}");
    /* load r0 with cpsr for task */
    asm("ldr r0, [sp, #4]");
    /* store r0 value in spsr */
    asm("msr spsr, r0");
    /* restore r0 from stack */
    asm("ldmfd sp!, {r0}");
    /* make sp refer to start address of task */
    asm("add sp, sp, #4");
    /* restore pc */
    asm("ldmfd sp!, {pc}");
}

void thread_arch_yield(void)
{
    context_save();
    sched_run();
    context_restore();
}


__attribute__((always_inline)) void context_save(void)
{
    /* save link register */
    asm("stmfd sp!, {lr}");
    /* save r0 on a free place on the stack */
    asm("str r0, [sp, #-8]");
    /* store spsr value in r0 */
    asm("mrs r0, spsr");
    /* push this on the stack */
    asm("stmfd sp!, {r0}");
    /* restore value of r0 */
    asm("ldr r0, [sp, #-4]");
    /* save registers */
    asm("stmfd sp!, {r0-r12, r14}");    
    /* r0 = &sched_active_thread */
    asm("ldr    r0, =sched_active_thread" );      
    /* r0 = *r0 = sched_active_thread */   
    asm("ldr    r0, [r0]"           );
    /* save sp */
    asm("str	sp, [r0]");
}

__attribute__((always_inline)) void context_restore(void)
{
    /* r0 = &sched_active_thread */
    asm("ldr    r0, =sched_active_thread" );      
    /* r0 = *r0 = sched_active_thread */   
    asm("ldr    r0, [r0]"           );
    /* restore stack pointer */ 
    asm("ldr sp,[r0]");
    /* restore registers from stack */
    asm("ldmfd sp!, {r0-r12, r14}");
    /* push r0 */
    asm("stmfd sp!, {r0}");
    /* save cpsr in r0 */
    asm("ldr r0, [sp, #4]");
    /* store that in spsr */
    asm("msr spsr, r0");
    /* pop r0 */
    asm("ldmfd sp!, {r0}");
    /* make sp refer to start address of task */
    asm("add sp, sp, #4");

    /* restore pc */
    asm("ldmfd sp!, {pc}");
}

/*
** @brief a prototype for a context save when coming from an interrupt
**		
**	Since registers are already being saved during the interrupt, things differ
**	from the context save above. If a context switch occours during an 
**	interrupt, the reference to the saved registers has to be passed to the 
**	task to be suspended. This address has to be saved by the interrupt handler.
**	In order to complete the context switch, we can now invoke the context 
**	restore as before (after the scheduler decided which task to restore).
*/

//void context_save_int(void)
//{
//	/* r0 = &sched_active_thread */
//    asm("ldr    r0, =sched_active_thread" );      
//    /* r0 = *r0 = sched_active_thread */   
//    asm("ldr    r0, [r0]"           );
//    /* save link register */
//    asm("stmfd sp!, {lr}");
//    /* save r3 */
//    asm("stmfd sp!, {r3}");
//    /* save r0 and r1 */
//    asm("stmfd sp!, {r0,r1}");
//    /* get stack pointer saved by int handler */
//    asm("bl int_status_get_saved_stack_pointer");
//    /* store stack pointer in r3*/
//    asm("mov r3, r0");
//    /* restore r0, r1 */
//    asm("ldmfd sp!, {r0, r1}");    
//    /* copy r3 to address of the task to be suspended */
//    asm("str r3, [r0]");      
//   /* restore r3 */   
//    asm("ldmfd sp!, {r3}");
//    /* clear interrupt active flag */
//    asm("bl int_status_clear_interrupt_active");
//	/* restore lr */
//	asm("ldmfd sp!, {lr}");
//}
