/*
 * Copyright (C) 2024 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       timer capture test application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <stdio.h>

#include "busy_wait.h"
#include "modules.h"
#include "mutex.h"
#include "periph/gpio_ll.h"
#include "periph/timer.h"
#include "test_utils/expect.h"
#include "tiny_strerror.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#ifndef CONFIG_TIMER_CAPTURE_DEV
#  define CONFIG_TIMER_CAPTURE_DEV  0
#endif

#ifndef CONFIG_TIMER_COMPARE_DEV
#  define CONFIG_TIMER_COMPARE_DEV  CONFIG_TIMER_CAPTURE_DEV
#endif

#ifndef CONFIG_TRIGGER_GPIO_PORT
#  define CONFIG_TRIGGER_GPIO_PORT  1
#endif

#ifndef CONFIG_TRIGGER_GPIO_PIN
#  define CONFIG_TRIGGER_GPIO_PIN   0
#endif

#ifndef CONFIG_TINY_DELAY_US
#  define CONFIG_TINY_DELAY_US      20
#endif

static const gpio_port_t trigger_port = GPIO_PORT(CONFIG_TRIGGER_GPIO_PORT);
static const uword_t trigger_mask = 1U << CONFIG_TRIGGER_GPIO_PIN;
static gpio_port_t capture_port = 0;
static uword_t capture_mask = 0;

static mutex_t sync_compare = MUTEX_INIT_LOCKED;
static mutex_t sync_capture = MUTEX_INIT_LOCKED;

static unsigned last_capture = 0;
static int capture_channel = 0;
static int compare_channel = 0;
static int last_compare_channel = 0;

static void _debug_print_gpio_levels(void)
{
    if (ENABLE_DEBUG) {
        if (gpio_ll_read_output(trigger_port) & trigger_mask) {
            DEBUG_PUTS("t^");
        }
        else {
            DEBUG_PUTS("tv");
        }
        if (gpio_ll_read(capture_port) & capture_mask) {
            DEBUG_PUTS("c^");
        }
        else {
            DEBUG_PUTS("cv");
        }
    }
}

static void compare_cb(void *arg, int channel)
{
    gpio_ll_toggle(trigger_port, trigger_mask);
    if (ENABLE_DEBUG) {
        if (gpio_ll_read_output(trigger_port) & trigger_mask) {
            DEBUG_PUTS("T^");
        }
        else {
            DEBUG_PUTS("Tv");
        }
    }

    last_compare_channel = channel;
    mutex_unlock(arg);
}

static void capture_cb(void *arg, int channel, unsigned capture)
{
    if (ENABLE_DEBUG) {
        if (gpio_ll_read(capture_port) & capture_mask) {
            DEBUG_PUTS("C^");
        }
        else {
            DEBUG_PUTS("Cv");
        }
    }
    last_capture = capture;

    expect(channel == capture_channel);

    mutex_unlock(arg);
}

static uint32_t find_common_freq(void)
{
    /* Trivial case: Same timer peripheral used for both capture and compare. */
    if (CONFIG_TIMER_CAPTURE_DEV == CONFIG_TIMER_COMPARE_DEV) {
        return timer_query_freqs(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV), 0);
    }

    /* Non-trivial case: We actually need to check both timers */
    uword_t compare_idx = 0;
    uword_t capture_idx = 0;
    uint32_t compare_freq = 0;
    uint32_t capture_freq = 0;

    do {
        if (compare_freq > capture_freq) {
            compare_idx++;
        }
        if (compare_freq < capture_freq) {
            capture_idx++;
        }
        compare_freq = timer_query_freqs(TIMER_DEV(CONFIG_TIMER_COMPARE_DEV), compare_idx);
        capture_freq = timer_query_freqs(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV), capture_idx);
    } while (compare_freq != capture_freq);

    return compare_freq;
}

static int print_test_failed(void)
{
    puts("TEST FAILED");
    return 1;
}

static void print_cabeling(gpio_t triggered_gpio)
{
    unsigned triggered_gpio_port = GPIO_PORT_NUM(gpio_get_port(triggered_gpio));
    unsigned triggered_gpio_pin = gpio_get_pin_num(triggered_gpio);
    printf("GPIO (Output)    GPIO(Input)   Timer\n"
           "P%u.%u/P%c%u ---> P%u.%u/P%c%u TIMER_DEV(%u) @ chan %u\n",
            (unsigned)CONFIG_TRIGGER_GPIO_PORT, (unsigned)CONFIG_TRIGGER_GPIO_PIN,
            'A' + (char)CONFIG_TRIGGER_GPIO_PORT, (unsigned)CONFIG_TRIGGER_GPIO_PIN,
            triggered_gpio_pin, triggered_gpio_pin,
            'A' + (char)triggered_gpio_port, triggered_gpio_pin,
            (unsigned)CONFIG_TIMER_CAPTURE_DEV, (unsigned)capture_channel);

}

