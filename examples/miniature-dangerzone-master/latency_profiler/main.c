#include <stdio.h>

#include "thread.h"
#include "hwtimer.h"
#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "gpioint.h"
#include "ps.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#include "latency.h"

#define MSEC    (1000)
#define SEC     (1000 * MSEC)

#ifdef CPU_LPC2387
#define LOOP_DELAY(USEC)    (USEC * 4)
#elif defined CPU_MSP430X16X
#define LOOP_DELAY(USEC)    (USEC / 4)
#endif

#ifdef RT_TEST
static int rt_pid;

static char busy_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];
static char rt_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];
#endif /* RT_TEST */

static void my_loop_delay(uint32_t delay)
{
    volatile uint32_t i;

    for (i = 1; i < delay; i++) {
            asm volatile(" nop ");
    }
}

#ifdef CPU_MSP430X16X
interrupt (PORT1_VECTOR) __attribute__ ((naked))
#else
void
#endif
pin_toggle_isr(void)
{
#ifdef CPU_MSP430X16X
    __enter_isr();
#endif
#ifdef THREADING_TEST
    thread_wakeup(rt_pid);
#elif defined MSG_TEST
    static msg_t m;
    msg_send_int(&m, rt_pid);
#endif /* THREADING_TEST */

#ifdef CPU_MSP430X16X
    P1IFG &= ~BIT7;      /* Clears the interrupt flag */
    __exit_isr();
#endif
}

void timer_cb(void *unused)
{
    DEBUG_PIN_TOGGLE;
}

#ifdef RT_TEST
static void busy_loop(void)
{
#ifdef WITH_PRINTING
    for (unsigned u = 0; u < MAX_PRINTING; u++) {
#else
    while (1) {
#endif
        puts("I'm so busy, I will just go on and on and on...");
    }

    /* won't be reached without printing enabled */
    puts("Printing timestamps");
    for (unsigned u = 0; u < MAX_PRINTING; u++) {
        printf("Timestamp[%u] - Timestamp[%u]: %lu\n", u, (u-1), (timestamps[u] - timestamps[u-1]));
    }
    while (1);
}

static void rt_loop(void)
{
#ifdef MSG_TEST
    msg_t m;
#endif
    while (1) {
#ifdef THREADING_TEST
        thread_sleep();
#elif defined MSG_TEST
        msg_receive(&m);
#endif
        DEBUG_PIN_TOGGLE;
    }
}
#endif /* RT_TEST */

int main(void)
{
    puts("Starting latency profiler");
    my_loop_delay(1);

    TRIGGER_PIN_INIT;

    TRIGGER_PIN_OFF;
    DEBUG_PIN_OFF;

#ifdef PIN_TEST
    while (1) {
        DEBUG_PIN_ON;
        my_loop_delay(LOOP_DELAY(1 * MSEC));
        DEBUG_PIN_OFF;
        hwtimer_wait(HWTIMER_TICKS(10 * MSEC));
        TRIGGER_PIN_TOGGLE;
    }
#endif /* PIN_TEST */

#ifdef GPIOINT_TEST
    gpioint_set(0, BIT10, GPIOINT_RISING_EDGE, pin_toggle_isr);
    while (1) {
        my_loop_delay(LOOP_DELAY(80 * MSEC));
        TRIGGER_PIN_TOGGLE;
    }
#endif /* GPIOINT_TEST */

#ifdef HWTIMER_TEST
    while (1) {
        hwtimer_set(HWTIMER_TICKS(100), timer_cb, NULL);
        my_loop_delay(LOOP_DELAY(20 * MSEC));
    }
#endif /* HWTIMER_TEST */

#ifdef HWTIMER_WAIT_TEST
    while (1) {
        TRIGGER_PIN_TOGGLE;
        hwtimer_wait(HWTIMER_TICKS(100));
        TRIGGER_PIN_TOGGLE;
        DEBUG_PIN_TOGGLE;
        my_loop_delay(LOOP_DELAY(10 * MSEC));
    }
#endif /* HWTIMER_WAIT_TEST */

#ifdef RT_TEST
    //thread_create(busy_thread_stack, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN-2, CREATE_STACKTEST | CREATE_WOUT_YIELD, busy_loop, "busy");
    rt_pid = thread_create(rt_thread_stack, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN-3, CREATE_STACKTEST | CREATE_WOUT_YIELD, rt_loop, "real time");
#ifdef CPU_LPC2387
    gpioint_set(0, BIT10, GPIOINT_RISING_EDGE, pin_toggle_isr);
#endif /* LPC2387 */
#ifdef CPU_MSP430X16X
    unsigned int state = disableIRQ(); /* Disable all interrupts */
    P1SEL = 0x00;       /* must be <> 1 to use interrupts */
    P1DIR &= ~BIT7;     /* set to input */
    P1IES &= ~BIT7;     /* Enables external interrupt on high edge */
    P1IE  |= BIT7;      /* Enable interrupt */
    P1IFG &= ~BIT7;     /* Clears the interrupt flag */
    restoreIRQ(state);
#endif /* MSP430X16X */
#endif /* RT_TEST */
    thread_sleep();

    return 0;
}
