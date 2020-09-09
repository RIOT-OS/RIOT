/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread
 *              interface
 *
 * Members of the Cortex-M family know stacks and are able to handle register
 * backups partly, so we make use of that.
 *
 * Cortex-M3 and Cortex-M4 use the
 * following register layout when saving their context onto the stack:
 *
 * -------- highest address (bottom of stack)
 * | xPSR |
 * --------
 * | PC   |
 * --------
 * | LR   |
 * --------
 * | R12  |
 * --------
 * | R3   |
 * --------
 * | R2   |
 * --------
 * | R1   |
 * --------
 * | R0   | <- the registers from xPSR to R0 are handled by hardware
 * --------
 * | R11  |
 * --------
 * | R10  |
 * --------
 * | R9   |
 * --------
 * | R8   |
 * --------
 * | R7   |
 * --------
 * | R6   |
 * --------
 * | R5   |
 * --------
 * | R4   |
 * --------
 * | RET  | <- exception return code
 * -------- lowest address (top of stack)
 *
 * For the Cortex-M0 and Cortex-M0plus we use a slightly different layout by
 * switching the blocks R11-R8 and R7-R4. This allows more efficient code when
 * saving/restoring the context:
 *
 * ------------- highest address (bottom of stack)
 * | xPSR - R0 | <- same as for Cortex-M3/4
 * -------------
 * | R7   |
 * --------
 * | R6   |
 * --------
 * | R5   |
 * --------
 * | R4   |
 * --------
 * | R11  |
 * --------
 * | R10  |
 * --------
 * | R9   |
 * --------
 * | R8   |
 * --------
 * | RET  | <- exception return code
 * -------- lowest address (top of stack)
 *
 * TODO: Implement handling of FPU registers for Cortex-M4 CPUs
 *
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>

#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern uint32_t _estack;
extern uint32_t _sstack;

/**
 * @brief   Noticeable marker marking the beginning of a stack segment
 *
 * This marker is used e.g. by *cpu_switch_context_exit* to identify the
 * stacks beginning.
 */
#define STACK_MARKER                (0x77777777)

/**
 * @brief   Initial program status register value for a newly created thread
 *
 * In the initial state, only the Thumb mode-bit is set
 */
#define INITIAL_XPSR                (0x01000000)

/**
 * @brief   ARM Cortex-M specific exception return value, that triggers the
 *          return to the task mode stack pointer
 */
#define EXCEPT_RET_TASK_MODE        (0xfffffffd)

char *thread_stack_init(thread_task_func_t task_func,
                             void *arg,
                             void *stack_start,
                             int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* adjust to 32 bit boundary by clearing the last two bits in the address */
    stk = (uint32_t *)(((uint32_t)stk) & ~((uint32_t)0x3));

    /* stack start marker */
    stk--;
    *stk = STACK_MARKER;

    /* make sure the stack is double word aligned (8 bytes) */
    /* This is required in order to conform with Procedure Call Standard for the
     * ARM® Architecture (AAPCS) */
    /* http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf */
    if (((uint32_t) stk & 0x7) != 0) {
        /* add a single word padding */
        --stk;
        *stk = ~((uint32_t)STACK_MARKER);
    }

    /* ****************************** */
    /* Automatically popped registers */
    /* ****************************** */

    /* The following eight stacked registers are popped by the hardware upon
     * return from exception. (bx instruction in select_and_restore_context) */

    /* xPSR - initial status register */
    stk--;
    *stk = (uint32_t)INITIAL_XPSR;
    /* pc - initial program counter value := thread entry function */
    stk--;
    *stk = (uint32_t)task_func;
    /* lr - contains the return address when the thread exits */
    stk--;
    *stk = (uint32_t)sched_task_exit;
    /* r12 */
    stk--;
    *stk = 0;
    /* r3 - r1 */
    for (int i = 3; i >= 1; i--) {
        stk--;
        *stk = i;
    }
    /* r0 - contains the thread function parameter */
    stk--;
    *stk = (uint32_t)arg;

    /* ************************* */
    /* Manually popped registers */
    /* ************************* */

    /* The following registers are not handled by hardware in return from
     * exception, but manually by select_and_restore_context.
     * For the Cortex-M0(plus) we write registers R11-R4 in two groups to allow
     * for more efficient context save/restore code.
     * For the Cortex-M3 and Cortex-M4 we write them continuously onto the stack
     * as they can be read/written continuously by stack instructions. */

#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
    /* start with r7 - r4 */
    for (int i = 7; i >= 4; i--) {
        stk--;
        *stk = i;
    }
    /* and put r11 - r8 on top of them */
    for (int i = 11; i >= 8; i--) {
        stk--;
        *stk = i;
    }
#else
    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }
#endif

    /* exception return code  - return to task-mode process stack pointer */
    stk--;
    *stk = (uint32_t)EXCEPT_RET_TASK_MODE;

    /* The returned stack pointer will be aligned on a 32 bit boundary not on a
     * 64 bit boundary because of the odd number of registers above (8+9).
     * This is not a problem since the initial stack pointer upon process entry
     * _will_ be 64 bit aligned (because of the cleared bit 9 in the stacked
     * xPSR and aligned stacking of the hardware-handled registers). */

    return (char*) stk;
}

