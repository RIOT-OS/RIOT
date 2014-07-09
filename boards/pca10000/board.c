/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_stm32f0discovery
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the STM32F0Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "periph/timer.h"
#include "board.h"
#include "cpu.h"
#include "nrf51.h"
//#include "nrf_delay.h"
//#include "nrf51_bitfields.h"
#include "periph/uart.h"

//#include "hwtimer_arch.h"
//#include "thread.h"
#include "periph/gpio.h"

//#include "radio.h"

extern void SystemInit(void);
void leds_init(void);

void printInt0(int i){
    printf("timer0: channel: %d\n",i);
}
void printInt1(int i){
    printf("timer1: channel: %d\n",i);
}
void printInt2(int i){
    printf("timer2: channel: %d\n",i);
}

void redoff(int i)  {
	LED_RED_OFF;
}



void board_init(void)
{
    int i = 0;
    /* initialize core clocks via STM-lib given function */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();

    /*initialize UART */
    uart_init_blocking(0, 115200);
    char* output = "Hello User!\r\n";
    char outputchar = output[i++];
    while ( outputchar != '\0')
    {
        uart_write_blocking(0, outputchar);
        outputchar = output[i++];
    }


    LED_GREEN_OFF;
    LED_RED_ON;
    LED_BLUE_OFF;

    //LED_BLUE_OFF;
    /*LED_BLUE_OFF;
    LED_GREEN_OFF;
    //LED_RED_OFF;
    gpio_init_out(GPIO_0, GPIO_NOPULL);
    gpio_set(GPIO_0);
    gpio_init_out(GPIO_1, GPIO_NOPULL);
    gpio_set(GPIO_1);*/
//    GPIO_DEV->OUTSET = (1UL << 1);

    i = 0;


    //timer_init(TIMER_0, 1, printInt0);
timer_init(TIMER_0, 1, redoff);
//
    timer_set(TIMER_0,0,2*1000*1000);
//    timer_set(TIMER_0,1,4*1000*1000);
//    timer_set(TIMER_0,2,6*1000*1000);



//    timer_set(TIMER_1,0,10*1000*1000);
//    timer_set(TIMER_1,1,11*1000*1000);
//    timer_set(TIMER_1,2,12*1000*1000);


	radioConfig(0);
	char msg = 'Q';
while(1) {

	// sending all the time some message

	sendPacket(1, msg);

	delay(3*1000*1000);
}


    /* blink stuff */

/*    while (1) {


        while(uart_read_blocking(0,&charUART))  {
                uart_write_blocking(0, charUART);
        }


        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_RED_OFF;
        LED_GREEN_OFF;
        LED_BLUE_OFF;
        uart_write_blocking(0, 'O');

        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_RED_ON;
        uart_write_blocking(0, 'R');
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_RED_OFF;
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_GREEN_ON;
        uart_write_blocking(0, 'G');
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_GREEN_OFF;
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_BLUE_ON;
        uart_write_blocking(0, 'B');
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_RED_OFF;
        LED_GREEN_OFF;
        LED_BLUE_OFF;
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }
        LED_GREEN_ON;
        LED_BLUE_ON;
        LED_RED_ON;
        uart_write_blocking(0, 'A');
        for (int i = 0; i < 1000000; i++) {
            asm("nop");
        }

    } */
}


void delay(uint32_t microseconds){
    /* perform busy-waiting for specified number of microseconds  */
    uint32_t cycles = microseconds * 2; // factor has been found by measure
    for (int i = 0; i < cycles; i++) {
        asm("nop");
    }

}

/**
 * @brief Initialize the boards on-board RGB LED
 *
 * The LED initialization is hard-coded in this function.
 *
 * The LED channels are connected to the following pins:
 * - RED:   21
 * - GREEN: 22
 * - BLUE:  23
 */
void leds_init(void)
{
    /* set LED pins to function as output */
    NRF_GPIO->DIRSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);

    /* turn all LEDs off */
    NRF_GPIO->OUTCLR = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
}
