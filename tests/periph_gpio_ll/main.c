/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for the Peripheral GPIO Low-Level API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mutex.h"
#include "periph/gpio_ll.h"
#include "periph/gpio_ll_irq.h"
#include "test_utils/expect.h"
#include "ztimer.h"
#include "timex.h"

#ifndef LOW_ROM
#define LOW_ROM 0
#endif

static const char *noyes[] = { "no", "yes" };
static gpio_port_t port_out = GPIO_PORT(PORT_OUT);
static gpio_port_t port_in = GPIO_PORT(PORT_IN);

static const uint64_t mutex_timeout = US_PER_MS;

static void puts_optional(const char *str)
{
    if (!LOW_ROM) {
        puts(str);
    }
}

#if LOW_ROM
#define printf_optional(...) (void)0
#else
#define printf_optional(...) printf(__VA_ARGS__)
#endif

static void print_cabling(unsigned port1, unsigned pin1,
                          unsigned port2, unsigned pin2)
{
    printf("  P%u.%u (P%c%u) -- P%u.%u (P%c%u)\n",
           port1, pin1, 'A' + (char)port1, pin1,
           port2, pin2, 'A' + (char)port2, pin2);
}

static void print_details(void)
{
    puts_optional("Test / Hardware Deatils:\n"
                  "========================\n"
                  "Cabling:");
    print_cabling(PORT_IN, PIN_IN_0, PORT_OUT, PIN_OUT_0);
    print_cabling(PORT_IN, PIN_IN_1, PORT_OUT, PIN_OUT_1);
    printf("Number of pull resistor values supported: %u\n", GPIO_PULL_NUMOF);
    printf("Number of drive strengths supported: %u\n", GPIO_DRIVE_NUMOF);
    printf("Number of slew rates supported: %u\n", GPIO_SLEW_NUMOF);
    puts("Valid GPIO ports:");

    /* 64 GPIO ports ought to be enough for anybody */
    for (uint_fast8_t i = 0; i < 64; i++) {
        if (is_gpio_port_num_valid(i)) {
            printf("- PORT %" PRIuFAST8 " (PORT %c)\n", i, 'A' + i);
        }
    }
}

static void test_gpio_port_pack(void)
{
    char stacked;

    puts_optional("\n"
                  "Testing gpio_port_pack_addr()\n"
                  "=============================\n");

    expect(gpio_port_unpack_addr(gpio_port_pack_addr(&stacked)) == &stacked);
    expect(gpio_port_unpack_addr(gpio_port_pack_addr(noyes)) == noyes);
    void *tmp = malloc(1);
    expect(tmp);
    expect(gpio_port_unpack_addr(gpio_port_pack_addr(tmp)) == tmp);
    free(tmp);
    expect(gpio_port_unpack_addr(port_in) == NULL);
    expect(gpio_port_unpack_addr(port_out) == NULL);
    puts_optional("All OK");
}