static int test_oneshot(gpio_flank_t flank)
{
    int retval;

    gpio_ll_clear(trigger_port, trigger_mask);
    if (flank == GPIO_FALLING) {
        gpio_ll_set(trigger_port, trigger_mask);
    }

    retval = timer_capture_enable(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV),
                                  capture_channel,
                                  flank, TIM_CAPTURE_FLAG_ONESHOT);

    if (retval) {
        printf("timer_capture_enable(): %s\n", tiny_strerror(retval));
        return -1;
    }

    mutex_trylock(&sync_capture);
    last_capture = 0;

    gpio_ll_toggle(trigger_port, trigger_mask);
    mutex_lock(&sync_capture);

    unsigned capture = last_capture;

    /* no capture should occur any more */
    for (unsigned i = 0; i < 10; i++) {
        gpio_ll_toggle(trigger_port, trigger_mask);
        busy_wait_us(CONFIG_TINY_DELAY_US);
    }

    /* last_capture should be unchanged */
    expect(capture == last_capture);

    /* mutex should still be locked */
    expect(mutex_trylock(&sync_capture) == false);

    return 0;
}

static int test_irqs_disabled(gpio_flank_t flank)
{
    int retval;

    gpio_ll_clear(trigger_port, trigger_mask);
    if (flank == GPIO_FALLING) {
        gpio_ll_set(trigger_port, trigger_mask);
    }

    retval = timer_capture_enable(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV),
                                  capture_channel,
                                  flank, 0);

    if (retval) {
        printf("timer_capture_enable(): %s\n", tiny_strerror(retval));
        return -1;
    }

    mutex_trylock(&sync_capture);
    last_capture = 0;

    unsigned irq_state = irq_disable();

    for (unsigned i = 0; i < 4; i++) {
        gpio_ll_toggle(trigger_port, trigger_mask);
        busy_wait_us(CONFIG_TINY_DELAY_US);
    }

    /* no captures while IRQ disabled */
    expect(last_capture == 0);

    irq_restore(irq_state);
    busy_wait_us(CONFIG_TINY_DELAY_US);
    /* capture should have happened now */
    expect(last_capture != 0);

    timer_capture_disable(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV),
                          capture_channel);

    return 0;
}

static int test_regular(gpio_flank_t flank, unsigned offset)
{
    int retval;

    gpio_ll_clear(trigger_port, trigger_mask);
    if (flank == GPIO_FALLING) {
        gpio_ll_set(trigger_port, trigger_mask);
    }

    retval = timer_capture_enable(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV),
                                  capture_channel,
                                  flank, 0);

    if (retval) {
        printf("timer_capture_enable(): %s\n", tiny_strerror(retval));
        return -1;
    }

    _debug_print_gpio_levels();

    mutex_trylock(&sync_capture);
    unsigned start = timer_read(TIMER_DEV(CONFIG_TIMER_COMPARE_DEV));
    gpio_ll_toggle(trigger_port, trigger_mask);
    mutex_lock(&sync_capture);
    unsigned capture_start = last_capture;

    /* change level if needed to generate a trigger event on next toggle */
    if (flank != GPIO_BOTH) {
        gpio_ll_toggle(trigger_port, trigger_mask);
    }

    _debug_print_gpio_levels();

    timer_set_absolute(TIMER_DEV(CONFIG_TIMER_COMPARE_DEV),
                       compare_channel, start + offset);

    mutex_lock(&sync_capture);
    /* The cause (toggling the GPIO from the compare timer IRQ) should have
     * happen before the effect (the capture being triggered). Hence, the
     * compare sync lock should already be unlocked when the capture sync lock
     * is released */
    expect(mutex_trylock(&sync_compare));
    /* The most recent compare ISR should have been on the compare channel */
    expect(last_compare_channel == compare_channel);

    unsigned capture_stop = last_capture;

    expect(capture_stop - capture_start >= offset);
    expect(capture_stop - capture_start <  offset + offset / 4);

    timer_capture_disable(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV),
                          capture_channel);

    /* no capture should occur any more */
    for (unsigned i = 0; i < 10; i++) {
        gpio_ll_toggle(trigger_port, trigger_mask);
        busy_wait_us(CONFIG_TINY_DELAY_US);
    }

    expect(capture_stop == last_capture);

    return 0;
}

