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
 * @brief       GPIO extension test routine
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdio.h>

#include "extend/gpio.h"
#include "periph/gpio.h"

/* GPIO extension test functions */
int test_gpio_init(void *dev, gpio_t pin, gpio_mode_t mode);
int test_gpio_init_int(void *dev, gpio_t pin, gpio_mode_t mode,
                       gpio_flank_t flank, gpio_cb_t cb, void *arg);
void test_gpio_irq_enable(void *dev, gpio_t pin);
void test_gpio_irq_disable(void *dev, gpio_t pin);
int test_gpio_read(void *dev, gpio_t pin);
void test_gpio_set(void *dev, gpio_t pin);
void test_gpio_clear(void *dev, gpio_t pin);
void test_gpio_toggle(void *dev, gpio_t pin);
void test_gpio_write(void *dev, gpio_t pin, int value);

/* GPIO extension test driver */
const gpio_ext_driver_t tests_extend_gpio_driver = {
    .init = test_gpio_init,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = test_gpio_init_int,
    .irq_enable = test_gpio_irq_enable,
    .irq_disable = test_gpio_irq_disable,
#endif /* MODULE_PERIPH_GPIO_IRQ */
    .read = test_gpio_read,
    .set = test_gpio_set,
    .clear = test_gpio_clear,
    .toggle = test_gpio_toggle,
    .write = test_gpio_write,
};

int test_gpio_init(void *dev, gpio_t pin, gpio_mode_t mode)
{
    (void)mode;

    printf("init on dev %p with pin %u\n", dev, (unsigned)pin);

    return 0;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int test_gpio_init_int(void *dev, gpio_t pin, gpio_mode_t mode,
                       gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;

    printf("init_int on dev %p with pin %u\n", dev, (unsigned)pin);

    return 0;
}

void test_gpio_irq_enable(void *dev, gpio_t pin)
{
    printf("irq_enable on dev %p with pin %u\n", dev, (unsigned)pin);
}

void test_gpio_irq_disable(void *dev, gpio_t pin)
{
    printf("irq_disable on dev %p with pin %u\n", dev, (unsigned)pin);
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

int test_gpio_read(void *dev, gpio_t pin)
{
    printf("read on dev %p with pin %u\n", dev, (unsigned)pin);

    return 0;
}

void test_gpio_set(void *dev, gpio_t pin)
{
    printf("set on dev %p with pin %u\n", dev, (unsigned)pin);
}

void test_gpio_clear(void *dev, gpio_t pin)
{
    printf("clear on dev %p with pin %u\n", dev, (unsigned)pin);
}

void test_gpio_toggle(void *dev, gpio_t pin)
{
    printf("toggle on dev %p with pin %u\n", dev, (unsigned)pin);
}

void test_gpio_write(void *dev, gpio_t pin, int value)
{
    (void)value;

    printf("write on dev %p with pin %u\n", dev, (unsigned)pin);
}

int main(void)
{
    gpio_t num, pin;

    puts("gpio extension test routine");

    for (num = 0; num < 4; num++) {
        pin = (1 << num);
        printf("Running gpio.h functions on pin %u\n", (unsigned int)pin);

        if (gpio_init(GPIO_EXT_PIN(0, pin), 0x0)) {
            puts("[FAILED]");
            return 1;
        }

#ifdef MODULE_PERIPH_GPIO_IRQ
        if (gpio_init_int(GPIO_EXT_PIN(0, pin), 0x0, 0x0, NULL, NULL)) {
            puts("[FAILED]");
            return 1;
        }

        gpio_irq_enable(GPIO_EXT_PIN(0, pin));
        gpio_irq_disable(GPIO_EXT_PIN(0, pin));
#endif /* MODULE_PERIPH_GPIO_IRQ */
        gpio_read(GPIO_EXT_PIN(0, pin));
        gpio_set(GPIO_EXT_PIN(0, pin));
        gpio_clear(GPIO_EXT_PIN(0, pin));
        gpio_toggle(GPIO_EXT_PIN(0, pin));
        gpio_write(GPIO_EXT_PIN(0, pin), 0);
    }

    puts("Running notsup functions");
    puts("(they should not print output)");

    for (num = 0; num < 4; num++) {
        pin = (1 << num);
        printf("Running notsup gpio.h functions on pin %u\n",
            (unsigned int)pin);

        if (!gpio_init(GPIO_EXT_PIN(1, pin), 0x0)) {
            puts("[FAILED]");
            return 1;
        }

#ifdef MODULE_PERIPH_GPIO_IRQ
        if (!gpio_init_int(GPIO_EXT_PIN(1, pin), 0x0, 0x0, NULL, NULL)) {
            puts("[FAILED]");
            return 1;
        }

        gpio_irq_enable(GPIO_EXT_PIN(1, pin));
        gpio_irq_disable(GPIO_EXT_PIN(1, pin));
#endif /* MODULE_PERIPH_GPIO_IRQ */
        gpio_read(GPIO_EXT_PIN(1, pin));
        gpio_set(GPIO_EXT_PIN(1, pin));
        gpio_clear(GPIO_EXT_PIN(1, pin));
        gpio_toggle(GPIO_EXT_PIN(1, pin));
        gpio_write(GPIO_EXT_PIN(1, pin), 0);
    }
/*
    puts("Checking that all pins in range have init error using notsup");
    puts("(lack of init error implies improper redirection)");

    for (num = 0; num <= GPIO_EXT_PIN_MASK; num++) {
        if (gpio_init(GPIO_EXT_PIN(1, num), 0x0) >= 0) {
            printf("init succeeded on pin %u\n", num);
            puts("[FAILED]");
            return 1;
        }
    }
*/
    puts("[SUCCESS]");

    return 0;
}
