/*
 * Copyright (C) 2025 Gunar Schorcht
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
 * @brief       Architecture-specific GPIO definitions for ESP32-H2 variant (family)
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
    PERIPHS_IO_MUX_GPIO0_U,         /* GPIO0 */
    PERIPHS_IO_MUX_GPIO1_U,         /* GPIO1 */
    PERIPHS_IO_MUX_MTMS_U,          /* GPIO2 */
    PERIPHS_IO_MUX_MTDO_U,          /* GPIO3 */
    PERIPHS_IO_MUX_MTCK_U,          /* GPIO4 */
    PERIPHS_IO_MUX_MTDI_U,          /* GPIO5 */
    0,                              /* GPIO6 is not available */
    0,                              /* GPIO7 is not available */
    PERIPHS_IO_MUX_GPIO8_U,         /* GPIO8 */
    PERIPHS_IO_MUX_GPIO9_U,         /* GPIO9 */
    PERIPHS_IO_MUX_GPIO10_U,        /* GPIO10 */
    PERIPHS_IO_MUX_GPIO11_U,        /* GPIO11 */
    PERIPHS_IO_MUX_GPIO12_U,        /* GPIO12 */
    PERIPHS_IO_MUX_XTAL_32K_P_U,    /* GPIO13 used for XTAL_32K_P */
    PERIPHS_IO_MUX_XTAL_32K_N_U,    /* GPIO14 used for XTAL_32K_N*/
    0,                              /* GPIO15 is not available */
    0,                              /* GPIO16 is not available */
    0,                              /* GPIO17 is not available */
    0,                              /* GPIO18 is not available */
    0,                              /* GPIO19 is not available */
    0,                              /* GPIO20 is not available */
    0,                              /* GPIO21 is not available */
    PERIPHS_IO_MUX_GPIO22_U,        /* GPIO22 */
    PERIPHS_IO_MUX_U0RXD_U,         /* GPIO23 */
    PERIPHS_IO_MUX_U0TXD_U,         /* GPIO24 */
    PERIPHS_IO_MUX_GPIO25_U,        /* GPIO25 */
    PERIPHS_IO_MUX_GPIO26_U,        /* GPIO26 */
    PERIPHS_IO_MUX_GPIO27_U,        /* GPIO27 */
};

#endif /* !IS_USED(MODULE_ESP_IDF_GPIO_HAL) */

/* Table of the usage type of each GPIO pin */
gpio_pin_usage_t _gpio_pin_usage[GPIO_PIN_NUMOF] = {
    _GPIO,        /* GPIO0 */
    _GPIO,        /* GPIO1 */
    _GPIO,        /* GPIO2 */
    _GPIO,        /* GPIO3 */
    _GPIO,        /* GPIO4 */
    _GPIO,        /* GPIO5 */
    _NOT_EXIST,   /* GPIO6 does not exist */
    _NOT_EXIST,   /* GPIO7 does not exist */
    _GPIO,        /* GPIO8 */
    _GPIO,        /* GPIO9 */
    _GPIO,        /* GPIO10 */
    _GPIO,        /* GPIO11 */
    _GPIO,        /* GPIO12 */
#if MODULE_ESP_RTC_TIMER_32K
    _NOT_EXIST,   /* GPIO13 is used for external 32K crystal */
    _NOT_EXIST,   /* GPIO14 is used for external 32K crystal */
#else
    _GPIO,        /* GPIO13 */
    _GPIO,        /* GPIO14 */
#endif
    _NOT_EXIST,   /* GPIO15 does not exist */
    _NOT_EXIST,   /* GPIO16 does not exist */
    _NOT_EXIST,   /* GPIO17 does not exist */
    _NOT_EXIST,   /* GPIO18 does not exist */
    _NOT_EXIST,   /* GPIO19 does not exist */
    _NOT_EXIST,   /* GPIO20 does not exist */
    _NOT_EXIST,   /* GPIO21 does not exist */
    _GPIO,        /* GPIO22 */
    _UART,        /* GPIO23 is used as direct I/O UART0 RxD */
    _UART,        /* GPIO24 is used as direct I/O UART0 TxD */
    _GPIO,        /* GPIO25 */
    _GPIO,        /* GPIO26 could be used for builtin USB2JTAG bridge */
    _GPIO,        /* GPIO27 could be used for builtin USB2JTAG bridge */
};
