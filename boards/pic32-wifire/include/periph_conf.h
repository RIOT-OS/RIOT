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
 * @defgroup    boards_pic32-wifire Digilent PIC32 WiFire
 * @ingroup     boards
 * @brief       peripheral configuration for the Digilent PIC32 WiFire
 * @{
 *
 * @file
 * @brief       peripheral configuration for the Digilent PIC32 WiFire
 *
 * @author       Neil Jones <Neil.Jones@imgtec.com>
 * @author       Francois Berder <fberder@outlook.fr>
 */
#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "vendor/p32mz2048efg100.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The peripheral clock is required for the UART Baud rate calculation
 *          It is configured by the 'config' registers (see pic32_config_settings.c)
 */
#define PERIPHERAL_CLOCK (100000000)  /* Hz */

/**
 * @name    Timer definitions
 * @{
 */
#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)
/** @} */

/**
  * @name    UART Definitions
  *          There are 6 UARTS available on this CPU.
  *          We route debug via UART4 on this board,
  *          this is the UART connected to the FTDI USB <-> UART device.
  *
  *          Note Microchip number the UARTS 1->4.
  * @{
  */
#define UART_NUMOF          (6)
#define DEBUG_VIA_UART      (4)
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
         * SPI 1 (J10 connector)
         *      MOSI -> RE5
         *      MISO -> RD2
         *      SCK  -> RD1
         */
        .mosi_pin = GPIO_PIN(PORT_E, 5),
        .mosi_reg = (volatile uint32_t*)&RPE5R,
        .mosi_af  = 0x5, /* 0b0101 */
        .miso_pin = GPIO_PIN(PORT_D, 2),
        .miso_reg = (volatile uint32_t*)&SDI1R,
        .miso_af  = 0x0 /* 0b000 */
    },

    {   /*
         * SPI 2 (J9 connector)
         *      MOSI -> RF0
         *      MISO -> RD11
         *      SCK  -> RG6
         */
        .mosi_pin = GPIO_PIN(PORT_F, 0),
        .mosi_reg = (volatile uint32_t*)&RPF0R,
        .mosi_af  = 0x6, /* 0b0110 */
        .miso_pin = GPIO_PIN(PORT_D, 11),
        .miso_reg = (volatile uint32_t*)&SDI2R,
        .miso_af  = 0x3, /* 0b0011 */
    },

    {   /*
         * SPI 3 (microSD card)
         *   MOSI -> RC4
         *   MISO -> RB10
         *   SCK  -> RB14
         */
        .mosi_pin = GPIO_PIN(PORT_C, 4),
        .mosi_reg = (volatile uint32_t*)&RPC4R,
        .mosi_af  = 0x7, /* 0b0111 */
        .miso_pin = GPIO_PIN(PORT_B, 10),
        .miso_reg = (volatile uint32_t*)&SDI3R,
        .miso_af  = 0x6, /* 0b0110 */
    },

    {   /*
         * SPI 4 (MRF24WG0MA - wifi module)
         *   MOSI -> RG0
         *   MISO -> RF5
         *   SCK  -> RD10
         */
        .mosi_pin = GPIO_PIN(PORT_G, 0),
        .mosi_reg = (volatile uint32_t*)&RPG0R,
        .mosi_af  = 0x8, /* 0b1000 */
        .miso_pin = GPIO_PIN(PORT_F, 5),
        .miso_reg = (volatile uint32_t*)&SDI4R,
        .miso_af  = 0x2, /* 0b0010 */
    },
};

#define SPI_NUMOF           (4)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
