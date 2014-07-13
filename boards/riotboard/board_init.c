#include <stdint.h>
#include "sdk.h"
#include "registers/regsccm.h"
#include "board.h"
#include "timer/timer.h"
#include "timer/epit.h"
#include "periph_conf.h"
#include "arch/hwtimer_arch.h"
#include "periph/timer.h"

#undef EPIT_TEST
#undef CPU_TEST
#undef HAL_TEST
#define TIMER_TEST

#undef HWTIMER_TEST
#define HWTIMER_TEST_TIMER 3

#ifdef TIMER_TEST
#define TIMER_TEST_TIMER TIMER_1
#define TIMER_TEST_CHANNEL 0
#endif


#ifdef EPIT_TEST
void isr_epit_test(void)
{
    LED_GREEN_TOGGLE;
    LED_RED_TOGGLE;
    epit_get_compare_event(HW_EPIT1);
    epit_counter_enable(HW_EPIT1, TIMER_0_MAX_VALUE, IRQ_MODE);
}
#endif

#ifdef HWTIMER_TEST
void isr_hwtimer_test(short channel)
{
    LED_GREEN_TOGGLE;
    LED_RED_TOGGLE;
    hwtimer_arch_set(1000000, HWTIMER_TEST_TIMER);
}
#endif

#ifdef TIMER_TEST
void isr_timer_test(int channel)
{
    LED_GREEN_TOGGLE;
    LED_RED_TOGGLE;
    timer_set_absolute(TIMER_TEST_TIMER, TIMER_TEST_CHANNEL, 2000000);
}
#endif

/**
 * Initialize the system
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void board_init(void)
{
    gpio_set_gpio(RIOTBOARD_LED1_PORT, RIOTBOARD_LED1_PIN);
    gpio_set_direction(RIOTBOARD_LED1_PORT, RIOTBOARD_LED1_PIN, GPIO_GDIR_OUTPUT);

    gpio_set_gpio(RIOTBOARD_LED2_PORT, RIOTBOARD_LED2_PIN);
    gpio_set_direction(RIOTBOARD_LED2_PORT, RIOTBOARD_LED2_PIN, GPIO_GDIR_OUTPUT);

    LED_GREEN_OFF;
    // TODO LED2 stopped working
    LED_RED_OFF;

#ifdef CPU_TEST
    /* LED-test (CPU): */
    unsigned int cnt = 0;
    printf("counting variable driven LED test\n");

    while (1) {
        if (((cnt++) & 0x3FFFF) == 0) {
            toggle_led(0);
        }
    }

#endif

#ifdef HAL_TEST
    /* LED-test (delay): */
    printf("\"hal_delay_us\"-driven LED-test\n");

    while (1) {
        toggle_led(0);
        hal_delay_us(100000);
    }

#endif

#ifdef EPIT_TEST
    /* LED-test (epit): */
    printf("epit-driven LED-test\n");
    epit_init(HW_EPIT1, HWTIMER_CLKSRC, HWTIMER_PRESCALE, FREE_RUNNING, 0, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(HW_EPIT1, isr_epit_test, 1);

    printf("enabling interrupt\n");

    printf("setting timeout\n");
    epit_set_compare_event(HW_EPIT1, TIMER_0_MAX_VALUE - 1000000);

    epit_counter_enable(HW_EPIT1, TIMER_0_MAX_VALUE, IRQ_MODE);
    printf("Entering loop\n");

    while (1) {
        printf("half a second\n");

        hal_delay_us(500000);
        printf("%d\n", TIMER_0_MAX_VALUE - epit_get_counter_value(HW_EPIT1));
    }

#endif

#ifdef HWTIMER_TEST
    /* LED-test (htimer-driver) */
    printf("hwtimer-Test\n");
    hwtimer_arch_init(isr_hwtimer_test, get_main_clock(IPG_CLK));

    hwtimer_arch_set(2000000, 3);

    while (1) {
        hal_delay_us(500000);
        printf("Timer value: %d\n", hwtimer_arch_now());
    }

#endif

#ifdef TIMER_TEST
    printf("timer-test\n");

    timer_init(TIMER_TEST_TIMER, 1, isr_timer_test);

    timer_set(TIMER_TEST_TIMER, TIMER_TEST_CHANNEL, 1000000);

    while (1) {
        hal_delay_us(500000);
        printf("Timer value: %d\n", timer_read(TIMER_TEST_TIMER));
    }

#endif

    /* TODO no threading support yet, so we wait here */
    printf("No threading support yet, so we wait here.\n"); // ^^ fate

    while (1) ;

    printf("Congratz; you found a glitch in the matrix, Neo!\n");
}
