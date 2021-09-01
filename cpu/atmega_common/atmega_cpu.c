/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 RWTH Aachen, Josua Arndt
 *               2018 Matthew Blue
 *               2021 Gerson Fernando Budke
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
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <avr/pgmspace.h>

#include "board.h"
#include "cpu.h"
#include "panic.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CPU_ATMEGA_CLK_SCALE_INIT
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
#endif

extern uint8_t mcusr_mirror;
extern uint8_t soft_rst;

void avr8_reset_cause(void)
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

void __attribute__((weak)) avr8_clk_init(void)
{
    atmega_set_prescaler(CPU_ATMEGA_CLK_SCALE_INIT);
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
    avr8_reset_cause();

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

#if defined(CPU_ATMEGA128RFA1) || defined (CPU_ATMEGA256RFR2)
ISR(BAT_LOW_vect, ISR_BLOCK)
{
    avr8_enter_isr();
    DEBUG("BAT_LOW\n");
    avr8_exit_isr();
}
#endif
