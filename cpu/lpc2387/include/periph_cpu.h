/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lpc2387
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "cpu.h"

/**
 * @brief   LPC2387 MCU defines
 * @{
 */
#define __IO volatile

/**
 * @brief Fast GPIO register definition struct
 */
typedef struct {
    __IO uint32_t DIR;      /**< he */
    uint32_t _reserved[3];  /**< really */
    __IO uint32_t MASK;     /**< wants */
    __IO uint32_t PIN;      /**< to */
    __IO uint32_t SET;      /**< know */
    __IO uint32_t CLR;      /**< everything */
} FIO_PORT_t;

#define FIO_PORTS   ((FIO_PORT_t*)FIO_BASE_ADDR)
#define PINSEL      ((__IO uint32_t *)(PINSEL_BASE_ADDR))
#define PINMODE     ((__IO uint32_t *)(PINSEL_BASE_ADDR + 0x40))

int gpio_init_mux(unsigned pin, unsigned mux);
void gpio_init_states(void);

#define GPIO_PIN(port, pin) (port*32 + pin)

#ifndef DOXYGEN
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 1,       /**< emit interrupt on falling flank */
    GPIO_RISING = 2,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* ndef DOXYGEN */

/**
 * @brief   Number of available timer channels
 */
#define TIMER_CHAN_NUMOF        (4U)

/**
 * @brief   Declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/* @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100,   /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400,   /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000,  /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = 5000,  /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/* @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
