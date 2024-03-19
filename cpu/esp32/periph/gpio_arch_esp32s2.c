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
 * @brief       Architecture-specific GPIO definitions for ESP32-S2 variant (family)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "gpio_arch.h"
#include "soc/io_mux_reg.h"

#if !IS_USED(MODULE_ESP_IDF_GPIO_HAL)

/* GPIO to IOMUX register mapping (see Technical Reference, Section 5.14.2 Register Summary)
   https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf */

const uint32_t _gpio_to_iomux_reg[GPIO_PIN_NUMOF] =
{
    PERIPHS_IO_MUX_GPIO0_U,      /* GPIO0 */
    PERIPHS_IO_MUX_GPIO1_U,      /* GPIO1 */
    PERIPHS_IO_MUX_GPIO2_U,      /* GPIO2 */
    PERIPHS_IO_MUX_GPIO3_U,      /* GPIO3 */
    PERIPHS_IO_MUX_GPIO4_U,      /* GPIO4 */
    PERIPHS_IO_MUX_GPIO5_U,      /* GPIO5 */
    PERIPHS_IO_MUX_GPIO6_U,      /* GPIO6 */
    PERIPHS_IO_MUX_GPIO7_U,      /* GPIO7 */
    PERIPHS_IO_MUX_GPIO8_U,      /* GPIO8 */
    PERIPHS_IO_MUX_GPIO9_U,      /* GPIO9 */
    PERIPHS_IO_MUX_GPIO10_U,     /* GPIO10 */
    PERIPHS_IO_MUX_GPIO11_U,     /* GPIO11 */
    PERIPHS_IO_MUX_GPIO12_U,     /* GPIO12 */
    PERIPHS_IO_MUX_GPIO13_U,     /* GPIO13 */
    PERIPHS_IO_MUX_GPIO14_U,     /* GPIO14 */
    PERIPHS_IO_MUX_XTAL_32K_P_U, /* GPIO15 used for 32k XTAL */
    PERIPHS_IO_MUX_XTAL_32K_N_U, /* GPIO16 used for 32k XTAL */
    PERIPHS_IO_MUX_DAC_1_U,      /* GPIO17 */
    PERIPHS_IO_MUX_DAC_2_U,      /* GPIO18 */
    PERIPHS_IO_MUX_GPIO19_U,     /* GPIO19 */
    PERIPHS_IO_MUX_GPIO20_U,     /* GPIO20 */
    PERIPHS_IO_MUX_GPIO21_U,     /* GPIO21 */
    0,                           /* GPIO22 is not available */
    0,                           /* GPIO23 is not available */
    0,                           /* GPIO24 is not available */
    0,                           /* GPIO25 is not available */
    PERIPHS_IO_MUX_SPICS1_U,     /* GPIO26 used as SPI CS1 */
    PERIPHS_IO_MUX_SPIHD_U,      /* GPIO27 used as SPI HS for Flash */
    PERIPHS_IO_MUX_SPIWP_U,      /* GPIO28 used as SPI WP for Flash */
    PERIPHS_IO_MUX_SPICS0_U,     /* GPIO29 used as SPI CS0 for Flash */
    PERIPHS_IO_MUX_SPICLK_U,     /* GPIO30 used as SPI CLK for Flash */
    PERIPHS_IO_MUX_SPID_U,       /* GPIO31 used as SPI D for Flash */
    PERIPHS_IO_MUX_SPIQ_U,       /* GPIO32 used as SPI Q for Flash */
    PERIPHS_IO_MUX_GPIO33_U,     /* GPIO33 */
    PERIPHS_IO_MUX_GPIO34_U,     /* GPIO34 */
    PERIPHS_IO_MUX_GPIO35_U,     /* GPIO35 */
    PERIPHS_IO_MUX_GPIO36_U,     /* GPIO36 */
    PERIPHS_IO_MUX_GPIO37_U,     /* GPIO37 */
    PERIPHS_IO_MUX_GPIO38_U,     /* GPIO38 */
    PERIPHS_IO_MUX_MTCK_U,       /* GPIO39 used as JTAG*/
    PERIPHS_IO_MUX_MTDO_U,       /* GPIO40 used as JTAG*/
    PERIPHS_IO_MUX_MTDI_U,       /* GPIO41 used as JTAG*/
    PERIPHS_IO_MUX_MTMS_U,       /* GPIO42 used as JTAG*/
    PERIPHS_IO_MUX_U0TXD_U,      /* GPIO43 used as UART0 TxD */
    PERIPHS_IO_MUX_U0RXD_U,      /* GPIO44 used as UART0 RxD */
    PERIPHS_IO_MUX_GPIO45_U,     /* GPIO45 */
    PERIPHS_IO_MUX_GPIO46_U,     /* GPIO46 */
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
    _GPIO,        /* GPIO6 */
    _GPIO,        /* GPIO7 */
    _GPIO,        /* GPIO8 */
    _GPIO,        /* GPIO9 */
    _GPIO,        /* GPIO10 */
    _GPIO,        /* GPIO11 */
    _GPIO,        /* GPIO12 */
    _GPIO,        /* GPIO13 */
    _GPIO,        /* GPIO14 */
#if MODULE_ESP_RTC_TIMER_32K
    _NOT_EXIST,   /* GPIO15 is used for external 32K crystal */
    _NOT_EXIST,   /* GPIO16 is used for external 32K crystal */
#else
    _GPIO,        /* GPIO15 */
    _GPIO,        /* GPIO16 */
#endif
    _GPIO,        /* GPIO17 */
    _GPIO,        /* GPIO18 */
    _GPIO,        /* GPIO19 could be used for ESP USB/builtin USB2JTAG bridge */
    _GPIO,        /* GPIO20 could be used for ESP USB/builtin USB2JTAG bridge */
    _GPIO,        /* GPIO21 */
    _NOT_EXIST,   /* GPIO22 does not exist */
    _NOT_EXIST,   /* GPIO23 does not exist */
    _NOT_EXIST,   /* GPIO24 does not exist */
    _NOT_EXIST,   /* GPIO25 does not exist */
    _NOT_EXIST,   /* GPIO26 is used as direct I/O SPI CS1 for Flash/PSRAM */
#if defined(FLASH_MODE_QIO) || defined(FLASH_MODE_QOUT)
    _SPIF,        /* GPIO27 is used as direct I/O SPI HD for Flash/PSRAM */
    _SPIF,        /* GPIO28 is used as direct I/O SPI WP for Flash/PSRAM */
#else
    _GPIO,        /* GPIO27 */
    _GPIO,        /* GPIO28 */
#endif
    _SPIF,        /* GPIO29 is used as direct I/O SPI CS0 for Flash/PSRAM */
    _SPIF,        /* GPIO30 is used as direct I/O SPI CLK for Flash/PSRAM */
    _SPIF,        /* GPIO31 is used as direct I/O SPI Q for Flash/PSRAM */
    _SPIF,        /* GPIO32 is used as direct I/O SPI D for Flash/PSRAM */
    _GPIO,        /* GPIO33 */
    _GPIO,        /* GPIO34 */
    _GPIO,        /* GPIO35 */
    _GPIO,        /* GPIO36 */
    _GPIO,        /* GPIO37 */
    _GPIO,        /* GPIO38 */
    _GPIO,        /* GPIO39 */
    _GPIO,        /* GPIO40 */
    _GPIO,        /* GPIO41 */
    _GPIO,        /* GPIO42 */
    _UART,        /* GPIO43 is used as direct I/O UART0 TxD */
    _UART,        /* GPIO44 is used as direct I/O UART0 RxD */
    _GPIO,        /* GPIO45 */
    _GPIO,        /* GPIO46 */
};
