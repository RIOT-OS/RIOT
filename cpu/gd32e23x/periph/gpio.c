/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       GD32E23x GPIO implementation
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph/gpio.h"
#include "gd32e23x.h"

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
#define GPIO_ISR_CHAN_MASK              (0xFFFF)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t exti_ctx[GPIO_ISR_CHAN_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_Type *_port(gpio_t pin)
{
    return (GPIO_Type *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number form the given identifier
 */
static inline int _port_num(gpio_t pin)
{
    return ((pin >> 10) & 0x0f);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline void _port_enable_clock(gpio_t pin)
{
    periph_clk_en(AHB, (RCU_AHBEN_PAEN_Msk << _port_num(pin)));  //GPIO are all on AHB2 which shares configuration with AHB
}

//static inline void set_mode(GPIO_Type *port, int pin_num, unsigned mode)
//{
//    uint32_t tmp = port->OMODE;
//    tmp &= ~(0x3 << (2 * pin_num));// is this MODE_MASK  or GPIO_MODE_MASK ?
//    tmp |=  ((mode & 0x3) << (2 * pin_num));
//    port->OMODE = tmp;
//}

/**
 * @brief   Check if the given mode is some kind of input mode
 * @param[in]   mode    Mode to check
 * @retval  1           @p mode is GPIO_IN, GPIO_IN_PD, or GPIO_IN_PU
 * @retval  0           @p mode is not an input mode
 */
static inline int gpio_mode_is_input(gpio_mode_t mode)
{
    return !(mode & 3);
}

static inline void _set_mode_or_af(GPIO_Type *port, unsigned pin_num,
                                   unsigned mode_or_af)
{
    volatile uint32_t *crl = (&port->CTL + (pin_num >> 3));
    uint32_t tmp = *crl;

    tmp &= ~(0xf << ((pin_num & 0x7) * 4));
    tmp |= ((mode_or_af & MODE_MASK) << ((pin_num & 0x7) * 4));
    *crl = tmp;
}

static inline bool _pin_is_output(GPIO_Type *port, unsigned pin_num)
{
    return (((port->CTL >> pin_num * 2) & 0x11) == 0x01);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    // GPIO_IN ,   /**< configure as input without pull resistor */
    // GPIO_IN_PD, /**< configure as input with pull-down resistor */
    // GPIO_IN_PU, /**< configure as input with pull-up resistor */
    // GPIO_OUT,   /**< configure as output in push-pull mode */
    // GPIO_OD,    /**< configure as output in open-drain mode without pull resistor */
    // GPIO_OD_PU

    /* enable the clock for the selected port */
    _port_enable_clock(pin);

    /* set pin mode */
    _set_mode_or_af(port, pin_num, mode);

    /* set control (2 bits per pin)*/
    port->CTL &= ~(0x3 << (2 * pin_num));
    port->CTL |= (((mode) & 0x3) << (2 *pin_num));
    /* set pull resistor configuration (2 bits per pin)*/
    port->PUD &= ~(0x3 << (2 * pin_num));
    port->PUD |= (((mode >> 2) & 0x3) << (2 * pin_num));
    /* set output mode (1 bit per pin)*/
    port->OMODE &= ~(0x1 << pin_num);
    port->OMODE |= (((mode >> 4) & 0x1) << pin_num);
    /* set pin speed to maximum */
    port->OSPD |= (3 << (2 * pin_num));

    return 0; /* all OK */
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    /* enable the clock for the selected port */
    _port_enable_clock(pin);
    /* configure the pin */
    _set_mode_or_af(port, pin_num, af);
}

void gpio_init_analog(gpio_t pin)
{
    /* enable the GPIO port RCC */
    _port_enable_clock(pin);

    /* map the pin as analog input */
    int pin_num = _pin_num(pin);

    volatile uint32_t *pin_reg = (uint32_t *)(&_port(pin)->CTL + (pin_num >= 8));
    *pin_reg &= ~(0xfl << (4 * (pin_num - ((pin_num >= 8) * 8))));
}

int gpio_read(gpio_t pin)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    if (_pin_is_output(port, pin_num)) {
        /* pin is output */
        return (port->OCTL & (1 << pin_num));
    }
    else {
        /* or input */
        return (port->ISTAT & (1 << pin_num));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BOP = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BC = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    _port(pin)->TG = (1 << _pin_num(pin));
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

/** @} */
