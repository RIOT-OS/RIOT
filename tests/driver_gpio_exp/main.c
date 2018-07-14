/*
 * Copyright (C) 2018 Acutam Automation, LLC
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
 * @brief       GPIO expander test routine
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdio.h>

#include "gpio_exp.h"

/* GPIO expansion test functions */
int test_gpio_exp_init(void *dev, gpio_t pin, gpio_mode_t mode);
int test_gpio_exp_init_int(void *dev, gpio_t pin, gpio_mode_t mode,
                           gpio_flank_t flank, gpio_cb_t cb, void *arg);
void test_gpio_exp_irq(void *dev, gpio_t pin, int state);
int test_gpio_exp_read(void *dev, gpio_t pin);
void test_gpio_exp_write(void *dev, gpio_t pin, int value);

/* GPIO expansion test drivers */
const gpio_exp_driver_t test_gpio_exp_test_driver = {
    .init = &test_gpio_exp_init,
    .init_int = &test_gpio_exp_init_int,
    .irq = &test_gpio_exp_irq,
    .read = &test_gpio_exp_read,
    .write = &test_gpio_exp_write
};
const gpio_exp_driver_t test_gpio_exp_notsup_driver = {
    .init = &gpio_exp_init_notsup,
    .init_int = &gpio_exp_init_int_notsup,
    .irq = &gpio_exp_irq_notsup,
    .read = &gpio_exp_read_notsup,
    .write = &gpio_exp_write_notsup
};

int test_gpio_exp_init(void *dev, gpio_t pin, gpio_mode_t mode)
{
    (void)mode;

    printf("init on dev 0x%x with pin %u\n",
        (uint16_t)(uintptr_t)dev, (unsigned int)pin);

    return 0;
}

int test_gpio_exp_init_int(void *dev, gpio_t pin, gpio_mode_t mode,
                           gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;

    printf("init_int on dev 0x%x with pin %u\n",
        (uint16_t)(uintptr_t)dev, (unsigned int)pin);

    return 0;
}

void test_gpio_exp_irq(void *dev, gpio_t pin, int state)
{
    (void)state;

    printf("irq on dev 0x%x with pin %u\n",
        (uint16_t)(uintptr_t)dev, (unsigned int)pin);
}

int test_gpio_exp_read(void *dev, gpio_t pin)
{
    printf("read on dev 0x%x with pin %u\n",
        (uint16_t)(uintptr_t)dev, (unsigned int)pin);

    return 0;
}

void test_gpio_exp_write(void *dev, gpio_t pin, int value)
{
    (void)value;

    printf("write on dev 0x%x with pin %u\n",
        (uint16_t)(uintptr_t)dev, (unsigned int)pin);
}

int main(void)
{
    uint16_t num;
    gpio_t pin;

    puts("gpio_exp test routine");

    for (num = 0; num < 4; num++) {
        pin = (1 << num);
        printf("Running gpio.h functions on pin %u\n", (unsigned int)pin);

        gpio_init(GPIO_EXP_PIN(0, pin), 0x0);
        gpio_init_int(GPIO_EXP_PIN(0, pin), 0x0, 0x0, NULL, NULL);
        gpio_irq_enable(GPIO_EXP_PIN(0, pin));
        gpio_irq_disable(GPIO_EXP_PIN(0, pin));
        gpio_read(GPIO_EXP_PIN(0, pin));
        gpio_set(GPIO_EXP_PIN(0, pin));
        gpio_clear(GPIO_EXP_PIN(0, pin));
        gpio_toggle(GPIO_EXP_PIN(0, pin));
        gpio_write(GPIO_EXP_PIN(0, pin), 0);
    }

    puts("Running notsup functions");
    puts("(they should not print output)");

    for (num = 0; num < 4; num++) {
        pin = (1 << num);
        printf("Running notsup gpio.h functions on pin %u\n",
            (unsigned int)pin);

        gpio_init(GPIO_EXP_PIN(1, pin), 0x0);
        gpio_init_int(GPIO_EXP_PIN(1, pin), 0x0, 0x0, NULL, NULL);
        gpio_irq_enable(GPIO_EXP_PIN(1, pin));
        gpio_irq_disable(GPIO_EXP_PIN(1, pin));
        gpio_read(GPIO_EXP_PIN(1, pin));
        gpio_set(GPIO_EXP_PIN(1, pin));
        gpio_clear(GPIO_EXP_PIN(1, pin));
        gpio_toggle(GPIO_EXP_PIN(1, pin));
        gpio_write(GPIO_EXP_PIN(1, pin), 0);
    }

    puts("Checking that all pins in range have init error using notsup");
    puts("(lack of init error implies improper redirection)");

    for (num = 0; num <= GPIO_EXP_MAXPINMASK; num++) {
        if (gpio_init(GPIO_EXP_PIN(1, num), 0x0) >= 0) {
            printf("init succeeded on pin %u\n", num);
            puts("[FAILED]");
            return 1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
