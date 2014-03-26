/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *
 * This file is subject to the terms and conditions of the GPLv2 License. 
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver periph
 * @{
 * 
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * 
 * @}
 */

#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_exti.h"

#include "periph/gpio.h"
#include "periph_conf.h"

typedef struct {
    uint8_t (*read)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    void (*cb)(void);
} gpio_state_t;

static inline void irq_handler(gpio_t dev);
static void init(gpio_t dev, gpio_pp_t pushpull, GPIO_InitTypeDef *config);

static gpio_state_t config[GPIO_NUMOF];


void gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    config[dev].read = &GPIO_ReadOutputDataBit;
    GPIO_InitTypeDef gpio_conf;
    gpio_conf.GPIO_Mode = GPIO_Mode_OUT;
    init(dev, pushpull, &gpio_conf);
}

void gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    config[dev].read = &GPIO_ReadInputDataBit;
    GPIO_InitTypeDef gpio_conf;
    gpio_conf.GPIO_Mode = GPIO_Mode_IN;
    init(dev, pushpull, &gpio_conf);
}

void gpio_init_int(gpio_t dev, gpio_pp_t pushpull, gpio_flank_t flank, void (*cb)(void))
{
    // set callback
    config[dev].cb = cb;

    gpio_init_in(dev, pushpull);

    EXTI_InitTypeDef exti;
    NVIC_InitTypeDef nvic;

    // config the exti mode
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    switch (flank) {
        case GPIO_RISING:
            exti.EXTI_Trigger = EXTI_Trigger_Rising;
            break;
        case GPIO_FALLING:
            exti.EXTI_Trigger = EXTI_Trigger_Falling;
            break;
        case GPIO_BOTH:
            exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
            break;
    }
    exti.EXTI_LineCmd = ENABLE;

    // nvic setup
    nvic.NVIC_IRQChannelPreemptionPriority = GPIO_IRQ_PRIO;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    // define pin spedific options
    switch (dev) {
        case GPIO_0:
            GPIO_0_EXTI_MAP();
            exti.EXTI_Line = GPIO_0_LINE;
            nvic.NVIC_IRQChannel = GPIO_0_IRQ;
            break;
        case GPIO_1:
            GPIO_1_EXTI_MAP();
            exti.EXTI_Line = GPIO_1_LINE;
            nvic.NVIC_IRQChannel = GPIO_1_IRQ;
            break;
        case GPIO_2:
            GPIO_2_EXTI_MAP();
            exti.EXTI_Line = GPIO_2_LINE;
            nvic.NVIC_IRQChannel = GPIO_2_IRQ;
            break;
        case GPIO_3:
            GPIO_3_EXTI_MAP();
            exti.EXTI_Line = GPIO_3_LINE;
            nvic.NVIC_IRQChannel = GPIO_3_IRQ;
            break;
        case GPIO_4:
            GPIO_4_EXTI_MAP();
            exti.EXTI_Line = GPIO_4_LINE;
            nvic.NVIC_IRQChannel = GPIO_4_IRQ;
            break;
        case GPIO_5:
            GPIO_5_EXTI_MAP();
            exti.EXTI_Line = GPIO_5_LINE;
            nvic.NVIC_IRQChannel = GPIO_5_IRQ;
            break;
        case GPIO_6:
            GPIO_6_EXTI_MAP();
            exti.EXTI_Line = GPIO_6_LINE;
            nvic.NVIC_IRQChannel = GPIO_6_IRQ;
            break;
        case GPIO_7:
            GPIO_7_EXTI_MAP();
            exti.EXTI_Line = GPIO_7_LINE;
            nvic.NVIC_IRQChannel = GPIO_7_IRQ;
            break;
        case GPIO_8:
            GPIO_8_EXTI_MAP();
            exti.EXTI_Line = GPIO_8_LINE;
            nvic.NVIC_IRQChannel = GPIO_8_IRQ;
            break;
        case GPIO_9:
            GPIO_9_EXTI_MAP();
            exti.EXTI_Line = GPIO_9_LINE;
            nvic.NVIC_IRQChannel = GPIO_9_IRQ;
            break;
        case GPIO_10:
            GPIO_10_EXTI_MAP();
            exti.EXTI_Line = GPIO_10_LINE;
            nvic.NVIC_IRQChannel = GPIO_10_IRQ;
            break;
        case GPIO_11:
            GPIO_11_EXTI_MAP();
            exti.EXTI_Line = GPIO_11_LINE;
            nvic.NVIC_IRQChannel = GPIO_11_IRQ;
            break;
        case GPIO_12:
            GPIO_12_EXTI_MAP();
            exti.EXTI_Line = GPIO_12_LINE;
            nvic.NVIC_IRQChannel = GPIO_12_IRQ;
            break;
        case GPIO_13:
            GPIO_13_EXTI_MAP();
            exti.EXTI_Line = GPIO_13_LINE;
            nvic.NVIC_IRQChannel = GPIO_13_IRQ;
            break;
        case GPIO_14:
            GPIO_14_EXTI_MAP();
            exti.EXTI_Line = GPIO_14_LINE;
            nvic.NVIC_IRQChannel = GPIO_14_IRQ;
            break;
        case GPIO_15:
            GPIO_15_EXTI_MAP();
            exti.EXTI_Line = GPIO_15_LINE;
            nvic.NVIC_IRQChannel = GPIO_15_IRQ;
            break;
    }

    EXTI_Init(&exti);
    NVIC_Init(&nvic);
}

