/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *
 * This file is subject to the terms and conditions of the GPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

#define GPIO_PIN_CNF_IN (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)\
                                                    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)\
                                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)\
                                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)\
                                                    | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos)

#define GPIO_PIN_CNF_OUT (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)\
                                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)\
                                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)\
                                                   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos)

typedef struct {
    void (*cb)(void);
} gpio_state_t;

static gpio_state_t config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_DEV->PIN_CNF[GPIO_0] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_DEV->PIN_CNF[GPIO_1] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_DEV->PIN_CNF[GPIO_2] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_DEV->PIN_CNF[GPIO_3] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_DEV->PIN_CNF[GPIO_4] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_DEV->PIN_CNF[GPIO_5] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_DEV->PIN_CNF[GPIO_6] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_DEV->PIN_CNF[GPIO_7] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_DEV->PIN_CNF[GPIO_8] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_DEV->PIN_CNF[GPIO_9] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_DEV->PIN_CNF[GPIO_10] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_DEV->PIN_CNF[GPIO_11] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_DEV->PIN_CNF[GPIO_12] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_DEV->PIN_CNF[GPIO_13] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_DEV->PIN_CNF[GPIO_14] = GPIO_PIN_CNF_IN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_DEV->PIN_CNF[GPIO_15] = GPIO_PIN_CNF_IN;
            break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }
    switch (pullup) {
        case GPIO_PULLDOWN:
            GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);
            break;
        case GPIO_PULLUP:
            GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
            break;
        case GPIO_NOPULL:
            GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
            break;
    }


    return 1;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_DEV->PIN_CNF[dev] = GPIO_PIN_CNF_OUT;
            break;
#endif
     case GPIO_UNDEFINED:
        default:
            return -1;
    }

    /* configure dev as input and pullup as state */    
    switch (pullup) {
        case GPIO_PULLDOWN:
            GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);
            break;
        case GPIO_PULLUP:
            GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
            break;
        case GPIO_NOPULL:
            GPIO_DEV->PIN_CNF[dev] |=  (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
            break;
    }
    return 1;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, void (*cb)(void))
{

    /* triggering interrupt by edge */

    int res;
    uint32_t pin = 50;

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }


    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            pin = GPIO_0_PIN;
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            pin = GPIO_1_PIN;
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
        pin = GPIO_2_PIN;
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            pin = GPIO_3_PIN;
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            pin = GPIO_4_PIN;
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            pin = GPIO_5_PIN;
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            pin = GPIO_6_PIN;
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            pin = GPIO_7_PIN;
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            pin = GPIO_8_PIN;
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            pin = GPIO_9_PIN;
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            pin = GPIO_10_PIN;
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            pin = GPIO_11_PIN;
            break;
#endif
        case GPIO_UNDEFINED:
        default:
            return -1;
    }

   NVIC_SetPriority(GPIOTE_IRQn, GPIO_IRQ_PRIO);
   NVIC_EnableIRQ(GPIOTE_IRQn);
    /* save callback */
   config[dev].cb = cb;

    gpio_init_in(pin,pullup);
    GPIO_IRQ_DEV->CONFIG[0] =  (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos)
                           | (pin << GPIOTE_CONFIG_PSEL_Pos)
                           | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);

    GPIO_IRQ_DEV->INTENSET  = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;

    return 0;
}

