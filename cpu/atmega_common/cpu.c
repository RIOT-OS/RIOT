/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 RWTH Aachen, Josua Arndt
 *               2018 Matthew Blue
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>

 * @}
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "cpu.h"
#include "board.h"
#include "periph/init.h"
#include "panic.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/*
* Since atmega MCUs do not feature a software reset, the watchdog timer
* is being used. It will be set to the shortest time and then force a
* reset. Therefore the MCUSR register needs to be resetted as fast as
* possible.
* Which means in the bootloader or in the following init0 if no bootloader is used.
* Bootloader resets watchdog and pass MCUSR in r2 (e.g. Optiboot) in order to pass
* information about the reset cause to the application.
* When no Bootloader is used the watchdog will be disabled in the init0 section.
* When a software reset was triggered, r3 will contain 0xAA.
* In order to prevent changes to the values from the .init section, MCUSR and r3
* are saved in the .init0 section
*/
uint8_t mcusr_mirror __attribute__((section(".noinit")));
uint8_t soft_rst __attribute__((section(".noinit")));
void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init0")));

void get_mcusr(void)
{
    /* save soft reset flag set in reset routine */
    __asm__ __volatile__("mov %0, r3\n" : "=r" (soft_rst) :);
#ifdef BOOTLOADER_CLEARS_WATCHDOG_AND_PASSES_MCUSR
    /* save the reset flags passed from the bootloader */
    __asm__ __volatile__("mov %0, r2\n" : "=r" (mcusr_mirror) :);
#else
    /* save the reset flags */
#ifdef MCUCSR
  mcusr_mirror = MCUCSR;
  MCUSR = 0;
#else
  mcusr_mirror = MCUSR;
  MCUSR = 0;
#endif
    wdt_disable();
#endif
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
#if !defined (CPU_ATMEGA328P)
    if (mcusr_mirror & (1 << JTRF)) {
        DEBUG("JTAG reset!\n");
    }
#endif
}

void cpu_init(void)
{
    _reset_cause();

    wdt_reset();   /* should not be nececessary as done in bootloader */
    wdt_disable(); /* but when used without bootloader this is needed */

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
ISR(BADISR_vect)
{
    _reset_cause();

#if defined (CPU_ATMEGA256RFR2)
    printf("IRQ_STATUS %#02x\nIRQ_STATUS1 %#02x\n",
            (unsigned int)IRQ_STATUS, (unsigned int)IRQ_STATUS1);

    printf("SCIRQS %#02x\nBATMON %#02x\n", (unsigned int)SCIRQS, (unsigned int)BATMON);

    printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR);
#endif
#ifdef LED_PANIC
    /* Use LED light to signal ERROR. */
    LED_PANIC;
#endif

    core_panic(PANIC_GENERAL_ERROR, PSTR("FATAL ERROR: BADISR_vect called, unprocessed Interrupt.\n"
                  "STOP Execution.\n"));
}

#if defined (CPU_ATMEGA256RFR2)
ISR(BAT_LOW_vect, ISR_BLOCK)
{
    __enter_isr();
    DEBUG("BAT_LOW\n");
    __exit_isr();
}
#endif
