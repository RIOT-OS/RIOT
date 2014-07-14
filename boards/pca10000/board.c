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

#include "radio.h"

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
    gpio_init_out(GPIO_6, GPIO_NOPULL);
    gpio_init_out(GPIO_7, GPIO_NOPULL);
    gpio_init_out(GPIO_1, GPIO_NOPULL);


    gpio_set(GPIO_1);

    delay(1*1000*1000);

    i = 0;





	radioConfig(0);
	char msg = 'Q';
while(1) {

	// sending all the time some message

	sendPacket(1, msg);

	delay(100*1000);
}



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
