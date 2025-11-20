/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Architecture-specific GPIO definitions for ESP32 variant (family)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "gpio_arch.h"
#include "soc/io_mux_reg.h"

#if !IS_USED(MODULE_ESP_IDF_GPIO_HAL)

/* GPIO to IOMUX register mapping (see Technical Reference, Section 4.12 Register Summary)
   https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf */

const uint32_t _gpio_to_iomux_reg[GPIO_PIN_NUMOF] =
{
    PERIPHS_IO_MUX_GPIO0_U,     /* GPIO0 */
    PERIPHS_IO_MUX_U0TXD_U,     /* GPIO1 */
    PERIPHS_IO_MUX_GPIO2_U,     /* GPIO2 */
    PERIPHS_IO_MUX_U0RXD_U,     /* GPIO3 */
    PERIPHS_IO_MUX_GPIO4_U,     /* GPIO4 */
    PERIPHS_IO_MUX_GPIO5_U,     /* GPIO5 */
    PERIPHS_IO_MUX_SD_CLK_U,    /* GPIO6 is used for Flash */
    PERIPHS_IO_MUX_SD_DATA0_U,  /* GPIO7 is used for Flash */
    PERIPHS_IO_MUX_SD_DATA1_U,  /* GPIO8 is used for Flash */
    PERIPHS_IO_MUX_SD_DATA2_U,  /* GPIO9 is used for Flash in qio or qout mode */
    PERIPHS_IO_MUX_SD_DATA3_U,  /* GPIO10 is used for Flash in qio or qout mode */
    PERIPHS_IO_MUX_SD_CMD_U,    /* GPIO11 is used for Flash */
    PERIPHS_IO_MUX_MTDI_U,      /* GPIO12 is used as JTAG for OCD */
    PERIPHS_IO_MUX_MTCK_U,      /* GPIO13 is used as JTAG for OCD  */
    PERIPHS_IO_MUX_MTMS_U,      /* GPIO14 is used as JTAG for OCD  */
    PERIPHS_IO_MUX_MTDO_U,      /* GPIO15 is used as JTAG for OCD  */
    PERIPHS_IO_MUX_GPIO16_U,    /* GPIO16 is used as CS for PSRAM TODO */
    PERIPHS_IO_MUX_GPIO17_U,    /* GPIO17 */
    PERIPHS_IO_MUX_GPIO18_U,    /* GPIO18 */
    PERIPHS_IO_MUX_GPIO19_U,    /* GPIO19 */
    0,                          /* GPIO20 is not available */
    PERIPHS_IO_MUX_GPIO21_U,    /* GPIO21 */
    PERIPHS_IO_MUX_GPIO22_U,    /* GPIO22 */
    PERIPHS_IO_MUX_GPIO23_U,    /* GPIO23 */
    0,                          /* GPIO24 is not available */
    PERIPHS_IO_MUX_GPIO25_U,    /* GPIO25 */
    PERIPHS_IO_MUX_GPIO26_U,    /* GPIO26 */
    PERIPHS_IO_MUX_GPIO27_U,    /* GPIO27 */
    0,                          /* GPIO28 is not available */
    0,                          /* GPIO29 is not available */
    0,                          /* GPIO30 is not available */
    0,                          /* GPIO31 is not available */
    PERIPHS_IO_MUX_GPIO32_U,    /* GPIO32 */
    PERIPHS_IO_MUX_GPIO33_U,    /* GPIO33 */
    PERIPHS_IO_MUX_GPIO34_U,    /* GPIO34 */
    PERIPHS_IO_MUX_GPIO35_U,    /* GPIO35 */
    PERIPHS_IO_MUX_GPIO36_U,    /* GPIO36 */
    PERIPHS_IO_MUX_GPIO37_U,    /* GPIO37 */
    PERIPHS_IO_MUX_GPIO38_U,    /* GPIO38 */
    PERIPHS_IO_MUX_GPIO39_U,    /* GPIO39 */
};

#endif /* !IS_USED(MODULE_ESP_IDF_GPIO_HAL) */

/* Table of the usage type of each GPIO pin */
gpio_pin_usage_t _gpio_pin_usage [GPIO_PIN_NUMOF] = {
    _GPIO,        /* GPIO0 */
    _UART,        /* GPIO1 is used as direct I/O UART0 RxD */
    _GPIO,        /* GPIO2 */
    _UART,        /* GPIO3 is used as direct I/O UART0 TxD */
    _GPIO,        /* GPIO4 */
    _GPIO,        /* GPIO5 */
    _SPIF,        /* GPIO6 is used as direct I/O SPI SCK for Flash */
    _SPIF,        /* GPIO7 is used as direct I/O SPI MISO for Flash */
    _SPIF,        /* GPIO8 is used as direct I/O SPI MOSI for Flash */
    #if defined(CONFIG_FLASHMODE_QIO) || defined(CONFIG_FLASHMODE_QOUT)
    /* in qio and qout mode these pins are used in Flash */
    _SPIF,        /* GPIO9 is used as direct I/O SPI HD for Flash */
    _SPIF,        /* GPIO10 is used as direct I/O SPI WP for Flash */
    #else
    /* otherwise these pins can be used as GPIO */
    _GPIO,        /* GPIO9 */
    _GPIO,        /* GPIO10 */
    #endif
    _SPIF,        /* GPIO11 is used as direct I/O SPI CS0 for Flash */
    _GPIO,        /* GPIO12 */
    _GPIO,        /* GPIO13 */
    _GPIO,        /* GPIO14 */
    _GPIO,        /* GPIO15 */
    _GPIO,        /* GPIO16 */
    _GPIO,        /* GPIO17 */
    _GPIO,        /* GPIO18 */
    _GPIO,        /* GPIO19 */
    _NOT_EXIST,   /* GPIO20 does not exist */
    _GPIO,        /* GPIO21 */
    _GPIO,        /* GPIO22 */
    _GPIO,        /* GPIO23 */
    _NOT_EXIST,   /* GPIO24 does not exist */
    _GPIO,        /* GPIO25 */
    _GPIO,        /* GPIO26 */
    _GPIO,        /* GPIO27 */
    _NOT_EXIST,   /* GPIO28 does not exist */
    _NOT_EXIST,   /* GPIO29 does not exist */
    _NOT_EXIST,   /* GPIO30 does not exist */
    _NOT_EXIST,   /* GPIO31 does not exist */
#if MODULE_ESP_RTC_TIMER_32K
    _NOT_EXIST,   /* GPIO32 is used for external 32K crystal */
    _NOT_EXIST,   /* GPIO33 is used for external 32K crystal */
#else
    _GPIO,        /* GPIO32 */
    _GPIO,        /* GPIO33 */
#endif
    _GPIO,        /* GPIO34 */
    _GPIO,        /* GPIO35 */
    _GPIO,        /* GPIO36 */
    _GPIO,        /* GPIO37 */
    _GPIO,        /* GPIO38 */
    _GPIO         /* GPIO39 */
};
