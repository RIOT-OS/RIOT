/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "cpu.h"
#include "io_reg.h"
#include "periph_cpu.h"

static void _clk_sys_set_source(uint32_t source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_SYS_CTRL, source << CLOCKS_CLK_SYS_CTRL_SRC_LSB,
                              CLOCKS_CLK_SYS_CTRL_SRC_BITS);
}

static void _clk_sys_set_aux_source(uint32_t source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_SYS_CTRL, source << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB,
                              CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS);
}

static void _clk_ref_set_source(uint32_t source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_REF_CTRL, source << CLOCKS_CLK_REF_CTRL_SRC_LSB,
                              CLOCKS_CLK_REF_CTRL_SRC_BITS);
}

static void _clk_ref_set_aux_source(uint32_t source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_REF_CTRL, source << CLOCKS_CLK_REF_CTRL_AUXSRC_LSB,
                              CLOCKS_CLK_REF_CTRL_AUXSRC_BITS);
}

static void _gpout_set_aux_source(volatile uint32_t *reg, uint32_t value)
{
    io_reg_write_dont_corrupt(reg,
                              value << CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_LSB,
                              CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_BITS);
}

void clock_sys_configure_source(uint32_t f_in, uint32_t f_out, uint32_t source)
{
    assert(f_out <= f_in);
    assert(source != CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_SYS_DIV_INT_LSB) / f_out;
    /* switch the glitchless mux to clk_ref */
    _clk_sys_set_source(source);
    /* apply divider */
    CLOCKS->CLK_SYS_DIV = div;
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED & (1U << source))) { }
}

void clock_sys_configure_aux_source(uint32_t f_in, uint32_t f_out,
                                    uint32_t aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_SYS_DIV_INT_LSB) / f_out;
    /* switch the glitchless mux to a source that is not the aux mux */
    _clk_sys_set_source(CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED & (1U << CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF))) { }
    /* change the auxiliary mux */
    _clk_sys_set_aux_source(aux);
    /* apply divider */
    CLOCKS->CLK_SYS_DIV = div;
    /* switch the glitchless mux to clk_sys_aux */
    _clk_sys_set_source(CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED &
             (1U << CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX))) { }
}

void clock_ref_configure_source(uint32_t f_in, uint32_t f_out, uint32_t source)
{
    assert(f_out <= f_in);
    assert(source != CLOCKS_CLK_REF_CTRL_SRC_VALUE_CLKSRC_CLK_REF_AUX);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_LSB) / f_out;
    /* switch the glitchless mux to clock source */
    _clk_ref_set_source(source);
    /* apply divider */
    CLOCKS->CLK_REF_DIV = div & CLOCKS_CLK_REF_DIV_INT_BITS;
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED & (1U << source))) { }
}

void clock_ref_configure_aux_source(uint32_t f_in, uint32_t f_out,
                                    uint32_t aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_LSB) / f_out;
    /* switch the glitchless mux to a source that is not the aux mux */
    _clk_ref_set_source(CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED & (1U << CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH))) { }
    /* change the auxiliary mux */
    _clk_ref_set_aux_source(aux);
    /* apply divider */
    CLOCKS->CLK_REF_DIV = div & CLOCKS_CLK_REF_DIV_INT_BITS;
    /* switch the glitchless mux to clk_ref_aux */
    _clk_ref_set_source(CLOCKS_CLK_REF_CTRL_SRC_VALUE_CLKSRC_CLK_REF_AUX);
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED &
             (1U << CLOCKS_CLK_REF_CTRL_SRC_VALUE_CLKSRC_CLK_REF_AUX))) { }
}

void clock_periph_configure(uint32_t aux)
{
    io_reg_atomic_clear(&CLOCKS->CLK_PERI_CTRL, (1u << CLOCKS_CLK_PERI_CTRL_ENABLE_LSB));
    io_reg_write_dont_corrupt(&CLOCKS->CLK_PERI_CTRL, aux << CLOCKS_CLK_PERI_CTRL_AUXSRC_LSB,
                              CLOCKS_CLK_PERI_CTRL_AUXSRC_BITS);
    io_reg_atomic_set(&CLOCKS->CLK_PERI_CTRL, (1u << CLOCKS_CLK_PERI_CTRL_ENABLE_LSB));
}

