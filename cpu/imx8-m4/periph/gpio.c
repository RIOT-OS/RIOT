/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_imx8mq
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "gpio_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   Number of external interrupt lines
 */
#define NUMOF_IRQS                  (32 * 5)

static gpio_isr_ctx_t gpio_config[NUMOF_IRQS];
#endif /* MODULE_PERIPH_GPIO_IRQ */

static inline uint8_t _port_nb (gpio_t pin)
{
    return (((pin >> 16) & 0x0F) + 1) ;
}
static inline uint8_t _pin_nb(gpio_t pin)
{
    return pin & 0x1F;
}
static inline GPIO_Type *_port(gpio_t pin)
{
    return (GPIO_Type *)(pin & ~(0x1f));
}

static inline int _pin_pos(gpio_t pin)
{
    return (pin & 0x1f);
}

static inline int _pin_mask(gpio_t pin)
{
    return (1 << _pin_pos(pin));
}
static inline uint8_t _get_mux(gpio_t pin)
{
    return _mux_ctl_start[_port_nb(pin)-1] + _pin_nb(pin);
}

static inline uint32_t* _mux_reg(gpio_t pin)
{
    return (uint32_t*) &IOMUXC->SW_MUX_CTL_PAD[_get_mux(pin)];
}

void gpio_init_mux(gpio_t pin, gpio_mux_t mux)
{
    *(_mux_reg(pin)) = mux;
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_Type *port = _port(pin);
    uint32_t mask = _pin_mask(pin);

    if (_pin_nb(pin) >= _gpio_per_port[_port_nb(pin)]) {
        return -1;
    }
    /* make sure pin mode is applicable */
    if (mode > 0x7) {
        return -1;
    }
    
    /* reset pin configuration */
    port->IMR &= ~(mask);
    /* set pin direction */
    if (mode & 0x2) {
        /* Input */
        port->GDIR &= ~_pin_mask(pin);
    }
    else {
        port->GDIR |= _pin_mask(pin);
    }

    /* configure the pin cfg */

    /* and set pull-up/pull-down if applicable */

    return 0;
}

int gpio_read(gpio_t pin)
{
    return (( _port(pin)->DR) &  _pin_mask(pin));
}

void gpio_set(gpio_t pin)
{
   _port(pin)->DR |= _pin_mask(pin);
}

void gpio_clear(gpio_t pin)
{
   _port(pin)->DR &= ~_pin_mask(pin);
}

void gpio_toggle(gpio_t pin)
{
    _port(pin)->DR ^= _pin_mask(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _port(pin)->DR |= _pin_mask(pin);
    } else {
        _port(pin)->DR &= ~_pin_mask(pin);
    }
}


#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    GPIO_Type *port = _port(pin);
    uint8_t nvic = GPIO1_Combined_0_15_IRQn;
    gpio_init(pin, mode);
    if(_pin_nb(pin) < 16 ) {
        port->ICR1 = (flank & 0x3) << (2*_pin_pos(pin));
    }
    else {
        port->ICR2 = (flank & 0x3) << (2*_pin_pos(pin));
    }

    /* save callback */
    nvic += (_port_nb(pin) * 2);
    nvic += (_pin_nb(pin) < 16 ? 0 : 1);
    gpio_config[(32 * _port_nb(pin)) + _pin_nb(pin)].cb = cb;
    gpio_config[(32 * _port_nb(pin)) + _pin_nb(pin)].arg = arg;
    NVIC_EnableIRQ(nvic);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    GPIO_Type *port = _port(pin);

    port->IMR |= _pin_mask(pin);
}

void gpio_irq_disable(gpio_t pin)
{
    GPIO_Type *port = _port(pin);

    port->IMR &= ~_pin_mask(pin);
}


void isr_eic(int port_nb)
{
    GPIO_Type *port = ((GPIO_Type *)(GPIO1_BASE + (port_nb << 16)));
    for(int i=0; i < 32; i++) {
        if (port->ISR & (1 << i)) {
            /* Clear interrupt bit */
            port->ISR = (1 << i);
            /* call callback */
            gpio_config[i].cb(gpio_config[i].arg);
        }
    }
    cortexm_isr_end();
}

void isr_gpio1_l(void)
{
    isr_eic(0);
}

void isr_gpio1_h(void)
{
    isr_eic(0);
}

void isr_gpio2_l(void)
{
    isr_eic(1);
}

void isr_gpio2_h(void)
{
    isr_eic(1);
}

void isr_gpio3_l(void)
{
    isr_eic(2);
}

void isr_gpio3_h(void)
{
    isr_eic(2);
}

void isr_gpio4_l(void)
{
    isr_eic(3);
}

void isr_gpio4_h(void)
{
    isr_eic(3);
}

void isr_gpio5_l(void)
{
    isr_eic(4);
}

void isr_gpio5_h(void)
{
    isr_eic(4);
}

#endif /* MODULE_PERIPH_GPIO_IRQ */