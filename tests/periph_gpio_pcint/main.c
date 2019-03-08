/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for GPIO interrupts for the ATmega boards that
 *              takes PCINT into account
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "atmega_pcint.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#ifdef MODULE_BOARDS_COMMON_ARDUINO_ATMEGA
#include "arduino_board.h"

static const gpio_t analog_map[] = {
    ARDUINO_PIN_A0,
    ARDUINO_PIN_A1,
    ARDUINO_PIN_A2,
    ARDUINO_PIN_A3,
    ARDUINO_PIN_A4,
    ARDUINO_PIN_A5,
#if !defined(CPU_ATMEGA328P) || !defined(CPU_ATMEGA32U4)
    ARDUINO_PIN_A6,
    ARDUINO_PIN_A7,
#endif /* !defined(CPU_ATMEGA328P) || !defined(CPU_ATMEGA32U4) */
#if defined(CPU_ATMEGA2560)
    ARDUINO_PIN_A8,
    ARDUINO_PIN_A9,
    ARDUINO_PIN_A10,
    ARDUINO_PIN_A11,
    ARDUINO_PIN_A12,
    ARDUINO_PIN_A13,
    ARDUINO_PIN_A14,
    ARDUINO_PIN_A15,
#endif /* CPU_ATMEGA2560 */
};

#define ANALOG_NUMOF        (sizeof(analog_map) / sizeof(analog_map[0]))
#define DIGITAL_NUMOF       (sizeof(arduino_pinmap) / sizeof(arduino_pinmap[0]))

#endif /* MODULE_BOARDS_COMMON_ARDUINO_ATMEGA */

/* Used to represent one empty PCINT bank */
#define EMPTY_PCINT_MAP \
        GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF, \
        GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF

static const gpio_t ext_int_pins[] = CPU_ATMEGA_EXT_INTS;

static const gpio_t pcint_pins[] = {
#ifdef ATMEGA_PCINT_MAP_PCINT0
ATMEGA_PCINT_MAP_PCINT0
#else
EMPTY_PCINT_MAP
#endif
,
#ifdef ATMEGA_PCINT_MAP_PCINT1
ATMEGA_PCINT_MAP_PCINT1
#else
EMPTY_PCINT_MAP
#endif
,
#ifdef ATMEGA_PCINT_MAP_PCINT2
ATMEGA_PCINT_MAP_PCINT2,
#else
EMPTY_PCINT_MAP
#endif
};

static const gpio_t blacklist[] = {
    GPIO_UNDEF,
#ifdef CPU_ATMEGA328P
    GPIO_PIN(PORT_D, 0), /* RXD (UART) used for stdio */
    GPIO_PIN(PORT_D, 1), /* TXD (UART) used for stdio */
#endif /* CPU_ATMEGA328P */
#ifdef CPU_ATMEGA32U4
    GPIO_PIN(PORT_D, 2), /* RXD (UART) used for stdio */
    GPIO_PIN(PORT_D, 3), /* TXD (UART) used for stdio */
#endif /* CPU_ATMEGA328P */
};

/**
 * Maps port number to port letter for ATmega CPUs. Not all CPUs have alls
 * ports. Somehow port number 7 is H and port number 8 is J (and not I, as
 * one would suspect). Port numbers are stored as the upper four bits, so
 * we just define 16 letters in order not having to check for out of bound
 * reads
 */
static const char port_names[16] = "ABCDEFGHJKL-----";

static const gpio_flank_t flanks[] = {
    GPIO_FALLING, GPIO_RISING, GPIO_BOTH,
};

static const char *flank_names[] = {
    "falling", "rising", "falling and rising",
};

#define GPIO_PCINT_NUMOF        (sizeof(pcint_pins) / sizeof(pcint_pins[0]))
#define GPIO_EXT_INT_NUMOF      (sizeof(ext_int_pins) / sizeof(ext_int_pins[0]))
#define FLANKS_NUMOF            (sizeof(flanks) / sizeof(flanks[0]))
#define BLACKLIST_NUMOF         (sizeof(blacklist) / sizeof(blacklist[0]))

