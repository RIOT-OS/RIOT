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
 * @brief       Implementation of the CPU clock configuration
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "vendor/RP2040.h"
#include "vendor/system_RP2040.h"
#include "io_reg.h"
#include "periph_cpu.h"

static void _clk_sys_set_source(CLOCKS_CLK_SYS_CTRL_SRC_Enum source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_SYS_CTRL.reg, source << CLOCKS_CLK_SYS_CTRL_SRC_Pos,
                              CLOCKS_CLK_SYS_CTRL_SRC_Msk);
}

static void _clk_sys_set_aux_source(CLOCKS_CLK_SYS_CTRL_AUXSRC_Enum source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_SYS_CTRL.reg, source << CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos,
                              CLOCKS_CLK_SYS_CTRL_AUXSRC_Msk);
}

static void _clk_ref_set_source(CLOCKS_CLK_REF_CTRL_SRC_Enum source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_REF_CTRL.reg, source << CLOCKS_CLK_REF_CTRL_SRC_Pos,
                              CLOCKS_CLK_REF_CTRL_SRC_Msk);
}

static void _clk_ref_set_aux_source(CLOCKS_CLK_REF_CTRL_AUXSRC_Enum source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_REF_CTRL.reg, source << CLOCKS_CLK_REF_CTRL_AUXSRC_Pos,
                              CLOCKS_CLK_REF_CTRL_AUXSRC_Msk);
}

static void _gpout_set_aux_source(volatile uint32_t *reg, uint32_t value)
{
    io_reg_write_dont_corrupt(reg,
                              value << CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_Pos,
                              CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_Msk);
}

void clock_sys_configure_source(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_SYS_CTRL_SRC_Enum source)
{
    assert(f_out <= f_in);
    assert(source != CLOCKS_CLK_SYS_CTRL_SRC_clksrc_clk_sys_aux);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_SYS_DIV_INT_Pos) / f_out;
    /* switch the glitchless mux to clk_ref */
    _clk_sys_set_source(source);
    /* apply divider */
    CLOCKS->CLK_SYS_DIV.reg = div;
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED & (1U << source))) { }
}

void clock_sys_configure_aux_source(uint32_t f_in, uint32_t f_out,
                                    CLOCKS_CLK_SYS_CTRL_AUXSRC_Enum aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_SYS_DIV_INT_Pos) / f_out;
    /* switch the glitchless mux to a source that is not the aux mux */
    _clk_sys_set_source(CLOCKS_CLK_SYS_CTRL_SRC_clk_ref);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED & (1U << CLOCKS_CLK_SYS_CTRL_SRC_clk_ref))) { }
    /* change the auxiliary mux */
    _clk_sys_set_aux_source(aux);
    /* apply divider */
    CLOCKS->CLK_SYS_DIV.reg = div;
    /* switch the glitchless mux to clk_sys_aux */
    _clk_sys_set_source(CLOCKS_CLK_SYS_CTRL_SRC_clksrc_clk_sys_aux);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED & (1U << CLOCKS_CLK_SYS_CTRL_SRC_clksrc_clk_sys_aux))) { }
}

void clock_ref_configure_source(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_REF_CTRL_SRC_Enum source)
{
    assert(f_out <= f_in);
    assert(source != CLOCKS_CLK_REF_CTRL_SRC_clksrc_clk_ref_aux);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    /* switch the glitchless mux to clock source */
    _clk_ref_set_source(source);
    /* apply divider */
    CLOCKS->CLK_REF_DIV.reg = div & CLOCKS_CLK_REF_DIV_INT_Msk;
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED & (1U << source))) { }
}

