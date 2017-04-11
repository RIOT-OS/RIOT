/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
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
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "cpu.h"

#include "avr/wdt.h"
#include "avr/pgmspace.h"
#include "board.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
	if(MCUSR & (1<<PORF )) printf_P(PSTR("Power-on reset.\n"));
	if(MCUSR & (1<<EXTRF)) printf_P(PSTR("External reset!\n"));
	if(MCUSR & (1<<BORF )) printf_P(PSTR("Brownout reset!\n"));
	if(MCUSR & (1<<WDRF )) printf_P(PSTR("Watchdog reset!\n"));
	if(MCUSR & (1<<JTRF )) printf_P(PSTR("JTAG reset!\n"));
	MCUSR = 0;


	wdt_reset();
	wdt_disable();

	/* Right now we need to do nothing here */
    ;
}

/* This is a vector which is aliased to __vector_default,
 * the vector executed when an ISR fires with no accompanying
 * ISR handler. This may be used along with the ISR() macro to
 * create a catch-all for undefined but used ISRs for debugging
 * purposes.
 */
// SCIRQS – Symbol Counter Interrupt Status Register
// BATMON – Battery Monitor Control and Status Register
// IRQ_STATUS /1 – Transceiver Interrupt Status Register
// EIFR – External Interrupt Flag Register
// PCIFR – Pin Change Interrupt Flag Register

ISR(BADISR_vect)
{

	printf_P(PSTR("FATAL ERROR: BADISR_vect called, unprocessed Interrupt.\nSTOP Execution.\n"));

	printf("IRQ_STATUS %#02x\nIRQ_STATUS1 %#02x\n", (unsigned int)IRQ_STATUS, (unsigned int)IRQ_STATUS1 );
	printf("SCIRQS %#02x\nBATMON %#02x\n", (unsigned int)SCIRQS, (unsigned int)BATMON );
	printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR );

	//printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR );


	/* White LED light is used to signal ERROR. */
	LED_PORT &= ~(BLUE|GREEN|RED);

	while(1){};
}