static short is_usable(gpio_t pin)
{
    for (uint8_t i = 0; i < BLACKLIST_NUMOF; i++) {
        if (pin == blacklist[i]) {
            return 0;
        }
    }

    return 1;
}

static void print_pin(gpio_t pin)
{
    uint8_t port_num = (pin >> 4) & 0x0f;
    uint8_t pin_num = pin & 0x0f;
    printf("P%c%" PRIu8, port_names[port_num], pin_num);
#ifdef MODULE_BOARDS_COMMON_ARDUINO_ATMEGA
    for (uint8_t i = 0; i < ANALOG_NUMOF; i++) {
        if (pin == analog_map[i]) {
            printf(" (A%" PRIu8 ")", i);
            return;
        }
    }
    for (uint8_t i = 0; i < DIGITAL_NUMOF; i++) {
        if (pin == arduino_pinmap[i]) {
            printf(" (D%" PRIu8 ")", i);
            return;
        }
    }
#endif /* MODULE_BOARDS_COMMON_ARDUINO_ATMEGA */
}

static void ext_int_cb(void *arg)
{
    uint8_t idx = (uint8_t)(uintptr_t)arg;
    gpio_t pin = ext_int_pins[idx];
    const char *flank = flank_names[idx % FLANKS_NUMOF];

    printf("INT%" PRIu8 " on ", idx);
    print_pin(pin);
    printf(", flank(s) = %s\n", flank);
}

static void pcint_cb(void *arg)
{
    uint8_t idx = (uint8_t)(uintptr_t)arg;
    gpio_t pin = pcint_pins[idx];
    const char *flank = flank_names[idx % FLANKS_NUMOF];

    printf("PCINT%" PRIu8 " on ", idx);
    print_pin(pin);
    printf(", flank(s) = %s\n", flank);
}

int main(void)
{
    puts(
        "GPIO interrupt test for ATmega boards\n"
        "\n"
        "For all pins supporting IRQs interrupts will be configured, which\n"
        "are triggered on falling edge, on rising edge, and on both edges\n"
        "(in turns). Pull the pins against ground one by one and check if the\n"
        "console output matches your actions.\n"
        "\n"
        "Installing interrupts...");

    /* first the external interrupts */
    for (uint8_t i = 0; i < GPIO_EXT_INT_NUMOF; i++) {
        gpio_t pin = ext_int_pins[i];
        printf("INT%" PRIu8 " on ", i);
        if (is_usable(pin)) {
            const char *flank = flank_names[i % FLANKS_NUMOF];
            print_pin(pin);
            printf(" with flank(s) %s: ", flank);
            if (gpio_init_int(pin, GPIO_IN_PU, flanks[i % FLANKS_NUMOF], ext_int_cb,
                              (void *)(uintptr_t)i)) {
                puts("Failed");
            }
            else {
                puts("OK");
            }
        }
        else {
            print_pin(pin);
            puts(": Blacklisted");
        }
    }

    /* next the pin changed interrupts */
    for (uint8_t i = 0; i < GPIO_PCINT_NUMOF; i++) {
        gpio_t pin = pcint_pins[i];
        printf("PCINT%" PRIu8, i);
        if (is_usable(pin)) {
            const char *flank = flank_names[i % FLANKS_NUMOF];
            printf(" on ");
            print_pin(pin);
            printf(" with flank(s) %s: ", flank);
            if (gpio_init_int(pin, GPIO_IN_PU, flanks[i % FLANKS_NUMOF], 
                              pcint_cb, (void *)(uintptr_t)i)) {
                puts("Failed");
            }
            else {
                puts("OK");
            }
        }
        else {
            if (pin == GPIO_UNDEF){
                puts(": Unsupported");
            }
            else {
                printf(" on ");
                print_pin(pin);
                puts(": Blacklisted");
            }
        }
    }

    puts("Interrupt handlers have been set up.");

    return 0;
}
