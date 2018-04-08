/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "sched.h"
#include "irq.h"
#include "cpu.h"
#include "board.h"
#include "periph_cpu_pins.h"


/**
 * @brief Boards must define one: AVR_CONTEXT_SWAP_PIN or AVR_CONTEXT_SWAP_CNTR
 * AVR_CONTEXT_SWAP_PIN is used to trigger a PCINT which switches the context
 * it may be defined as any unused pin that has a PCINT.
 * AVR_CONTEXT_SWAP_CNTR is used to trigger an OVF which switches the context
 * it may be defined as the number of an unused timer/counter.
 */
#if !(defined AVR_CONTEXT_SWAP_PIN) && !(defined AVR_CONTEXT_SWAP_CNTR)
#error AVR_CONTEXT_SWAP_PIN or AVR_CONTEXT_SWAP_CNTR must be defined in board.h
#endif
#if defined AVR_CONTEXT_SWAP_PIN && defined AVR_CONTEXT_SWAP_CNTR
#error AVR_CONTEXT_SWAP_PIN and AVR_CONTEXT_SWAP_CNTR are conflicting defines
#endif

/**
 * @brief If using pin context swaps, AVR_CONTEXT_SWAP_INTERRUPT_VECT must also
 * be defined.
 */
#if defined AVR_CONTEXT_SWAP_PIN && !(defined AVR_CONTEXT_SWAP_INTERRUPT_VECT)
#error AVR_CONTEXT_SWAP_INTERRUPT_VECT must be defined with AVR_CONTEXT_SWAP_PIN
#endif

/**
 * @brief Defines for AVR_CONTEXT_SWAP_INTERRUPT_VECT when using a counter.
 */
#ifdef AVR_CONTEXT_SWAP_CNTR
#if AVR_CONTEXT_SWAP_CNTR == 0
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT TIMER0_OVF_vect
#elif AVR_CONTEXT_SWAP_CNTR == 1
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT TIMER1_OVF_vect
#elif AVR_CONTEXT_SWAP_CNTR == 2
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT TIMER2_OVF_vect
#elif AVR_CONTEXT_SWAP_CNTR == 3
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT TIMER3_OVF_vect
#elif AVR_CONTEXT_SWAP_CNTR == 4
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT TIMER4_OVF_vect
#elif AVR_CONTEXT_SWAP_CNTR == 5
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT TIMER5_OVF_vect
#endif /* AVR_CONTEXT_SWAP_CNTR == # */
#endif /* ifdef AVR_CONTEXT_SWAP_CNTR */


/*
 * local function declarations  (prefixed with __)
 */
static void __context_save(void);
static void __context_restore(void);
static void __enter_thread_mode(void);

/**
 * @brief Since AVR doesn't support direct manipulation of the program counter we
 * model a stack like it would be left by __context_save().
 * The resulting layout in memory is the following:
 * ---------------thread_t (not created by thread_stack_init) ----------
 * local variables (a temporary value and the stackpointer)
 * -----------------------------------------------------------------------
 * a marker (AFFE) - for debugging purposes (helps finding the stack
 * -----------------------------------------------------------------------
 * a 16 Bit pointer to sched_task_exit
 * (Optional 17 bit (bit is set to zero) for devices with > 128kb FLASH)
 * -----------------------------------------------------------------------
 * a 16 Bit pointer to task_func
 * this is placed exactly at the place where the program counter would be
 * stored normally and thus can be returned to when __context_restore()
 * has been run
 * (Optional 17 bit (bit is set to zero) for devices with > 128kb FLASH)
 * -----------------------------------------------------------------------
 * saved registers from context:
 * r0
 * status register
 * (Optional EIND and RAMPZ registers)
 * r1 - r23
 * pointer to arg in r24 and r25
 * r26 - r31
 * -----------------------------------------------------------------------
 *
 * After the invocation of __context_restore() the pointer to task_func is
 * on top of the stack and can be returned to. This way we can actually place
 * it inside of the programm counter of the MCU.
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
    *stk = (uint8_t) 0xAF;
    stk--;
    *stk = (uint8_t) 0xFE;

    /* save sched_task_exit */
    stk--;
    tmp_adress = (uint16_t) sched_task_exit;
    *stk = (uint8_t)(tmp_adress & (uint16_t) 0x00ff);
    stk--;
    tmp_adress >>= 8;
    *stk = (uint8_t)(tmp_adress & (uint16_t) 0x00ff);

#if FLASHEND > 0x1ffff
    /* Devices with more than 128kb FLASH use a 17 bit PC, we set whole the top byte forcibly to 0 */
    stk--;
    *stk = (uint8_t) 0x00;
