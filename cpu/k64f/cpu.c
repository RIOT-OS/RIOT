/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k64f
 * @{
 *
 * @file
 * @brief       Implementation of the K64F CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "mcg.h"
#include "cpu_conf.h"

#define SIM_CLKDIV1_60MHZ      (SIM_CLKDIV1_OUTDIV1(0) | \
                                SIM_CLKDIV1_OUTDIV2(0) | \
                                SIM_CLKDIV1_OUTDIV3(1) | \
                                SIM_CLKDIV1_OUTDIV4(2))

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the clock system */
    cpu_clock_init();
}

/**
 * @brief Configure the controllers clock system
 *
 * | Clock name | Run mode frequency (max) | VLPR mode frequency (max) |
 *
 * | Core       | 120 MHz                  |   4 MHz                   |
 * | System     | 120 MHz                  |   4 MHz                   |
 * | Bus        |  60 MHz                  |   4 MHz                   |
 * | FlexBus    |  50 MHz                  | 800 kHz                   |
 * | Flash      |  25 MHz                  |   4 MHz                   |
 */
static void cpu_clock_init(void)
{
    /* setup system prescalers */
    SIM->CLKDIV1 = (uint32_t)SIM_CLKDIV1_60MHZ;

    /* RMII RXCLK */
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[18] &= ~(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07));

    kinetis_mcg_set_mode(KINETIS_MCG_PEE);
}
