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

#include "flash_utils.h"
#include "irq.h"
#include "modules.h"
#include "mutex.h"
#include "periph/gpio_ll.h"
#include "periph/gpio_ll_irq.h"
#include "time_units.h"
#include "ztimer.h"

#ifndef LOW_ROM
#define LOW_ROM 0
#endif

static const char *noyes[] = { "no", "yes" };
static gpio_port_t port_out = GPIO_PORT(PORT_OUT);
static gpio_port_t port_in = GPIO_PORT(PORT_IN);

static const uint64_t mutex_timeout = US_PER_MS;

#if LOW_ROM
static void puts_optional(const char *str)
{
    (void)str;
}
#else
/* this is like puts() on most platforms, but e.g. on AVR safes lots of RAM by
 * placing the string literal to puts in flash. */
#define puts_optional(str_literal) flash_puts(TO_FLASH(str_literal))
#endif

#if LOW_ROM
#define printf_optional(...) (void)0
#else
#define printf_optional(...) printf(__VA_ARGS__)
#endif

/* a custom expect that keeps the CPU alive makes debugging easier with
 * stdio that requires RIOT to remain alive, e.g. USB CDC ACM */
static void expect_impl(int val, unsigned line)
{
    if (!val) {
        printf("expect failed at line %u\n", line);
        fflush(stdout);
        while(1) {}
    }
}

#define expect(x) expect_impl((int)(x), __LINE__)

static void print_cabling(unsigned port1, unsigned pin1,
                          unsigned port2, unsigned pin2)
{
    printf("  P%u.%u (P%c%u) -- P%u.%u (P%c%u)\n",
           port1, pin1, 'A' + (char)port1, pin1,
           port2, pin2, 'A' + (char)port2, pin2);
}

static void print_details(void)
{
    puts_optional("Test / Hardware Details:\n"
                  "========================\n"
                  "Cabling:\n"
                  "(INPUT -- OUTPUT)");
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

static void print_conf(gpio_conf_t conf)
{
    gpio_ll_print_conf(conf);
    puts("");
}

static void test_gpio_ll_init_input_configs(void)
{
    bool is_supported;
    puts_optional("\nTesting input configurations for PIN_IN_0:");
    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pu));
    printf_optional("Support for input with pull up: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_INPUT_PULL_UP));
    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
        print_conf(conf);
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_PULL_UP));
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pd));
    printf_optional("Support for input with pull down: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_INPUT_PULL_DOWN));
    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
        print_conf(conf);
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_PULL_DOWN));
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pk));
    printf_optional("Support for input with pull to bus level: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_INPUT_PULL_KEEP));
    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
        print_conf(conf);
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_PULL_KEEP));
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in));
    printf_optional("Support for floating input (no pull resistors): %s\n",
                    noyes[is_supported]);
    {
        gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
        print_conf(conf);
        expect((conf.state == GPIO_INPUT) && (conf.pull == GPIO_FLOATING));
    }
    /* Support for floating inputs is mandatory */
    expect(is_supported);
}

