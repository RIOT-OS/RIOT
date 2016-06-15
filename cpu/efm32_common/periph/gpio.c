/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "periph/gpio.h"

#include "em_gpio.h"

/**
 * @brief   Number of external interrupt lines.
 */
#define NUMOF_IRQS         (GPIO_PIN_MAX)

/**
 * @brief   Hold one interrupt context per interrupt line
 */
static gpio_isr_ctx_t isr_ctx[NUMOF_IRQS];

static inline GPIO_Port_TypeDef _port_num(gpio_t pin)
{
    return ((pin & 0xf0) >> 4);
}

static inline uint32_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline uint32_t _pin_mask(gpio_t pin)
{
    return (1 << _pin_num(pin));
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pushpull)
{
    /* check for valid pin */
    if (pin == GPIO_UNDEF) {
        return -1;
    }

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    /* configure pin */
    if (dir == GPIO_DIR_OUT && pushpull == GPIO_NOPULL) {
#ifdef _SILICON_LABS_32B_PLATFORM_1
        GPIO_DriveModeSet(_port_num(pin), gpioDriveModeStandard);
#endif
        GPIO_PinModeSet(_port_num(pin), _pin_num(pin), gpioModePushPull, 0);
    }
    else if (dir == GPIO_DIR_IN) {
        if (pushpull == GPIO_NOPULL) {
            GPIO_PinModeSet(_port_num(pin), _pin_num(pin), gpioModeInput, 0);
        }
        else if (pushpull == GPIO_PULLUP) {
            GPIO_PinModeSet(_port_num(pin), _pin_num(pin), gpioModeInputPull, 1);
        }
        else if (pushpull == GPIO_PULLDOWN) {
            GPIO_PinModeSet(_port_num(pin), _pin_num(pin), gpioModeInputPull, 0);
        }
        else {
            return -1;
        }
    }
    else if (dir == GPIO_DIR_BI) {
        if (pushpull == GPIO_NOPULL) {
            GPIO_PinModeSet(_port_num(pin), _pin_num(pin), gpioModeWiredAnd, 1);
        }
        else if (pushpull == GPIO_PULLUP) {
            GPIO_PinModeSet(_port_num(pin), _pin_num(pin), gpioModeWiredAndPullUp, 1);
        }
        else {
            return -1;
        }
    }
    else {
        return -1;
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int result = gpio_init(pin, GPIO_DIR_IN, GPIO_NOPULL);

    if (result != 0) {
        return result;
    }

    /* just in case, disable the interrupt for this pin */
    GPIO_IntDisable(_pin_num(pin));

    /* store interrupt callback */
    isr_ctx[_pin_num(pin)].cb = cb;
    isr_ctx[_pin_num(pin)].arg = arg;

    /* configure interrupt */
    GPIO_IntConfig(_port_num(pin), _pin_num(pin), flank & GPIO_RISING, flank & GPIO_FALLING, true);

    /* enable interrupts */
    GPIO_IntEnable(_pin_num(pin));

    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);

    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    GPIO_IntEnable(_pin_mask(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    GPIO_IntDisable(_pin_mask(pin));
}

int gpio_read(gpio_t pin)
{
    return GPIO_PinInGet(_port_num(pin), _pin_num(pin));
}

void gpio_set(gpio_t pin)
{
    GPIO_PinOutSet(_port_num(pin), _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    GPIO_PinOutClear(_port_num(pin), _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    GPIO_PinOutToggle(_port_num(pin), _pin_num(pin));
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        GPIO_PinOutSet(_port_num(pin), _pin_num(pin));
    }
    else {
        GPIO_PinOutClear(_port_num(pin), _pin_num(pin));
    }
}

/**
 * @brief   Actual interrupt handler for both even and odd pin index numbers.
 */
static void gpio_irq(void)
{
    for (int i = 0; i < NUMOF_IRQS; i++) {
        if (GPIO_IntGet() & (1 << i)) {
            isr_ctx[i].cb(isr_ctx[i].arg);
            GPIO_IntClear(1 << i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/**
 * @brief   External interrupt handler for even pin index numbers
 */
void isr_gpio_even(void)
{
    gpio_irq();
}

/**
 * @brief   External interrupt handler for odd pin index numbers
 */
void isr_gpio_odd(void)
{
    gpio_irq();
}
