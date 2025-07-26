/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Architecture-specific GPIO definitions for ESP32-C3 variant (family)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "gpio_arch.h"
#include "soc/io_mux_reg.h"

#if !IS_USED(MODULE_ESP_IDF_GPIO_HAL)

/* GPIO to IOMUX register mapping (see Technical Reference, Section 6.13 IO MUX Functions List)
   https://www.espressif.com/sites/default/files/documentation/esp32-h2_technical_reference_manual_en.pdf */

const uint32_t _gpio_to_iomux_reg[GPIO_PIN_NUMOF] =
{
    PERIPHS_IO_MUX_XTAL_32K_P_U,    /* GPIO0 used for XTAL_32K_P */
    PERIPHS_IO_MUX_XTAL_32K_N_U,    /* GPIO1 used for XTAL_32K_N*/
    PERIPHS_IO_MUX_GPIO2_U,         /* GPIO2 */
    PERIPHS_IO_MUX_GPIO3_U,         /* GPIO3 */
    PERIPHS_IO_MUX_MTMS_U,          /* GPIO4 */
    PERIPHS_IO_MUX_MTDI_U,          /* GPIO5 */
    PERIPHS_IO_MUX_MTCK_U,          /* GPIO6 */
    PERIPHS_IO_MUX_MTDO_U,          /* GPIO7 */
    PERIPHS_IO_MUX_GPIO8_U,         /* GPIO8 */
    PERIPHS_IO_MUX_GPIO9_U,         /* GPIO9 */
    PERIPHS_IO_MUX_GPIO10_U,        /* GPIO10 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_GPIO11_U,        /* GPIO11 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_GPIO12_U,        /* GPIO12 could be used for builtin USB2JTAG bridge */
    PERIPHS_IO_MUX_GPIO13_U,        /* GPIO13 could be used for builtin USB2JTAG bridge */
    PERIPHS_IO_MUX_GPIO14_U,        /* GPIO14 not broken out on chips w/o in-packge flash */
    PERIPHS_IO_MUX_GPIO15_U,        /* GPIO15 */
    PERIPHS_IO_MUX_U0TXD_U,         /* GPIO16 */
    PERIPHS_IO_MUX_U0RXD_U,         /* GPIO17 */
    PERIPHS_IO_MUX_SDIO_CMD_U,      /* GPIO18 */
    PERIPHS_IO_MUX_SDIO_CLK_U,      /* GPIO19 */
    PERIPHS_IO_MUX_SDIO_DATA0_U,    /* GPIO20 */
    PERIPHS_IO_MUX_SDIO_DATA1_U,    /* GPIO21 */
    PERIPHS_IO_MUX_SDIO_DATA2_U,    /* GPIO22 */
    PERIPHS_IO_MUX_SDIO_DATA3_U,    /* GPIO22 */
    PERIPHS_IO_MUX_SPICS0_U,        /* GPIO24 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_SPIQ_U,          /* GPIO25 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_SPIWP_U,         /* GPIO26 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_VDD_SPI_U,       /* GPIO27 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_SPIHD_U,         /* GPIO28 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_SPICLK_U,        /* GPIO29 not broken out on chips with in-packge flash */
    PERIPHS_IO_MUX_SPID_U,          /* GPIO30 not broken out on chips with in-packge flash */
};

#endif /* !IS_USED(MODULE_ESP_IDF_GPIO_HAL) */

/* Table of the usage type of each GPIO pin */
gpio_pin_usage_t _gpio_pin_usage[GPIO_PIN_NUMOF] = {
#if MODULE_ESP_RTC_TIMER_32K
    _NOT_EXIST,   /* GPIO0 is used for external 32K crystal */
    _NOT_EXIST,   /* GPIO1 is used for external 32K crystal */
#else
    _GPIO,        /* GPIO0 */
    _GPIO,        /* GPIO1 */
#endif
    _GPIO,        /* GPIO2 */
    _GPIO,        /* GPIO3 */
    _GPIO,        /* GPIO4 */
    _GPIO,        /* GPIO5 */
    _GPIO,        /* GPIO6 */
    _GPIO,        /* GPIO7 */
    _GPIO,        /* GPIO8 */
    _GPIO,        /* GPIO9 */
    _GPIO,        /* GPIO10 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO11 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO12 could be used for builtin USB2JTAG bridge */
    _GPIO,        /* GPIO13 could be used for builtin USB2JTAG bridge */
    _GPIO,        /* GPIO14 not broken out on chips w/o in-packge flash */
    _GPIO,        /* GPIO15 */
    _UART,        /* GPIO16 is used as direct I/O UART0 TxD */
    _UART,        /* GPIO17 is used as direct I/O UART0 RxD */
    _GPIO,        /* GPIO18 */
    _GPIO,        /* GPIO19 */
    _GPIO,        /* GPIO20 */
    _GPIO,        /* GPIO21 */
    _GPIO,        /* GPIO22 */
    _GPIO,        /* GPIO23 */
    _GPIO,        /* GPIO24 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO25 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO26 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO27 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO28 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO29 not broken out on chips with in-package flash */
    _GPIO,        /* GPIO30 not broken out on chips with in-package flash */
};