#endif

    /* save address to task_func in place of the program counter */
    stk--;
    tmp_adress = (uint16_t) task_func;
    *stk = (uint8_t)(tmp_adress & (uint16_t) 0x00ff);
    stk--;
    tmp_adress >>= 8;
    *stk = (uint8_t)(tmp_adress & (uint16_t) 0x00ff);

#if FLASHEND > 0x1ffff
    /* Devices with more than 128kb FLASH use a 17 bit PC, we set whole the top byte forcibly to 0 */
    stk--;
    *stk = (uint8_t) 0x00;
#endif


    /* r0 */
    stk--;
    *stk = (uint8_t) 0x00;

    /* status register (with interrupts enabled) */
    stk--;
    *stk = (uint8_t) 0x80;

#if defined(EIND)
    stk--;
    *stk = (uint8_t) 0x00;
#endif
#if defined(RAMPZ)
    stk--;
    *stk = (uint8_t) 0x00;
#endif

    /* r1 - has always to be 0 */
    stk--;
    *stk = (uint8_t) 0x00;
    /*
     * Space for registers r2 -r23
     *
     * use loop for better readability, the compiler unrolls anyways
     */

    int i;

    for (i = 2; i <= 23 ; i++) {
        stk--;
        *stk = (uint8_t) 0;
    }

    /*
     * In accordance with the AVR calling conventions *arg has to be inside
     * r24 and r25
     * */
    stk--;
    tmp_adress = (uint16_t) arg;
    *stk = (uint8_t)(tmp_adress & (uint16_t) 0x00ff);
    stk--;
    tmp_adress >>= 8;
    *stk = (uint8_t)(tmp_adress & (uint16_t) 0x00ff);

    /*
     * Space for registers r26-r31
     */
    for (i = 26; i <= 31; i++) {
        stk--;
        *stk = (uint8_t) i;
    }

    stk--;
    return (char *) stk;
}

/**
 * @brief thread_stack_print prints the stack to stdout.
 * It depends on getting the correct values for stack_start, stack_size and sp
 * from sched_active_thread.
 * Maybe it would be good to change that to way that is less dependant on
 * getting correct values elsewhere (since it is a debugging tool and in the
 * presence of bugs the data may be corrupted).
 */
void thread_stack_print(void)
{
    uint8_t  found_marker = 1;
    uint8_t *sp = (uint8_t *)sched_active_thread->sp;
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
    }
    while (found_marker == 1);

    printf("stack size: %u bytes\n", size);
}

void cpu_switch_context_exit(void) __attribute__((naked));
void cpu_switch_context_exit(void)
{
    sched_run();

/* Context swap pin option */
#ifdef AVR_CONTEXT_SWAP_PIN
    /* Set pin mode low (ready to trigger */
    _SFR_MEM8(_port_addr(AVR_CONTEXT_SWAP_PIN)) &=
        ~(1 << _pin_num(AVR_CONTEXT_SWAP_PIN));

    /* Set data direction to output */
    _SFR_MEM8(_ddr_addr(AVR_CONTEXT_SWAP_PIN)) |=
        (1 << _pin_num(AVR_CONTEXT_SWAP_PIN));

    /* Clear interrupt flags on pin */
    PCIFR &= ~(1 << _pcicr_num(AVR_CONTEXT_SWAP_PIN));

    /* Enable interrupts on port */
    PCICR |= (1 << _pcicr_num(AVR_CONTEXT_SWAP_PIN));

    /* Enable interrupt for pin */
    _SFR_MEM8(_pcmsk_addr(AVR_CONTEXT_SWAP_PIN)) |=
        (1 << _pcint_num(AVR_CONTEXT_SWAP_PIN));
#endif /* AVR_CONTEXT_SWAP_PIN */

/* Context swap counter option */
#ifdef AVR_CONTEXT_SWAP_CNTR
    /* Counter based context swap initialization procedure:
     * - Set CTC mode (TCCR0A & TCCR0B)
     *   (so it can never loop around and overflow again without reset)
     * - Set prescaler to no clock source (TCCR0B)
     * - Set max value if not reset (OCR0A)
     * - Enable overflow interrupt (TIMSK0)
     * - Clear overflow flags (TIFR0)
     * - Set counter to just short of overflowing (TCNT0)
     */
#if AVR_CONTEXT_SWAP_CNTR == 0
    TCCR0A = (1 << WGM01);
    TCCR0B = 0;
    OCR0A = 0x80;
    TIMSK0 = (1 << TOIE0);
    TIFR0 = 0;
    TCNT0 = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 1
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    OCR1AH = 0x80;
    OCR1AL = 0;
    TIMSK1 = (1 << TOIE1);
    TIFR1 = 0;
    TCNT1H = 0xFF;
    TCNT1L = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 2
    TCCR2A = (1 << WGM21);
    TCCR2B = 0;
    OCR2A = 0x80;
    TIMSK2 = (1 << TOIE2);
    TIFR2 = 0;
    TCNT2 = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 3
    TCCR3A = 0;
    TCCR3B = (1 << WGM32);
    OCR3AH = 0x80;
    OCR3AL = 0;
    TIMSK3 = (1 << TOIE3);
    TIFR3 = 0;
    TCNT3H = 0xFF;
    TCNT3L = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 4
    TCCR4A = 0;
    TCCR4B = (1 << WGM42);
    OCR4AH = 0x80;
    OCR4AL = 0;
    TIMSK4 = (1 << TOIE4);
    TIFR4 = 0;
    TCNT4H = 0xFF;
    TCNT4L = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 5
    TCCR5A = 0;
    TCCR5B = (1 << WGM52);
    OCR5AH = 0x80;
    OCR5AL = 0;
    TIMSK5 = (1 << TOIE5);
    TIFR5 = 0;
    TCNT5H = 0xFF;
    TCNT5L = 0xFF;
#endif /* AVR_CONTEXT_SWAP_CNTR == # */
#endif /* ifdef AVR_CONTEXT_SWAP_CNTR */

    __enter_thread_mode();
}

