/*
 * Copyright (C) 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SEN5X_PARAMS_H
#define SEN5X_PARAMS_H

/**
 * @ingroup     drivers_sen5x
 *
 * @{
 * @file
 * @brief       Default configuration for SEN50/54/55
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 */

#ifndef SEN5X_PARAMS_H
#define SEN5X_PARAMS_H

#include "board.h"
#include "sen5x.h"
#include "saul_reg.h"
#include "sen5x_constants.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for SEN5x sensors
 * @{
 */
#ifndef SEN5X_PARAM_I2C_DEV
#define SEN5X_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef SEN5X_PARAM_ADDR
#define SEN5X_PARAM_ADDR            SEN5X_ADRESS_I2C
#endif

#ifndef SEN5X_PARAMS
#define SEN5X_PARAMS                { .i2c_dev = SEN5X_PARAM_I2C_DEV, \
                                      .i2c_addr = SEN5X_PARAM_ADDR }
#endif
#ifndef SEN5X_SAUL_INFO
#define SEN5X_SAUL_INFO             { .name = "sen5x" }
#endif
/**@}*/

/**
 * @brief   Configure SEN55/54
 */
static const sen5x_params_t sen5x_params[] =
{
    SEN5X_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t sen5x_saul_info[] =
{
    SEN5X_SAUL_INFO
};

/**
 * @brief   Get the number of configured SCD30 devices
 */

#define SEN5X_NUM    ARRAY_SIZE(sen5x_params)

#ifdef __cplusplus
}
#endif

#endif /* SEN5X_PARAMS_H */
/** @} */