void thread_stack_print(void)
{
    int count = 0;
    uint32_t *sp = (uint32_t *)thread_get_active()->sp;

    printf("printing the current stack of thread %" PRIkernel_pid "\n",
           thread_getpid());
    printf("  address:      data:\n");

    do {
        printf("  0x%08x:   0x%08x\n", (unsigned int)sp, (unsigned int)*sp);
        sp++;
        count++;
    } while (*sp != STACK_MARKER);

    printf("current stack size: %i byte\n", count);
}

/* This function returns the number of bytes used on the ISR stack */
int thread_isr_stack_usage(void)
{
    uint32_t *ptr = &_sstack;

    while(((*ptr) == STACK_CANARY_WORD) && (ptr < &_estack)) {
        ++ptr;
    }

    ptrdiff_t num_used_words = &_estack - ptr;
    return num_used_words * sizeof(*ptr);
}

void *thread_isr_stack_pointer(void)
{
    void *msp = (void *)__get_MSP();
    return msp;
}

void *thread_isr_stack_start(void)
{
    return (void *)&_sstack;
}

void NORETURN cpu_switch_context_exit(void)
{
    /* enable IRQs to make sure the SVC interrupt is reachable */
    irq_enable();
    /* trigger the SVC interrupt */
    __asm__ volatile (
        "svc    #1                            \n"
        : /* no outputs */
        : /* no inputs */
        : /* no clobbers */
    );

    UNREACHABLE();
}

