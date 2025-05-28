/*
 * Copyright (C) 2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_hip_badge
 * @brief       Peripheral configurations for the HiP Badge
 * @{
 *
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   I2C configuration
 *
 * One I2C interface I2C_DEV(0) is defined and available via the SAO headers.
 *
 * The GPIOs listed in the configuration are only initialized as I2C signals
 * when module `periph_i2c` is used. Otherwise they are not allocated and
 * can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO6           /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO5           /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-C3 provides 2 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is used for IrDA (untested).<br>
 *
 * @{
 */
#define UART0_TXD   GPIO21  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO20  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

#define UART1_TXD   GPIO7   /**< IrDA TX - Bad Choice? */
#define UART1_RXD   GPIO3   /**< IrDA RX - Bad Choice? */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
