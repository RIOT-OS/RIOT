/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Bluetooth Low Energy receive application
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "periph/timer.h"
#include "board.h"
#include "cpu.h"
#include "nrf51.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "radio.h"

int main(void)
{
    gpio_init_out(GPIO_6, GPIO_NOPULL);
    gpio_init_out(GPIO_7, GPIO_NOPULL);
    gpio_init_out(GPIO_1, GPIO_NOPULL);
    //gpio_set(GPIO_6);

    radioConfig(1);
    char msg = 'H';

//    if(msg == 'H')  {
//    	gpio_set(GPIO_6);
//    	delay(2*1000*1000);
//    	gpio_clear(GPIO_6);
//    }


    gpio_set(GPIO_6);


    //vtimer_usleep(1000000);



    //gpio_init_out(GPIO_0, GPIO_NOPULL);
    //gpio_set(GPIO_0);
//    GPIO_DEV->OUTSET = (1UL << 1);

    int i = 0;


    //timer_init(TIMER_0, 1, printInt0);
//    timer_init(TIMER_0, 1, printInt0);
//
//    timer_set(TIMER_0,0,2*1000*1000);
//    timer_set(TIMER_0,1,4*1000*1000);
//    timer_set(TIMER_0,2,6*1000*1000);



//    timer_set(TIMER_1,0,10*1000*1000);
//    timer_set(TIMER_1,1,11*1000*1000);
//    timer_set(TIMER_1,2,12*1000*1000);


	while(1) {

		msg = receivePacket();


	//    if(msg == 'H')  {
	//    	gpio_set(GPIO_6);
	//    	delay(1*1000*1000);
	//    	gpio_clear(GPIO_6);
	//    }
	//    else  {
	//    	gpio_set(GPIO_7);
	//    	delay(1*1000*1000);
	//    	gpio_clear(GPIO_7);
	//    }

	}

    return 0;
}



void delay(uint32_t microseconds){
    /* perform busy-waiting for specified number of microseconds  */
    uint32_t cycles = microseconds * 2; // factor has been found by measure
    for (int i = 0; i < cycles; i++) {
        asm("nop");
    }

}
