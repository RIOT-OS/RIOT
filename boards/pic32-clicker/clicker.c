/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include "board.h"
#include "periph/gpio.h"

extern void dummy(void);

void board_init(void)
{
    /*
     * Setup pin mux for UART3 this is the one connected
     * to the mickroBUS
     */
    U3RXR =    0x2;          /*connect pin RPF5 to UART3 RX*/
    RPF4R =    0x1;          /*connect pin RPF4 to UART3 TX*/

    /* Turn off all LED's */
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    LED1_OFF;
    LED2_OFF;

    /* initialize the CPU */
    cpu_init();

    /* Stop the linker from throwing away the PIC32 config register settings */
    dummy();
}

void pm_reboot(void)
{
    /* TODO, note this is needed to get 'default' example to build */
}