/**
 * @brief Set the MCU into Thread-Mode and load the initial task from the stack and run it
 */

void NORETURN __enter_thread_mode(void) __attribute__((naked));
void NORETURN __enter_thread_mode(void)
{
    irq_enable();
    __context_restore();
    __asm__ volatile("ret");

    UNREACHABLE();
}

void thread_yield_higher(void) {
/* Context swap pin option */
#ifdef AVR_CONTEXT_SWAP_PIN
    /* Trigger ISR by setting pin high */
    _SFR_MEM8(_port_addr(AVR_CONTEXT_SWAP_PIN)) |=
        (1 << _pin_num(AVR_CONTEXT_SWAP_PIN));

    /* Wait for ISR to set pin low to prevent race condition */
    while (_port_addr(AVR_CONTEXT_SWAP_PIN) &
        (1 << _pin_num(AVR_CONTEXT_SWAP_PIN)));
#endif /* AVR_CONTEXT_SWAP_PIN */

/* Context swap counter option */
#ifdef AVR_CONTEXT_SWAP_CNTR
    /* Trigger ISR by enabling counter clock */
    /* Then wait for ISR to set counter to prevent race condition */
#if AVR_CONTEXT_SWAP_CNTR == 0
    TCCR0B |= (1 << CS00);
    while (TCNT0 != 0xFF);
#elif AVR_CONTEXT_SWAP_CNTR == 1
    TCCR1B |= (1 << CS10);
    while (TCNT1H != 0xFF);
#elif AVR_CONTEXT_SWAP_CNTR == 2
    TCCR2B |= (1 << CS20);
    while (TCNT2 != 0xFF);
#elif AVR_CONTEXT_SWAP_CNTR == 3
    TCCR3B |= (1 << CS30);
    while (TCNT1H != 0xFF);
#elif AVR_CONTEXT_SWAP_CNTR == 4
    TCCR4B |= (1 << CS40);
    while (TCNT1H != 0xFF);
#elif AVR_CONTEXT_SWAP_CNTR == 5
    TCCR5B |= (1 << CS50);
    while (TCNT1H != 0xFF);
#endif /* AVR_CONTEXT_SWAP_CNTR == # */
#endif /* ifdef AVR_CONTEXT_SWAP_CNTR */
}


/* Use this interrupt to perform all context switches */
ISR(AVR_CONTEXT_SWAP_INTERRUPT_VECT, ISR_NAKED) {
/* Context swap pin option */
#ifdef AVR_CONTEXT_SWAP_PIN
    /* Disable interrupt to prevent second interrupt flag */
    PCICR &= ~(1 << _pcicr_num(AVR_CONTEXT_SWAP_PIN));

    /* Set pin low to signal ISR grab */
    _SFR_MEM8(_port_addr(AVR_CONTEXT_SWAP_PIN)) &=
        ~(1 << _pin_num(AVR_CONTEXT_SWAP_PIN));

    /* Wait for pin low to complete (to prevent second interrupt race) */
    while (~(_port_addr(AVR_CONTEXT_SWAP_PIN)) &
        (1 << _pin_num(AVR_CONTEXT_SWAP_PIN)));

    /* Reenable interrupt */
    PCICR |= (1 << _pcicr_num(AVR_CONTEXT_SWAP_PIN));
#endif /* AVR_CONTEXT_SWAP_PIN */

/* Context swap counter option */
#ifdef AVR_CONTEXT_SWAP_CNTR
    /* Disable counter clock and reset count */
#if AVR_CONTEXT_SWAP_CNTR == 0
    TCCR0B &= ~(1 << CS00);
    TCNT0 = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 1
    TCCR1B &= ~(1 << CS10);
    TCNT1L = 0xFF;
    TCNT1H = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 2
    TCCR2B &= ~(1 << CS20);
    TCNT2 = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 3
    TCCR3B &= ~(1 << CS30);
    TCNT3L = 0xFF;
    TCNT3H = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 4
    TCCR4B &= ~(1 << CS40);
    TCNT4L = 0xFF;
    TCNT4H = 0xFF;
#elif AVR_CONTEXT_SWAP_CNTR == 5
    TCCR5B &= ~(1 << CS50);
    TCNT5L = 0xFF;
    TCNT5H = 0xFF;
#endif /* AVR_CONTEXT_SWAP_CNTR == # */
#endif /* ifdef AVR_CONTEXT_SWAP_CNTR */

    __context_save();
    sched_run();
    __context_restore();
    __asm__ volatile("reti");
}