static void test_gpio_ll_init(void)
{
    bool is_supported;
    puts_optional("\n"
                  "Testing gpip_ng_init()\n"
                  "======================\n"
                  "\n"
                  "Testing is_gpio_port_num_valid() is true for PORT_OUT and "
                  "PORT_IN:");
    expect(is_gpio_port_num_valid(PORT_IN));
    expect(is_gpio_port_num_valid(PORT_OUT));

    puts_optional("\nTesting input configurations for PIN_IN_0:");
    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pu));
    printf_optional("Support for input with pull up: %s\n",
                    noyes[is_supported]);
    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_PULL_UP));
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pd));
    printf_optional("Support for input with pull down: %s\n",
                    noyes[is_supported]);
    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_PULL_DOWN));
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pk));
    printf_optional("Support for input with pull to bus level: %s\n",
                    noyes[is_supported]);
    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_PULL_KEEP));
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in));
    printf_optional("Support for floating input (no pull resistors): %s\n",
                    noyes[is_supported]);
    {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_FLOATING));
    }
    /* Support for floating inputs is mandatory */
    expect(is_supported);

    puts_optional("\nTesting output configurations for PIN_OUT_0:");
    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
            .initial_value = false,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (push-pull) with initial value of "
                    "LOW: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_PUSH_PULL) && !conf.initial_value);
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed LOW: %s\n", noyes[is_supported]);
        expect(is_supported);
        gpio_ll_set(port_out, (1ULL<< PIN_OUT_0));
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output can be pushed HIGH: %s\n", noyes[is_supported]);
        expect(is_supported);
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        expect((conf.state == GPIO_OUTPUT_PUSH_PULL) && conf.initial_value);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
            .initial_value = true,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (push-pull) with initial value of "
                    "HIGH: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_PUSH_PULL) && conf.initial_value);
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed HIGH: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_DRAIN,
            .initial_value = false,
            .pull = GPIO_PULL_UP
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain with pull up) with initial "
                    "value of LOW: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_OPEN_DRAIN) && !conf.initial_value
               && (conf.pull == GPIO_PULL_UP));
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed LOW: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_DRAIN,
            .initial_value = true,
            .pull = GPIO_PULL_UP
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain with pull up) with initial "
                    "value of HIGH: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_OPEN_DRAIN) && conf.initial_value
               && (conf.pull == GPIO_PULL_UP));
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !!(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed HIGH: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_DRAIN,
            .initial_value = false,
            .pull = GPIO_FLOATING,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain) with initial value of "
                    "LOW: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_OPEN_DRAIN) && !conf.initial_value
               && (conf.pull == GPIO_FLOATING));
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed LOW: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_DRAIN,
            .initial_value = true,
            .pull = GPIO_FLOATING,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain) with initial value of "
                    "HIGH: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_OPEN_DRAIN) && conf.initial_value
               && (conf.pull == GPIO_FLOATING));
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pd));
        if (is_supported) {
            gpio_conf_t conf;
            gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
            gpio_ll_print_conf(&conf);
            puts_optional("");
            ztimer_sleep(ZTIMER_USEC, US_PER_MS);
            is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
            printf_optional("Output can indeed be pulled LOW: %s\n",
                            noyes[is_supported]);
            expect(is_supported);
        }
        else {
            puts_optional("WARN: Cannot enable pull down of PIN_IN_0 to verify "
                          "correct Open Drain behavior");
        }
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pu));
        if (is_supported) {
            gpio_conf_t conf;
            gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
            gpio_ll_print_conf(&conf);
            puts_optional("");
            ztimer_sleep(ZTIMER_USEC, US_PER_MS);
            is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
            printf_optional("Output can indeed be pulled HIGH: %s\n",
                            noyes[is_supported]);
            expect(is_supported);
        }
        else {
            puts_optional("WARN: Cannot enable pull up of PIN_IN_0 to verify "
                          "correct Open Drain behavior");
        }
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_SOURCE,
            .initial_value = false,
            .pull = GPIO_FLOATING,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open source) with initial value of "
                    "LOW: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_OPEN_SOURCE) && !conf.initial_value
               && (conf.pull == GPIO_FLOATING));
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pd));
        if (is_supported) {
            gpio_conf_t conf;
            gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
            gpio_ll_print_conf(&conf);
            puts_optional("");
            ztimer_sleep(ZTIMER_USEC, US_PER_MS);
            is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
            printf_optional("Output can indeed be pulled LOW: %s\n",
                            noyes[is_supported]);
            expect(is_supported);
        }
        else {
            puts_optional("WARN: Cannot enable pull down of PIN_IN_0 to verify "
                          "correct Open Source behavior");
        }
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pu));
        if (is_supported) {
            gpio_conf_t conf;
            gpio_ll_query_conf(&conf, port_in, PIN_IN_0);
            gpio_ll_print_conf(&conf);
            puts_optional("");
            ztimer_sleep(ZTIMER_USEC, US_PER_MS);
            is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
            printf_optional("Output can indeed be pulled HIGH: %s\n",
                            noyes[is_supported]);
            expect(is_supported);
        }
        else {
            puts_optional("WARN: Cannot enable pull up of PIN_IN_0 to verify "
                          "correct Open Source behavior");
        }
    }

    expect(0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in));

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_SOURCE,
            .initial_value = true,
            .pull = GPIO_FLOATING,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain) with initial value of "
                    "HIGH: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        gpio_conf_t conf;
        gpio_ll_query_conf(&conf, port_out, PIN_OUT_0);
        gpio_ll_print_conf(&conf);
        puts_optional("");
        expect((conf.state == GPIO_OUTPUT_OPEN_SOURCE) && conf.initial_value
               && (conf.pull == GPIO_FLOATING));
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed HIGH: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_SOURCE,
            .initial_value = true,
            .pull = GPIO_PULL_DOWN,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain with pull up) with initial "
                    "value of HIGH: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed HIGH: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_SOURCE,
            .initial_value = false,
            .pull = GPIO_PULL_DOWN,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for output (open drain with pull up) with initial "
                    "value of LOW: %s\n",
                    noyes[is_supported]);

    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed LOW: %s\n", noyes[is_supported]);
        expect(is_supported);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_DISCONNECT,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, &conf));
    }
    printf_optional("Support for disconnecting GPIO: %s\n", noyes[is_supported]);
    /* This is mandatory */
    expect(is_supported);

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pd));
    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output can indeed be pulled LOW: %s\n",
                        noyes[is_supported]);
        expect(is_supported);
    }
    else {
        puts_optional("WARN: Cannot enable pull down of PIN_IN_0 to verify "
                      "correct disabled behavior");
    }
    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in_pu));
    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output can indeed be pulled HIGH: %s\n",
                        noyes[is_supported]);
        expect(is_supported);
    }
    else {
        puts_optional("WARN: Cannot enable pull up of PIN_IN_0 to verify "
                      "correct disabled behavior");
    }
}

