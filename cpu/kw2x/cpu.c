/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kw2x
 * @{
 *
 * @file
 * @brief       Implementation of the KW2xD CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "mcg.h"
#include "cpu_conf.h"
#include "periph/init.h"

#define FLASH_BASE          (0x00000000)

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
    /* trigger static peripheral initialization */
    periph_init();
}

static inline void modem_clock_init(void)
{
    /* Use the CLK_OUT of the modem as the clock source. */

    /* Enable GPIO clock gates */
    KW2XDRF_PORT_CLKEN();
    KW2XDRF_CLK_CTRL_CLKEN();

    /* Modem RST_B is connected to PTB19 and can be used to reset the modem. */
    KW2XDRF_PORT_DEV->PCR[KW2XDRF_RST_PIN] = PORT_PCR_MUX(1);
    BITBAND_REG32(KW2XDRF_GPIO->PDDR, KW2XDRF_RST_PIN) = 1;
    KW2XDRF_GPIO->PCOR = (1 << KW2XDRF_RST_PIN);

    /* Modem GPIO5 is connected to PTC0 and can be used to select CLK_OUT frequency, */
    /* set PTC0 high for CLK_OUT=32.787kHz and low for CLK_OUT=4MHz. */
    KW2XDRF_CLK_CTRL_PORT_DEV->PCR[KW2XDRF_CLK_CTRL_PIN] = PORT_PCR_MUX(1);
    BITBAND_REG32(KW2XDRF_CLK_CTRL_GPIO->PDDR, KW2XDRF_CLK_CTRL_PIN) = 1;
    KW2XDRF_CLK_CTRL_GPIO->PCOR = (1 << KW2XDRF_CLK_CTRL_PIN);

    /* Modem IRQ_B is connected to PTB3, modem interrupt request to the MCU. */
    KW2XDRF_PORT_DEV->PCR[KW2XDRF_IRQ_PIN] = PORT_PCR_MUX(1);
    BITBAND_REG32(KW2XDRF_GPIO->PDDR, KW2XDRF_IRQ_PIN) = 0;

    /* release the reset */
    KW2XDRF_GPIO->PSOR = (1 << KW2XDRF_RST_PIN);

    /* wait for modem IRQ_B interrupt request */
    while (KW2XDRF_GPIO->PDIR & (1 << KW2XDRF_IRQ_PIN));
}

/**
 * @brief Configure the controllers clock system
 */
static void cpu_clock_init(void)
{
    /* setup system prescalers */
    SIM->CLKDIV1 = (uint32_t)SIM_CLKDIV1_OUTDIV4(1);

    modem_clock_init();
    kinetis_mcg_set_mode(KINETIS_MCG_PEE);
}