__attribute__((always_inline)) static inline void __context_save(void)
{
    __asm__ volatile(
        "push r0                             \n\t"
        "in   r0, __SREG__                   \n\t"
        "cli                                 \n\t"
        "push r0                             \n\t"
#if defined(RAMPZ)
        "in     r0, __RAMPZ__                \n\t"
        "push   r0                           \n\t"
#endif
#if defined(EIND)
        "in     r0, 0x3c                     \n\t"
        "push   r0                           \n\t"
#endif
        "push r1                             \n\t"
        "clr  r1                             \n\t"
        "push r2                             \n\t"
        "push r3                             \n\t"
        "push r4                             \n\t"
        "push r5                             \n\t"
        "push r6                             \n\t"
        "push r7                             \n\t"
        "push r8                             \n\t"
        "push r9                             \n\t"
        "push r10                            \n\t"
        "push r11                            \n\t"
        "push r12                            \n\t"
        "push r13                            \n\t"
        "push r14                            \n\t"
        "push r15                            \n\t"
        "push r16                            \n\t"
        "push r17                            \n\t"
        "push r18                            \n\t"
        "push r19                            \n\t"
        "push r20                            \n\t"
        "push r21                            \n\t"
        "push r22                            \n\t"
        "push r23                            \n\t"
        "push r24                            \n\t"
        "push r25                            \n\t"
        "push r26                            \n\t"
        "push r27                            \n\t"
        "push r28                            \n\t"
        "push r29                            \n\t"
        "push r30                            \n\t"
        "push r31                            \n\t"
        "lds  r26, sched_active_thread       \n\t"
        "lds  r27, sched_active_thread + 1   \n\t"
        "in   r0, __SP_L__                   \n\t"
        "st   x+, r0                         \n\t"
        "in   r0, __SP_H__                   \n\t"
        "st   x+, r0                         \n\t"
    );

}

__attribute__((always_inline)) static inline void __context_restore(void)
{
    __asm__ volatile(
        "lds  r26, sched_active_thread       \n\t"
        "lds  r27, sched_active_thread + 1   \n\t"
        "ld   r28, x+                        \n\t"
        "out  __SP_L__, r28                  \n\t"
        "ld   r29, x+                        \n\t"
        "out  __SP_H__, r29                  \n\t"
        "pop  r31                            \n\t"
        "pop  r30                            \n\t"
        "pop  r29                            \n\t"
        "pop  r28                            \n\t"
        "pop  r27                            \n\t"
        "pop  r26                            \n\t"
        "pop  r25                            \n\t"
        "pop  r24                            \n\t"
        "pop  r23                            \n\t"
        "pop  r22                            \n\t"
        "pop  r21                            \n\t"
        "pop  r20                            \n\t"
        "pop  r19                            \n\t"
        "pop  r18                            \n\t"
        "pop  r17                            \n\t"
        "pop  r16                            \n\t"
        "pop  r15                            \n\t"
        "pop  r14                            \n\t"
        "pop  r13                            \n\t"
        "pop  r12                            \n\t"
        "pop  r11                            \n\t"
        "pop  r10                            \n\t"
        "pop  r9                             \n\t"
        "pop  r8                             \n\t"
        "pop  r7                             \n\t"
        "pop  r6                             \n\t"
        "pop  r5                             \n\t"
        "pop  r4                             \n\t"
        "pop  r3                             \n\t"
        "pop  r2                             \n\t"
        "pop  r1                             \n\t"
#if defined(EIND)
        "pop    r0                           \n\t"
        "out    0x3c, r0                     \n\t"
#endif
#if defined(RAMPZ)
        "pop    r0                           \n\t"
        "out    __RAMPZ__, r0                \n\t"
#endif
        "pop  r0                             \n\t"
        "out  __SREG__, r0                   \n\t"
        "pop  r0                             \n\t"
    );
}
