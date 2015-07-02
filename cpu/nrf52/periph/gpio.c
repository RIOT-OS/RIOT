/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf52832
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * NOTE: this GPIO driver implementation supports due to hardware limitations
 *       only one pin configured as external interrupt source at a time!
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/* guard file in case no GPIO device is defined */
#if GPIO_NUMOF

typedef struct {
    gpio_t dev;
    gpio_cb_t cb;
    void *arg;
} gpio_state_t;

static gpio_state_t gpio_config;

/**
 * @brief helper function to get the pin that corresponds to a GPIO device
 *
 * @param[in] dev       the device the returned pin corresponds to
 *
 * @return              the pin number of for the given device
 * @return              -1 if device unknown
 */
static inline int get_pin(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN

        case GPIO_0:
            return GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN

        case GPIO_1:
            return GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN

        case GPIO_2:
            return GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN

        case GPIO_3:
            return GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN

        case GPIO_4:
            return GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN

        case GPIO_5:
            return GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN

        case GPIO_6:
            return GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN

        case GPIO_7:
            return GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN

        case GPIO_8:
            return GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN

        case GPIO_9:
            return GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN

        case GPIO_10:
            return GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN

        case GPIO_11:
            return GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN

        case GPIO_12:
            return GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN

        case GPIO_13:
            return GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN

        case GPIO_14:
            return GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN

        case GPIO_15:
            return GPIO_15_PIN;
            break;
#endif
#if GPIO_16_EN

        case GPIO_16:
            return GPIO_16_PIN;
            break;
#endif
#if GPIO_17_EN

        case GPIO_17:
            return GPIO_17_PIN;
            break;
#endif
#if GPIO_18_EN

        case GPIO_18:
            return GPIO_18_PIN;
            break;
#endif
#if GPIO_19_EN

        case GPIO_19:
            return GPIO_19_PIN;
            break;
#endif
#if GPIO_20_EN

        case GPIO_20:
            return GPIO_20_PIN;
            break;
#endif
#if GPIO_21_EN

        case GPIO_21:
            return GPIO_21_PIN;
            break;
#endif
#if GPIO_22_EN

        case GPIO_22:
            return GPIO_22_PIN;
            break;
#endif
#if GPIO_23_EN

        case GPIO_23:
            return GPIO_23_PIN;
            break;
#endif
#if GPIO_24_EN

        case GPIO_24:
            return GPIO_24_PIN;
            break;
#endif
#if GPIO_25_EN

        case GPIO_25:
            return GPIO_25_PIN;
            break;
#endif
#if GPIO_26_EN

        case GPIO_26:
            return GPIO_26_PIN;
            break;
#endif
#if GPIO_27_EN

        case GPIO_27:
            return GPIO_27_PIN;
            break;
#endif
#if GPIO_28_EN

        case GPIO_28:
            return GPIO_28_PIN;
            break;
#endif
#if GPIO_29_EN

        case GPIO_29:
            return GPIO_29_PIN;
            break;
#endif
#if GPIO_30_EN

        case GPIO_30:
            return GPIO_30_PIN;
            break;
#endif
#if GPIO_31_EN

        case GPIO_31:
            return GPIO_31_PIN;
            break;
#endif
    }

    return -1;
}

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    int pin = get_pin(dev);

    if (pin < 0) {
        return pin;
    }

    /* configure pin: output, input buffer disabled */
    NRF_P0->PIN_CNF[pin] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                           (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);

    /* configure pull up value, map 0x1 -> 0x2; 0x2 -> 0x1 */
    if (pullup > 0) {
        NRF_P0->PIN_CNF[pin] |= ((pullup ^ 0x3) << GPIO_PIN_CNF_PULL_Pos);
    }

    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    int pin = get_pin(dev);

    if (pin < 0) {
        return pin;
    }

    /* configure pin: output, input buffer disabled */
    NRF_P0->PIN_CNF[pin] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                           (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);

    /* configure pull up value, map 0x1 -> 0x2; 0x2 -> 0x1 */
    if (pullup > 0) {
        NRF_P0->PIN_CNF[pin] |= ((pullup ^ 0x3) << GPIO_PIN_CNF_PULL_Pos);
    }

    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    uint32_t pin;

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);

    if (res < 0) {
        return res;
    }

    /* get pin */
    pin = get_pin(dev);         /* no need to check return value, init_in already did */

    /* set interrupt priority and enable global GPIOTE interrupt */
    NVIC_SetPriority(GPIOTE_IRQn, GPIO_IRQ_PRIO);
    NVIC_EnableIRQ(GPIOTE_IRQn);

    /* save callback */
    gpio_config.dev = dev;
    gpio_config.cb = cb;
    gpio_config.arg = arg;

    /* reset GPIOTE configuration register to EVENT mode*/
    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Event;

    /* select active pin for external interrupt */
    NRF_GPIOTE->CONFIG[0] |= (pin << GPIOTE_CONFIG_PSEL_Pos);

    /* set active flank */
    switch (flank) {
        case GPIO_FALLING:
            NRF_GPIOTE->CONFIG[0] |= (1 << GPIOTE_CONFIG_POLARITY_Pos);
            break;

        case GPIO_RISING:
            NRF_GPIOTE->CONFIG[0] |= (2 << GPIOTE_CONFIG_POLARITY_Pos);
            break;

        case GPIO_BOTH:
            NRF_GPIOTE->CONFIG[0] |= (3 << GPIOTE_CONFIG_POLARITY_Pos);
            break;
    }

    /* enable external interrupt */
    NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    if (gpio_config.dev == dev) {
        NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;
    }
}

void gpio_irq_disable(gpio_t dev)
{
    if (gpio_config.dev == dev) {
        NRF_GPIOTE->INTENCLR |= GPIOTE_INTENSET_IN0_Msk;
    }
}

int gpio_read(gpio_t dev)
{
    /* get pin */
    int pin = get_pin(dev);

    if (pin < 0) {
        return pin;
    }

    /* read pin value depending if pin is input or output */
    int res;

    if (NRF_P0->DIR & (1 << pin)) {
        res = (NRF_P0->OUT & (1 << pin));
    }
    else {
        res = (NRF_P0->IN & (1 << pin));
    }

    /* fix issue with negative number if pin 31 is set */
    if (res < -1) {
        res = 1;
    }

    return res;
}

void gpio_set(gpio_t dev)
{
    int pin = get_pin(dev);

    if (pin < 0) {
        return;
    }

    NRF_P0->OUTSET = (1 << pin);
}

void gpio_clear(gpio_t dev)
{
    int pin = get_pin(dev);

    if (pin < 0) {
        return;
    }

    NRF_P0->OUTCLR = (1 << pin);
}

void gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        gpio_clear(dev);
    }
    else {
        gpio_set(dev);
    }
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    }
    else {
        gpio_clear(dev);
    }
}

void isr_gpiote(void)
{
    if (NRF_GPIOTE->EVENTS_IN[0] == 1) {
        NRF_GPIOTE->EVENTS_IN[0] = 0;
        gpio_config.cb(gpio_config.arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* GPIO_NUMOF */
