/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

/* GPIO to IOMUX register mapping (see Technical Reference, Section 5.13.2 Register Summary)
   https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf */

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
    PERIPHS_IO_MUX_GPIO10_U,        /* GPIO10 */
    PERIPHS_IO_MUX_VDD_SPI_U,       /* GPIO11 */
    PERIPHS_IO_MUX_SPIHD_U,         /* GPIO12 */
    PERIPHS_IO_MUX_SPIWP_U,         /* GPIO13 */
    PERIPHS_IO_MUX_SPICS0_U,        /* GPIO14 */
    PERIPHS_IO_MUX_SPICLK_U,        /* GPIO15 */
    PERIPHS_IO_MUX_SPID_U,          /* GPIO16 */
    PERIPHS_IO_MUX_SPIQ_U,          /* GPIO17 */
    PERIPHS_IO_MUX_GPIO18_U,        /* GPIO18 */
    PERIPHS_IO_MUX_GPIO19_U,        /* GPIO19 */
    PERIPHS_IO_MUX_U0RXD_U,         /* GPIO20 */
    PERIPHS_IO_MUX_U0TXD_U,         /* GPIO21 */
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
    _GPIO,        /* GPIO10 */
    _NOT_EXIST,   /* GPIO11 is used as SPI VDD */
#if defined(CONFIG_FLASHMODE_QIO) || defined(CONFIG_FLASHMODE_QOUT)
    /* in qio and qout mode these pins are used for Flash */
    _SPIF,        /* GPIO12 is used as direct I/O SPI HD for Flash */
    _SPIF,        /* GPIO13 is used as direct I/O SPI WP for Flash */
#else
    /* otherwise these pins can be used as GPIO */
    _GPIO,        /* GPIO12 */
    _GPIO,        /* GPIO13 */
#endif
    _SPIF,        /* GPIO14 is used as direct I/O SPI CS0 for Flash */
    _SPIF,        /* GPIO15 is used as direct I/O SPI CLK for Flash */
    _SPIF,        /* GPIO16 is used as direct I/O SPI MISO for Flash */
    _SPIF,        /* GPIO17 is used as direct I/O SPI MOSI for Flash */
    _GPIO,        /* GPIO18 could be used for builtin USB2JTAG bridge */
    _GPIO,        /* GPIO19 could be used for builtin USB2JTAG bridge */
    _UART,        /* GPIO20 is used as direct I/O UART0 RxD */
    _UART,        /* GPIO21 is used as direct I/O UART0 TxD */
};
