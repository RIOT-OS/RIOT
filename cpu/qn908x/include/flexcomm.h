/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_qn908x
 *
 * @{
 *
 * @file
 * @brief       Flexcomm interface functions.
 *
 * The FLEXCOMM blocks can operate in different modes such as UART, SPI and I2C,
 * but not all modules support all modes. These functions allow to initialize
 * and configure the FLEXCOMM, as well as route back the ISRs to the
 * corresponding module.
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#include <stdint.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Flexcomm PSELID values
 *
 * This value identifies the current function of a FLEXCOMM module.
 */
typedef enum {
    FLEXCOMM_ID_UART    = 1,    /**< UART mode. */
    FLEXCOMM_ID_SPI     = 2,    /**< SPI mode. */
    FLEXCOMM_ID_I2C     = 3,    /**< I2C mode. */
} flexcom_pselid_t;

/**
 * @brief   Initialize a flexcomm module to operate as the selected mode.
 *
 * @returns -1 in case of error, otherwise returns the number of flexcomm
 * instance initialized, such as 2 for FLEXCOMM2.
 */
int flexcomm_init(FLEXCOMM_Type *dev, flexcom_pselid_t mode);

/**
 * @brief   Obtain the flexcomm block number (0-based) from the address.
 *
 * For example, the flexcomm block number of FLEXCOMM2, the pointer to the
 * FLEXCOMM_Type block is 2. If an invalid address is passed returns -1.
 */
int flexcomm_instance_from_addr(const FLEXCOMM_Type *dev);

#ifdef __cplusplus
}
#endif

/** @} */
