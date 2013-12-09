/*
 * main.c - Main function of the SRF02 ultrasonic sensor project.
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup msba2
 * @{
 */

/**
 * @file
 * @brief       MSB-A2 board initialization
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Heiko Will
 * @author      Kaspar Schleiser
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 * @author		Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @note        $Id$
 */


#include <board.h>
#include <cpu.h>

void loop_delay(void)
{
    volatile uint16_t i, j;

    for (i = 1; i < 30; i++) {
        for (j = 1; j != 0; j++) {
            asm volatile(" nop ");
        }
    }
}

void bl_blink(void)
{
    LED_RED_ON;
    LED_GREEN_ON;

    loop_delay();

    LED_RED_OFF;
    LED_GREEN_OFF;
}

void bl_init_ports(void)
{
    SCS |= BIT0;                                            // Set IO Ports to fast switching mode

    /* UART0 */
    PINSEL0 |= BIT4 + BIT6;                                 // RxD0 and TxD0
    PINSEL0 &= ~(BIT5 + BIT7);

    /* LEDS */
    FIO3DIR |= LED_RED_PIN;
    FIO3DIR |= LED_GREEN_PIN;
    LED_RED_OFF;
    LED_GREEN_OFF;

    /* short blinking of both of the LEDs on startup */
    bl_blink();
}

static inline void
pllfeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}

void init_clks1(void)
{
    // Disconnect PLL
    PLLCON &= ~0x0002;
    pllfeed();

    // Disable PLL
    PLLCON &= ~0x0001;
    pllfeed();

    SCS |= 0x20;                        // Enable main OSC

    while (!(SCS & 0x40));              // Wait until main OSC is usable

    /* select main OSC, 16MHz, as the PLL clock source */
    CLKSRCSEL = 0x0001;

    // Setting Multiplier and Divider values
    PLLCFG = 0x0008;                    // M=9 N=1 Fcco = 288 MHz
    pllfeed();

    // Enabling the PLL */
    PLLCON = 0x0001;
    pllfeed();

    /* Set clock divider to 4 (value+1) */
    CCLKCFG = CL_CPU_DIV - 1;           // Fcpu = 72 MHz

#if USE_USB
    USBCLKCFG = USBCLKDivValue;     /* usbclk = 288 MHz/6 = 48 MHz */
#endif
}
