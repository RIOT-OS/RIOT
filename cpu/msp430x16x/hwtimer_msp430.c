#include <msp430x16x.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>
#include <cpu.h>

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void TB_unset(short timer);
extern uint16_t overflow_interrupt[ARCH_MAXTIMERS+1];
extern uint16_t timer_round;

void timerB_init(void)
{
    volatile unsigned int *ccr;
    volatile unsigned int *ctl;
    ticks = 0;                               // Set tick counter value to 0
    timer_round = 0;                         // Set to round 0
    TBCTL = TBSSEL_1 + TBCLR;                // Clear the timer counter, set ACLK
    TBCTL &= ~TBIFG;                         // Clear the IFG
    TBCTL &= ~TBIE;                          // Disable the counter overflow interrupt

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        ccr = &TBCCR0 + (i);
        ctl = &TBCCTL0 + (i);
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);
    }

    TBCTL |= MC_2;                           // Set timer to continuous mode
}

interrupt(TIMERB0_VECTOR) __attribute__((naked)) timer_isr_ccr0(void)
{
    __enter_isr();
    timer_round += 1;
    __exit_isr();

}

interrupt(TIMERB1_VECTOR) __attribute__((naked)) timer_isr_ccr1_6(void)
{
    __enter_isr();

    short intvec = TBIV;

    if (intvec & TBIV_TBIFG) {
    } else {
        short timer = (intvec / 2);
        if(overflow_interrupt[timer] == timer_round)
        {
            TB_unset(timer);
            int_handler(timer);
        }
    }
    __exit_isr();
}
