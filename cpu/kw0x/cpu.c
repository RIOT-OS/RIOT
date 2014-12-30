/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kw0x
 * @{
 *
 * @file
 * @brief       Implementation of the KW0x CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @}
 */

#include <stdint.h>
#include "cpu-conf.h"

#define FLASH_BASE         	(0x00000000)

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* configure the vector table location to internal flash */
    SCB->VTOR = FLASH_BASE;

    /* initialize the clock system */
    cpu_clock_init();

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}

/**
 * @brief Configure the controllers clock system
 *
 */
static void cpu_clock_init(void)
{
    /* setup system prescalers */
    SIM->CLKDIV1 = (uint32_t)SIM_CLKDIV1_OUTDIV4(1);

    kinetis_mcg_set_mode(KINETIS_MCG_FEE);

    /* Setup System Intergraion Module */
    SIM->SOPT2 = SIM_SOPT2_UART0SRC(1) | SIM_SOPT2_TPMSRC(1);
    /* Selects the clock source OSC32KCLK for RTC and LPTMR (ERCLK32K) */
    SIM->SOPT1 = SIM_SOPT1_OSC32KSEL(0);

#ifdef KW01Z128_FLL_PLL_TEST
    SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK);
    PORTE->PCR[0] = PORT_PCR_MUX(4);
    /* OSCERCLK is output on the RTC_CLKOUT pin */
    SIM->SOPT2 |= SIM_SOPT2_RTCCLKOUTSEL_MASK;

    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK);
    PORTA->PCR[1] = PORT_PCR_MUX(1);
    GPIOA->PDDR |= (1 << 1);
    while(42) {
        GPIOA->PTOR = (1 << 1);
    }
#endif
}
