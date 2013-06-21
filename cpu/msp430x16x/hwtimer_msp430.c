#include <msp430x16x.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>
#include <cpu.h>

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void TA0_unset(short timer);

void timerA_init(void)
{
    ticks = 0;								 // Set tick counter value to 0
    TA0CTL = TASSEL_1 + TACLR;   	  		 // Clear the timer counter, set ACLK
    TA0CTL &= ~TAIFG;						 // Clear the IFG
    TA0CTL &= ~TAIE;						 // Clear the IFG
    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;

    for(int i = 0; i < ARCH_MAXTIMERS; i++) {
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);
    }

    ITA0CTL |= MC_2;
}

interrupt(TIMERA0_VECTOR) __attribute__((naked)) timer_isr_ccr0(void)
{
    __enter_isr();

    TA0_unset(0);
    int_handler(0);

    __exit_isr();
}

interrupt(TIMERA1_VECTOR) __attribute__((naked)) timer_isr(void)
{
    __enter_isr();

    short taiv = TA0IV;

    if(taiv & TAIFG) {
        // puts("msp430/hwtimer_cpu TAIFG set!");
        //    TA0CTL &= ~TAIFG;
        //    ticks += 0xFFFF;
    }
    else {

        short timer = (taiv / 2);
        TA0_unset(timer);
        int_handler(timer);
    }

    __exit_isr();
}