void thread_yield_higher(void)
{
    /* trigger the PENDSV interrupt to run scheduler and schedule new thread if
     * applicable */
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void __attribute__((naked)) __attribute__((used)) isr_pendsv(void) {
    __asm__ volatile (
    /* PendSV handler entry point */
    /* save context by pushing unsaved registers to the stack */
    /* {r0-r3,r12,LR,PC,xPSR,s0-s15,FPSCR} are saved automatically on exception entry */
    ".thumb_func                      \n"

    /* skip context saving if sched_active_thread == NULL */
    "ldr    r1, =sched_active_thread  \n" /* r1 = &sched_active_thread  */
    "ldr    r1, [r1]                  \n" /* r1 = sched_active_thread   */
    "cmp    r1, #0                    \n" /* if r1 == NULL:             */
    "beq select_and_restore_context   \n" /*   goto select_and_restore_context */

    "mrs    r0, psp                   \n" /* get stack pointer from user mode */
#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
    "push   {r1}                      \n" /* push sched_active_thread */
    "mov    r12, sp                   \n" /* remember the exception SP */
    "mov    sp, r0                    \n" /* set user mode SP as active SP */
    /* we can not push high registers directly, so we move R11-R8 into
     * R4-R0, as these are already saved */
    "mov    r0, r8                    \n" /* move R11-R8 into R3-R0 */
    "mov    r1, r9                    \n"
    "mov    r2, r10                   \n"
    "mov    r3, r11                   \n"
    "push   {r0-r7}                   \n" /* now push them onto the stack */
    "mov    r0, lr                    \n" /* next we save the link register */
    "push   {r0}                      \n"
    "mov    r0, sp                    \n" /* switch back to the exception SP */
    "mov    sp, r12                   \n"
    "pop    {r1}                      \n" /* r1 = sched_active_thread */
#else
#ifdef MODULE_CORTEXM_FPU
    "tst    lr, #0x10                 \n"
    "it     eq                        \n"
    "vstmdbeq r0!, {s16-s31}          \n" /* save FPU registers if FPU is used */
#endif
    "stmdb  r0!,{r4-r11}              \n" /* save regs */
    "stmdb  r0!,{lr}                  \n" /* exception return value */
#endif
    "str    r0, [r1]                  \n" /* write r0 to thread->sp */

    /* current thread context is now saved */

    "select_and_restore_context:      \n"

    "bl     sched_run                 \n" /* perform scheduling */

    /* restore now current thread context */
#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
    "mov    lr, sp                    \n" /* save MSR stack pointer for later */
    "ldr    r0, =sched_active_thread  \n" /* load address of current TCB */
    "ldr    r0, [r0]                  \n" /* dereference TCB */
    "ldr    r0, [r0]                  \n" /* load tcb-sp to R0 */
    "mov    sp, r0                    \n" /* make user mode SP active SP */
    "pop    {r0}                      \n" /* restore LR from stack */
    "mov    r12, r0                   \n" /* remember LR by parking it in R12 */
    "pop    {r0-r7}                   \n" /* get R11-R8 and R7-R4 from stack */
    "mov    r8, r0                    \n" /* move R11-R8 to correct registers */
    "mov    r9, r1                    \n"
    "mov    r10, r2                   \n"
    "mov    r11, r3                   \n"
    /* restore the application mode stack pointer PSP */
    "mov    r0, sp                    \n" /* restore the user mode SP */
    "msr    psp, r0                   \n" /* for this write it to the PSP reg */
    "mov    sp, lr                    \n" /* and get the parked MSR SP back */
    /* return from exception mode to application mode */
    "bx     r12                       \n" /* return from exception mode */
#else
    "ldr    r0, =sched_active_thread  \n" /* load address of current TCB */
    "ldr    r0, [r0]                  \n" /* dereference TCB */
    "ldr    r1, [r0]                  \n" /* load tcb->sp to register 1 */
    "ldmia  r1!, {r0}                 \n" /* restore exception return value */
    "ldmia  r1!, {r4-r11}             \n" /* restore other registers */
#ifdef MODULE_CORTEXM_FPU
    "tst    r0, #0x10                 \n"
    "it     eq                        \n"
    "vldmiaeq r1!, {s16-s31}          \n" /* load FPU registers if saved */
#endif
    "msr    psp, r1                   \n" /* restore user mode SP to PSP reg */
    "bx     r0                        \n" /* load exception return value to PC,
                                           * causes end of exception*/
#endif
    /* {r0-r3,r12,LR,PC,xPSR,s0-s15,FPSCR} are restored automatically on exception return */
     ".ltorg                           \n" /* literal pool needed to access
                                            * sched_active_thread */
    );
}

#ifdef MODULE_CORTEXM_SVC
void __attribute__((naked)) __attribute__((used)) isr_svc(void)
{
    /* these two variants do exactly the same, but Cortex-M3 can use Thumb2
     * conditional execution, which are a bit faster. */

    /* TODO: currently, cpu_switch_context_exit() is used to start threading
     * from kernel_init(), which executes on MSP.  That could probably be
     * rewritten to not use the supervisor call at all. Then we can assume that
     * svc is only used by threads, saving a couple of instructions. /Kaspar
     */

#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
    || defined(CPU_CORE_CORTEX_M23)
    __asm__ volatile (
    ".thumb_func            \n"
    "movs   r0, #4          \n" /* if bit4(lr) == 1):       */
    "mov    r1, lr          \n"
    "tst    r0, r1          \n"
    "beq    came_from_msp   \n" /*     goto came_from_msp   */
    "mrs    r0, psp         \n" /* r0 = psp                 */
    "b      _svc_dispatch   \n" /* return svc_dispatch(r0)  */
    "came_from_msp:         \n"
    "mrs    r0, msp         \n" /* r0 = msp                 */
    "b      _svc_dispatch   \n" /* return svc_dispatch(r0)  */
    );
#else
    __asm__ volatile (
    ".thumb_func            \n"
    "tst    lr, #4          \n" /* switch bit4(lr) == 1):   */
    "ite    eq              \n"
    "mrseq  r0, msp         \n" /* case 1: r0 = msp         */
    "mrsne  r0, psp         \n" /* case 0: r0 = psp         */
    "b      _svc_dispatch   \n" /* return svc_dispatch()    */
    );
#endif
}

static void __attribute__((used)) _svc_dispatch(unsigned int *svc_args)
{
    /* stack frame:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * - r0   = svc_args[0]
     * - r1   = svc_args[1]
     * - r2   = svc_args[2]
     * - r3   = svc_args[3]
     * - r12  = svc_args[4]
     * - lr   = svc_args[5]
     * - pc   = svc_args[6]
     * - xPSR = svc_args[7]
     */

    /* svc_args[6] is the stacked PC / return address. It is the address of the
     * instruction after the SVC.  The SVC instruction is located in the memory
     * address [stacked_PC - 2], because SVC is a 2 byte instruction.  The SVC
     * number is the lower byte of the instruction.
     */
    unsigned int svc_number = ((char *)svc_args[6])[-2];

    switch (svc_number) {
        case 1: /* SVC number used by cpu_switch_context_exit */
            SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            break;
        default:
            DEBUG("svc: unhandled SVC #%u\n", svc_number);
            break;
    }
}

#else /* MODULE_CORTEXM_SVC */
void __attribute__((used)) isr_svc(void)
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
#endif /* MODULE_CORTEXM_SVC */

void sched_arch_idle(void)
{
    /* by default, PendSV has the same priority as other ISRs.
     * In this function, we temporarily lower the priority (set higher value),
     * allowing other ISRs to interrupt.
     *
     * According to [this](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHJICIE.html),
     * dynamically changing the priority is not supported on CortexM0(+).
     */
    unsigned state = irq_disable();
    NVIC_SetPriority(PendSV_IRQn, CPU_CORTEXM_PENDSV_IRQ_PRIO + 1);
    __DSB();
    __ISB();
#ifdef MODULE_PM_LAYERED
    void pm_set_lowest(void);
    pm_set_lowest();
#else
    __WFI();
#endif
    irq_restore(state);
    NVIC_SetPriority(PendSV_IRQn, CPU_CORTEXM_PENDSV_IRQ_PRIO);
    SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk;
}