static void test_gpio_ll_init_output_configs(void)
{
    bool is_supported;
    puts_optional("\nTesting output configurations for PIN_OUT_0:");
    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
            .initial_value = false,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (push-pull) with initial value of "
                    "LOW: %s\n",
                    noyes[is_supported]);

    /* this is mandatory */
    expect(is_supported);
    {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
        expect((conf.state == GPIO_OUTPUT_PUSH_PULL) && !conf.initial_value);
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed LOW: %s\n", noyes[is_supported]);
        expect(is_supported);
        gpio_ll_set(port_out, (1ULL<< PIN_OUT_0));
        conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output can be pushed HIGH: %s\n", noyes[is_supported]);
        expect(is_supported);
        conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        expect((conf.state == GPIO_OUTPUT_PUSH_PULL) && conf.initial_value);
    }

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
            .initial_value = true,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (push-pull) with initial value of "
                    "HIGH: %s\n",
                    noyes[is_supported]);

    /* this is mandatory */
    expect(is_supported);
    {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open drain with pull up) with initial "
                    "value of LOW: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_DRAIN_PULL_UP));

    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open drain with pull up) with initial "
                    "value of HIGH: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_DRAIN_PULL_UP));

    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open drain) with initial value of "
                    "LOW: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_DRAIN));

    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open drain) with initial value of "
                    "HIGH: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_DRAIN));

    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
        expect((conf.state == GPIO_OUTPUT_OPEN_DRAIN) && conf.initial_value
               && (conf.pull == GPIO_FLOATING));
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pd));
        if (is_supported) {
            gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
            print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pu));
        if (is_supported) {
            gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
            print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open source) with initial value of "
                    "LOW: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_SOURCE));

    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
        expect((conf.state == GPIO_OUTPUT_OPEN_SOURCE) && !conf.initial_value
               && (conf.pull == GPIO_FLOATING));
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pd));
        if (is_supported) {
            gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
            print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pu));
        if (is_supported) {
            gpio_conf_t conf = gpio_ll_query_conf(port_in, PIN_IN_0);
            print_conf(conf);
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

    expect(0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in));

    {
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_OPEN_SOURCE,
            .initial_value = true,
            .pull = GPIO_FLOATING,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open source) with initial value of "
                    "HIGH: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_SOURCE));

    if (is_supported) {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        print_conf(conf);
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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open source with pull down) with initial "
                    "value of HIGH: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_SOURCE_PULL_DOWN));

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
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("Support for output (open source with pull down) with initial "
                    "value of LOW: %s\n",
                    noyes[is_supported]);
    /* check if this matches compile time checks */
    expect(is_supported == IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_SOURCE_PULL_DOWN));

    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output is indeed LOW: %s\n", noyes[is_supported]);
        expect(is_supported);
    }
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

    /* first, iterate through input configurations and test them one by one */
    test_gpio_ll_init_input_configs();

    /* second, iterate through output configurations and test them */
    test_gpio_ll_init_output_configs();

    /* finally, test disconnecting pins */
    {
        gpio_conf_t conf = {
            .state = GPIO_DISCONNECT,
        };
        is_supported = (0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
    }
    printf_optional("\nSupport for disconnecting GPIO: %s\n", noyes[is_supported]);
    /* This is mandatory */
    expect(is_supported);

    /* Ensure that gpio_ll_query_conf() correctly detects the state as
     * disconnected. On MCUs that don't support this (e.g. ATmega),
     * GPIO_DISCONNECT must be an alias of GPIO_INPUT. */
    {
        gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
        gpio_ll_print_conf(conf);
        puts("");
        expect(conf.state == GPIO_DISCONNECT);
    }

    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pd));
    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = !(gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output can indeed be pulled LOW: %s\n",
                        noyes[is_supported]);
        /* If this expects fails, try with a different pin. Often pins intended
         * for use as UART, I2C, or hardware /CS on SPI cannot be configured
         * as high impedance and will instead remain HIGH. An implementation
         * is free to work around this e.g. by configuring these pins as
         * input with the input buffer disabled or as analog input without
         * routing them to the ADC, or just to restore the reset configuration
         * and have them high.
         *
         * Even though this can fail on some pins, the `expect()` here should
         * remain. Just test with a different pin that can be configured as
         * high impedance to confirm that this functionality does work where
         * supported. */
        expect(is_supported);
    }
    else {
        puts_optional("WARN: Cannot enable pull down of PIN_IN_0 to verify "
                      "correct disabled behavior");
    }
    is_supported = (0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in_pu));
    if (is_supported) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
        is_supported = (gpio_ll_read(port_in) & (1ULL << PIN_IN_0));
        printf_optional("Output can indeed be pulled HIGH: %s\n",
                        noyes[is_supported]);
        /* May also fail for some pins, if the reset configuration of the
         * pin results in LOW output. Do not comment this out, but rather
         * try with a different pin to confirm that this functionality does
         * work where supported */
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

    expect(0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in));
    expect(0 == gpio_ll_init(port_in, PIN_IN_1, gpio_ll_in));
    expect(0 == gpio_ll_init(port_out, PIN_OUT_0, gpio_ll_out));
    expect(0 == gpio_ll_init(port_out, PIN_OUT_1, gpio_ll_out));

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

struct irq_level_cb_arg {
    mutex_t mutex;
    unsigned counter;
    enum {
        LOW,
        HIGH
    } trigger_level;
};

