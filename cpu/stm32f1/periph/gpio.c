/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Extract information from mode parameter
 */
#define MODE_MASK                       (0x0f)
#define ODR_POS                         (4U)

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF             (16U)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t exti_ctx[GPIO_ISR_CHAN_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Convert port register address to port register structure
 */
static inline GPIO_TypeDef *_port(gpio_port_t port)
{
    return (GPIO_TypeDef *)port.reg;
}

/**
 * @brief   Extract the port number from the given device address
 *
 * Isolating bits 10 to 13 of the port base addresses leads to unique port
 * numbers.
 */
static inline int _port_num(gpio_port_t port)
{
    return (((port.reg >> 10) & 0x0f) - 2);
}

int gpio_cpu_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode)
{
    /* open-drain output with pull-up is not supported */
    if (mode == GPIO_OD_PU) {
        return -1;
    }

    /* enable the clock for the selected port */
    periph_clk_en(APB2, (RCC_APB2ENR_IOPAEN << _port_num(port)));

#ifdef BOARD_NUCLEO_F103RB
    /* disable the default SWJ RST mode to allow using the pin as IO
       this may also work on other f103 based boards but it was only tested on
       nucleo-f103rb */
    if ((pin == 4) && _port_num(port)) {
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
        AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_NOJNTRST;
    }
#endif

    /* set pin mode */
    _port(port)->CR[pin >> 3] &= ~(0xf << ((pin & 0x7) * 4));
    _port(port)->CR[pin >> 3] |=  ((mode & MODE_MASK) << ((pin & 0x7) * 4));

    /* set ODR */
    if (mode == GPIO_IN_PU)
        _port(port)->ODR |= 1 << pin;
    else
        _port(port)->ODR &= ~(1 << pin);

    return 0; /* all OK */
}

void gpio_init_af(gpio_t gpio, gpio_af_t af)
{
    int pin_num = gpio.pin;
    GPIO_TypeDef *port = _port(GPIO_PORT(gpio));

    /* enable the clock for the selected port */
    periph_clk_en(APB2, (RCC_APB2ENR_IOPAEN << _port_num(GPIO_PORT(gpio))));
    /* configure the pin */
    port->CR[pin_num >> 3] &= ~(0xf << ((pin_num & 0x7) * 4));
    port->CR[pin_num >> 3] |=  (af << ((pin_num & 0x7) * 4));
}

void gpio_init_analog(gpio_t gpio)
{
    GPIO_TypeDef *port = _port(GPIO_PORT(gpio));

    /* enable the GPIO port RCC */
    periph_clk_en(APB2, (RCC_APB2ENR_IOPAEN << _port_num(GPIO_PORT(gpio))));

    /* map the pin as analog input */
    port->CR[gpio.pin >= 8] &= ~(0xfl << (4 * (gpio.pin - ((gpio.pin >= 8) * 8))));
}

gpio_mask_t gpio_cpu_read(gpio_port_t port)
{
    return _port(port)->IDR;
}

void gpio_cpu_set(gpio_port_t port, gpio_mask_t pins)
{
    _port(port)->BSRR = pins;
}

void gpio_cpu_clear(gpio_port_t port, gpio_mask_t pins)
{
    _port(port)->BSRR = ((uint32_t)pins << 16);
}

void gpio_cpu_toggle(gpio_port_t port, gpio_mask_t pins)
{
    _port(port)->ODR = _port(port)->ODR ^ pins;
}

void gpio_cpu_write(gpio_port_t port, gpio_mask_t values)
{
    _port(port)->ODR = values;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_cpu_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                      gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    /* disable interrupts on the channel we want to edit (just in case) */
    EXTI->IMR &= ~(1 << pin);
    /* configure pin as input */
    gpio_cpu_init(port, pin, mode);
    /* set callback */
    exti_ctx[pin].cb = cb;
    exti_ctx[pin].arg = arg;
    /* enable alternate function clock for the GPIO module */
    periph_clk_en(APB2, RCC_APB2ENR_AFIOEN);
    /* configure the EXTI channel */
    AFIO->EXTICR[pin >> 2] &= ~(0xf << ((pin & 0x3) * 4));
    AFIO->EXTICR[pin >> 2] |=  (_port_num(port) << ((pin & 0x3) * 4));
    /* configure the active flank */
    EXTI->RTSR &= ~(1 << pin);
    EXTI->RTSR |=  ((flank & 0x1) << pin);
    EXTI->FTSR &= ~(1 << pin);
    EXTI->FTSR |=  ((flank >> 1) << pin);
    /* active global interrupt for the selected port */
    if (pin < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin);
    }
    else if (pin < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
    /* clear event mask */
    EXTI->EMR &= ~(1 << pin);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin);
    return 0;
}

void gpio_cpu_irq_enable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EXTI->IMR |= (1 << pin);
}

void gpio_cpu_irq_disable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EXTI->IMR &= ~(1 << pin);
}

void isr_exti(void)
{
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (EXTI->PR & EXTI->IMR);
    for (unsigned i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
        if (pending_isr & (1 << i)) {
            EXTI->PR = (1 << i);        /* clear by writing a 1 */
            exti_ctx[i].cb(exti_ctx[i].arg);
        }
    }
    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_GPIO_IRQ */
