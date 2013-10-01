#include <msp430x16x.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>
#include <cpu.h>

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void timer_unset(short timer);
extern uint16_t overflow_interrupt[ARCH_MAXTIMERS+1];
extern uint16_t timer_round;

void timerA_init(void)
{
    volatile unsigned int *ccr;
    volatile unsigned int *ctl;
    ticks = 0;                               // Set tick counter value to 0
    timer_round = 0;                         // Set to round 0
    TACTL = TASSEL_1 + TACLR;                // Clear the timer counter, set ACLK
    TACTL &= ~TAIFG;                         // Clear the IFG
    TACTL &= ~TAIE;                          // Clear the IFG

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        ccr = &TACCR0 + (i);
        ctl = &TACCTL0 + (i);
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);
    }

    TACTL |= MC_2;
}

interrupt(TIMERA0_VECTOR) __attribute__((naked)) timer_isr_ccr0(void)
{
    __enter_isr();
    timer_round += 1;
    __exit_isr();

}

interrupt(TIMERA1_VECTOR) __attribute__((naked)) timer_isr(void)
{
    __enter_isr();

    short taiv = TAIV;

    if (taiv & TAIV_TAIFG) {
    } else {

        short timer = (taiv/2);
        if(overflow_interrupt[timer] == timer_round)
        {
            timer_unset(timer);
            int_handler(timer);
        }
    }
    __exit_isr();
}
