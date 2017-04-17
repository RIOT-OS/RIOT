/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_s5p4418
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Dang Minh Phuong <kamejoko80@yahoo.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "s5p4418_gpio.h"

/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF (160U)

/**
 * @brief   GPIO alternative table
 */
static const gpio_alt_t gpio_alt_tbl[] = {
/* GPIOA[0-31] */
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
/* GPIOB[0-31] */
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT2, GPIO_ALT2, GPIO_ALT1, GPIO_ALT2, GPIO_ALT1,
GPIO_ALT2, GPIO_ALT1, GPIO_ALT2, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1,
GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1,
/* GPIOC[0-31] */
GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1,
GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1,
GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1,
GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
/* GPIOD[0-31] */
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
/* GPIOE[0-31] */
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0, GPIO_ALT0,
GPIO_ALT0, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1, GPIO_ALT1
};

/**
 * @brief   Hold one callback function pointer for each interrupt line
 */
static gpio_isr_ctx_t exti_chan[GPIO_ISR_CHAN_NUMOF];

void gpio_isr_handler(uint8_t gpio)
{
    exti_chan[gpio].cb(exti_chan[gpio].arg);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /* Set alternative gpio mode */
    s5p4418_gpio_set_alt(pin, gpio_alt_tbl[pin]);

    /* GPIO mode setting */
    switch(mode)
    {
        case GPIO_IN:
           s5p4418_gpio_set_dir(pin, GPIO_DIR_IN);
           s5p4418_gpio_set_pull(pin, GPIO_PULL_NONE);
        break;

        case GPIO_IN_PD:
           s5p4418_gpio_set_dir(pin, GPIO_DIR_IN);
           s5p4418_gpio_set_pull(pin, GPIO_PULL_DOWN);
        break;
        
        case GPIO_IN_PU:
           s5p4418_gpio_set_dir(pin, GPIO_DIR_IN);
           s5p4418_gpio_set_pull(pin, GPIO_PULL_UP);
        break;
        
        case GPIO_OUT:
           s5p4418_gpio_set_dir(pin, GPIO_DIR_OUT);
           s5p4418_gpio_set_drv(pin, GPIO_DRV_MEDIUM);
           s5p4418_gpio_set_rate(pin, GPIO_RATE_NORM);
        break;

        default:
            printf("%s : unsupported gpio mode %d\r\n",__FUNCTION__, mode);
            return -1;
        break;
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                   gpio_cb_t cb, void *arg)
{
    bool status = false;
 
    /* Init GPIO */
    if(0 != gpio_init(pin, mode))
    {
        return -1;
    }
 
    /* Set pull up */
    s5p4418_gpio_set_pull(pin, GPIO_PULL_UP);
 
    switch(flank)
    {
        case GPIO_FALLING:
            status = s5p4418_gpio_irq_register(pin, GPIO_IRQ_FALLING, &gpio_isr_handler);
        break;
        
        case GPIO_RISING:
            status = s5p4418_gpio_irq_register(pin, GPIO_IRQ_RISING, &gpio_isr_handler);
        break;
        
        case GPIO_BOTH:
            status = s5p4418_gpio_irq_register(pin, GPIO_IRQ_BOTH, &gpio_isr_handler);
        break;
        
        default:
        break;
    }

    if(status == false)
    {
        printf("%s : failed\r\n",__FUNCTION__);
    }
    else
    {
        /* save external IRQ context */
        exti_chan[pin].cb = cb;
        exti_chan[pin].arg = arg;

        /* enable gpio irq */
        s5p4418_gpio_irq_enable(pin);
    }

    return (status == true ? 0 : -1);
}

void gpio_irq_enable(gpio_t pin)
{
    s5p4418_gpio_irq_enable(pin);
}

void gpio_irq_disable(gpio_t pin)
{
    s5p4418_gpio_irq_disable(pin);
}

int gpio_read(gpio_t pin)
{
   return (int)s5p4418_gpio_get(pin);
}

void gpio_set(gpio_t pin)
{
    s5p4418_gpio_set(pin, true);
}

void gpio_clear(gpio_t pin)
{
    s5p4418_gpio_set(pin, false);
}

void gpio_toggle(gpio_t pin)
{
    bool ret = s5p4418_gpio_get(pin);
    
    if(ret)
    {
        s5p4418_gpio_set(pin, false);
    }
    else
    {
        s5p4418_gpio_set(pin, true);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if(value > 0)
    {
        s5p4418_gpio_set(pin, true);
    }
    else
    {
        s5p4418_gpio_set(pin, false);
    }
}
