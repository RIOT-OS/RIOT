/*
 * Copyright (C) 2014 Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation for SAMR21
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "port_util.h"



#define ENABLE_DEBUG (1)
#include "debug.h"
#include <unistd.h>
/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

static gpio_state_t gpio_config[GPIO_NUMOF];

//simplifing getting the gpio number from gpio_t
static uint32_t port_get_gpio_pin(gpio_t dev)
{
    uint32_t pin = 0;
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
               pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
             pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
             pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
             pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
             pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
             pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
             pin = GPIO_15_PIN;
            break;
#endif
    }
    return pin;
}
//Helper function to map devices to External interrupts
/*
*   Returns -1 for gpio without extint
*/
static int32_t get_extint(gpio_t dev)
{
    int32_t extint = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            extint = GPIO_0_EXTINT;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            extint = GPIO_1_EXTINT;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            extint = GPIO_2_EXTINT;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            extint = GPIO_3_EXTINT;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            extint = GPIO_4_EXTINT;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            extint = GPIO_5_EXTINT;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            extint = GPIO_6_EXTINT;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            extint = GPIO_7_EXTINT;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            extint = GPIO_8_EXTINT;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            extint = GPIO_9_EXTINT;
            break;
#endif
#if GPIO_10_ENN
        case GPIO_10:
            extint = GPIO_10_EXTINT;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            extint = GPIO_11_EXTINT;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            extint = GPIO_12_EXTINT;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            extint = GPIO_13_EXTINT;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            extint = GPIO_14_EXTINT;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            extint = GPIO_15_EXTINT;
            break;
#endif
        default:
            extint = -1;
    }
    return extint; 
}

int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    uint32_t pin = port_get_gpio_pin(dev);

    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->DIRSET.reg = (1 << (pin % 32));

    /* configure the pin's pull resistor state */
    //TODO: Does this matter in output mode?
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;
        case GPIO_PULLUP:
            group->PINCFG[pin % 32].bit.PULLEN = true;
            break;
        case GPIO_NOPULL:
            group->PINCFG[pin % 32].bit.PULLEN = false;
            break;
    }

    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    uint32_t pin = port_get_gpio_pin(dev);
    /* configure pin as input */
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->DIRCLR.reg = (1 << (pin % 32)); //write a 1 to clear the bit in dir
    group->PINCFG[pin % 32].bit.INEN = true;
    /* configure the pin's pull resistor state */
    switch (pushpull) {
        //To enable PullDown, set out = 1, and pullen = 1
        case GPIO_PULLDOWN:
            group->OUTSET.reg = (1 << (pin % 32));
            group->PINCFG[pin % 32].bit.PULLEN = true;
            break;
        case GPIO_PULLUP:
            group->OUTCLR.reg = (1 << (pin % 32));
            group->PINCFG[pin % 32].bit.PULLEN = true;
            break;
        case GPIO_NOPULL:
            group->PINCFG[pin % 32].bit.PULLEN = false;
            break;
    }

    return 0;
}
/* There are 16 external EXTINT pins*/
int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    /* TODO: implement */
    int res;
    uint32_t pin = port_get_gpio_pin(dev);
    int32_t extint = get_extint(dev);
    if(extint < 0)
    {
        return -1;
    }

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }
    #define EIC_FUNCTION 0
    pmux_set(pin, EIC_FUNCTION); //set pinmux to eic function

    /* Turn on APB clock */
    PM->APBAMASK.reg |= PM_APBAMASK_EIC; //Cannot use PM->APBAMASK.bit.EIC because of defines

    /* Enable GCLK_EIC, Init process is: */
    //Write to gendiv, no division needed
    //Write to genctrl
    //Write to clkctrl
    //Maybe create function for this
    
    GCLK_CLKCTRL_Type clkctrl = 
    {
        .bit.ID = EIC_GCLK_ID,
        .bit.GEN = 0, //Generator 0
        .bit.CLKEN = 1,
        .bit.WRTLOCK = 0
    };
    GCLK->CLKCTRL = clkctrl;

    /* Setup interrupt */
    NVIC_SetPriority(EIC_IRQn, 0); //TODO: Come up with a sensible prio, now highest
    NVIC_EnableIRQ(EIC_IRQn);

    /* save callback */
    gpio_config[dev].cb = cb;
    gpio_config[dev].arg = arg;



    /*Set config */
    uint32_t config_pos = (4 * (extint % 8));
    uint32_t config_reg    = extint / 8;

    /*Set flank detection */
    switch (flank) {
        case GPIO_FALLING:
            EIC->CONFIG[config_reg].reg 
                                |= (EIC_CONFIG_SENSE0_FALL_Val << (config_pos));
            break;
        case GPIO_RISING:
            EIC->CONFIG[config_reg].reg 
                        |= (EIC_CONFIG_SENSE0_RISE_Val << (config_pos));
            break;
        case GPIO_BOTH:
            EIC->CONFIG[config_reg].reg 
                        |= (EIC_CONFIG_SENSE0_BOTH_Val << (config_pos));
            break;
    }

    
    /*Enable pin interrupt */
    EIC->INTENSET.reg = (1 << (extint));
    DEBUG("GPIO initialized\n");
    /*Enable external interrupts*/
    EIC->CTRL.bit.ENABLE = true;


    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    uint32_t extint = get_extint(dev);
    EIC->INTENSET.reg = (1 << extint);
}

void gpio_irq_disable(gpio_t dev)
{
    uint32_t extint = get_extint(dev);
    EIC->INTENCLR.reg = (1 << extint);
}

int gpio_read(gpio_t dev)
{
    uint32_t pin = port_get_gpio_pin(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    int result = group->IN.reg;
    return result;
}

void gpio_set(gpio_t dev)
{
    uint32_t pin = port_get_gpio_pin(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->OUTSET.reg = (1 << (pin %32)); //To set bit, set in set register
}

void gpio_clear(gpio_t dev)
{
    uint32_t pin = port_get_gpio_pin(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->OUTCLR.reg = (1 << (pin %32)); //To clear, set the bit in the clear register
}

void gpio_toggle(gpio_t dev)
{
    uint32_t pin = port_get_gpio_pin(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->OUTTGL.reg = (1 << (pin %32)); //To toggle, set bit in toggle register
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    } else {
        gpio_clear(dev);
    }
}
/** Handler for exint interrupts **/

void EIC_Handler(void)
{
    DEBUG("CALLBACK");
    //Find callback
    for(int i = 0; i < GPIO_NUMOF; ++i)
    {
        uint8_t extint = get_extint(i);

        if((extint != -1)  && (EIC->INTFLAG.vec.EXTINT & ( 1 << extint)))
        {
            gpio_config[i].cb(gpio_config[i].arg); //execute callback with args
        }
    }
}

#endif /* GPIO_NUMOF */