static void test_input_output(void)
{
    puts_optional("\n"
         "Testing Reading/Writing GPIO Ports\n"
         "==================================\n");

    expect(0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in));
    expect(0 == gpio_ll_init(port_in, PIN_IN_1, &gpio_ll_in));
    expect(0 == gpio_ll_init(port_out, PIN_OUT_0, &gpio_ll_out));
    expect(0 == gpio_ll_init(port_out, PIN_OUT_1, &gpio_ll_out));

    uword_t mask_in_0 = (1UL << PIN_IN_0);
    uword_t mask_in_1 = (1UL << PIN_IN_1);
    uword_t mask_in_both = mask_in_0 | mask_in_1;
    uword_t mask_out_both = (1UL << PIN_OUT_0) | (1UL << PIN_OUT_1);

    puts_optional("testing initial value of 0 after init");
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(0x00 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing setting both outputs_optional simultaneously");
    gpio_ll_set(port_out, (1UL << PIN_OUT_0) | (1UL << PIN_OUT_1));
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(mask_in_both == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing clearing both outputs_optional simultaneously");
    gpio_ll_clear(port_out, (1UL << PIN_OUT_0) | (1UL << PIN_OUT_1));
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(0x00 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing toggling first output (0 --> 1)");
    gpio_ll_toggle(port_out, 1UL << PIN_OUT_0);
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(mask_in_0 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing toggling first output (1 --> 0)");
    gpio_ll_toggle(port_out, 1UL << PIN_OUT_0);
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(0x00 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing toggling second output (0 --> 1)");
    gpio_ll_toggle(port_out, 1UL << PIN_OUT_1);
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(mask_in_1 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing toggling second output (1 --> 0)");
    gpio_ll_toggle(port_out, 1UL << PIN_OUT_1);
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(0x00 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("testing setting first output and clearing second with "
                  "write");
    /* Preventing side-effects on output GPIO port due to changes on unrelated
     * pins between the calls to gpio_ll_prepare_write() and gpio_ll_write()
     * by disabling IRQs. Better safe than sorry. */
    unsigned irq_state = irq_disable();
    gpio_ll_write(port_out, gpio_ll_prepare_write(port_out, mask_out_both,
                                                  1UL << PIN_OUT_0));
    irq_restore(irq_state);
    ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    expect(mask_in_0 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    irq_state = irq_disable();
    gpio_ll_write(port_out, gpio_ll_prepare_write(port_out, mask_out_both,
                                                  1UL << PIN_OUT_1));
    irq_restore(irq_state);
    puts_optional("testing setting second output and clearing first with "
                  "write");
    expect(mask_in_1 == (gpio_ll_read(port_in) & mask_in_both));
    puts_optional("...OK");

    puts_optional("All input/output operations worked as expected");
}

static void irq_edge_cb(void *mut)
{
    mutex_unlock(mut);
}

static void test_irq_edge(void)
{
    mutex_t irq_mut = MUTEX_INIT_LOCKED;

    puts_optional("Testing rising edge on PIN_IN_0");
    expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_EDGE_RISING,
                            irq_edge_cb, &irq_mut));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_set(port_out, 1UL << PIN_OUT_0);
    /* test for IRQ on rising edge */
    expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                          mutex_timeout));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
    /* test for no IRQ on falling edge */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    puts_optional("... OK");

    puts_optional("Testing falling edge on PIN_IN_0");
    expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_EDGE_FALLING,
                            irq_edge_cb, &irq_mut));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_set(port_out, 1UL << PIN_OUT_0);
    /* test for no IRQ on rising edge */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
    /* test for IRQ on falling edge */
    expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                          mutex_timeout));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    puts_optional("... OK");

    puts_optional("Testing both edges on PIN_IN_0");
    expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_EDGE_BOTH,
                            irq_edge_cb, &irq_mut));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_set(port_out, 1UL << PIN_OUT_0);
    /* test for IRQ on rising edge */
    expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                          mutex_timeout));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
    /* test for IRQ on falling edge */
    expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                          mutex_timeout));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    puts_optional("... OK");

    puts_optional("Testing masking of IRQs (still both edges on PIN_IN_0)");
    gpio_ll_irq_mask(port_in, PIN_IN_0);
    gpio_ll_set(port_out, 1UL << PIN_OUT_0);
    /* test for no IRQ while masked */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_irq_unmask_and_clear(port_in, PIN_IN_0);
    /* test for IRQ of past event is cleared */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    /* testing for normal behavior after unmasked */
    gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
    expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                          mutex_timeout));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
