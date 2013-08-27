#include <msp430x16x.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>
#include <cpu.h>

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void TA0_unset(short timer);
extern uint16_t overflow_interrupt[ARCH_MAXTIMERS+1];
extern uint16_t timer_round;

void timerA_init(void)
{
    volatile unsigned int *ccr;
    volatile unsigned int *ctl;
    ticks = 0;                               // Set tick counter value to 0
    timer_round = 0;                         // Set to round 0
    TA0CTL = TASSEL_1 + TACLR;               // Clear the timer counter, set ACLK
    TA0CTL &= ~TAIFG;                        // Clear the IFG
    TA0CTL &= ~TAIE;                         // Clear the IFG

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        ccr = &TA0CCR0 + (i);
        ctl = &TA0CCTL0 + (i);
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);
    }

    TA0CTL |= MC_2;
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

    short taiv = TA0IV;

    if (taiv & TAIFG) {
    } else {

        short timer = (taiv/2);
        if(overflow_interrupt[timer] == timer_round)
        {
            TA0_unset(timer);
            int_handler(timer);
        }
    }
    __exit_isr();
}