static void init(gpio_t dev, gpio_pp_t pushpull, GPIO_InitTypeDef *config)
{
    config->GPIO_Speed = GPIO_Speed_50MHz;
    config->GPIO_OType = GPIO_OType_PP;
    switch (pushpull) {
        case GPIO_PULLDOWN:
            config->GPIO_PuPd = GPIO_PuPd_DOWN;
            break;
        case GPIO_PULLUP:
            config->GPIO_PuPd = GPIO_PuPd_UP;
            break;
        case GPIO_NOPULL:
            config->GPIO_PuPd = GPIO_PuPd_NOPULL;
            break;
    }
    switch (dev) {
        case GPIO_0:
            GPIO_0_CLKEN();
            config->GPIO_Pin = GPIO_0_PIN;
            GPIO_Init(GPIO_0_PORT, config);
            break;
        case GPIO_1:
            GPIO_1_CLKEN();
            config->GPIO_Pin = GPIO_1_PIN;
            GPIO_Init(GPIO_1_PORT, config);
            break;
        case GPIO_2:
            GPIO_2_CLKEN();
            config->GPIO_Pin = GPIO_2_PIN;
            GPIO_Init(GPIO_2_PORT, config);
            break;
        case GPIO_3:
            GPIO_3_CLKEN();
            config->GPIO_Pin = GPIO_3_PIN;
            GPIO_Init(GPIO_3_PORT, config);
            break;
        case GPIO_4:
            GPIO_4_CLKEN();
            config->GPIO_Pin = GPIO_4_PIN;
            GPIO_Init(GPIO_4_PORT, config);
            break;
        case GPIO_5:
            GPIO_5_CLKEN();
            config->GPIO_Pin = GPIO_5_PIN;
            GPIO_Init(GPIO_5_PORT, config);
            break;
        case GPIO_6:
            GPIO_6_CLKEN();
            config->GPIO_Pin = GPIO_6_PIN;
            GPIO_Init(GPIO_6_PORT, config);
            break;
        case GPIO_7:
            GPIO_7_CLKEN();
            config->GPIO_Pin = GPIO_7_PIN;
            GPIO_Init(GPIO_7_PORT, config);
            break;
        case GPIO_8:
            GPIO_8_CLKEN();
            config->GPIO_Pin = GPIO_8_PIN;
            GPIO_Init(GPIO_8_PORT, config);
            break;
        case GPIO_9:
            GPIO_9_CLKEN();
            config->GPIO_Pin = GPIO_9_PIN;
            GPIO_Init(GPIO_9_PORT, config);
            break;
        case GPIO_10:
            GPIO_10_CLKEN();
            config->GPIO_Pin = GPIO_10_PIN;
            GPIO_Init(GPIO_10_PORT, config);
            break;
        case GPIO_11:
            GPIO_11_CLKEN();
            config->GPIO_Pin = GPIO_11_PIN;
            GPIO_Init(GPIO_11_PORT, config);
            break;
        case GPIO_12:
            GPIO_12_CLKEN();
            config->GPIO_Pin = GPIO_12_PIN;
            GPIO_Init(GPIO_12_PORT, config);
            break;
        case GPIO_13:
            GPIO_13_CLKEN();
            config->GPIO_Pin = GPIO_13_PIN;
            GPIO_Init(GPIO_13_PORT, config);
            break;
        case GPIO_14:
            GPIO_14_CLKEN();
            config->GPIO_Pin = GPIO_14_PIN;
            GPIO_Init(GPIO_14_PORT, config);
            break;
        case GPIO_15:
            GPIO_15_CLKEN();
            config->GPIO_Pin = GPIO_15_PIN;
            GPIO_Init(GPIO_15_PORT, config);
            break;
    }
}