int gpio_read(gpio_t dev)
{

    /* TODO: also implement this read function for pins who are configured as output */
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            if (GPIO_DEV->IN >> GPIO_0_PIN){
                return  ((GPIO_DEV->IN >> GPIO_0_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_0_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_0_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            if (GPIO_DEV->IN >> GPIO_1_PIN){
                return  ((GPIO_DEV->IN >> GPIO_1_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_1_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_1_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            if (GPIO_DEV->IN >> GPIO_2_PIN){
                return  ((GPIO_DEV->IN >> GPIO_2_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_2_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_2_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            if (GPIO_DEV->IN >> GPIO_3_PIN){
                return  ((GPIO_DEV->IN >> GPIO_3_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_3_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_3_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            if (GPIO_DEV->IN >> GPIO_4_PIN){
                return  ((GPIO_DEV->IN >> GPIO_4_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_4_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_4_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            if (GPIO_DEV->IN >> GPIO_5_PIN){
                return  ((GPIO_DEV->IN >> GPIO_5_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_5_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_5_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            if (GPIO_DEV->IN >> GPIO_6_PIN){
                return  ((GPIO_DEV->IN >> GPIO_6_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_6_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_6_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            if (GPIO_DEV->IN >> GPIO_7_PIN){
                return  ((GPIO_DEV->IN >> GPIO_7_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_7_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_7_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            if (GPIO_DEV->IN >> GPIO_8_PIN){
                return  ((GPIO_DEV->IN >> GPIO_8_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_8_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_8_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            if (GPIO_DEV->IN >> GPIO_9_PIN){
                return  ((GPIO_DEV->IN >> GPIO_9_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_9_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_9_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            if (GPIO_DEV->IN >> GPIO_10_PIN){
                return  ((GPIO_DEV->IN >> GPIO_10_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_10_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_10_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            if (GPIO_DEV->IN >> GPIO_11_PIN){
                return  ((GPIO_DEV->IN >> GPIO_11_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_11_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_11_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            if (GPIO_DEV->IN >> GPIO_12_PIN){
                return  ((GPIO_DEV->IN >> GPIO_12_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_12_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_12_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            if (GPIO_DEV->IN >> GPIO_13_PIN){
                return  ((GPIO_DEV->IN >> GPIO_13_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_13_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_13_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            if (GPIO_DEV->IN >> GPIO_14_PIN){
                return  ((GPIO_DEV->IN >> GPIO_14_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_14_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_14_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            if (GPIO_DEV->IN >> GPIO_15_PIN){
                return  ((GPIO_DEV->IN >> GPIO_15_PIN) & 1UL);
            } else if (GPIO_DEV->OUT  >> GPIO_15_PIN){
                return  ((GPIO_DEV->OUT >> GPIO_15_PIN) & 1UL);
            } else {
                return -1;
            }
            break;
#endif

        case GPIO_UNDEFINED:
           default:
               return -1;
       }
}

int gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_DEV->OUTSET = (1U << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_DEV->OUTSET = (1U << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_DEV->OUTSET = (1U << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_DEV->OUTSET = (1U << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_DEV->OUTSET = (1U << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_DEV->OUTSET = (1U << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_DEV->OUTSET = (1U << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_DEV->OUTSET = (1U << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_DEV->OUTSET = (1U << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_DEV->OUTSET = (1U << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_DEV->OUTSET = (1U << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_DEV->OUTSET = (1U << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_DEV->OUTSET = (1U << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_DEV->OUTSET = (1U << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_DEV->OUTSET = (1U << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_DEV->OUTSET = (1U << GPIO_15_PIN);
            break;
#endif

        case GPIO_UNDEFINED:
        default:
            return -1;
    }
    return 0;
}

int gpio_clear(gpio_t dev)
{
       switch (dev) {
    #if GPIO_0_EN
            case GPIO_0:
                GPIO_DEV->OUTCLR = (1 << GPIO_0_PIN);
                break;
    #endif
    #if GPIO_1_EN
            case GPIO_1:
                GPIO_DEV->OUTCLR = (1 << GPIO_1_PIN);
                break;
    #endif
    #if GPIO_2_EN
            case GPIO_2:
                GPIO_DEV->OUTCLR = (1 << GPIO_2_PIN);
                break;
    #endif
    #if GPIO_3_EN
            case GPIO_3:
                GPIO_DEV->OUTCLR = (1 << GPIO_3_PIN);
                break;
    #endif
    #if GPIO_4_EN
            case GPIO_4:
                GPIO_DEV->OUTCLR = (1 << GPIO_4_PIN);
                break;
    #endif
    #if GPIO_5_EN
            case GPIO_5:
                GPIO_DEV->OUTCLR = (1 << GPIO_5_PIN);
                break;
    #endif
    #if GPIO_6_EN
            case GPIO_6:
                GPIO_DEV->OUTCLR = (1 << GPIO_6_PIN);
                break;
    #endif
    #if GPIO_7_EN
            case GPIO_7:
                GPIO_DEV->OUTCLR = (1 << GPIO_7_PIN);
                break;
    #endif
    #if GPIO_8_EN
            case GPIO_8:
                GPIO_DEV->OUTCLR = (1 << GPIO_8_PIN);
                break;
    #endif
    #if GPIO_9_EN
            case GPIO_9:
                GPIO_DEV->OUTCLR = (1 << GPIO_9_PIN);
                break;
    #endif
    #if GPIO_10_EN
            case GPIO_10:
                GPIO_DEV->OUTCLR = (1 << GPIO_10_PIN);
                break;
    #endif
    #if GPIO_11_EN
            case GPIO_11:
                GPIO_DEV->OUTCLR = (1 << GPIO_11_PIN);
                break;
    #endif
    #if GPIO_12_EN
            case GPIO_12:
                GPIO_DEV->OUTCLR = (1 << GPIO_12_PIN);
                break;
    #endif
    #if GPIO_13_EN
            case GPIO_13:
                GPIO_DEV->OUTCLR = (1 << GPIO_13_PIN);
                break;
    #endif
    #if GPIO_14_EN
            case GPIO_14:
                GPIO_DEV->OUTCLR = (1 << GPIO_14_PIN);
                break;
    #endif
    #if GPIO_15_EN
            case GPIO_15:
                GPIO_DEV->OUTCLR = (1 << GPIO_15_PIN);
                break;
    #endif

            case GPIO_UNDEFINED:
            default:
                return -1;
        }
    return 0;
}


int gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        return gpio_clear(dev);
    } else {
        return gpio_set(dev);
    }
}

int gpio_write(gpio_t dev, int value)
{
    if (value) {
        return gpio_set(dev);
    } else {
        return gpio_clear(dev);
    }
}


__attribute__((naked)) void EXTI0_0_IRQn(void)
{
    ISR_ENTER();
        // Event causing the interrupt must be cleared.
        if ((GPIO_IRQ_DEV->EVENTS_IN[0] == 1) &&
            (GPIO_IRQ_DEV->INTENSET & GPIOTE_INTENSET_IN0_Msk))
        {
            GPIO_IRQ_DEV->EVENTS_IN[0] = 0;
        }

        config[GPIO_IRQ_0].cb();
        ISR_EXIT();
}

__attribute__((naked)) void EXTI0_1_IRQn(void)
{
    ISR_ENTER();
        // Event causing the interrupt must be cleared.
        if ((GPIO_IRQ_DEV->EVENTS_IN[0] == 1) &&
            (GPIO_IRQ_DEV->INTENSET & GPIOTE_INTENSET_IN0_Msk))
        {
            GPIO_IRQ_DEV->EVENTS_IN[0] = 0;
        }

        config[GPIO_IRQ_1].cb();
        ISR_EXIT();
}
