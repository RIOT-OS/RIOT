/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega1284p
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"
#include <avr/wdt.h>

#if (__AVR_LIBC_VERSION__ >= 10700UL)
/* The proper way to set the signature is */
#include <avr/signature.h>
#else

/* signature API not available before avr-lib-1.7.0. Do it manually.*/
typedef struct {
    const unsigned char B2;
    const unsigned char B1;
    const unsigned char B0;
} __signature_t;
#define SIGNATURE __signature_t __signature __attribute__((section(".signature")))
SIGNATURE = {
    .B2 = 0x05, //SIGNATURE_2, //ATMEGA1284p
    .B1 = 0x97, //SIGNATURE_1, //128KB flash
    .B0 = 0x1E, //SIGNATURE_0, //Atmel
};
#endif /* (__AVR_LIBC_VERSION__ >= 10700UL) */

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    wdt_disable();
    periph_init();
}
