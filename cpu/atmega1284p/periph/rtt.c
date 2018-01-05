#include "board.h"
#include "periph/rtt.h"
#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

#include "irq.h"
#include "thread.h"

rtt_cb_t rtt_next_cb;
volatile int cnt = 0;

void rtt_init(void) {
    printf("init rtt\n");
    rtt_poweron();

    /* Disable all interrupts */
    TIMSK2 = 0;

    /* Select asynchronous clock source */
    ASSR = (1 << AS2);

    /* Reset output compare */
    TCCR2A = 0;
    /* 32768Hz / 1024 -> 32 ticks per second! */
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 <<  CS20);
    /* Match register -> 1 interrupt every second */
    OCR2A = 32;
    /* Set count register to 0 */
    TCNT2 = 0;
    /* Enable interrupts () */
    TIMSK2 |= ( 1 << OCIE2A) /*| (1 << TOIE2)*/;

    /* Wait until not busy anymore */
    while( ASSR  & (1 << TCN2UB) );
    while( ASSR  & (1 << OCR2AUB) );
    while( ASSR  & (1 << TCR2AUB) );

    /* Clear interrupt flags */
    TIFR2 &= ~( (1 << OCF2B)  | (1 << OCF2A) | (1 << TOV2) );
    /*
a. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2.
b. Select clock source by setting AS2 as appropriate.
c. Write new values to TCNT2, OCR2x, and TCCR2x.
d. To switch to asynchronous operation: Wait for TCN2UB, OCR2xUB, and TCR2xUB.
e. Clear the Timer/Counter2 Interrupt Flags.
f. Enable interrupts, if needed
    */

    irq_enable();
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg) {
    rtt_next_cb = cb;
    (void) arg;
}

void rtt_clear_overflow_cb(void) {
    rtt_next_cb = NULL;
}

uint32_t rtt_get_counter(void) {
    return TCNT2;
}

void rtt_set_counter(uint32_t counter) {
    TCNT2 = (uint8_t)counter;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg) {
    (void)arg;
    (void)alarm;
    rtt_next_cb = cb;
    OCR2A = (uint8_t)alarm;
}

uint32_t rtt_get_alarm(void) {
    return OCR2A;
}

void rtt_clear_alarm(void) {
    TIMSK2 &= ~(1 << OCIE2A);
    OCR2A = 0;
}

void rtt_poweron(void) {
#ifdef MODULE_PM_LAYERED
    pm_block(PM_SLEEPMODE_INVALID_TIMER2);
#endif
    power_timer2_enable();
}

void rtt_poweroff(void) {
    power_timer2_disable();
#ifdef MODULE_PM_LAYERED
    pm_unblock(PM_SLEEPMODE_INVALID_TIMER2);
#endif
}

/*
ISR(TIMER2_OVF_vect) {
    __enter_isr();
    /// What to do here?
    //puts("TIMER2_OVF");
    //rtt_next_cb(NULL);
    __exit_isr();
}
*/

ISR(TIMER2_COMPA_vect) {
    __enter_isr();
    //puts("TIMER2_COMPA");
    //TCNT2 = 0;
    rtt_next_cb(NULL);
    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}

/*
ISR(TIMER2_COMPB_vect) {
    __enter_isr();
    //puts("TIMER2_COMPB");
    //TCNT2 = 0;
    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}
*/
