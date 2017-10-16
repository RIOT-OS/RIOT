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
#include <avr/io.h>

#include "avr/wdt.h"
#include "avr/pgmspace.h"
#include "board.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
	if( MCUSR&(1<<PORF)  ){printf( ("Power-on reset.\n"));}
	if( MCUSR&(1<<EXTRF) ){printf( ("External reset!\n"));}
	if( MCUSR&(1<<BORF)  ){printf( ("Brownout reset!\n"));}
	if( MCUSR&(1<<WDRF)  ){printf( ("Watchdog reset!\n"));}
	if( MCUSR&(1<<JTRF)  ){printf( ("JTAG reset!\n"));}
	MCUSR = 0;


	wdt_reset();
	wdt_disable();

	/*
	 * Set system clock Prescaler
	 */
	CLKPR = (1 << CLKPCE); // enable a change to CLKPR
	CLKPR = 0; // set the Division factor to 1, internal system clock speed to 16MHz

	/* Right now we need to do nothing here */
    //;
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

	if( MCUSR&(1<<PORF)  ){printf( ("Power-on reset.\n")); }
	if( MCUSR&(1<<EXTRF) ){printf( ("External reset!\n")); }
	if( MCUSR&(1<<BORF)  ){printf( ("Brownout reset!\n")); }
	if( MCUSR&(1<<WDRF)  ){printf( ("Watchdog reset!\n")); }
	if( MCUSR&(1<<JTRF)  ){printf( ("JTAG reset!\n")); }

	printf_P(PSTR("FATAL ERROR: BADISR_vect called, unprocessed Interrupt.\nSTOP Execution.\n"));

	printf("IRQ_STATUS %#02x\nIRQ_STATUS1 %#02x\n", (unsigned int)IRQ_STATUS, (unsigned int)IRQ_STATUS1 );
	printf("SCIRQS %#02x\nBATMON %#02x\n", (unsigned int)SCIRQS, (unsigned int)BATMON );
	printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR );

	//printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR );


	/* White LED light is used to signal ERROR. */
	LED_PORT &= ~(BLUE|GREEN|RED);

	while(1){};
}

/* All Interrupts are liste here to test them.*/
/*TODO Remove not needed interupt vector routines */

/*INT0 - 7 defined in gpio.c */
//	ISR(INT0_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT0_vect \n");
//		__exit_isr();
//	}
//	ISR(INT1_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT1_vect \n");
//		__exit_isr();
//	}
//	ISR(INT2_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT2_vect \n");
//		__exit_isr();
//	}
//	ISR(INT3_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT3_vect \n");
//		__exit_isr();
//	}
//
//	ISR(INT4_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT4_vect \n");
//		__exit_isr();
//	}
//	ISR(INT5_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT5_vect \n");
//		__exit_isr();
//	}
//	ISR(INT6_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT6_vect \n");
//		__exit_isr();
//	}
//	ISR(INT7_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT7_vect \n");
//		__exit_isr();
//	}


// context save interrupt
//	ISR(PCINT0_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("PCINT0_vect \n");
//		LED_PORT |= RED;
//		__exit_isr();
//	}


	ISR(PCINT1_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("PCINT1_vect \n");
		__exit_isr();
	}
	ISR(PCINT2_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("PCINT2_vect \n");
		__exit_isr();
	}
	ISR(WDT_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("WDT_vect \n");
		__exit_isr();
	}

//	ISR(TIMER2_COMPA_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER2_COMPA_vect \n");
//		__exit_isr();
//	}
//	ISR(TIMER2_COMPB_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER2_COMPB_vect \n");
//		__exit_isr();
//	}
//	ISR(TIMER2_OVF_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER2_OVF_vect \n");
//		__exit_isr();
//	}

