/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "io_reg.h"
#include "stdio_base.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static void _cpu_reset(void)
{
    /* 2.14 subsystem resets */
    uint32_t rst;
    /* Reset hardware components except for critical ones */
    rst = RESETS_RESET_MASK
        & ~(RESETS_RESET_USBCTRL_BITS
        |   RESETS_RESET_SYSCFG_BITS
        |   RESETS_RESET_PLL_USB_BITS
        |   RESETS_RESET_PLL_SYS_BITS
        |   RESETS_RESET_PADS_QSPI_BITS
        |   RESETS_RESET_PADS_BANK0_BITS
        |   RESETS_RESET_IO_QSPI_BITS);
    periph_reset(rst);
    /* Assert that reset has completed except for those components which
       are not clocked by clk_ref or clk_sys */
    rst = RESETS_RESET_MASK
        & ~(RESETS_RESET_USBCTRL_BITS
        |   RESETS_RESET_UART1_BITS
        |   RESETS_RESET_UART0_BITS
        |   RESETS_RESET_SPI1_BITS
        |   RESETS_RESET_SPI0_BITS
        |   RESETS_RESET_RTC_BITS
        |   RESETS_RESET_ADC_BITS);
    periph_reset_done(rst);

    /* power the reference clock from its default source: the ROSC */
    clock_ref_configure_source(MHZ(12), MHZ(12), CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH);
    /* power the system clock from its default source: the reference clock */
    clock_sys_configure_source(MHZ(12), MHZ(12), CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF);
    /* start XOSC, typically running at 12 MHz */
    xosc_start(CLOCK_XOSC);
    /* reset system PLL */
    pll_reset_sys();
    /* start the system PLL (typically takes the 12 MHz XOSC to generate 125 MHz) */
    pll_start_sys(PLL_SYS_REF_DIV, PLL_SYS_VCO_FEEDBACK_SCALE, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
    /* configure reference clock to run from XOSC (typically 12 MHz) */
    clock_ref_configure_source(CLOCK_XOSC, CLOCK_XOSC,
                               CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC);
    /* configure system clock output */
    clock_sys_configure_aux_source(CLOCK_CORECLOCK, CLOCK_CORECLOCK,
                                   CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS);
    /* configure the peripheral clock to run from the system clock */
    clock_periph_configure(CLOCK_PERIPH_SOURCE);
    /* Configure USB PLL to deliver 48MHz needed by ADC */
    if (IS_USED(MODULE_PERIPH_ADC)) {
        pll_start_usb(PLL_USB_REF_DIV, PLL_USB_VCO_FEEDBACK_SCALE,
                      PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
        clock_adc_configure(CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB);
    }
}

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    _cpu_reset();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    DEBUG_PUTS("[rpx0xx] GPOUT0 (GPIO pin 21) is clocked from XOSC (typically 12 MHz)");

    /* trigger static peripheral initialization */
    periph_init();
}
