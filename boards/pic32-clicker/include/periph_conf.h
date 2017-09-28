/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *              2017 Francois Berder <fberder@outlook.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    boards_pic32-clicker MikroE PIC32 Clicker
 * @ingroup     boards
 * @brief       peripheral configuration for the MikroE PIC32 Clicker
 * @{
 *
 * @file
 * @brief       peripheral configuration for the MikroE PIC32 Clicker
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
 * @author      Francois Berder <fberder@outlook.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "vendor/p32mx470f512h.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   The peripheral clock is required for the UART Baud rate calculation
 *          It is configured by the 'config' registers (see pic32_config_settings.c)
 *          Note 120MHz is the max F for this device.
 */
#define PERIPHERAL_CLOCK (96000000)  /* Hz */

/**
 * @name    Timer definitions
 * @{
 */
#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)
/** @} */

/**
  * @name    UART Definitions
  *          There are 4 UARTS available on this CPU.
  *          We route debug via UART3 on this board,
  *          this is the UART connected to the MikroBUS
  *
  *          Note Microchip number the UARTS 1->4
  * @{
  */
#define UART_NUMOF          (4)
#define DEBUG_VIA_UART      (3)
#define DEBUG_UART_BAUD     (9600)
/** @} */


/**
 * @name    SPI device configuration
 *
 * @{
 */

static const spi_conf_t spi_config[] = {
    {
        /* No SPI0 on PIC32 */
        .mosi_pin = 0,
        .mosi_reg = (volatile uint32_t*)0,
        .mosi_af  = 0,
        .miso_pin = 0,
        .miso_reg = (volatile uint32_t*)0,
        .miso_af  = 0,
    },

    {   /*
         * SPI 1 (Mikrobus)
         *      MOSI -> RD4
         *      MISO -> RD3
         *      SCK  -> RD2
         */
        .mosi_pin = GPIO_PIN(PORT_D, 4),
        .mosi_reg = (volatile uint32_t*)&RPD4R,
        .mosi_af  = 0x8, /* 0b1000 */
        .miso_pin = GPIO_PIN(PORT_D, 3),
        .miso_reg = (volatile uint32_t*)&SDI1R,
        .miso_af  = 0x0, /* 0b0000 */
    },

    {   /*
         * SPI 2 (6LoWPAN radio)
         *      MOSI -> RG8
         *      MISO -> RG7
         *      SCK  -> RG6
         */
        .mosi_pin = GPIO_PIN(PORT_G, 8),
        .mosi_reg = (volatile uint32_t*)&RPG8R,
        .mosi_af  = 0x6, /* 0b0110 */
        .miso_pin = GPIO_PIN(PORT_G, 7),
        .miso_reg = (volatile uint32_t*)&SDI2R,
        .miso_af  = 0x1 /* 0b0001 */
    }
};

#define SPI_NUMOF           (2)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
