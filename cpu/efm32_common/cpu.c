/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_int.h"

extern void board_init(void);
extern void __libc_init_array(void);
extern void kernel_init(void);
extern void isr_svc(void);
extern void isr_pendsv(void);

static void clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the clock system */
    clock_init();

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}

/**
 * @brief Configure the controllers clock system
 *
 */
static void clock_init(void)
{
    // Enable high speed crystal
    CMU->OSCENCMD |= CMU_OSCENCMD_HFXOEN;

    // Wait for high speed crystal to stabilise
    while (!(CMU->STATUS & CMU_STATUS_HFXORDY)) {
        ;
    }

    // Set high speed crystal as core clock with divisor of 2
    CMU->CMD |= CMU_CMD_HFCLKSEL_HFXO;
    CMU->CTRL |= (1 << 14);

    // Wait for clock switch
    while ((CMU->STATUS & CMU_STATUS_HFRCOSEL)) {
        ;
    }

    // Disable high speed oscillator
    CMU->OSCENCMD |= CMU_OSCENCMD_HFRCODIS;

    // Enable low energy interface (for watchdog)
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;
}

//Replacement start function
//STM port has this in the startup_* file
void _start(void)
{
    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