/*	ISR(TIMER1_CAPT_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER1_CAPT_vect \n");
		__exit_isr();
	}
	ISR(TIMER1_COMPA_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER1_COMPA_vect \n");
		__exit_isr();
	}
	ISR(TIMER1_COMPB_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER1_COMPB_vect \n");
		__exit_isr();
	}
	ISR(TIMER1_COMPC_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER1_COMPC_vect \n");
		__exit_isr();
	}
	ISR(TIMER1_OVF_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER1_OVF_vect \n");
		__exit_isr();
	}

*/
	/*ISR(TIMER0_COMPA_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER0_COMPA_vect \n");
		__exit_isr();
	}
	ISR(TIMER0_COMPB_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER0_COMPB_vect \n");
		__exit_isr();
	}
	ISR(TIMER0_OVF_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER0_OVF_vect \n");
		__exit_isr();
	}

	ISR(SPI_STC_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SPI_STC_vect \n");
		__exit_isr();
	}*/
// Defined in atmega_common/periph/uart.c
//	ISR(USART0_RX_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("USART0_RX_vect \n");
//		__exit_isr();
//	}

	ISR(USART0_UDRE_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("USART0_UDRE_vect \n");
		__exit_isr();
	}
	ISR(USART0_TX_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("USART0_TX_vect \n");
		LED_PORT |= RED;
		__exit_isr();
	}

	/*ISR(ANALOG_COMP_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("ANALOG_COMP_vect \n");
		__exit_isr();
	}*/
	ISR(ADC_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("ADC_vect \n");
		__exit_isr();
	}
	ISR(EE_READY_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("EE_READY_vect \n");
		__exit_isr();
	}

	ISR(TIMER3_CAPT_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER3_CAPT_vect \n");
		__exit_isr();
	}
	ISR(TIMER3_COMPA_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER3_COMPA_vect \n");
		__exit_isr();
	}
	ISR(TIMER3_COMPB_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER3_COMPB_vect \n");
		__exit_isr();
	}
	ISR(TIMER3_COMPC_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER3_COMPC_vect \n");
		__exit_isr();
	}
	ISR(TIMER3_OVF_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER3_OVF_vect \n");
		__exit_isr();
	}

// Defined in atmega_common/periph/uart.c
//	ISR(USART1_RX_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("USART1_RX_vect \n");
//		__exit_isr();
//	}

	ISR(USART1_UDRE_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("USART1_UDRE_vect \n");
		__exit_isr();
	}
	ISR(USART1_TX_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("USART1_TX_vect \n");
		__exit_isr();
	}
	ISR(TWI_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TWI_vect \n");
		__exit_isr();
	}
	ISR(SPM_READY_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SPM_READY_vect \n");
		__exit_isr();
	}


	ISR(TIMER4_CAPT_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER4_CAPT_vect \n");
		__exit_isr();
	}
// configured in /[BOARD]/include/periph_conf.h
// Defined in atmega_common/periph/timer.c
//	ISR(TIMER4_COMPA_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER4_COMPA_vect \n");
//		__exit_isr();
//	}
//	ISR(TIMER4_COMPB_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER4_COMPB_vect \n");
//		__exit_isr();
//	}
//	ISR(TIMER4_COMPC_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER4_COMPC_vect \n");
//		__exit_isr();
//	}

	ISR(TIMER4_OVF_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER4_OVF_vect \n");
		__exit_isr();
	}

	ISR(TIMER5_CAPT_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER5_CAPT_vect \n");
		__exit_isr();
	}

// configured in /[BOARD]/include/periph_conf.h
// Defined in atmega_common/periph/timer.c
//	ISR(TIMER5_COMPA_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER5_COMPA_vect \n");
//		__exit_isr();
//	}
//	ISR(TIMER5_COMPB_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER5_COMPB_vect \n");
//		__exit_isr();
//	}
//	ISR(TIMER5_COMPC_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("TIMER5_COMPC_vect \n");
//		__exit_isr();
//	}

	ISR(TIMER5_OVF_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TIMER5_OVF_vect \n");
		__exit_isr();
	}

	ISR(BAT_LOW_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("BAT_LOW \n");
		__exit_isr();
	}
