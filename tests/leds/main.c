/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for the on-board LED macros
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/gpio.h"

// #include "gd32e23x_gpio.h"

#define DELAY_SHORT         (coreclk() / 50)
#define DELAY_LONG          (DELAY_SHORT * 4)

void dumb_delay(uint32_t delay)
{
    for (uint32_t i = 0; i < delay; i++) {
        __asm__("nop");
    }
}

int main(void)
{

//code below works
//make gpio_inits work by themselves
//make init done by saul work like these...

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_init(BTN0_PIN, GPIO_IN);
    gpio_init(BTN1_PIN, GPIO_IN);

    // uint32_t ctl = GPIO_CTL(GPIOA);
    // uint32_t pupd = GPIO_PUD(GPIOA);
    // for(uint16_t i = 0U;i < 16U;i++){
    //     if(((1U << i) & LED0_MASK) ||
    //        ((1U << i) & LED1_MASK) ||
    //        ((1U << i) & LED2_MASK) ||
    //        ((1U << i) & LED3_MASK))
    //     {
    //         ctl &= ~GPIO_MODE_MASK(i);
    //         ctl |= GPIO_MODE_SET(i, GPIO_MODE_OUTPUT);
    //         pupd &= ~GPIO_PUPD_MASK(i);
    //         pupd |= GPIO_PUPD_SET(i, GPIO_PUPD_NONE);
    //     }
    //     if((1U << i) & BTN0_MASK)
    //     {
    //         ctl &= ~GPIO_MODE_MASK(i);
    //         ctl |= GPIO_MODE_SET(i, GPIO_MODE_INPUT);
    //         pupd &= ~GPIO_PUPD_MASK(i);
    //         pupd |= GPIO_PUPD_SET(i, GPIO_PUPD_NONE);
    //     }
    // }

    // GPIO_CTL(GPIOA) = ctl;
    // GPIO_PUD(GPIOA) = pupd;
    // if(GPIO_OTYPE_OD == GPIO_OTYPE_PP){
    //     GPIO_OMODE(GPIOA) |= (uint32_t)LED0_MASK;
    // }else{
    //     GPIO_OMODE(GPIOA) &= (uint32_t)(~LED0_MASK);
    // }
    // uint32_t ospeed = GPIO_OSPD(GPIOA);
    // for(uint16_t i = 0U;i < 16U;i++){
    //     if(((1U << i) & LED0_MASK) ||
    //        ((1U << i) & LED1_MASK) ||
    //        ((1U << i) & LED2_MASK) ||
    //        ((1U << i) & LED3_MASK))
    //     {
    //         ospeed &= ~GPIO_OSPEED_MASK(i);
    //         ospeed |= GPIO_OSPEED_SET(i,GPIO_OSPEED_50MHZ);
    //     }
    // }
    // GPIO_OSPD(GPIOA) = ospeed;


    int numof = 0;

    /* get the number of available LED's and turn them all off*/
#ifdef LED0_ON
    ++numof;
    LED0_OFF;
#endif
#ifdef LED1_ON
    ++numof;
    LED1_OFF;
#endif
#ifdef LED2_ON
    ++numof;
    LED2_OFF;
#endif
#ifdef LED3_ON
    ++numof;
    LED3_OFF;
#endif
#ifdef LED4_ON
    ++numof;
    LED4_OFF;
#endif
#ifdef LED5_ON
    ++numof;
    LED5_OFF;
#endif
#ifdef LED6_ON
    ++numof;
    LED6_OFF;
#endif
#ifdef LED7_ON
    ++numof;
    LED7_OFF;
#endif

    puts("On-board LED test\n");
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: board-dependent ifdefs) */
    if (numof == 0) {
        puts("NO LEDs AVAILABLE");
    }
    else {
        printf("Available LEDs: %i\n\n", numof);
        puts("Will now light up each LED once short and twice long");
    }

    for (unsigned i = 0; i < 4; ++i) {
#ifdef LED0_ON
        LED0_ON;
        dumb_delay(DELAY_LONG);
        LED0_OFF;
        dumb_delay(DELAY_LONG);
        LED0_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED0_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED0_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED0_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED1_ON
        LED1_ON;
        dumb_delay(DELAY_LONG);
        LED1_OFF;
        dumb_delay(DELAY_LONG);
        LED1_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED1_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED1_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED1_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED2_ON
        LED2_ON;
        dumb_delay(DELAY_LONG);
        LED2_OFF;
        dumb_delay(DELAY_LONG);
        LED2_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED2_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED2_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED2_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED3_ON
        LED3_ON;
        dumb_delay(DELAY_LONG);
        LED3_OFF;
        dumb_delay(DELAY_LONG);
        LED3_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED3_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED3_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED3_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED4_ON
        LED4_ON;
        dumb_delay(DELAY_LONG);
        LED4_OFF;
        dumb_delay(DELAY_LONG);
        LED4_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED4_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED4_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED4_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED5_ON
        LED5_ON;
        dumb_delay(DELAY_LONG);
        LED5_OFF;
        dumb_delay(DELAY_LONG);
        LED5_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED5_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED5_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED5_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED6_ON
        LED6_ON;
        dumb_delay(DELAY_LONG);
        LED6_OFF;
        dumb_delay(DELAY_LONG);
        LED6_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED6_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED6_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED6_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
#ifdef LED7_ON
        LED7_ON;
        dumb_delay(DELAY_LONG);
        LED7_OFF;
        dumb_delay(DELAY_LONG);
        LED7_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED7_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED7_TOGGLE;
        dumb_delay(DELAY_SHORT);
        LED7_TOGGLE;
        dumb_delay(DELAY_LONG);
#endif
    }

    puts("Mapping each LED to a button (if available)");

    while (1) {
#if defined(LED0_PIN) && defined(BTN0_PIN)
        gpio_write(LED0_PIN, gpio_read(BTN0_PIN));
#endif
#if defined(LED1_PIN) && defined(BTN1_PIN)
        gpio_write(LED1_PIN, gpio_read(BTN1_PIN));
#endif
#if defined(LED2_PIN) && defined(BTN2_PIN)
        gpio_write(LED2_PIN, gpio_read(BTN2_PIN));
#endif
#if defined(LED3_PIN) && defined(BTN3_PIN)
        gpio_write(LED3_PIN, gpio_read(BTN3_PIN));
#endif
#if defined(LED4_PIN) && defined(BTN4_PIN)
        gpio_write(LED4_PIN, gpio_read(BTN4_PIN));
#endif
#if defined(LED5_PIN) && defined(BTN5_PIN)
        gpio_write(LED5_PIN, gpio_read(BTN5_PIN));
#endif
#if defined(LED6_PIN) && defined(BTN6_PIN)
        gpio_write(LED6_PIN, gpio_read(BTN6_PIN));
#endif
#if defined(LED7_PIN) && defined(BTN7_PIN)
        gpio_write(LED7_PIN, gpio_read(BTN7_PIN));
#endif
    }

    return 0;
}
