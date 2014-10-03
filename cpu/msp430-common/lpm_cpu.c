/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file        lpm_cpu.c
 * @brief       low-power mode implementation for MSP430 MCUs
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#if (__GNUC__ >= 4) && (__GNUC_MINOR__ > 5)
    #include <intrinsics.h>   // MSP430-gcc compiler instrinsics
#endif

#include "board.h"
#include <msp430.h>

#include "lpm.h"

/* Initialise the MSP430 power-saving mechanisms. */
void lpm_init(void)
{
    // nothing to initialize on MSP430s: everything is done by fiddling
    //  with 4 bits of the status register (SR).

    // just ensure MCU is fully up and running at start
    lpm_awake();
}

/* Change the current power-saving mode. */
enum lpm_mode lpm_set(enum lpm_mode target)
{
    enum lpm_mode last_mode = lpm_get();

    switch (target)
    {
    case LPM_ON:
        // fully running MCU
        __bic_status_register(CPUOFF | OSCOFF | SCG0 | SCG1);
        break;
    case LPM_IDLE:
        // lightest mode => LPM0 mode of MSP430
        __bic_status_register(OSCOFF | SCG0 | SCG1);
        // only stops CPU block
        __bis_status_register(CPUOFF);
        break;
    case LPM_SLEEP:
        // mid-level mode => LPM1 mode of MSP430
        __bic_status_register(OSCOFF | SCG1);
        // stops CPU and master clock blocks
        __bis_status_register(CPUOFF | SCG0);
        break;
    case LPM_POWERDOWN:
        // deep-level mode => LPM3 mode of MSP430
        __bic_status_register(OSCOFF);
        // stops all blocks except auxiliary clock (timers)
        __bis_status_register(CPUOFF | SCG0 | SCG1);
        break;
    case LPM_OFF:
        // MCU totally down (LPM4), only RESET or NMI can resume execution
        __bis_status_register(CPUOFF | OSCOFF | SCG0 | SCG1);  // all blocks off
        break;
    default:
        printf("ERROR: trying to set an invalid low-power mode!\n");
        printf("       Operation aborted.\n\n");
    }

    return last_mode;
}

#define LPM_MASK_SR   (CPUOFF | OSCOFF | SCG0 | SCG1)

/* Return the current LPM mode of the MSP430 MCU. */
enum lpm_mode lpm_get(void)
{
    enum lpm_mode current_mode = LPM_UNKNOWN;

    unsigned int current_sr = __read_status_register();
    switch (current_sr & LPM_MASK_SR) {
    case CPUOFF + OSCOFF + SCG0 + SCG1:   // MSP430's LPM4
        current_mode = LPM_OFF;
        break;
    case CPUOFF + SCG0 + SCG1:   // MSP430's LPM3
    case CPUOFF + SCG1:   // MSP430's LPM2
        current_mode = LPM_POWERDOWN;
        break;
    case CPUOFF + SCG0:   // MSP430's LPM1
        current_mode = LPM_SLEEP;
        break;
    case CPUOFF:   // MSP430's LPM1
        current_mode = LPM_IDLE;
        break;
    case 0:   // MSP430 active
        current_mode = LPM_ON;
        break;
    }

    return current_mode;
}

/* resume the MSP430 MCU */
inline void lpm_awake(void)
{
    // disable all power savings mechanisms
    __bic_status_register(CPUOFF | OSCOFF | SCG0 | SCG1);
}

/* the following two functions have no actual role to play MSP430s */
inline void lpm_begin_awake(void) { }
inline void lpm_end_awake(void) { }
