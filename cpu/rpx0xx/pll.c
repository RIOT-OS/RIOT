/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Implementation of the phase locked loop (PLL)
 *              to drive the ROSC or XOSC to a higher frequency
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <assert.h>

#include "macros/units.h"
#include "periph_cpu.h"
#include "vendor/RP2040.h"
#include "io_reg.h"

/**
 * @brief   Start the PLL for the system clock (@p pll = PLL_SYS) or
 *          the USB clock (@p pll = PLL_USB)
 */
static void _pll_start(PLL_SYS_Type *pll, uint8_t ref_div,
                       uint16_t vco_feedback_scale,
                       uint8_t post_div_1, uint8_t post_div_2)
{
    assert(PLL_REF_DIV_MIN <= ref_div);
    assert(ref_div <= PLL_REF_DIV_MAX);
    assert(PLL_VCO_FEEDBACK_SCALE_MIN <= vco_feedback_scale);
    assert(vco_feedback_scale <= PLL_VCO_FEEDBACK_SCALE_MAX);
    assert(PLL_POSTDIV_MIN <= post_div_1);
    assert(post_div_1 <= PLL_POSTDIV_MAX);
    assert(PLL_POSTDIV_MIN <= post_div_2);
    assert(post_div_2 <= PLL_POSTDIV_MAX);

    /* program reference clock divider */
    io_reg_write_dont_corrupt(&pll->CS.reg, ref_div << PLL_SYS_CS_REFDIV_Pos,
                              PLL_SYS_CS_REFDIV_Msk);
    /* program feedback divider */
    io_reg_write_dont_corrupt(&pll->FBDIV_INT.reg,
                              vco_feedback_scale << PLL_SYS_FBDIV_INT_FBDIV_INT_Pos,
                              PLL_SYS_FBDIV_INT_FBDIV_INT_Msk);
    /* turn on the main power */
    io_reg_atomic_clear(&pll->PWR.reg, (1U << PLL_SYS_PWR_VCOPD_Pos)
                                     | (1U << PLL_SYS_PWR_DSMPD_Pos)
                                     | (1U << PLL_SYS_PWR_PD_Pos));
    /* wait for VCO to lock (i.e. keep its output stable) */
    while (!pll->CS.bit.LOCK) { }
    /* set up post divisors and turn them on */
    pll->PRIM.reg = (post_div_1 << PLL_SYS_PRIM_POSTDIV1_Pos)
                  | (post_div_2 << PLL_SYS_PRIM_POSTDIV2_Pos);
    io_reg_atomic_clear(&pll->PWR.reg, 1U << PLL_SYS_PWR_POSTDIVPD_Pos);
}

/**
 * @brief   Stop the PLL for the system clock (@p pll = PLL_SYS) or
 *          the USB clock (@p pll = PLL_USB)
 */
static void _pll_stop(PLL_SYS_Type *pll)
{
    uint32_t reg = (1U << PLL_SYS_PWR_VCOPD_Pos)
                 | (1U << PLL_SYS_PWR_POSTDIVPD_Pos)
                 | (1U << PLL_SYS_PWR_DSMPD_Pos)
                 | (1U << PLL_SYS_PWR_PD_Pos);
    io_reg_atomic_set(&pll->PWR.reg, reg);
}

void pll_start_sys(uint8_t ref_div,
                   uint16_t vco_feedback_scale,
                   uint8_t post_div_1, uint8_t post_div_2)
{
    _pll_start(PLL_SYS, ref_div, vco_feedback_scale, post_div_1, post_div_2);
}

void pll_start_usb(uint8_t ref_div,
                   uint16_t vco_feedback_scale,
                   uint8_t post_div_1, uint8_t post_div_2)
{
    _pll_start(PLL_USB, ref_div, vco_feedback_scale, post_div_1, post_div_2);
}

void pll_stop_sys(void)
{
    _pll_stop(PLL_SYS);
}

void pll_stop_usb(void)
{
    _pll_stop(PLL_USB);
}

/**
 * @brief   Reset the PLL of the system clock
 */
void pll_reset_sys(void)
{
    periph_reset(1U << RESETS_RESET_pll_sys_Pos);
    periph_reset_done(1U << RESETS_RESET_pll_sys_Pos);
}

/**
 * @brief   Reset the PLL of the USB clock
 */
void pll_reset_usb(void)
{
    periph_reset(1U << RESETS_RESET_pll_usb_Pos);
    periph_reset_done(1U << RESETS_RESET_pll_usb_Pos);
}
