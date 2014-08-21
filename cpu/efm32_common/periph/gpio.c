/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#include "em_gpio.h"

#define GPIO_CLKEN()    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO

static inline void gpio_irq_handler(gpio_t dev);

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;
    void *arg;
} gpio_state_t;

static gpio_state_t gpio_config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_Port_TypeDef port = 0;
    uint32_t pin = 0;

    GPIO_CLKEN();

    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        port = GPIO_0_PORT;
        pin = GPIO_0_PIN;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        port = GPIO_1_PORT;
        pin = GPIO_1_PIN;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        port = GPIO_2_PORT;
        pin = GPIO_2_PIN;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        port = GPIO_3_PORT;
        pin = GPIO_3_PIN;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        port = GPIO_4_PORT;
        pin = GPIO_4_PIN;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        port = GPIO_5_PORT;
        pin = GPIO_5_PIN;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        port = GPIO_6_PORT;
        pin = GPIO_6_PIN;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        port = GPIO_7_PORT;
        pin = GPIO_7_PIN;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        port = GPIO_8_PORT;
        pin = GPIO_8_PIN;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        port = GPIO_9_PORT;
        pin = GPIO_9_PIN;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        port = GPIO_10_PORT;
        pin = GPIO_10_PIN;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        port = GPIO_11_PORT;
        pin = GPIO_11_PIN;
        break;
#endif
    }

    //TODO: Implement pullup
    GPIO_PinModeSet(port, pin, gpioModePushPull, 1);

    return 0; /* all OK */
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_Port_TypeDef port = 0;
    uint32_t pin = 0;

    GPIO_CLKEN();

    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        port = GPIO_0_PORT;
        pin = GPIO_0_PIN;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        port = GPIO_1_PORT;
        pin = GPIO_1_PIN;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        port = GPIO_2_PORT;
        pin = GPIO_2_PIN;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        port = GPIO_3_PORT;
        pin = GPIO_3_PIN;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        port = GPIO_4_PORT;
        pin = GPIO_4_PIN;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        port = GPIO_5_PORT;
        pin = GPIO_5_PIN;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        port = GPIO_6_PORT;
        pin = GPIO_6_PIN;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        port = GPIO_7_PORT;
        pin = GPIO_7_PIN;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        port = GPIO_8_PORT;
        pin = GPIO_8_PIN;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        port = GPIO_9_PORT;
        pin = GPIO_9_PIN;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        port = GPIO_10_PORT;
        pin = GPIO_10_PIN;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        port = GPIO_11_PORT;
        pin = GPIO_11_PIN;
        break;
#endif
    }

    if (pullup == GPIO_NOPULL) {
        GPIO_PinModeSet(port, pin, gpioModeInput, 0);

    } else if (pullup == GPIO_PULLUP) {
        GPIO_PinModeSet(port, pin, gpioModeInputPull, 0);
        GPIO_PinOutSet(port, pin);

    } else if (pullup == GPIO_PULLDOWN) {
        GPIO_PinModeSet(port, pin, gpioModeInputPull, 0);
        GPIO_PinOutClear(port, pin);
    }

    return 0; /* everything alright here */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    uint32_t pin = 0;
    GPIO_Port_TypeDef port = 0;

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    /* set interrupt priority (its the same for all EXTI interrupts) */
    NVIC_SetPriority(GPIO_ODD_IRQn, GPIO_IRQ_PRIO);
    NVIC_SetPriority(GPIO_EVEN_IRQn, GPIO_IRQ_PRIO);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    //NVIC_SetPriority(PCNT0_IRQn, GPIO_IRQ_PRIO);
    //NVIC_SetPriority(PCNT0_IRQn, GPIO_IRQ_PRIO);
    //NVIC_SetPriority(PCNT0_IRQn, GPIO_IRQ_PRIO);

    /* enable clock of the SYSCFG module for EXTI configuration */
    //RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

    /* read pin number, set EXIT channel and enable global interrupt for EXTI channel */
    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        pin = GPIO_0_PIN;
        port = GPIO_0_PORT;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        pin = GPIO_1_PIN;
        port = GPIO_1_PORT;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        pin = GPIO_2_PIN;
        port = GPIO_2_PORT;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        pin = GPIO_3_PIN;
        port = GPIO_3_PORT;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        pin = GPIO_4_PIN;
        port = GPIO_4_PORT;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        pin = GPIO_5_PIN;
        port = GPIO_5_PORT;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        pin = GPIO_6_PIN;
        port = GPIO_6_PORT;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        pin = GPIO_7_PIN;
        port = GPIO_7_PORT;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        pin = GPIO_8_PIN;
        port = GPIO_8_PORT;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        pin = GPIO_9_PIN;
        port = GPIO_9_PORT;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        pin = GPIO_10_PIN;
        port = GPIO_10_PORT;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        pin = GPIO_11_PIN;
        port = GPIO_11_PORT;
        break;