int gpio_read(gpio_t dev)
{
    switch (dev) {
        case GPIO_0:
            return (int)config[dev].read(GPIO_0_PORT, GPIO_0_PIN);
        case GPIO_1:
            return (int)config[dev].read(GPIO_1_PORT, GPIO_1_PIN);
        case GPIO_2:
            return (int)config[dev].read(GPIO_2_PORT, GPIO_2_PIN);
        case GPIO_3:
            return (int)config[dev].read(GPIO_3_PORT, GPIO_3_PIN);
        case GPIO_4:
            return (int)config[dev].read(GPIO_4_PORT, GPIO_4_PIN);
        case GPIO_5:
            return (int)config[dev].read(GPIO_5_PORT, GPIO_5_PIN);
        case GPIO_6:
            return (int)config[dev].read(GPIO_6_PORT, GPIO_6_PIN);
        case GPIO_7:
            return (int)config[dev].read(GPIO_7_PORT, GPIO_7_PIN);
        case GPIO_8:
            return (int)config[dev].read(GPIO_8_PORT, GPIO_8_PIN);
        case GPIO_9:
            return (int)config[dev].read(GPIO_9_PORT, GPIO_9_PIN);
        case GPIO_10:
            return (int)config[dev].read(GPIO_10_PORT, GPIO_10_PIN);
        case GPIO_11:
            return (int)config[dev].read(GPIO_11_PORT, GPIO_11_PIN);
        case GPIO_12:
            return (int)config[dev].read(GPIO_12_PORT, GPIO_12_PIN);
        case GPIO_13:
            return (int)config[dev].read(GPIO_13_PORT, GPIO_13_PIN);
        case GPIO_14:
            return (int)config[dev].read(GPIO_14_PORT, GPIO_14_PIN);
        case GPIO_15:
            return (int)config[dev].read(GPIO_15_PORT, GPIO_15_PIN);
    }
    return -1;
}

void gpio_set(gpio_t dev)
{
    switch (dev) {
        case GPIO_0:
            GPIO_SetBits(GPIO_0_PORT, GPIO_0_PIN);
            break;
        case GPIO_1:
            GPIO_SetBits(GPIO_1_PORT, GPIO_1_PIN);
            break;
        case GPIO_2:
            GPIO_SetBits(GPIO_2_PORT, GPIO_2_PIN);
            break;
        case GPIO_3:
            GPIO_SetBits(GPIO_3_PORT, GPIO_3_PIN);
            break;
        case GPIO_4:
            GPIO_SetBits(GPIO_4_PORT, GPIO_4_PIN);
            break;
        case GPIO_5:
            GPIO_SetBits(GPIO_5_PORT, GPIO_5_PIN);
            break;
        case GPIO_6:
            GPIO_SetBits(GPIO_6_PORT, GPIO_6_PIN);
            break;
        case GPIO_7:
            GPIO_SetBits(GPIO_7_PORT, GPIO_7_PIN);
            break;
        case GPIO_8:
            GPIO_SetBits(GPIO_8_PORT, GPIO_8_PIN);
            break;
        case GPIO_9:
            GPIO_SetBits(GPIO_9_PORT, GPIO_9_PIN);
            break;
        case GPIO_10:
            GPIO_SetBits(GPIO_10_PORT, GPIO_10_PIN);
            break;
        case GPIO_11:
            GPIO_SetBits(GPIO_11_PORT, GPIO_11_PIN);
            break;
        case GPIO_12:
            GPIO_SetBits(GPIO_12_PORT, GPIO_12_PIN);
            break;
        case GPIO_13:
            GPIO_SetBits(GPIO_13_PORT, GPIO_13_PIN);
            break;
        case GPIO_14:
            GPIO_SetBits(GPIO_14_PORT, GPIO_14_PIN);
            break;
        case GPIO_15:
            GPIO_SetBits(GPIO_15_PORT, GPIO_15_PIN);
            break;
    }
}