static int test(gpio_flank_t flank, unsigned offset)
{
    int retval;
    printf("- one shot mode: ");
    retval = test_oneshot(flank);
    if (retval) {
        return retval;
    }
    puts("[OK]");

    printf("- regular mode: ");
    retval = test_regular(flank, offset);
    if (retval) {
        return retval;
    }
    puts("[OK]");

    printf("- interaction with disabling/enabling IRQs: ");
    retval = test_irqs_disabled(flank);
    if (retval) {
        return retval;
    }
    puts("[OK]");

    printf("- capture channel still works in compare mode: ");
    if ((unsigned)capture_channel < timer_query_channel_numof(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV))) {
        mutex_trylock(&sync_compare);
        timer_set(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV), capture_channel, offset);
        mutex_lock(&sync_compare);
        expect(last_compare_channel == capture_channel);
        puts("[OK]");
    }
    else {
        puts("not applicable (channel not available in compare mode)");
    }

    return 0;
}

static int init_timer_checked(unsigned timer_dev, uint32_t freq)
{
    int retval = timer_init(TIMER_DEV(timer_dev), freq,
                           compare_cb, &sync_compare);

    printf("timer_init(%u, %" PRIu32 ", compare_cb, &sync_compare): %s\n",
           timer_dev, freq,
           tiny_strerror(retval));

    return retval;
}

int main(void)
{
    int retval;
    puts("testing periph_timer_capture on " RIOT_BOARD);
    capture_channel = timer_capture_channel_first(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV));

    if (capture_channel < 0) {
        printf("Capture mode not supported on timer %u\n",
               (unsigned)CONFIG_TIMER_CAPTURE_DEV);
        return print_test_failed();
    }

    /* let's make sure we don't use the same channel on the same timer for
     * both capture and compare */
    if (CONFIG_TIMER_CAPTURE_DEV == CONFIG_TIMER_COMPARE_DEV) {
        if (capture_channel == 0) {
            compare_channel = 1;
        }
    }

    if ((unsigned)compare_channel >= timer_query_channel_numof(TIMER_DEV(CONFIG_TIMER_COMPARE_DEV))) {
        puts("Channel for compare timer out of range.\n"
             "(Hint: Same hw selected for capture and compare but timer has only one channel?)");
        return print_test_failed();
    }

    gpio_t triggered_gpio = timer_capture_pin(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV),
                                              capture_channel);

    capture_port = gpio_get_port(triggered_gpio);
    capture_mask = 1U << gpio_get_pin_num(triggered_gpio);

    if (!gpio_is_valid(triggered_gpio)) {
        printf("On TIMER_DEV(%u) channel %d: No external (via GPIO pin) "
               "trigger configured/available\n",
               (unsigned)CONFIG_TIMER_CAPTURE_DEV, capture_channel);
        return print_test_failed();
    }

    print_cabeling(triggered_gpio);

    uint32_t freq = find_common_freq();

    unsigned offset = freq * 2ULL / MS_PER_SEC;
    if (offset < 20) {
        offset = 20;
    }

    _debug_print_gpio_levels();

    if (!freq) {
        printf("Timers TIMER_DEV(%u) and TIMER_DEV(%u) cannot run at the same "
               "frequency\n",
               (unsigned)CONFIG_TIMER_COMPARE_DEV,
               (unsigned)CONFIG_TIMER_CAPTURE_DEV);
        return print_test_failed();
    }

    printf("Running @ %" PRIu32 " Hz, offset %u ticks\n", freq, offset);

    if (init_timer_checked(CONFIG_TIMER_CAPTURE_DEV, freq)) {
        return print_test_failed();
    }

    if (CONFIG_TIMER_CAPTURE_DEV != CONFIG_TIMER_COMPARE_DEV) {
        if (init_timer_checked(CONFIG_TIMER_COMPARE_DEV, freq)) {
            return print_test_failed();
        }
    }

    timer_set_capture_cb(TIMER_DEV(CONFIG_TIMER_CAPTURE_DEV), capture_cb, &sync_capture);

    _debug_print_gpio_levels();

    retval = gpio_ll_init(trigger_port, CONFIG_TRIGGER_GPIO_PIN, gpio_ll_out);
    printf("gpio_ll_init(): %s\n", tiny_strerror(retval));
    if (retval) {
        return print_test_failed();
    }

    if (IS_USED(MODULE_PERIPH_TIMER_CAPTURE_RISING)) {
        puts("# capture on rising edge");
        if (test(GPIO_RISING, offset)) {
            return print_test_failed();
        }
    }

    if (IS_USED(MODULE_PERIPH_TIMER_CAPTURE_FALLING)) {
        puts("# capture on falling edge");
        if (test(GPIO_FALLING, offset)) {
            return print_test_failed();
        }
    }

    if (IS_USED(MODULE_PERIPH_TIMER_CAPTURE_BOTH)) {
        puts("# capture on both edges");
        if (test(GPIO_BOTH, offset)) {
            return print_test_failed();
        }
    }

    puts("TEST SUCCEEDED");
    return 0;
}