#endif
    }

    /* set callback */
    gpio_config[dev].cb = cb;
    gpio_config[dev].arg = arg;

    /* configure the event that triggers an interrupt */
    switch (flank) {
    case GPIO_RISING:
        GPIO_IntConfig(port, pin, true, false, true);
        break;
    case GPIO_FALLING:
        GPIO_IntConfig(port, pin, false, true, true);
        break;
    case GPIO_BOTH:
        GPIO_IntConfig(port, pin, true, true, true);
        break;
    }

    gpio_irq_enable(dev);

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_0_PIN, 1);
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_1_PIN, 1);
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_2_PIN, 1);
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_3_PIN, 1);
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_4_PIN, 1);
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_5_PIN, 1);
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_6_PIN, 1);
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_7_PIN, 1);
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_8_PIN, 1);
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_9_PIN, 1);
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_10_PIN , 1);
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_11_PIN, 1);
        break;
#endif
    }
}

void gpio_irq_disable(gpio_t dev)
{
    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_0_PIN, 0);
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_1_PIN, 0);
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_2_PIN, 0);
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_3_PIN, 0);
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_4_PIN, 0);
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_5_PIN, 0);
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_6_PIN, 0);
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_7_PIN, 0);
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_8_PIN, 0);
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_9_PIN, 0);
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_10_PIN , 0);
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        BITBAND_Peripheral(&(GPIO->IEN), GPIO_11_PIN, 0);
        break;
#endif
    }
}

int gpio_read(gpio_t dev)
{
    GPIO_Port_TypeDef port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        port = GPIO_0_PORT;
        pin = GPIO_0_PIN;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        port = GPIO_1_PORT;
        pin = GPIO_1_PIN;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        port = GPIO_2_PORT;
        pin = GPIO_2_PIN;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        port = GPIO_3_PORT;
        pin = GPIO_3_PIN;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        port = GPIO_4_PORT;
        pin = GPIO_4_PIN;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        port = GPIO_5_PORT;
        pin = GPIO_5_PIN;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        port = GPIO_6_PORT;
        pin = GPIO_6_PIN;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        port = GPIO_7_PORT;
        pin = GPIO_7_PIN;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        port = GPIO_8_PORT;
        pin = GPIO_8_PIN;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        port = GPIO_9_PORT;
        pin = GPIO_9_PIN;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        port = GPIO_10_PORT;
        pin = GPIO_10_PIN;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        port = GPIO_11_PORT;
        pin = GPIO_11_PIN;
        break;
#endif
    }

    if (GPIO->P[port].DOUT & (1 << pin)) {
        return GPIO_PinOutGet(port, pin);
    } else {
        return GPIO_PinInGet(port, pin);
    }
}

void gpio_set(gpio_t dev)
{
    GPIO_Port_TypeDef port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        port = GPIO_0_PORT;
        pin = GPIO_0_PIN;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        port = GPIO_1_PORT;
        pin = GPIO_1_PIN;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        port = GPIO_2_PORT;
        pin = GPIO_2_PIN;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        port = GPIO_3_PORT;
        pin = GPIO_3_PIN;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        port = GPIO_4_PORT;
        pin = GPIO_4_PIN;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        port = GPIO_5_PORT;
        pin = GPIO_5_PIN;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        port = GPIO_6_PORT;
        pin = GPIO_6_PIN;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        port = GPIO_7_PORT;
        pin = GPIO_7_PIN;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        port = GPIO_8_PORT;
        pin = GPIO_8_PIN;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        port = GPIO_9_PORT;
        pin = GPIO_9_PIN;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        port = GPIO_10_PORT;
        pin = GPIO_10_PIN;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        port = GPIO_11_PORT;
        pin = GPIO_11_PIN;
        break;
#endif
    }

    GPIO_PinOutSet(port, pin);
}

void gpio_clear(gpio_t dev)
{
    GPIO_Port_TypeDef port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        port = GPIO_0_PORT;
        pin = GPIO_0_PIN;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        port = GPIO_1_PORT;
        pin = GPIO_1_PIN;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        port = GPIO_2_PORT;
        pin = GPIO_2_PIN;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        port = GPIO_3_PORT;
        pin = GPIO_3_PIN;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        port = GPIO_4_PORT;
        pin = GPIO_4_PIN;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        port = GPIO_5_PORT;
        pin = GPIO_5_PIN;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        port = GPIO_6_PORT;
        pin = GPIO_6_PIN;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        port = GPIO_7_PORT;
        pin = GPIO_7_PIN;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        port = GPIO_8_PORT;
        pin = GPIO_8_PIN;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        port = GPIO_9_PORT;
        pin = GPIO_9_PIN;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        port = GPIO_10_PORT;
        pin = GPIO_10_PIN;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        port = GPIO_11_PORT;
        pin = GPIO_11_PIN;
        break;
#endif
    }

    GPIO_PinOutClear(port, pin);
}