void gpio_clear(gpio_t dev)
{
    switch (dev) {
        case GPIO_0:
            GPIO_ResetBits(GPIO_0_PORT, GPIO_0_PIN);
            break;
        case GPIO_1:
            GPIO_ResetBits(GPIO_1_PORT, GPIO_1_PIN);
            break;
        case GPIO_2:
            GPIO_ResetBits(GPIO_2_PORT, GPIO_2_PIN);
            break;
        case GPIO_3:
            GPIO_ResetBits(GPIO_3_PORT, GPIO_3_PIN);
            break;
        case GPIO_4:
            GPIO_ResetBits(GPIO_4_PORT, GPIO_4_PIN);
            break;
        case GPIO_5:
            GPIO_ResetBits(GPIO_5_PORT, GPIO_5_PIN);
            break;
        case GPIO_6:
            GPIO_ResetBits(GPIO_6_PORT, GPIO_6_PIN);
            break;
        case GPIO_7:
            GPIO_ResetBits(GPIO_7_PORT, GPIO_7_PIN);
            break;
        case GPIO_8:
            GPIO_ResetBits(GPIO_8_PORT, GPIO_8_PIN);
            break;
        case GPIO_9:
            GPIO_ResetBits(GPIO_9_PORT, GPIO_9_PIN);
            break;
        case GPIO_10:
            GPIO_ResetBits(GPIO_10_PORT, GPIO_10_PIN);
            break;
        case GPIO_11:
            GPIO_ResetBits(GPIO_11_PORT, GPIO_11_PIN);
            break;
        case GPIO_12:
            GPIO_ResetBits(GPIO_12_PORT, GPIO_12_PIN);
            break;
        case GPIO_13:
            GPIO_ResetBits(GPIO_13_PORT, GPIO_13_PIN);
            break;
        case GPIO_14:
            GPIO_ResetBits(GPIO_14_PORT, GPIO_14_PIN);
            break;
        case GPIO_15:
            GPIO_ResetBits(GPIO_15_PORT, GPIO_15_PIN);
            break;
    }
}


void gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        gpio_clear(dev);
    } else {
        gpio_set(dev);
    }
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    } else {
        gpio_clear(dev);
    }
}


static inline void irq_handler(gpio_t dev) 
{
    config[dev].cb();
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) == SET) {
        irq_handler(GPIO_IRQ_0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) == SET) {
        irq_handler(GPIO_IRQ_1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) == SET) {
        irq_handler(GPIO_IRQ_2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) == SET) {
        irq_handler(GPIO_IRQ_3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) == SET) {
        irq_handler(GPIO_IRQ_4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) == SET) {
        irq_handler(GPIO_IRQ_5);
        EXTI_ClearITPendingBit(EXTI_Line5);
    } else if (EXTI_GetITStatus(EXTI_Line6) == SET) {
        irq_handler(GPIO_IRQ_6);
        EXTI_ClearITPendingBit(EXTI_Line6);
    } else if (EXTI_GetITStatus(EXTI_Line7) == SET) {
        irq_handler(GPIO_IRQ_7);
        EXTI_ClearITPendingBit(EXTI_Line7);
    } else if (EXTI_GetITStatus(EXTI_Line8) == SET) {
        irq_handler(GPIO_IRQ_8);
        EXTI_ClearITPendingBit(EXTI_Line8);
    } else if (EXTI_GetITStatus(EXTI_Line9) == SET) {
        irq_handler(GPIO_IRQ_9);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line10)  == SET) {
        irq_handler(GPIO_IRQ_10);
        EXTI_ClearITPendingBit(EXTI_Line10);
    } else if (EXTI_GetITStatus(EXTI_Line11) == SET) {
        irq_handler(GPIO_IRQ_11);
        EXTI_ClearITPendingBit(EXTI_Line11);
    } else if (EXTI_GetITStatus(EXTI_Line12) == SET) {
        irq_handler(GPIO_IRQ_12);
        EXTI_ClearITPendingBit(EXTI_Line12);
    } else if (EXTI_GetITStatus(EXTI_Line13) == SET) {
        irq_handler(GPIO_IRQ_13);
        EXTI_ClearITPendingBit(EXTI_Line13);
    } else if (EXTI_GetITStatus(EXTI_Line14) == SET) {
        irq_handler(GPIO_IRQ_14);
        EXTI_ClearITPendingBit(EXTI_Line14);
    } else if (EXTI_GetITStatus(EXTI_Line15) == SET) {
        irq_handler(GPIO_IRQ_15);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}