__attribute__((unused))
static void irq_level_cb(void *_arg)
{
    struct irq_level_cb_arg *arg = _arg;

    if (!arg->counter) {
        if (arg->trigger_level == HIGH) {
            gpio_ll_clear(port_out, 1UL << PIN_OUT_0);
        }
        else {
            gpio_ll_set(port_out, 1UL << PIN_OUT_0);
        }
        mutex_unlock(&arg->mutex);
    }
    else {
        arg->counter--;
    }
}

static void test_irq_level(void)
{
    struct irq_level_cb_arg arg = { .mutex = MUTEX_INIT_LOCKED, .counter = 10 };

    if (IS_USED(MODULE_PERIPH_GPIO_LL_IRQ_LEVEL_TRIGGERED_HIGH)) {
        arg.trigger_level = HIGH;
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
        arg.trigger_level = LOW;
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
    puts_optional("\n"
         "Testing External IRQs\n"
         "=====================\n");

    expect(0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in));
    expect(0 == gpio_ll_init(port_out, PIN_OUT_0, gpio_ll_out));

    test_irq_edge();
    test_irq_level();
}

static void test_switch_dir(void)
{
    bool test_passed;
    puts_optional("\n"
         "Testing Switching Direction\n"
         "===========================\n");

    uword_t mask_out = 1U << PIN_OUT_0;
    uword_t mask_in = 1U << PIN_IN_0;

    /* floating input must be supported by every MCU */
    expect(0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_in));
    expect(0 == gpio_ll_init(port_out, PIN_OUT_0, gpio_ll_in));
    gpio_conf_t conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
    expect(conf.state == GPIO_INPUT);
    gpio_conf_t conf_orig = conf;

    /* capture output state before switching from input mode to output mode, so
     * that it can be restored when switching back to input mode */
    uword_t out_state = gpio_ll_read_output(port_out);

    /* now, switch to output mode and verify the switch */
    gpio_ll_switch_dir_output(port_out, mask_out);
    conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
    test_passed = (conf.state == GPIO_OUTPUT_PUSH_PULL);
    printf_optional("Input pin can be switched to output (push-pull) mode: %s\n",
                    noyes[test_passed]);
    expect(test_passed);

    gpio_ll_clear(port_out, mask_out);
    test_passed = (0 == (gpio_ll_read(port_in) & mask_in));
    gpio_ll_set(port_out, mask_out);
    test_passed = test_passed && (gpio_ll_read(port_in) & mask_in);
    printf_optional("Pin behaves as output after switched to output mode: %s\n",
                    noyes[test_passed]);
    expect(test_passed);

    /* switch back to input mode */
    gpio_ll_switch_dir_input(port_out, mask_out);
    /* restore out state from before the switch */
    gpio_ll_write(port_out, out_state);
    /* verify we are back at the old config */
    conf = gpio_ll_query_conf(port_out, PIN_OUT_0);
    test_passed = (conf.bits == conf_orig.bits);
    printf_optional("Returning back to input had no side effects on config: %s\n",
                    noyes[test_passed]);
    if (!test_passed) {
        puts_optional("Before:");
        print_conf(conf_orig);
        puts_optional("After:");
        print_conf(conf);
        expect(0);
    }

    /* Finally: check if input behaves like a proper input. For that, we
     * configure the pin it is connected to (PORT_IN.PIN_IN_0) as output and
     * see if we can read that from the pin used to switch back and force */
    expect(0 == gpio_ll_init(port_in, PIN_IN_0, gpio_ll_out));

    gpio_ll_clear(port_in, mask_in);
    test_passed = (0 == (gpio_ll_read(port_out) & mask_out));
    gpio_ll_set(port_in, mask_in);
    test_passed = test_passed && (gpio_ll_read(port_out) & mask_out);
    printf_optional("Pin behaves as input after switched back to input mode: %s\n",
                    noyes[test_passed]);
    expect(test_passed);
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

    if (IS_USED(MODULE_PERIPH_GPIO_LL_SWITCH_DIR)) {
        test_switch_dir();
    }

    /* if no expect() didn't blow up until now, the test is passed */

    puts("\n\nTEST SUCCEEDED");
    return 0;
}