void clock_ref_configure_aux_source(uint32_t f_in, uint32_t f_out,
                                    CLOCKS_CLK_REF_CTRL_AUXSRC_Enum aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    /* switch the glitchless mux to a source that is not the aux mux */
    _clk_ref_set_source(CLOCKS_CLK_REF_CTRL_SRC_rosc_clksrc_ph);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED & (1U << CLOCKS_CLK_REF_CTRL_SRC_rosc_clksrc_ph))) { }
    /* change the auxiliary mux */
    _clk_ref_set_aux_source(aux);
    /* apply divider */
    CLOCKS->CLK_REF_DIV.reg = div & CLOCKS_CLK_REF_DIV_INT_Msk;
    /* switch the glitchless mux to clk_ref_aux */
    _clk_ref_set_source(CLOCKS_CLK_REF_CTRL_SRC_clksrc_clk_ref_aux);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED & (1U << CLOCKS_CLK_REF_CTRL_SRC_clksrc_clk_ref_aux))) { }
}

void clock_periph_configure(CLOCKS_CLK_PERI_CTRL_AUXSRC_Enum aux)
{
    io_reg_atomic_clear(&CLOCKS->CLK_PERI_CTRL.reg, (1u << CLOCKS_CLK_PERI_CTRL_ENABLE_Pos));
    io_reg_write_dont_corrupt(&CLOCKS->CLK_PERI_CTRL.reg, aux << CLOCKS_CLK_PERI_CTRL_AUXSRC_Pos,
                              CLOCKS_CLK_PERI_CTRL_AUXSRC_Msk);
    io_reg_atomic_set(&CLOCKS->CLK_PERI_CTRL.reg, (1u << CLOCKS_CLK_PERI_CTRL_ENABLE_Pos));
}

void clock_gpout0_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_Enum aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT0_CTRL.reg, 1U << CLOCKS_CLK_GPOUT0_CTRL_ENABLE_Pos);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT0_CTRL.reg, aux);
    CLOCKS->CLK_GPOUT0_DIV.reg = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT0_CTRL.reg, 1U << CLOCKS_CLK_GPOUT0_CTRL_ENABLE_Pos);
    io_reg_atomic_set(&PADS_BANK0->GPIO21.reg, 1U << PADS_BANK0_GPIO21_IE_Pos);
    gpio_set_function_select(21, FUNCTION_SELECT_CLOCK);
}

void clock_gpout1_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT1_CTRL_AUXSRC_Enum aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT1_CTRL.reg, 1U << CLOCKS_CLK_GPOUT1_CTRL_ENABLE_Pos);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT1_CTRL.reg, aux);
    CLOCKS->CLK_GPOUT1_DIV.reg = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT1_CTRL.reg, 1U << CLOCKS_CLK_GPOUT1_CTRL_ENABLE_Pos);
    io_reg_atomic_set(&PADS_BANK0->GPIO23.reg, 1U << PADS_BANK0_GPIO23_IE_Pos);
    gpio_set_function_select(23, FUNCTION_SELECT_CLOCK);
}

void clock_gpout2_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT2_CTRL_AUXSRC_Enum aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT2_CTRL.reg, 1U << CLOCKS_CLK_GPOUT2_CTRL_ENABLE_Pos);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT2_CTRL.reg, aux);
    CLOCKS->CLK_GPOUT2_DIV.reg = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT2_CTRL.reg, 1U << CLOCKS_CLK_GPOUT2_CTRL_ENABLE_Pos);
    io_reg_atomic_set(&PADS_BANK0->GPIO24.reg, 1U << PADS_BANK0_GPIO24_IE_Pos);
    gpio_set_function_select(24, FUNCTION_SELECT_CLOCK);
}

void clock_gpout3_configure(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_GPOUT3_CTRL_AUXSRC_Enum aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT3_CTRL.reg, 1U << CLOCKS_CLK_GPOUT3_CTRL_ENABLE_Pos);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT3_CTRL.reg, aux);
    CLOCKS->CLK_GPOUT3_DIV.reg = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT3_CTRL.reg, 1U << CLOCKS_CLK_GPOUT3_CTRL_ENABLE_Pos);
    io_reg_atomic_set(&PADS_BANK0->GPIO25.reg, 1U << PADS_BANK0_GPIO25_IE_Pos);
    gpio_set_function_select(25, FUNCTION_SELECT_CLOCK);
}