void clock_usb_configure(uint32_t aux)
{
    io_reg_atomic_clear(&CLOCKS->CLK_USB_CTRL, (1u << CLOCKS_CLK_USB_CTRL_ENABLE_LSB));
    io_reg_write_dont_corrupt(&CLOCKS->CLK_USB_CTRL, aux << CLOCKS_CLK_USB_CTRL_AUXSRC_LSB,
                              CLOCKS_CLK_USB_CTRL_AUXSRC_BITS);
    io_reg_atomic_set(&CLOCKS->CLK_USB_CTRL, (1u << CLOCKS_CLK_USB_CTRL_ENABLE_LSB));
}

void clock_gpout0_configure(uint32_t f_in, uint32_t f_out, uint32_t aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_LSB) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT0_CTRL, 1U << CLOCKS_CLK_GPOUT0_CTRL_ENABLE_LSB);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT0_CTRL, aux);
    CLOCKS->CLK_GPOUT0_DIV = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT0_CTRL, 1U << CLOCKS_CLK_GPOUT0_CTRL_ENABLE_LSB);
    io_reg_atomic_set(&PADS_BANK0->GPIO21, 1U << PADS_BANK0_GPIO21_IE_LSB);
    gpio_set_function_select(21, FUNCTION_SELECT_CLOCK);
}

void clock_gpout1_configure(uint32_t f_in, uint32_t f_out, uint32_t aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_LSB) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT1_CTRL, 1U << CLOCKS_CLK_GPOUT1_CTRL_ENABLE_LSB);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT1_CTRL, aux);
    CLOCKS->CLK_GPOUT1_DIV = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT1_CTRL, 1U << CLOCKS_CLK_GPOUT1_CTRL_ENABLE_LSB);
    io_reg_atomic_set(&PADS_BANK0->GPIO23, 1U << PADS_BANK0_GPIO23_IE_LSB);
    gpio_set_function_select(23, FUNCTION_SELECT_CLOCK);
}

void clock_gpout2_configure(uint32_t f_in, uint32_t f_out, uint32_t aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_LSB) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT2_CTRL, 1U << CLOCKS_CLK_GPOUT2_CTRL_ENABLE_LSB);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT2_CTRL, aux);
    CLOCKS->CLK_GPOUT2_DIV = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT2_CTRL, 1U << CLOCKS_CLK_GPOUT2_CTRL_ENABLE_LSB);
    io_reg_atomic_set(&PADS_BANK0->GPIO24, 1U << PADS_BANK0_GPIO24_IE_LSB);
    gpio_set_function_select(24, FUNCTION_SELECT_CLOCK);
}

void clock_gpout3_configure(uint32_t f_in, uint32_t f_out, uint32_t aux)
{
    assert(f_out <= f_in);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_LSB) / f_out;
    io_reg_atomic_clear(&CLOCKS->CLK_GPOUT3_CTRL, 1U << CLOCKS_CLK_GPOUT3_CTRL_ENABLE_LSB);
    _gpout_set_aux_source(&CLOCKS->CLK_GPOUT3_CTRL, aux);
    CLOCKS->CLK_GPOUT3_DIV = div;
    io_reg_atomic_set(&CLOCKS->CLK_GPOUT3_CTRL, 1U << CLOCKS_CLK_GPOUT3_CTRL_ENABLE_LSB);
    io_reg_atomic_set(&PADS_BANK0->GPIO25, 1U << PADS_BANK0_GPIO25_IE_LSB);
    gpio_set_function_select(25, FUNCTION_SELECT_CLOCK);
}

void clock_adc_configure(uint32_t aux)
{
    /* Stop the clock generator */
    io_reg_atomic_clear(&CLOCKS->CLK_ADC_CTRL,
                        (1u << CLOCKS_CLK_ADC_CTRL_ENABLE_LSB));
    /* Selects the new auxiliary clock source */
    io_reg_write_dont_corrupt(&CLOCKS->CLK_ADC_CTRL,
                              aux << CLOCKS_CLK_ADC_CTRL_AUXSRC_LSB,
                              CLOCKS_CLK_ADC_CTRL_AUXSRC_BITS);
    /* Restart the clock generator */
    io_reg_atomic_set(&CLOCKS->CLK_ADC_CTRL,
                      (1u << CLOCKS_CLK_ADC_CTRL_ENABLE_LSB));
}
