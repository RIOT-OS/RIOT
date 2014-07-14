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
 * @brief       Bluetooth Low Energy send application
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "vtimer.h"
#include "periph/timer.h"
#include "board.h"
#include "cpu.h"
#include "nrf51.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "radio.h"

void cb(int i )  {
	printf("blaaa");

}

int main(void)
{


    gpio_init_out(GPIO_6, GPIO_NOPULL);
    gpio_init_out(GPIO_7, GPIO_NOPULL);
    gpio_init_out(GPIO_1, GPIO_NOPULL);


    gpio_set(GPIO_1);

    LED_RED_OFF;

    //vtimer_usleep(2);

    LED_RED_OFF;






	radioConfig(0);
	char msg = 'Q';
	while(1) {

		// sending all the time some message

		sendPacket(1, msg);

		delay(100*1000);
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
