/*
 * Copyright (C) 2017 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include "cpu.h"
#include "board.h"
#include "periph/init.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/*
* Since this MCU does not feature a software reset, the watchdog timer
* is being used. It will be set to the shortest time and then force a
* reset. Therefore the MCUSR register needs to be resetted as fast as
* possible. In this case in the bootloader already. In order to regain
* information about the reset cause, the MCUSR is copied to r2 beforehand.
* When a software reset was triggered, r3 will contain 0xAA. In order to
* prevent changes to the values from the .init section, r2 and r3 are saved
* in the .init0 section
*/
uint8_t mcusr_mirror __attribute__((section(".noinit")));
uint8_t soft_rst __attribute__((section(".noinit")));
void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init0")));
void get_mcusr(void)
{
    /* save the reset flags passed from the bootloader */
    __asm__ __volatile__("mov %0, r2\n" : "=r" (mcusr_mirror) :);
    __asm__ __volatile__("mov %0, r3\n" : "=r" (soft_rst) :);
}

void _reset_cause(void)
{
    if (mcusr_mirror & (1 << PORF)) {
        DEBUG("Power-on reset.\n");
    }
    if (mcusr_mirror & (1 << EXTRF)) {
        DEBUG("External reset!\n");
    }
    if (mcusr_mirror & (1 << BORF)) {
        DEBUG("Brownout reset!\n");
    }
    if (mcusr_mirror & (1 << WDRF)) {
        if (soft_rst & 0xAA) {
            DEBUG("Software reset!\n");
        } else {
            DEBUG("Watchdog reset!\n");
        }
    }
    if (mcusr_mirror & (1 << JTRF)) {
        DEBUG("JTAG reset!\n");
    }
}

void cpu_init(void)
{
    _reset_cause();

    wdt_reset();   /* should not be nececessary as done in bootloader */
    wdt_disable(); /* but when used without bootloader this is needed */

    /* Set system clock Prescaler */
    CLKPR = (1 << CLKPCE);  /* enable a change to CLKPR */
    /* set the Division factor to 1 results in divisor 2 for internal Oscillator
     * So FCPU = 8MHz
     *
     * Attention!
     * The CPU can not be used with the external xtal oscillator if the core
     * should be put in sleep while the transceiver is in rx mode.
     *
     * It seems the as teh peripheral clock divider is set to 1 and this all
     * clocks of the timer, etc run with 16MHz increasing power consumption.
     * */
    CLKPR = 0;

    /* Initialize peripherals for which modules are included in the makefile.*/
    /* spi_init */
    /* rtc_init */
    /* hwrng_init */
    periph_init();
}

/* This is a vector which is aliased to __vector_default,
 * the vector executed when an ISR fires with no accompanying
 * ISR handler. This may be used along with the ISR() macro to
 * create a catch-all for undefined but used ISRs for debugging
 * purposes.
 * SCIRQS – Symbol Counter Interrupt Status Register
 * BATMON – Battery Monitor Control and Status Register
 * IRQ_STATUS /1 – Transceiver Interrupt Status Register
 * EIFR – External Interrupt Flag Register
 * PCIFR – Pin Change Interrupt Flag Register
 */
ISR(BADISR_vect){

    _reset_cause();

    printf_P(PSTR("FATAL ERROR: BADISR_vect called, unprocessed Interrupt.\n"
                  "STOP Execution.\n"));

    printf("IRQ_STATUS %#02x\nIRQ_STATUS1 %#02x\n",
            (unsigned int)IRQ_STATUS, (unsigned int)IRQ_STATUS1 );

    printf("SCIRQS %#02x\nBATMON %#02x\n", (unsigned int)SCIRQS, (unsigned int)BATMON );

    printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR );

    /* White LED light is used to signal ERROR. */
    LED_PORT |= (LED2_MASK | LED1_MASK | LED0_MASK);

    while (1) {}
}

ISR(BAT_LOW_vect, ISR_BLOCK){
    __enter_isr();
    DEBUG("BAT_LOW \n");
    __exit_isr();
}
