/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf51
 * @{
 *
 * @file
 * @brief           nRF51 specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_
#define PERIPH_CPU_

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Redefine some peripheral names to unify them between nRF51 and 52
 * @{
 */
#define GPIO_BASE           (NRF_GPIO)
#define UART_IRQN           (UART0_IRQn)
#define SPI_SCKSEL          (dev(bus)->PSELSCK)
#define SPI_MOSISEL         (dev(bus)->PSELMOSI)
#define SPI_MISOSEL         (dev(bus)->PSELMISO)
/** @} */

/**
 * @brief   Override I2C speed settings
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 0x01,                         /**< not supported */
    I2C_SPEED_NORMAL    = TWI_FREQUENCY_FREQUENCY_K100, /**< 100kbit/s */
    I2C_SPEED_FAST      = TWI_FREQUENCY_FREQUENCY_K400, /**< 400kbit/s */
    I2C_SPEED_FAST_PLUS = 0x02,                         /**< not supported */
    I2C_SPEED_HIGH      = 0x03,                         /**< not supported */
} i2c_speed_t;
/** @} */

/**
 * @brief   I2C (TWI) configuration options
 */
typedef struct {
    NRF_TWI_Type *dev;          /**< hardware device */
    uint8_t pin_scl;            /**< SCL pin */
    uint8_t pin_sda;            /**< SDA pin */
    uint8_t ppi;                /**< PPI channel to use */
} i2c_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_ */
/** @} */
