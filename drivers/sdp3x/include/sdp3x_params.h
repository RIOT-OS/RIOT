/*
 * SPDX-FileCopyrightText: 2019 Dirk Ehmen
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sensors
 * @{
 * @file
 * @brief       Device driver params interface for the SDP3x sensor.
 *
 * @author      Dirk Ehmen       <ehmen@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @}
 */

#include "board.h"
#include "sdp3x.h"
#include "saul_reg.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SDP3x I2C addresses
 * @{
 */
#define SDP3X_ADDR1                   (0x21) /* 7 bit address */
#define SDP3X_ADDR2                   (0x22) /* 7 bit address */
#define SDP3X_ADDR3                   (0x23) /* 7 bit address */
/** @} */

/**
 * @name    SDP3x Models
 * @{
 */
#define SDP3X_MODEL_31                1
#define SDP3X_MODEL_32                2

#define SDP31_PRODUCT_NO_BYTE_0       0x03
#define SDP31_PRODUCT_NO_BYTE_1       0x01
#define SDP31_PRODUCT_NO_BYTE_3       0x01
/** @} */

/**
 * @name    Set default configuration parameters for the SDP3X
 * @{
 */
#ifndef SDP3X_PARAM_I2C_DEV
#define SDP3X_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef SDP3X_PARAM_I2C_ADDR
#define SDP3X_PARAM_I2C_ADDR        SDP3X_ADDR1
#endif
#ifndef SDP3X_PARAM_IRQ_PIN
#define SDP3X_PARAM_IRQ_PIN         GPIO_UNDEF
#endif

#ifndef SDP3X_PARAMS
#define SDP3X_PARAMS                { .i2c_dev       = SDP3X_PARAM_I2C_DEV,  \
                                      .i2c_addr      = SDP3X_PARAM_I2C_ADDR, \
                                      .irq_pin       = SDP3X_PARAM_IRQ_PIN }
#endif

#ifndef SDP3X_SAUL_INFO
#define SDP3X_SAUL_INFO             { .name = "sdp3x" }
#endif

/**@}*/

/**
 * @brief   Configure SDP3X
 */
static const sdp3x_params_t sdp3x_params[] =
{
    SDP3X_PARAMS
};

/**
 * @brief   Get the number of configured SDP3X devices
 */
#define SDP3X_NUMOF       ARRAY_SIZE(sdp3x_params)

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t sdp3x_saul_info[SDP3X_NUMOF] =
{
    SDP3X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif
