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

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

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

#define GPIO(port, pin) (port*32 + pin)

#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_PULLUP = 0,        /**< enable internal pull-up resistor */
    GPIO_NOPULL = 2,        /**< do not use internal pull resistors */
    GPIO_PULLDOWN = 3       /**< enable internal pull-down resistor */
} gpio_pp_t;

#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 1,       /**< emit interrupt on falling flank */
    GPIO_RISING = 2,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/* @} */

/* @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
