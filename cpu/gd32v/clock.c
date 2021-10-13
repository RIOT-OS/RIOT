/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           GD32V Clock initialization
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 *
 * Heavily based on the stm32f1 connectivity line clock initialization
 */
#include "kernel_defines.h"
#include "irq.h"
#include "cpu.h"
#include "periph_conf.h"

#define CLOCK_AHB            CLOCK_CORECLOCK /* Equal to the CPU clock */
#define CLOCK_APB1           CLOCK_AHB/2     /* Half AHB clock */
#define CLOCK_APB2           CLOCK_AHB       /* Equal to the AHB clock */

#define CLOCK_AHB_DIV        0          /* Max speed at 108 MHz */
#define CLOCK_APB1_DIV       (0x04 | 0) /* Max speed at 54 MHz */
#define CLOCK_APB2_DIV       (0x0 | 0)  /* Max speed at 108 MHz */

#define CLOCK_AHB_DIV_CONF   (CLOCK_AHB_DIV << RCU_CFG0_AHBPSC_Pos)
#define CLOCK_APB1_DIV_CONF  (CLOCK_APB1_DIV << RCU_CFG0_APB1PSC_Pos)
#define CLOCK_APB2_DIV_CONF  (CLOCK_APB2_DIV << RCU_CFG0_APB2PSC_Pos)

#define PREDV0_CONF          1  /* Divide by 2 */
#define PLL_MULT_FACTOR      (CLOCK_CORECLOCK / \
                                (CLOCK_HXTAL / (PREDV0_CONF + 1)) - 1)

#define RCU_CFG0_SCS_IRC8    (0 << RCU_CFG0_SCS_Pos)
#define RCU_CFG0_SCS_HXTAL   (1 << RCU_CFG0_SCS_Pos)
#define RCU_CFG0_SCS_PLL     (2 << RCU_CFG0_SCS_Pos)

#define ENABLE_DEBUG 0
#include "debug.h"

void periph_clk_en(bus_t bus, uint32_t mask)
{
    switch (bus) {
    case AHB:
        cpu_reg_enable_bits(&RCU->AHBEN, mask);
        break;
    case APB1:
        cpu_reg_enable_bits(&RCU->APB1EN, mask);
        break;
    case APB2:
        cpu_reg_enable_bits(&RCU->APB2EN, mask);
        break;
    default:
        DEBUG("unsupported bus %d\n", (int)bus);
        break;
    }
}

void periph_clk_dis(bus_t bus, uint32_t mask)
{
    switch (bus) {
    case AHB:
        cpu_reg_disable_bits(&RCU->AHBEN, mask);
        break;
    case APB1:
        cpu_reg_disable_bits(&RCU->APB1EN, mask);
        break;
    case APB2:
        cpu_reg_disable_bits(&RCU->APB2EN, mask);
        break;
    default:
        DEBUG("unsupported bus %d\n", (int)bus);
        break;
    }
}

uint32_t periph_apb_clk(bus_t bus)
{
    switch (bus) {
    case AHB:
        return CLOCK_AHB;
    case APB1:
        return CLOCK_APB1;
    case APB2:
        return CLOCK_APB2;
    }
    return 0;
}

void gd32v_enable_irc8(void)
{
    cpu_reg_enable_bits(&RCU->CTL, RCU_CTL_IRC8MEN_Msk);
    while (!(RCU->CTL & RCU_CTL_IRC8MSTB_Msk)) {}
}

void gd32v_disable_irc8(void)
{
    RCU->CTL &= ~(RCU_CTL_IRC8MEN_Msk);
}

void gd32vf103_clock_init(void)
{
    unsigned is = irq_disable();

    /* enable HSI clock for the duration of initialization */
    gd32v_enable_irc8();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    RCU->CFG0 = (RCU_CFG0_SCS_IRC8 | CLOCK_AHB_DIV_CONF |
                 CLOCK_APB1_DIV_CONF | CLOCK_APB2_DIV_CONF);
    while ((RCU->CFG0 & RCU_CFG0_SCSS_Msk) !=
           (RCU_CFG0_SCS_IRC8 << RCU_CFG0_SCSS_Pos)) {}

    /* disable all active clocks except IRC8 -> resets the clk configuration */
    RCU->CTL = (RCU_CTL_IRC8MEN_Msk);

    if (IS_ACTIVE(CONFIG_BOARD_HAS_HXTAL)) {
        cpu_reg_enable_bits(&RCU->CTL, RCU_CTL_HXTALEN_Msk);
        while (!(RCU->CTL & RCU_CTL_HXTALSTB_Msk)) {}
    }

    RCU->CFG1 = (PREDV0_CONF);

    RCU->CFG0 |= (CLOCK_HXTAL << RCU_CFG0_PLLSEL_Pos) |
                 ((PLL_MULT_FACTOR & 0xf) << RCU_CFG0_PLLMF_3_0_Pos) |
                 ((PLL_MULT_FACTOR & 0x10) << (RCU_CFG0_PLLMF_4_Pos - 4));

    RCU->CTL |= (RCU_CTL_PLLEN_Msk);

    /* Wait for PLL to stabilize */
    while ((RCU->CTL & RCU_CTL_PLLSTB_Msk) != RCU_CTL_PLLSTB_Msk) {}

    /* Switch clock input */
    RCU->CFG0 |= RCU_CFG0_SCS_PLL;

    RCU->AHBEN &= ~RCU_AHBEN_FMCSPEN_Msk;

    while ((RCU->CFG0 & RCU_CFG0_SCSS_Msk) !=
           (RCU_CFG0_SCS_PLL << RCU_CFG0_SCSS_Pos)) {}
    gd32v_disable_irc8();
    irq_restore(is);
}