#if MODULE_PERIPH_GPIO_LL_IRQ_UNMASK
    gpio_ll_irq_mask(port_in, PIN_IN_0);
    gpio_ll_set(port_out, 1UL << PIN_OUT_0);
    /* test for no IRQ while masked */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
    gpio_ll_irq_unmask(port_in, PIN_IN_0);
    /* test for IRQ of past event */
    expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                          mutex_timeout));
    /* test for spurious IRQ */
    expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &irq_mut,
                                                   mutex_timeout));
#endif
    puts_optional("... OK");
    gpio_ll_irq_off(port_in, PIN_IN_0);
}

struct mutex_counter {
    mutex_t mutex;
    unsigned counter;
};

__attribute__((unused))
static void irq_level_cb(void *_arg)
{
    struct mutex_counter *arg = _arg;

    if (!arg->counter) {
        gpio_ll_toggle(port_out, 1UL << PIN_OUT_0);
        mutex_unlock(&arg->mutex);
    }
    else {
        arg->counter--;
    }
}

static void test_irq_level(void)
{
    struct mutex_counter arg = { .mutex = MUTEX_INIT_LOCKED, .counter = 10 };

    if (IS_USED(MODULE_PERIPH_GPIO_LL_IRQ_LEVEL_TRIGGERED_HIGH)) {
        puts_optional("Testing level-triggered on HIGH on PIN_IN_0 (when input "
                      "is LOW when setting up IRQ)");
        gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
        expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_LEVEL_HIGH,
                                irq_level_cb, &arg));
        /* test for spurious IRQ */
        expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                                       mutex_timeout));
        /* test for 10 level triggered IRQs on high */
        gpio_ll_set(port_out, 1UL << PIN_OUT_0);
        expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                              mutex_timeout));
        /* test for spurious IRQ */
        expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                                       mutex_timeout));
        gpio_ll_irq_off(port_in, PIN_IN_0);
        puts_optional("... OK");

        puts_optional("Testing level-triggered on HIGH on PIN_IN_0 (when input "
                      "is HIGH when setting up IRQ)");
        gpio_ll_set(port_out, 1UL << PIN_OUT_0);
        expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_LEVEL_HIGH,
                                irq_level_cb, &arg));
        /* test for 10 level triggered IRQs */
        expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                              mutex_timeout));
        /* test for spurious IRQ */
        expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                                       mutex_timeout));
        gpio_ll_irq_off(port_in, PIN_IN_0);
        puts_optional("... OK");
    }

    if (IS_USED(MODULE_PERIPH_GPIO_LL_IRQ_LEVEL_TRIGGERED_LOW)) {
        puts_optional("Testing level-triggered on LOW on PIN_IN_0 (when input "
                      "is HIGH when setting up IRQ)");
        gpio_ll_set(port_out, 1UL << PIN_OUT_0);
        arg.counter = 10;
        expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_LEVEL_LOW,
                                irq_level_cb, &arg));
        /* test for spurious IRQ */
        expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                                       mutex_timeout));
        /* test for 10 level triggered IRQs on low */
        gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
        expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                              mutex_timeout));
        /* test for spurious IRQ */
        expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                                       mutex_timeout));
        gpio_ll_irq_off(port_in, PIN_IN_0);
        puts_optional("... OK");

        puts_optional("Testing level-triggered on LOW on PIN_IN_0 (when input "
                      "is LOW when setting up IRQ)");
        gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
        arg.counter = 10;
        expect(0 == gpio_ll_irq(port_in, PIN_IN_0, GPIO_TRIGGER_LEVEL_LOW,
                                irq_level_cb, &arg));
        /* test for 10 level triggered IRQs */
        expect(0 == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                              mutex_timeout));
        /* test for spurious IRQ */
        expect(-ECANCELED == ztimer_mutex_lock_timeout(ZTIMER_USEC, &arg.mutex,
                                                       mutex_timeout));
        gpio_ll_irq_off(port_in, PIN_IN_0);
        puts_optional("... OK");
    }
}

static void test_irq(void)
{
    if (IS_USED(MODULE_PERIPH_GPIO_LL_IRQ)) {
        puts_optional("\n"
             "Testing External IRQs\n"
             "=====================\n");

        expect(0 == gpio_ll_init(port_in, PIN_IN_0, &gpio_ll_in));
        expect(0 == gpio_ll_init(port_out, PIN_OUT_0, &gpio_ll_out));

        test_irq_edge();
        test_irq_level();
    }
}

int main(void)
{
    print_details();
    test_gpio_port_pack();
    test_gpio_ll_init();
    test_input_output();
    if (IS_USED(MODULE_PERIPH_GPIO_LL_IRQ)) {
        test_irq();
    }

    /* if no expect() didn't blow up until now, the test is passed */

    puts("\n\nTEST SUCCEEDED");
    return 0;
}
