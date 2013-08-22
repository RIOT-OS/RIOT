#include <legacymsp430.h>
#include <board.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>
#include <cpu.h>

#define ENABLE_DEBUG (0)
#include <debug.h>

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void TB_unset(short timer);

void timerB_init(void)
{
    ticks = 0;								 // Set tick counter value to 0
    TBCTL = TBSSEL_1 + TBCLR;   	  		 // Clear the timer counter, set ACLK
    TBCTL &= ~TBIE;						     // Clear the IFG

    volatile unsigned int *ccr = &TBCCR0;
    volatile unsigned int *ctl = &TBCCTL0;

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        *(ccr + i) = 0;
        *(ctl + i) &= ~(CCIFG);
        *(ctl + i) &= ~(CCIE);
    }

    TBCTL |= MC_2;
}

interrupt(TIMER0_B0_VECTOR) __attribute__((naked)) timerb0_isr(void)
{
    __enter_isr();

    TB_unset(0);
    int_handler(0);
    __exit_isr();
}

interrupt(TIMER0_B1_VECTOR) __attribute__((naked)) timerb16_isr(void)
{
    __enter_isr();

    short intvec = TBIV;
    short timer;

    if (intvec & TBIV_TBIFG) {
        DEBUG("Overflow\n");
    }
    else {
        timer = (intvec / 2);
        TB_unset(timer);
        int_handler(timer);
    }

    __exit_isr();
}