void gpio_toggle(gpio_t dev)
{
    GPIO_Port_TypeDef port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
    case GPIO_0:
        port = GPIO_0_PORT;
        pin = GPIO_0_PIN;
        break;
#endif
#ifdef GPIO_1_EN
    case GPIO_1:
        port = GPIO_1_PORT;
        pin = GPIO_1_PIN;
        break;
#endif
#ifdef GPIO_2_EN
    case GPIO_2:
        port = GPIO_2_PORT;
        pin = GPIO_2_PIN;
        break;
#endif
#ifdef GPIO_3_EN
    case GPIO_3:
        port = GPIO_3_PORT;
        pin = GPIO_3_PIN;
        break;
#endif
#ifdef GPIO_4_EN
    case GPIO_4:
        port = GPIO_4_PORT;
        pin = GPIO_4_PIN;
        break;
#endif
#ifdef GPIO_5_EN
    case GPIO_5:
        port = GPIO_5_PORT;
        pin = GPIO_5_PIN;
        break;
#endif
#ifdef GPIO_6_EN
    case GPIO_6:
        port = GPIO_6_PORT;
        pin = GPIO_6_PIN;
        break;
#endif
#ifdef GPIO_7_EN
    case GPIO_7:
        port = GPIO_7_PORT;
        pin = GPIO_7_PIN;
        break;
#endif
#ifdef GPIO_8_EN
    case GPIO_8:
        port = GPIO_8_PORT;
        pin = GPIO_8_PIN;
        break;
#endif
#ifdef GPIO_9_EN
    case GPIO_9:
        port = GPIO_9_PORT;
        pin = GPIO_9_PIN;
        break;
#endif
#ifdef GPIO_10_EN
    case GPIO_10:
        port = GPIO_10_PORT;
        pin = GPIO_10_PIN;
        break;
#endif
#ifdef GPIO_11_EN
    case GPIO_11:
        port = GPIO_11_PORT;
        pin = GPIO_11_PIN;
        break;
#endif
    }
    GPIO_PinOutToggle(port, pin);
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    } else {
        gpio_clear(dev);
    }
}

static inline void gpio_irq_handler(gpio_t dev)
{
    gpio_config[dev].cb(gpio_config[dev].arg);
    
}


void GPIO_EVEN_IRQHandler(void)
{
    uint32_t iflags;

    ISR_ENTER();

    /* Get all even interrupts. */
    iflags = GPIO_IntGetEnabled() & 0x00005555;

    printf("even 0x%.8x\n", iflags);

    /* Call IRQ handlers */
#ifdef GPIO_0_EN
    if (iflags & (1 << GPIO_IRQ_0)) {
        //GPIO->IFC = (1 << GPIO_IRQ_0);
        printf("GPIO_0 int\n");
        gpio_irq_handler(GPIO_0);
    }
#endif
#ifdef GPIO_1_EN
    if (iflags & (1 << GPIO_IRQ_1)) {
        //GPIO->IFC = (1 << GPIO_IRQ_1);
        printf("GPIO_1 int\n");
        gpio_irq_handler(GPIO_1);
    }
#endif
#ifdef GPIO_2_EN
    if (iflags & (1 << GPIO_IRQ_2)) {
        //GPIO->IFC = (1 << GPIO_IRQ_2);
        printf("GPIO_2 int\n");
        gpio_irq_handler(GPIO_2);
    }
#endif

    /* Clean only even interrupts. */
    GPIO_IntClear(iflags);

    if (sched_context_switch_request) {
        thread_yield();
    }

    ISR_EXIT();
}


void GPIO_ODD_IRQHandler(void)
{
    uint32_t iflags;

    ISR_ENTER();

    /* Get all odd interrupts. */
    iflags = GPIO_IntGetEnabled() & 0x0000AAAA;

    printf("odd 0x%.8x\n", iflags);

    /* Call IRQ handlers */
#ifdef GPIO_0_EN
    if (iflags & (1 << GPIO_IRQ_0)) {
        //GPIO->IFC = (1 << GPIO_IRQ_0);
        printf("GPIO_0 int\n");
        gpio_irq_handler(GPIO_0);
    }
#endif
#ifdef GPIO_1_EN
    if (iflags & (1 << GPIO_IRQ_1)) {
        //GPIO->IFC = (1 << GPIO_IRQ_1);
        printf("GPIO_1 int\n");
        gpio_irq_handler(GPIO_1);
    }
#endif
#ifdef GPIO_2_EN
    if (iflags & (1 << GPIO_IRQ_2)) {
        //GPIO->IFC = (1 << GPIO_IRQ_2);
        printf("GPIO_2 int\n");
        gpio_irq_handler(GPIO_2);
    }
#endif

    /* Clean only even interrupts. */
    GPIO_IntClear(iflags);

    if (sched_context_switch_request) {
        thread_yield();
    }

    ISR_EXIT();
}


#endif /* GPIO_NUMOF */
