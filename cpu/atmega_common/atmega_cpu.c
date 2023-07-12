/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 RWTH Aachen, Josua Arndt
 *               2018 Matthew Blue
 *               2021-2023 Gerson Fernando Budke
 *               2023 Hugues Larrive
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "irq_arch.h"
#include "panic.h"
#include "sys/bus.h"

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
#if defined(JTRF)
    if (mcusr_mirror & (1 << JTRF)) {
        DEBUG("JTAG reset!\n");
    }
#endif
}

void __attribute__((weak)) avr8_clk_init(void)
{
#if defined(CLKPR)
    atmega_set_prescaler(CPU_ATMEGA_CLK_SCALE_INIT);
#endif
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
ISR(BADISR_vect, ISR_NAKED)
{
    avr8_reset_cause();

#if defined(TRX_CTRL_0) /* megaRF */
    printf("IRQ_STATUS %#02x\n", (unsigned int)IRQ_STATUS);

#if defined(IRQ_STATUS1)
    printf("IRQ_STATUS1 %#02x\n", (unsigned int)IRQ_STATUS1);
#endif

    printf("SCIRQS %#02x\nBATMON %#02x\n", (unsigned int)SCIRQS, (unsigned int)BATMON);

    printf("EIFR %#02x\nPCIFR %#02x\n", (unsigned int)EIFR, (unsigned int)PCIFR);
#endif
#ifdef LED_PANIC
    /* Use LED light to signal ERROR. */
    LED_PANIC;
#endif

    core_panic(PANIC_GENERAL_ERROR, "BADISR");
}

#if defined(BAT_LOW_vect)
static inline void bat_low_handler(void)
{
    DEBUG("BAT_LOW\n");

#if MODULE_SYS_BUS_POWER
    msg_bus_t *bus = sys_bus_get(SYS_BUS_POWER);
    msg_bus_post(bus, SYS_BUS_POWER_EVENT_LOW_VOLTAGE, NULL);
#endif
}

AVR8_ISR(BAT_LOW_vect, bat_low_handler);
#endif
